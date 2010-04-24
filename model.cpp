#include <QDebug>
#include <QMetaProperty>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "model.h"
#include "queryset.h"

static QSqlDatabase *db = 0;

QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent)
{
}

QVariant QDjangoModel::pk() const
{
    return property(databasePkName().toLatin1());
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
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
        const QString field = QString::fromLatin1(meta->property(i).name());
        if (meta->property(i).type() == QVariant::String)
        {
            propSql << field + " TEXT";
        } else {
            qWarning() << "Unhandled property type" << meta->property(i).typeName();
        }
    }

    QString sql = QString("CREATE TABLE %1 (%2)").arg(databaseTable(), propSql.join(", "));
    QSqlQuery createQuery(sql, *db);
    if (false && !createQuery.exec())
    {
        qWarning() << "Query failed" << sql << createQuery.lastError();
        return false;
    }

    // FIXME: make generic
    sql = QString("CREATE UNIQUE INDEX username ON %1 (username)").arg(databaseTable());
    QSqlQuery indexQuery(sql, *db);
    if (false && !indexQuery.exec())
    {
        qWarning() << "Query failed" << sql << indexQuery.lastError();
        return false;
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
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
        properties << QString::fromLatin1(meta->property(i).name());
    return properties;
}

QString QDjangoModel::databasePkName() const
{
    QStringList properties = databaseFields();
    if (properties.isEmpty())
        return "id";
    return properties.first();
}

QString QDjangoModel::databaseTable() const
{
    QString className(metaObject()->className());
    return className.toLower();
}

bool QDjangoModel::remove()
{
    const QString pkName = databasePkName();
    QString sql = QString("DELETE FROM %1 WHERE %2 = :pk")
                  .arg(databaseTable(), pkName);
    //qDebug() << "SQL" << sql;
    QSqlQuery query(sql, *db);
    query.bindValue(":pk", property(pkName.toLatin1()));
    return query.exec();
}

bool QDjangoModel::save()
{
    const QMetaObject* meta = metaObject();

    QStringList fieldNames = databaseFields();
    QStringList fieldHolders;
    foreach (const QString &name, fieldNames)
        fieldHolders << ":" + name;

    QString sql = QString("INSERT INTO %1 (%2) VALUES(%3)")
                  .arg(databaseTable(), fieldNames.join(", "), fieldHolders.join(", "));
    //qDebug() << "SQL" << sql;
    QSqlQuery query(sql, *db);
    foreach (const QString &name, fieldNames)
        query.bindValue(":" + name, property(name.toLatin1()));
    return query.exec();
}
