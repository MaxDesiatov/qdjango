/*
 * QDjango
 * Copyright (C) 2010 Bolloré telecom
 * See AUTHORS file for a full list of contributors.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QDebug>
#include <QMetaProperty>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "QDjango.h"
#include "QDjangoModel.h"
#include "QDjangoQuerySet.h"

static QSqlDatabase globalDb;

static void closeDatabase()
{
    globalDb = QSqlDatabase();
}

/** Construct a new QDjangoModel.
 *
 * @param parent
 */
QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent), m_id(0), m_pkName("id")
{
}

/** Returns primary key for this QDjangoModel.
 */
QVariant QDjangoModel::pk() const
{
    if (m_pkName == "id")
        return m_id;
    else
        return property(m_pkName.toLatin1());
}

/** Sets the primary key for this QDjangoModel.
 *
 * @param pk
 */
void QDjangoModel::setPk(const QVariant &pk)
{
    if (m_pkName == "id")
        m_id = pk.toInt();
    else
        setProperty(m_pkName.toLatin1(), pk);
}

/** Returns the database used by QDjango.
 */
QSqlDatabase QDjangoModel::database()
{
    return globalDb;
}

/** Sets the database used by QDjango.
 */
void QDjangoModel::setDatabase(QSqlDatabase database)
{
    if (database.driverName() != "QSQLITE" &&
        database.driverName() != "QSQLITE2" &&
        database.driverName() != "QMYSQL")
    {
        qWarning() << "Unsupported database driver" << database.driverName();
    }
    globalDb = database;
    qAddPostRoutine(closeDatabase);
}

/** Creates the database table for this QDjangoModel.
 */
bool QDjangoModel::createTable() const
{
    QSqlDatabase db = database();
    const QMetaObject* meta = metaObject();

    QStringList propSql;
    foreach (const QString &fieldName, databaseFields())
    {
        bool autoIncrement = false;
        QVariant::Type fieldType;
        if (fieldName == "id")
        {
            autoIncrement = true;
            fieldType = QVariant::Int;
        }
        else
        {
            int i = meta->indexOfProperty(fieldName.toLatin1());
            Q_ASSERT(i >= 0);
            fieldType = meta->property(i).type();
        }

        QString fieldSql = QDjango::quote(fieldName);
        if (fieldType == QVariant::DateTime)
            fieldSql += " DATETIME";
        else if (fieldType == QVariant::Double)
            fieldSql += " REAL";
        else if (fieldType == QVariant::Int)
            fieldSql += " INTEGER";
        else if (fieldType == QVariant::String)
            fieldSql += " TEXT";
        else {
            qWarning() << "Unhandled property" << fieldName;
            continue;
        }

        // primary key
        if (fieldName == m_pkName)
        {
            fieldSql += " PRIMARY KEY";
            if (autoIncrement)
            {
                if (db.driverName() == "QSQLITE" || db.driverName() == "QSQLITE2")
                    fieldSql += " AUTOINCREMENT";
                else if (db.driverName() == "QMYSQL")
                    fieldSql += " AUTO_INCREMENT";
            }
        }

        propSql << fieldSql;
    }

    // create table
    QSqlQuery createQuery(db);
    createQuery.prepare(QString("CREATE TABLE %1 (%2)").arg(QDjango::quote(databaseTable()), propSql.join(", ")));
    if (!sqlExec(createQuery))
        return false;

    // create index
    if (m_pkName != "id")
    {
        QString indexName = m_pkName;
        QSqlQuery indexQuery(db);
        indexQuery.prepare(QString("CREATE UNIQUE INDEX %1 ON %2 (%3)").arg(indexName, QDjango::quote(databaseTable()), m_pkName));
        if (!sqlExec(indexQuery))
            return false;
    }
    return true;
}

/** Drops the database table for this QDjangoModel.
 */
bool QDjangoModel::dropTable() const
{
    QSqlQuery query(database());
    query.prepare(QString("DROP TABLE %1").arg(QDjango::quote(databaseTable())));
    return sqlExec(query);
}

