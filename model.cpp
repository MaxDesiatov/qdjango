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
static QSqlDatabase globalDb;

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
#ifdef QDJANGO_DEBUG_SQL
    sqlDebug(query);
    if (!query.exec())
    {
        qWarning() << "SQL error" << query.lastError();
        return false;
    }
    return true;
#else
    return query.exec();
#endif
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

const QDjangoModel *QDjango::model(const QString &name)
{
    return registry.value(name);
}

QString QDjango::quote(const QString &name)
{
    return "`" + name + "`";
}

QString QDjango::unquote(const QString &quoted)
{
    if (quoted.startsWith("`") && quoted.endsWith("`"))
        return quoted.mid(1, quoted.size() - 2);
    return quoted;
}

QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent), m_id(0), m_pkName("id")
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

QSqlDatabase QDjangoModel::database()
{
    return globalDb;
}

void QDjangoModel::setDatabase(QSqlDatabase database)
{
    globalDb = database;
}

bool QDjangoModel::createTable() const
{
    QSqlDatabase db = database();
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
    QString sql = QString("CREATE TABLE %1 (%2)").arg(QDjango::quote(databaseTable()), propSql.join(", "));
    QSqlQuery createQuery(sql, db);
    if (!sqlExec(createQuery))
        return false;

    // create index
    if (m_pkName != "id")
    {
        QString indexName = m_pkName;
        sql = QString("CREATE UNIQUE INDEX %1 ON %2 (%3)").arg(indexName, QDjango::quote(databaseTable()), m_pkName);
        QSqlQuery indexQuery(sql, db);
        if (!sqlExec(indexQuery))
            return false;
    }
    return true;
}

bool QDjangoModel::dropTable() const
{
    QString sql = QString("DROP TABLE %1").arg(QDjango::quote(databaseTable()));
    QSqlQuery query(sql, database());
    return sqlExec(query);
}

QString QDjangoModel::databaseColumn(const QString &name) const
{
    return QDjango::quote(databaseTable()) + "." + QDjango::quote(name);
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

void QDjangoModel::databaseLoad(const QMap<QString, QVariant> &props)
{
    foreach (const QString &key, props.keys())
    {
        QStringList bits = key.split(".");
        const QString field = QDjango::unquote(bits[1]);
        if (field == m_pkName)
            setPk(props[key]);
        else
            setProperty(field.toLatin1(), props[key]);
    }
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

void QDjangoModel::addForeignKey(const QString &field, const QString &modelName)
{
    m_foreignKeys[field] = modelName;
}

QMap<QString,QString> QDjangoModel::foreignKeys() const
{
    return m_foreignKeys;
}

bool QDjangoModel::remove()
{
    QString sql = QString("DELETE FROM %1 WHERE %2 = :pk")
                  .arg(QDjango::quote(databaseTable()), m_pkName);
    QSqlQuery query(sql, database());
    query.bindValue(":pk", pk());
    return sqlExec(query);
}

bool QDjangoModel::save()
{
    QSqlDatabase db = database();
    QStringList fieldNames = databaseFields();
    fieldNames.removeAll(m_pkName);

    if (!pk().isNull() && !(m_pkName == "id" && !pk().toInt()))
    {
        QString sql = QString("SELECT 1 AS a FROM %1 WHERE %2 = :pk")
                      .arg(QDjango::quote(databaseTable()), m_pkName);
        QSqlQuery query(sql, db);
        query.bindValue(":pk", pk());
        if (sqlExec(query) && query.next())
        {
            QStringList fieldAssign;
            foreach (const QString &name, fieldNames)
                fieldAssign << name + " = :" + name;

            QString sql = QString("UPDATE %1 SET %2 WHERE %3 = :pk")
                  .arg(QDjango::quote(databaseTable()), fieldAssign.join(", "), m_pkName);
            QSqlQuery query(sql, db);
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
                  .arg(QDjango::quote(databaseTable()), fieldNames.join(", "), fieldHolders.join(", "));
    QSqlQuery query(sql, db);
    foreach (const QString &name, fieldNames)
        query.bindValue(":" + name, property(name.toLatin1()));

    bool ret = sqlExec(query);
    if (m_pkName == "id")
        setPk(query.lastInsertId());
    return ret;
}
