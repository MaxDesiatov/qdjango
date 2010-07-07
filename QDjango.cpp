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
#include "QDjangoQuerySet_p.h"
#include "QDjangoModel.h"

QMap<QString, QDjangoMetaModel> globalMetaModels = QMap<QString, QDjangoMetaModel>();
static QDjangoDatabase *globalDatabase = 0;

QDjangoDatabase::QDjangoDatabase(QObject *parent)
    : QObject(parent), connectionId(0)
{
}

void QDjangoDatabase::threadFinished()
{
    QThread *thread = qobject_cast<QThread*>(sender());
    copies.remove(thread);
}

static void closeDatabase()
{
    delete globalDatabase;
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


bool sqlExec(QSqlQuery &query)
{
#ifdef QDJANGO_DEBUG_SQL
    qDebug() << "SQL query" << query.lastQuery();
    QMapIterator<QString, QVariant> i(query.boundValues());
    while (i.hasNext()) {
        i.next();
        qDebug() << "   " << i.key().toAscii().data() << "="
                 << i.value().toString().toAscii().data();
    }
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
    Q_ASSERT(globalDatabase != 0);
    QThread *thread = QThread::currentThread();

    // if we are in the main thread, return reference connection
    if (thread == globalDatabase->thread())
        return globalDatabase->reference;

    // if we have a connection for this thread, return it
    if (globalDatabase->copies.contains(thread))
        return globalDatabase->copies[thread];

    // create a new connection for this thread
    QObject::connect(thread, SIGNAL(finished()), globalDatabase, SLOT(threadFinished()));
    QSqlDatabase db = QSqlDatabase::cloneDatabase(globalDatabase->reference,
        QString("_qdjango_%1").arg(globalDatabase->connectionId++));
    Q_ASSERT(db.open());
    globalDatabase->copies.insert(thread, db);
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
    if (!globalDatabase)
    {
        globalDatabase = new QDjangoDatabase();
        qAddPostRoutine(closeDatabase);
    }
    globalDatabase->reference = database;
}

/** Creates the database tables for all registered models.
 */
bool QDjango::createTables()
{
    bool ret = true;
    foreach (const QString &key, globalMetaModels.keys())
        if (!globalMetaModels[key].createTable())
            ret = false;
    return ret;
}

/** Drops the database tables for all registered models.
 */
bool QDjango::dropTables()
{
    bool ret = true;
    foreach (const QString &key, globalMetaModels.keys())
        if (!globalMetaModels[key].dropTable())
            ret = false;
    return ret;
}

/** Returns the QDjangoMetaModel with the given name.
 *
 * \param name
 */
QDjangoMetaModel QDjango::metaModel(const QString &name)
{
    return globalMetaModels.value(name);
}

void QDjango::registerModel(const QObject *model)
{
    const QString name = model->metaObject()->className();
    if (!globalMetaModels.contains(name))
        globalMetaModels.insert(name, QDjangoMetaModel(model));
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

QDjangoMetaField::QDjangoMetaField()
    : autoIncrement(false),
    index(false),
    maxLength(0),
    primaryKey(false)
{
}

/** Constructs a new QDjangoMetaModel by inspecting the given model instance.
 *
 * \param model
 */
QDjangoMetaModel::QDjangoMetaModel(const QObject *model)
{
    if (!model)
        return;

    const QMetaObject* meta = model->metaObject();
    m_table = QString(meta->className()).toLower().toLatin1();

    const int count = meta->propertyCount();
    for(int i = meta->propertyOffset(); i < count; ++i)
    {
        QString typeName = meta->property(i).typeName();

        // foreign field
        if (typeName.endsWith("*"))
        {
            const QByteArray fkName = meta->property(i).name();
            const QString fkModel = typeName.left(typeName.size() - 1);
            m_foreignFields.insert(fkName, fkModel);

            QDjangoMetaField field;
            field.name = fkName + "_id";
            // FIXME : the key is not necessarily an INTEGER field, we should
            // probably perform a lookup on the foreign model, but are we sure
            // it is already registered?
            field.type = QVariant::Int;
            field.foreignModel = fkModel;
            field.index = true;
            m_localFields << field;
            continue;
        }

        // local field
        QDjangoMetaField field;
        field.name = meta->property(i).name();
        field.type = meta->property(i).type();

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
                    else if (key == "primary_key" && value.toInt() == 1)
                    {
                        field.index = true;
                        field.primaryKey = true;

                        m_primaryKey = field.name;
                    }
                }
            }
        }

        m_localFields << field;
    }

    // automatic primary key
    if (m_primaryKey.isEmpty())
    {
        QDjangoMetaField field;
        field.name = "id";
        field.type = QVariant::Int;
        field.autoIncrement = true;
        field.index = true;
        field.primaryKey = true;
        m_localFields.prepend(field);
        m_primaryKey = field.name;
    }
 
}