QString QDjangoModel::databaseColumn(const QString &name) const
{
    // foreign key lookup
    if (name.count("__"))
    {
        QStringList bits = name.split("__");
        QString fk = bits.takeFirst();
        if (m_foreignModels.contains(fk))
        {
            QDjangoModel *foreign = m_foreignModels[fk];
            return foreign->databaseColumn(bits.join("__"));
        }
    }

    QString realName = (name == "pk") ? m_pkName : name;
    return QDjango::quote(databaseTable()) + "." + QDjango::quote(realName);
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
    // process local fields
    foreach (const QString &key, props.keys())
    {
        QStringList bits = key.split(".");
        if (bits.size() != 2)
        {
            qWarning() << "Invalid column name" << key;
            continue;
        }

        const QString table = QDjango::unquote(bits[0]);
        if (table == databaseTable())
        {
            const QString field = QDjango::unquote(bits[1]);
            if (field == m_pkName)
                setPk(props[key]);
            else
                setProperty(field.toLatin1(), props[key]);
        }
    }

    // process foreign fields
    foreach (const QString &key, m_foreignModels.keys())
        m_foreignModels[key]->databaseLoad(props);
}

QString QDjangoModel::databaseTable() const
{
    QString className(metaObject()->className());
    return className.toLower();
}

/** Declares a foreign-key pointing to a QDjangoModel.
 *
 * @param name
 * @param field
 * @param model
 */
void QDjangoModel::addForeignKey(const QString &name, const QString &field, QDjangoModel *model)
{
    m_foreignKeys[name] = field;
    m_foreignModels[name] = model;
}

/** Retrieves the QDjangoModel pointed to by the given foreign-key.
 *
 * @param name
 */
QDjangoModel *QDjangoModel::foreignKey(const QString &name) const
{
    if (!m_foreignKeys.contains(name) || !m_foreignModels.contains(name))
    {
        qWarning() << "Unknown foreign key" << name;
        return 0;
    }

    QDjangoModel *foreign = m_foreignModels[name];
    QVariant foreignPk = property(m_foreignKeys[name].toLatin1());

    // if the foreign object was not loaded yet, do it now
    if (foreign->pk() != foreignPk)
    {
        QDjangoQueryBase qs(foreign->metaObject()->className());
        qs.addFilter(QDjangoWhere(foreign->m_pkName, QDjangoWhere::Equals, foreignPk));
        qs.sqlFetch();
        if (qs.m_properties.size() != 1)
            return 0;
        foreign->databaseLoad(qs.m_properties.at(0));
    }
    return foreign;
}

/** Deletes the QDjangoModel from the database.
 */
bool QDjangoModel::remove()
{
    QSqlQuery query(database());
    query.prepare(QString("DELETE FROM %1 WHERE %2 = :pk")
                  .arg(QDjango::quote(databaseTable()), QDjango::quote(m_pkName)));
    query.bindValue(":pk", pk());
    return sqlExec(query);
}

/** Saves the QDjangoModel to the database.
 */
bool QDjangoModel::save()
{
    QSqlDatabase db = database();
    QStringList fieldNames = databaseFields();
    fieldNames.removeAll(m_pkName);

    if (!pk().isNull() && !(m_pkName == "id" && !pk().toInt()))
    {
        QSqlQuery query(db);
        query.prepare(QString("SELECT 1 AS a FROM %1 WHERE %2 = :pk")
                      .arg(QDjango::quote(databaseTable()), QDjango::quote(m_pkName)));
        query.bindValue(":pk", pk());
        if (sqlExec(query) && query.next())
        {
            QStringList fieldAssign;
            foreach (const QString &name, fieldNames)
                fieldAssign << QDjango::quote(name) + " = :" + name;

            QSqlQuery query(db);
            query.prepare(QString("UPDATE %1 SET %2 WHERE %3 = :pk")
                  .arg(QDjango::quote(databaseTable()), fieldAssign.join(", "), m_pkName));
            foreach (const QString &name, fieldNames)
                query.bindValue(":" + name, property(name.toLatin1()));
            query.bindValue(":pk", pk());
            return sqlExec(query);
        }
    }

    // perform insert
    QStringList fieldColumns;
    QStringList fieldHolders;
    foreach (const QString &name, fieldNames)
    {
        fieldColumns << QDjango::quote(name);
        fieldHolders << ":" + name;
    }

    QSqlQuery query(db);
    query.prepare(QString("INSERT INTO %1 (%2) VALUES(%3)")
                  .arg(QDjango::quote(databaseTable()), fieldColumns.join(", "), fieldHolders.join(", ")));
    foreach (const QString &name, fieldNames)
        query.bindValue(":" + name, property(name.toLatin1()));

    bool ret = sqlExec(query);
    if (m_pkName == "id")
        setPk(query.lastInsertId());
    return ret;
}
