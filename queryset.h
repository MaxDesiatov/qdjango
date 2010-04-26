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
#include "where.h"

template <class T>
    class QDjangoQuerySet
{
public:
    QDjangoQuerySet(const QDjangoModel *model = 0);
    ~QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet exclude(const QString &key, const QVariant &value) const;
    QDjangoQuerySet filter(const QString &key, const QVariant &value) const;
    T *get(const QString &key, const QVariant &value) const;
    T *at(int index);
    int size();
    QDjangoWhere where() const { return m_where; };

private:
    void sqlFetch(); 

    const QDjangoModel *m_model;
    QDjangoWhere m_where;
    bool m_haveResults;
    QList< QMap<QString, QVariant> > m_properties;
};

template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet(const QDjangoModel *model)
    : m_model(model), m_haveResults(false)
{
    if (!model)
        m_model = QDjango::model(T::staticMetaObject.className());
}

template <class T>
QDjangoQuerySet<T>::~QDjangoQuerySet()
{
}

template <class T>
T *QDjangoQuerySet<T>::at(int index)
{
    sqlFetch();

    QMap<QString, QVariant> props = m_properties.at(index);
    T *entry = new T;
    QString pkField = entry->databasePkName();
    foreach (const QString &key, props.keys())
    {
        if (key == pkField)
            entry->setPk(props[key]);
        else
            entry->setProperty(key.toLatin1(), props[key]);
    }
    return entry;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::all() const
{
    QDjangoQuerySet<T> other(m_model);
    other.m_where = m_where;
    return other;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::exclude(const QString &key, const QVariant &value) const
{
    QDjangoWhere q(key, QDjangoWhere::NotEquals, value);
    QDjangoQuerySet<T> other(m_model);
    if (m_where.isEmpty())
        other.m_where = q;
    else
        other.m_where = m_where && q;
    return other;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QString &key, const QVariant &value) const
{
    QDjangoWhere q(key, QDjangoWhere::Equals, value);
    QDjangoQuerySet<T> other(m_model);
    if (m_where.isEmpty())
        other.m_where = q;
    else
        other.m_where = m_where && q;
    return other;
}

template <class T>
T *QDjangoQuerySet<T>::get(const QString &key, const QVariant &value) const
{
    QDjangoQuerySet<T> qs = filter(key, value);
    return qs.size() == 1 ? qs.at(0) : 0;
}

template <class T>
int QDjangoQuerySet<T>::size()
{
    sqlFetch();
    return m_properties.size();
}

template <class T>
void QDjangoQuerySet<T>::sqlFetch()
{
    if (m_haveResults)
        return;

    // build query
    QStringList fields = m_model->databaseFields();
    QString sql = "SELECT " + fields.join(", ") + " FROM " + m_model->databaseTable();
    if (!m_where.isEmpty())
        sql += " WHERE " + m_where.sql();
    QSqlQuery query(sql, QDjangoModel::database());
    if (!m_where.isEmpty())
        m_where.bindValues(query);

    // store results
    if (sqlExec(query))
    {
        while (query.next())
        {
            QMap<QString, QVariant> props;
            for (int i = 0; i < fields.size(); ++i)
                props.insert(fields[i], query.value(i));
            m_properties.append(props);
        }
    }
    m_haveResults = true;
}

#endif
