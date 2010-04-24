#ifndef QDJANGO_QUERYSET_H
#define QDJANGO_QUERYSET_H

#include <QList>

#include "query.h"

template <class T>
    class QDjangoQuerySet
{
public:
    QDjangoQuerySet();
    ~QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet filter(const QString &key, const QVariant &value) const;
    T *at(int index);
    T *get(const QString &key, const QVariant &value);
    int size() const;

private:
    QSqlQuery sqlQuery(const QString &baseSql) const;

    QHash<QString, QVariant> m_filters;
    bool m_haveResults;
    QList<T*> m_results;
};

template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet()
    : m_haveResults(false)
{
}

template <class T>
QDjangoQuerySet<T>::~QDjangoQuerySet()
{
}

template <class T>
T *QDjangoQuerySet<T>::at(int index)
{
    if (!m_haveResults)
    {
        T model;
        QStringList fields = model.databaseFields();
        QSqlQuery query = sqlQuery("SELECT " + fields.join(", "));
        if (query.exec())
        {
            while (query.next())
            {
                T *entry = new T;
                for (int i = 0; i < fields.size(); ++i)
                    entry->setProperty(fields[i].toLatin1(), query.value(i));
                m_results.append(entry);
            }
        }
        m_haveResults = true;
    }

    return m_results.at(index);
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::all() const
{
    QDjangoQuerySet<T> other;
    other.m_filters = m_filters;
    return other;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QString &key, const QVariant &value) const
{
    QDjangoQuerySet<T> other;
    other.m_filters = m_filters;
    other.m_filters[key] = value;
    return other;
}

template <class T>
T *QDjangoQuerySet<T>::get(const QString &key, const QVariant &value)
{
    return filter(key, value).at(0);
}

template <class T>
int QDjangoQuerySet<T>::size() const
{
    if (m_haveResults)
        return m_results.size();

    QSqlQuery query = sqlQuery("SELECT COUNT(*)");
    if (!query.exec())
        return 0;
    query.next();
    return query.value(0).toInt();
}

template <class T>
QSqlQuery QDjangoQuerySet<T>::sqlQuery(const QString &baseSql) const
{
    T *model = new T;
    QString sql = baseSql + " FROM " + model->databaseTable();
    if (!m_filters.isEmpty())
    QString sql = baseSql;
    if (!m_filters.isEmpty())
    {
        sql += " WHERE ";
        QStringList bits;
        foreach (const QString &key, m_filters.keys())
            bits << key + " = :" + key;
        sql += bits.join(" AND ");
    }
    qDebug() << sql;
    QSqlQuery query(sql, model->database());
    foreach (const QString &key, m_filters.keys())
        query.bindValue(":" + key, m_filters[key]);
    delete model;
    return query;
}

#endif
