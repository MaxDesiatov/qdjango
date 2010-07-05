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
#include <QThread>

#include "QDjango.h"
#include "QDjango_p.h"
#include "QDjangoModel.h"

QMap<QString, QDjangoModel*> QDjango::registry = QMap<QString, QDjangoModel*>();
QMap<QString, QDjangoMetaModel> QDjango::metaModels = QMap<QString, QDjangoMetaModel>();

static QDjangoWatcher *globalWatcher = 0;

QDjangoWatcher::QDjangoWatcher(QObject *parent)
    : QObject(parent), connectionId(0)
{
}

void QDjangoWatcher::threadFinished()
{
    QThread *thread = qobject_cast<QThread*>(sender());
    copies.remove(thread);
}

static void closeDatabase()
{
    delete globalWatcher;
}

/*! \mainpage
 *
 * QDjango is a simple yet powerful Object Relation Mapper (ORM) built
 * on top of the Qt library. Where possible it tries to follow django's
 * ORM API, hence its name.
 *
 * \sa QDjango
 * \sa QDjangoModel
 * \sa QDjangoWhere
 * \sa QDjangoQuerySet
 */


static void sqlDebug(const QSqlQuery &query)
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

/** Returns the database used by QDjango.
 *
 *  If you call this method from any thread but the application's main thread,
 *  a new connection to the database will be created. The connection will
 *  automatically be torn down once the thread finishes.
 *
 *  \sa setDatabase()
 */
QSqlDatabase QDjango::database()
{
    Q_ASSERT(globalWatcher != 0);
    QThread *thread = QThread::currentThread();

    // if we are in the main thread, return reference connection
    if (thread == globalWatcher->thread())
        return globalWatcher->reference;

    // if we have a connection for this thread, return it
    if (globalWatcher->copies.contains(thread))
        return globalWatcher->copies[thread];

    // create a new connection for this thread
    QObject::connect(thread, SIGNAL(finished()), globalWatcher, SLOT(threadFinished()));
    QSqlDatabase db = QSqlDatabase::cloneDatabase(globalWatcher->reference,
        QString("_qdjango_%1").arg(globalWatcher->connectionId++));
    Q_ASSERT(db.open());
    globalWatcher->copies.insert(thread, db);
    return db;
}

/** Sets the database used by QDjango.
 *
 *  You must call this method from your application's main thread.
 *
 *  \sa database()
 */
void QDjango::setDatabase(QSqlDatabase database)
{
    if (database.driverName() != "QSQLITE" &&
        database.driverName() != "QSQLITE2" &&
        database.driverName() != "QMYSQL")
    {
        qWarning() << "Unsupported database driver" << database.driverName();
    }
    if (!globalWatcher)
    {
        globalWatcher = new QDjangoWatcher();
        qAddPostRoutine(closeDatabase);
    }
    globalWatcher->reference = database;
}

/** Creates the database tables for all registered models.
 */
void QDjango::createTables()
{
    foreach (const QString &key, metaModels.keys())
        metaModels[key].createTable();
}

/** Drops the database tables for all registered models.
 */
void QDjango::dropTables()
{
    foreach (const QString &key, metaModels.keys())
        metaModels[key].dropTable();
}

QDjangoMetaModel QDjango::metaModel(const QString &name)
{
    return metaModels.value(name);
}

/** Returns the QDjangoModel with the given name.
 */
const QDjangoModel *QDjango::model(const QString &name)
{
    return registry.value(name);
}

void QDjango::registerModel(QDjangoModel *model)
{
    const QString name = model->metaObject()->className();
    metaModels.insert(name, QDjangoMetaModel(model));
}

/** Returns the SQL used to declare a field as auto-increment.
 */
QString QDjango::autoIncrementSql()
{
    const QString driverName = QDjango::database().driverName();
    if (driverName == "QSQLITE" || driverName == "QSQLITE2")
        return QLatin1String(" AUTOINCREMENT");
    else if (driverName == "QMYSQL")
        return QLatin1String(" AUTO_INCREMENT");
    else
        return QString();
}

/** Returns the empty SQL limit clause.
 */
QString QDjango::noLimitSql()
{
    const QString driverName = QDjango::database().driverName();
    if (driverName == "QSQLITE" || driverName == "QSQLITE2")
        return QLatin1String(" LIMIT -1");
    else if (driverName == "QMYSQL")
        // 2^64 - 1, as recommended by the MySQL documentation
        return QLatin1String(" LIMIT 18446744073709551615");
    else
        return QString();
}

/** Quotes a database table or column name.
 */
QString QDjango::quote(const QString &name)
{
    return "`" + name + "`";
}

/** Unquotes a database table or column name.
 */
QString QDjango::unquote(const QString &quoted)
{
    if (quoted.startsWith("`") && quoted.endsWith("`"))
        return quoted.mid(1, quoted.size() - 2);
    return quoted;
}

QDjangoMetaField::QDjangoMetaField()
    : autoIncrement(false),
    index(false),
    maxLength(0),
    primaryKey(false)
{
}