/** Creates the database table for this QDjangoMetaModel.
 */
bool QDjangoMetaModel::createTable() const
{
    QSqlDatabase db = QDjango::database();

    QStringList propSql;
    foreach (const QDjangoMetaField &field, m_localFields)
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
        if (!field.foreignModel.isEmpty())
        {
            const QDjangoMetaModel foreignMeta = QDjango::metaModel(field.foreignModel);
            fieldSql += QString(" REFERENCES %1 (%2)").arg(
                QDjango::quote(foreignMeta.m_table), QDjango::quote(foreignMeta.m_primaryKey));
        }
        propSql << fieldSql;
    }

    // create table
    QSqlQuery createQuery(db);
    createQuery.prepare(QString("CREATE TABLE %1 (%2)").arg(QDjango::quote(m_table), propSql.join(", ")));
    if (!sqlExec(createQuery))
        return false;

    // create indices
    foreach (const QDjangoMetaField &field, m_localFields)
    {
        if (field.index)
        {
            const QByteArray indexName = m_table + "_" + field.name;
            QSqlQuery indexQuery(db);
            indexQuery.prepare(QString("CREATE %1 %2 ON %3 (%4)").arg(
                field.primaryKey ? "UNIQUE INDEX" : "INDEX",
                QDjango::quote(indexName),
                QDjango::quote(m_table),
                QDjango::quote(field.name)));
            if (!sqlExec(indexQuery))
                return false;
        }
    }

    return true;
}

/** Returns the quoted database column name for the given field.
 *
 *  The "pk" field will be resolved to the actual primary key name.
 *
 * \param name
 * \param needsJoin
 */
QString QDjangoMetaModel::databaseColumn(const QString &name, bool *needsJoin) const
{
    // foreign key lookup
    if (name.count("__"))
    {
        QStringList bits = name.split("__");
        const QByteArray fk = bits.takeFirst().toLatin1();
        if (m_foreignFields.contains(fk))
        {
            const QDjangoMetaModel foreignMeta = QDjango::metaModel(m_foreignFields[fk]);
            if (needsJoin)
                *needsJoin = true;
            return foreignMeta.databaseColumn(bits.join("__"));
        }
    }

    QString realName = (name == "pk") ? m_primaryKey : name;
    return QDjango::quote(m_table) + "." + QDjango::quote(realName);
}

/** Drops the database table for this QDjangoMetaModel.
 */
bool QDjangoMetaModel::dropTable() const
{
    QSqlQuery query(QDjango::database());
    query.prepare(QString("DROP TABLE %1").arg(QDjango::quote(m_table)));
    return sqlExec(query);
}

/** Retrieves the QDjangoModel pointed to by the given foreign-key.
 *
 * \param model
 * \param name
 */
QObject *QDjangoMetaModel::foreignKey(const QObject *model, const QByteArray &name) const
{
    QObject *foreign = model->property(name + "_ptr").value<QObject*>();
    if (!foreign)
        return 0;

    // if the foreign object was not loaded yet, do it now
    const QString foreignClass = m_foreignFields[name];
    const QDjangoMetaModel foreignMeta = QDjango::metaModel(foreignClass);
    const QVariant foreignPk = model->property(name + "_id");
    if (foreign->property(foreignMeta.primaryKey()) != foreignPk)
    {
        QDjangoQueryBase qs(foreignClass);
        qs.addFilter(QDjangoWhere("pk", QDjangoWhere::Equals, foreignPk));
        qs.sqlFetch();
        if (qs.m_properties.size() != 1 || !qs.sqlLoad(foreign, 0))
            return 0;
    }
    return foreign;
}

/** Sets the QDjangoModel pointed to by the given foreign-key.
 *
 * \param model
 * \param name
 * \param foreign
 */
