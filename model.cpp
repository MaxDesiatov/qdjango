#include <QDebug>
#include <QMetaProperty>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "model.h"
#include "queryset.h"

static QSqlDatabase *db = 0;

QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent), m_pkName("id")
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

    QString sql = QString("CREATE TABLE %1 (%2)").arg(databaseTable(), propSql.join(", "));
    qDebug() << "SQL" << sql;
    QSqlQuery createQuery(sql, *db);
    if (false && !createQuery.exec())
    {
        qWarning() << "Query failed" << sql << createQuery.lastError();
        return false;
    }

    // FIXME: make generic
    if (m_pkName != "id")
    {
        QString indexName = m_pkName;
        sql = QString("CREATE UNIQUE INDEX %1 ON %2 (%3)").arg(indexName, databaseTable(), m_pkName);
        qDebug() << "SQL" << sql;
        QSqlQuery indexQuery(sql, *db);
        if (false && !indexQuery.exec())
        {
            qWarning() << "Query failed" << sql << indexQuery.lastError();
            return false;
        }
    }
    return true;
}

bool QDjangoModel::dropTable() const
{
    QString sql = QString("DROP TABLE %1").arg(databaseTable());
    QSqlQuery query(sql, *db);
    if (!query.exec())
    {
        qWarning() << "Query failed" << query.lastQuery() << query.lastError();
        return false;
    }
    return true;
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
    qDebug() << "SQL" << sql;
    QSqlQuery query(sql, *db);
    query.bindValue(":pk", pk());
    return query.exec();
}

bool QDjangoModel::save()
{
    const QMetaObject* meta = metaObject();

    QStringList fieldNames = databaseFields();
    if (m_pkName == "id")
        fieldNames.removeAll(m_pkName);

    QStringList fieldHolders;
    foreach (const QString &name, fieldNames)
        fieldHolders << ":" + name;

    QString sql = QString("INSERT INTO %1 (%2) VALUES(%3)")
                  .arg(databaseTable(), fieldNames.join(", "), fieldHolders.join(", "));
    qDebug() << "SQL" << sql;
    QSqlQuery query(sql, *db);
    foreach (const QString &name, fieldNames)
        query.bindValue(":" + name, property(name.toLatin1()));

    bool ret = query.exec();
    if (m_pkName == "id")
        setPk(query.lastInsertId());
    return ret;
}
