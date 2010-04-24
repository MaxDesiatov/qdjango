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

#ifndef QDJANGO_QUERYSET_H
#define QDJANGO_QUERYSET_H

#include <QList>
#include <QStringList>
#include <QSqlQuery>

#include "model.h"
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
    int size();

private:
    void sqlFetch(); 
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
    sqlFetch();
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
    QDjangoQuerySet<T> qs = filter(key, value);
    return qs.size() == 1 ? qs.at(0) : 0;
}

template <class T>
int QDjangoQuerySet<T>::size()
{
    sqlFetch();
    return m_results.size();

    QSqlQuery query = sqlQuery("SELECT COUNT(*)");
    if (!query.exec())
        return 0;
    query.next();
    return query.value(0).toInt();
}

template <class T>
void QDjangoQuerySet<T>::sqlFetch()
{
    if (m_haveResults)
        return;

    T model;
    QStringList fields = model.databaseFields();
    QString pkField = model.databasePkName();
    QSqlQuery query = sqlQuery("SELECT " + fields.join(", "));
    if (query.exec())
    {
        while (query.next())
        {
            T *entry = new T;
            for (int i = 0; i < fields.size(); ++i)
            {
                if (fields[i] == pkField)
                    entry->setPk(query.value(i));
                else
                    entry->setProperty(fields[i].toLatin1(), query.value(i));
            }
            m_results.append(entry);
        }
    }
    m_haveResults = true;
}

template <class T>
QSqlQuery QDjangoQuerySet<T>::sqlQuery(const QString &baseSql) const
{
    T model;
    QString sql = baseSql + " FROM " + model.databaseTable();
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
    sqlDebug() << sql;
    QSqlQuery query(sql, model.database());
    foreach (const QString &key, m_filters.keys())
        query.bindValue(":" + key, m_filters[key]);
    return query;
}

#endif