void QDjangoMetaModel::setForeignKey(QObject *model, const QByteArray &name, QObject *foreign) const
{
    QObject *old = model->property(name + "_ptr").value<QObject*>();
    if (old == foreign)
        return;
    if (old)
        delete old;

    // store the new pointer and update the foreign key
    model->setProperty(name + "_ptr", qVariantFromValue(foreign));
    if (foreign)
    {
        const QDjangoMetaModel foreignMeta = QDjango::metaModel(m_foreignFields[name]);
        model->setProperty(name + "_id", foreign->property(foreignMeta.primaryKey()));
        foreign->setParent(model);
    }
}


/** Returns true if the current QDjangoMetaModel is valid, false otherwise.
 */
bool QDjangoMetaModel::isValid() const
{
    return !m_table.isEmpty() && !m_primaryKey.isEmpty();
}

/** Loads the given properties into a model instance.
 *
 * \param model
 * \param properties
 */
void QDjangoMetaModel::load(QObject *model, const QMap<QString, QVariant> &properties) const
{
    // process local fields
    foreach (const QDjangoMetaField &field, m_localFields)
    {
        const QString key = databaseColumn(field.name);
        model->setProperty(field.name, properties.value(key));
    }

    // process foreign fields
    foreach (const QByteArray &fkName, m_foreignFields.keys())
    {
        QObject *object = model->property(fkName + "_ptr").value<QObject*>();
        if (object)
        {
            const QDjangoMetaModel foreignMeta = QDjango::metaModel(m_foreignFields[fkName]);
            foreignMeta.load(object, properties);
        }
    }
}

/** Returns the name of the primary key for the current QDjangoMetaModel.
 */
QByteArray QDjangoMetaModel::primaryKey() const
{
    return m_primaryKey;
}

/** Removes the given QObject from the database.
 *
 * \param model
 */
bool QDjangoMetaModel::remove(QObject *model) const
{
    QSqlQuery query(QDjango::database());
    query.prepare(QString("DELETE FROM %1 WHERE %2 = ?")
                  .arg(QDjango::quote(m_table), QDjango::quote(m_primaryKey)));
    query.addBindValue(model->property(m_primaryKey));
    return sqlExec(query);
}

/** Saves the given QObject to the database.
 *
 * \param model
 *
 * \return true if saving succeeded, false otherwise
 */
bool QDjangoMetaModel::save(QObject *model) const
{
    QSqlDatabase db = QDjango::database();

    QStringList fieldNames;
    QDjangoMetaField primaryKey;
    foreach (const QDjangoMetaField &field, m_localFields)
    {
        if (field.primaryKey == true)
            primaryKey = field;
        fieldNames << field.name;
    }

    const QVariant pk = model->property(primaryKey.name);
    if (!pk.isNull() && !(primaryKey.type == QVariant::Int && !pk.toInt()))
    {
        QSqlQuery query(db);
        query.prepare(QString("SELECT 1 AS a FROM %1 WHERE %2 = ?")
                      .arg(QDjango::quote(m_table), QDjango::quote(primaryKey.name)));
        query.addBindValue(pk);
        if (sqlExec(query) && query.next())
        {
            // remove primary key
            fieldNames.removeAll(primaryKey.name);

            // perform update
            QStringList fieldAssign;
            foreach (const QString &name, fieldNames)
                fieldAssign << QDjango::quote(name) + " = ?";

            QSqlQuery query(db);
            query.prepare(QString("UPDATE %1 SET %2 WHERE %3 = ?")
                  .arg(QDjango::quote(m_table), fieldAssign.join(", "), primaryKey.name));
            foreach (const QString &name, fieldNames)
                query.addBindValue(model->property(name.toLatin1()));
            query.addBindValue(pk);
            return sqlExec(query);
        }
    }

    // remove auto-increment column
    if (primaryKey.autoIncrement)
        fieldNames.removeAll(primaryKey.name);

    // perform insert
    QStringList fieldColumns;
    QStringList fieldHolders;
    foreach (const QString &name, fieldNames)
    {
        fieldColumns << QDjango::quote(name);
        fieldHolders << "?";
    }

    QSqlQuery query(db);
    query.prepare(QString("INSERT INTO %1 (%2) VALUES(%3)")
                  .arg(QDjango::quote(m_table), fieldColumns.join(", "), fieldHolders.join(", ")));
    foreach (const QString &name, fieldNames)
        query.addBindValue(model->property(name.toLatin1()));

    bool ret = sqlExec(query);
    if (primaryKey.autoIncrement)
        model->setProperty(primaryKey.name, query.lastInsertId());
    return ret;
}


