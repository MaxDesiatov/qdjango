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

class QDjangoQueryBase
{
public:
    QDjangoQueryBase(const QString &modelName);
    int size();
    QDjangoWhere where() const { return m_where; };

protected:
    void addFilter(const QString &key, QDjangoWhere::Operation op, const QVariant &value);
    void sqlFetch();

    QDjangoWhere m_where;
    QList< QMap<QString, QVariant> > m_properties;
    bool m_selectRelated;

private:
    QStringList fieldNames(const QDjangoModel *model, QString &from, int depth);

    bool m_haveResults;
    QString m_modelName;
};

template <class T>
    class QDjangoQuerySet : public QDjangoQueryBase
{
public:
    QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet exclude(const QString &key, const QVariant &value) const;
    QDjangoQuerySet filter(const QString &key, const QVariant &value) const;
    QDjangoQuerySet selectRelated() const;
    T *get(const QString &key, const QVariant &value) const;
    T *at(int index);
};

template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet()
    : QDjangoQueryBase(T::staticMetaObject.className())
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
        QStringList bits = key.split(".");
        const QString field = QDjango::unquote(bits[1]);
        if (field == pkField)
            entry->setPk(props[key]);
        else
            entry->setProperty(field.toLatin1(), props[key]);
    }
    return entry;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::all() const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    return other;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::exclude(const QString &key, const QVariant &value) const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    other.addFilter(key, QDjangoWhere::NotEquals, value);
    return other;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QString &key, const QVariant &value) const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    other.addFilter(key, QDjangoWhere::Equals, value);
    return other;
}

template <class T>
T *QDjangoQuerySet<T>::get(const QString &key, const QVariant &value) const
{
    QDjangoQuerySet<T> qs = filter(key, value);
    return qs.size() == 1 ? qs.at(0) : 0;
}

template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::selectRelated() const
{
    QDjangoQuerySet<T> other;
    other.m_where = m_where;
    other.m_selectRelated = true;
    return other;
}

#endif
