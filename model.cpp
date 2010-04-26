/*
 * QDjango
 * Copyright (C) 2010 Bolloré telecom
 * See AUTHORS file for a full list of contributors.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QMetaProperty>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "model.h"
#include "queryset.h"

static QMap<QString, QDjangoModel*> registry;
static QSqlDatabase *db = 0;

void sqlDebug(const QSqlQuery &query)
{
    qDebug() << "SQL query" << query.lastQuery();
    QMapIterator<QString, QVariant> i(query.boundValues());
    while (i.hasNext()) {
        i.next();
        qDebug() << "   " << i.key().toAscii().data() << "="
                 << i.value().toString().toAscii().data();
    }
}

bool sqlExec(QSqlQuery &query)
{
    sqlDebug(query);
    if (!query.exec())
    {
        qWarning() << "SQL error" << query.lastError();
        return false;
    }
    return true;
}

void QDjango::createTables()
{
    foreach (const QString &key, registry.keys())
        registry[key]->createTable();
}

bool QDjango::registerModel(QDjangoModel *model)
{
    const QString name = model->metaObject()->className();
    if (registry.contains(name))
        return false;
    qDebug() << "Registering model" << name;
    registry.insert(name, model);
    return true;
}

bool QDjango::isRegistered(const QString &modelName)
{
    return registry.contains(modelName);
}

QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent), m_pkName("id"), m_id(0)
{
}

QVariant QDjangoModel::pk() const
{
    if (m_pkName == "id")
        return m_id;
    else
        return property(m_pkName.toLatin1());
}

void QDjangoModel::setPk(const QVariant &pk)
{
    if (m_pkName == "id")
        m_id = pk.toInt();
    else
        setProperty(m_pkName.toLatin1(), pk);
}

QSqlDatabase &QDjangoModel::database()
{
    return *db;
}

void QDjangoModel::setDatabase(QSqlDatabase *database)
{
    db = database;
}

bool QDjangoModel::createTable() const
{
    const QMetaObject* meta = metaObject();

    QStringList propSql;
    if (m_pkName == "id")
        propSql << m_pkName + " INTEGER PRIMARY KEY AUTOINCREMENT";
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
        const QString field = QString::fromLatin1(meta->property(i).name());
        if (meta->property(i).type() == QVariant::Double)
            propSql << field + " REAL";
        else if (meta->property(i).type() == QVariant::Int)
            propSql << field + " INTEGER";
        else if (meta->property(i).type() == QVariant::String)
            propSql << field + " TEXT";
        else
            qWarning() << "Unhandled property type" << meta->property(i).typeName();
    }

    // create table
    QString sql = QString("CREATE TABLE %1 (%2)").arg(databaseTable(), propSql.join(", "));
    QSqlQuery createQuery(sql, *db);
    if (!sqlExec(createQuery))
        return false;

    // create index
    if (m_pkName != "id")
    {
        QString indexName = m_pkName;
        sql = QString("CREATE UNIQUE INDEX %1 ON %2 (%3)").arg(indexName, databaseTable(), m_pkName);
        QSqlQuery indexQuery(sql, *db);
        if (!sqlExec(indexQuery))
            return false;
    }
    return true;
}

bool QDjangoModel::dropTable() const
{
    QString sql = QString("DROP TABLE %1").arg(databaseTable());
    QSqlQuery query(sql, *db);
    return sqlExec(query);
}

QStringList QDjangoModel::databaseFields() const
{
    const QMetaObject* meta = metaObject();
    QStringList properties;
    if (m_pkName == "id")
        properties << m_pkName;
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
        properties << QString::fromLatin1(meta->property(i).name());
    return properties;
}

QString QDjangoModel::databasePkName() const
{
    return m_pkName;
}

QString QDjangoModel::databaseTable() const
{
    QString className(metaObject()->className());
    return className.toLower();
}

bool QDjangoModel::remove()
{
    QString sql = QString("DELETE FROM %1 WHERE %2 = :pk")
                  .arg(databaseTable(), m_pkName);
    QSqlQuery query(sql, *db);
    query.bindValue(":pk", pk());
    return sqlExec(query);
}

bool QDjangoModel::save()
{
    QStringList fieldNames = databaseFields();
    fieldNames.removeAll(m_pkName);

    if (!pk().isNull())
    {
        QString sql = QString("SELECT 1 AS a FROM %1 WHERE %2 = :pk")
                      .arg(databaseTable(), m_pkName);
        QSqlQuery query(sql, *db);
        query.bindValue(":pk", pk());
        if (sqlExec(query) && query.next())
        {
            QStringList fieldAssign;
            foreach (const QString &name, fieldNames)
                fieldAssign << name + " = :" + name;

            QString sql = QString("UPDATE %1 SET %2 WHERE %3 = :pk")
                  .arg(databaseTable(), fieldAssign.join(", "), m_pkName);
            QSqlQuery query(sql, *db);
            foreach (const QString &name, fieldNames)
                query.bindValue(":" + name, property(name.toLatin1()));
            query.bindValue(":pk", pk());
            return sqlExec(query);
        }
    }

    // perform insert
    QStringList fieldHolders;
    foreach (const QString &name, fieldNames)
        fieldHolders << ":" + name;

    QString sql = QString("INSERT INTO %1 (%2) VALUES(%3)")
                  .arg(databaseTable(), fieldNames.join(", "), fieldHolders.join(", "));
    QSqlQuery query(sql, *db);
    foreach (const QString &name, fieldNames)
        query.bindValue(":" + name, property(name.toLatin1()));

    bool ret = sqlExec(query);
    if (m_pkName == "id")
        setPk(query.lastInsertId());
    return ret;
}