QDjangoMetaModel::QDjangoMetaModel(const QDjangoModel *model)
    : m_model(model)
{
    const QMetaObject* meta = model->metaObject();
    table = QString(meta->className()).toLower();

    // local fields
    const int count = meta->propertyCount();
    for(int i = meta->propertyOffset(); i < count; ++i)
    {
        QDjangoMetaField field;
        field.name = meta->property(i).name();
        field.type = meta->property(i).type();
        field.index = (model->m_pkName == field.name);
        field.primaryKey = (model->m_pkName == field.name);

        // FIXME get rid of reference to model
        const QString fkName = model->m_foreignKeys.key(field.name);
        if (!fkName.isEmpty())
            field.index = true;

        // parse options
        const int infoIndex = meta->indexOfClassInfo(meta->property(i).name());
        if (infoIndex >= 0)
        {
            QMetaClassInfo classInfo = meta->classInfo(infoIndex);
            QStringList items = QString(classInfo.value()).split(' ');
            foreach (const QString &item, items)
            {
                QStringList assign = item.split('=');
                if (assign.length() == 2)
                {
                    const QString key = assign[0].toLower();
                    const QString value = assign[1];
                    if (key == "max_length")
                        field.maxLength = value.toInt();
                    else if (key == "primary_key")
                    {
                        field.index = true;
                        field.primaryKey = true;

                        primaryKey = field.name;
                    }
                }
            }
        }

        localFields << field;
    }

    // automatic primary key
    if (primaryKey.isEmpty())
    {
        QDjangoMetaField field;
        field.name = "id";
        field.type = QVariant::Int;
        field.autoIncrement = true;
        field.index = true;
        field.primaryKey = true;
        localFields.prepend(field);

        primaryKey = field.name;
    }
 
}

/** Creates the database table for this QDjangoModel.
 */
bool QDjangoMetaModel::createTable() const
{
    QSqlDatabase db = QDjango::database();
//    const QMetaObject* meta = metaObject();

    QStringList propSql;
    foreach (const QDjangoMetaField &field, localFields)
    {
        QString fieldSql = QDjango::quote(field.name);
        if (field.type == QVariant::Bool)
            fieldSql += " BOOLEAN";
        else if (field.type == QVariant::ByteArray)
        {
            fieldSql += " BLOB";
            if (field.maxLength > 0)
                fieldSql += QString("(%1)").arg(field.maxLength);
        }
        else if (field.type == QVariant::Date)
            fieldSql += " DATE";
        else if (field.type == QVariant::DateTime)
            fieldSql += " DATETIME";
        else if (field.type == QVariant::Double)
            fieldSql += " REAL";
        else if (field.type == QVariant::Int)
            fieldSql += " INTEGER";
        else if (field.type == QVariant::LongLong)
            fieldSql += " INTEGER";
        else if (field.type == QVariant::String)
        {
            if (field.maxLength > 0)
                fieldSql += QString(" VARCHAR(%1)").arg(field.maxLength);
            else
                fieldSql += " TEXT";
        }
        else {
            qWarning() << "Unhandled type" << field.type << "for property" << field.name;
            continue;
        }

        // primary key
        if (field.primaryKey)
            fieldSql += " PRIMARY KEY";

        // auto-increment is backend specific
        if (field.autoIncrement)
            fieldSql += QDjango::autoIncrementSql();

        // foreign key
        // FIXME : get rid of reference to model
        const QString fkName = m_model->m_foreignKeys.key(field.name);
        if (!fkName.isEmpty())
        {
            const QDjangoModel *fkModel = m_model->m_foreignModels[fkName];
            fieldSql += QString(" REFERENCES %1 (%2)").arg(
                QDjango::quote(fkModel->databaseTable()), QDjango::quote(fkModel->m_pkName));
        }
        propSql << fieldSql;
    }

    // create table
    QSqlQuery createQuery(db);
    createQuery.prepare(QString("CREATE TABLE %1 (%2)").arg(QDjango::quote(table), propSql.join(", ")));
    if (!sqlExec(createQuery))
        return false;

    // create indices
    foreach (const QDjangoMetaField &field, localFields)
    {
        if (field.index)
        {
            const QString indexName = QString("%1_%2").arg(table, field.name);
            QSqlQuery indexQuery(db);
            indexQuery.prepare(QString("CREATE %1 %2 ON %3 (%4)").arg(
                field.primaryKey ? "UNIQUE INDEX" : "INDEX",
                QDjango::quote(indexName),
                QDjango::quote(table),
                QDjango::quote(field.name)));
            if (!sqlExec(indexQuery))
                return false;
        }
    }

    return true;
}

/** Drops the database table for this QDjangoMetaModel.
 */
bool QDjangoMetaModel::dropTable() const
{
    QSqlQuery query(QDjango::database());
    query.prepare(QString("DROP TABLE %1").arg(QDjango::quote(table)));
    return sqlExec(query);
}

