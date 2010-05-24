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

#include "QDjangoWhere.h"
#include "QDjangoQuerySet_p.h"

template <class T>
    class QDjangoQuerySet : private QDjangoQueryBase
{
public:
    QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet exclude(const QDjangoWhere &where) const;
    QDjangoQuerySet exclude(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const;
    QDjangoQuerySet filter(const QDjangoWhere &where) const;
    QDjangoQuerySet filter(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const;
    void remove();
    QDjangoQuerySet selectRelated() const;
    int size();
    QDjangoWhere where() const;

    T *get(const QDjangoWhere &where) const;
    T *get(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const;
    T *at(int index);
};

template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet()
    : QDjangoQueryBase(T::staticMetaObject.className())
{
}

/** Returns the object in the QDjangoQuerySet at the given index.
 *  Returns 0 if the index is out of bounds.
 *
 *  You must free the newly allocated object yourself.
 *
 * @param index
 */
template <class T>
T *QDjangoQuerySet<T>::at(int index)
{
    T *entry = new T;
    if (!sqlLoad(entry, index))
    {
        delete entry;
        return 0;
    }
    return entry;
}

/** Returns a copy of the current QDjangoQuerySet.
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::all() const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    return other;
}

/** Returns a new QDjangoQuerySet containing objects for which the given key
 *  where condition is false.
 *
 * @where
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::exclude(const QDjangoWhere &where) const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    other.addFilter(!where);
    return other;
}

/** Returns a new QDjangoQuerySet containing objects for which the given key
 *  does not match the given value.
 *
 * @param key
 * @param op
 * @param value
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::exclude(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const
{
    return exclude(QDjangoWhere(key, op, value));
}

/** Returns a new QDjangoQuerySet containing objects for which the given
 *  where condition is true.
 *
 * @param where
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QDjangoWhere &where) const
{
    QDjangoQuerySet<T> other;
    other.m_selectRelated = m_selectRelated;
    other.m_where = m_where;
    other.addFilter(where);
    return other;
}

/** Returns a new QDjangoQuerySet containing objects for which the given key
 *  matches the given value.
 *
 * @param key
 * @param op
 * @param value
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const
{
    return filter(QDjangoWhere(key, op, value));
}

/** Returns the object in the QDjangoQuerySet for which the given
 *  where condition is true.
 *  Returns 0 if the number of matching object is not exactly one.
 *
 *  You must free the newly allocated object yourself.
 *
 * @param where
 */
template <class T>
T *QDjangoQuerySet<T>::get(const QDjangoWhere &where) const
{
    QDjangoQuerySet<T> qs = filter(where);
    return qs.size() == 1 ? qs.at(0) : 0;
}

/** Returns the object in the QDjangoQuerySet for which the given key matches
 *  the given value.
 *  Returns 0 if the number of matching object is not exactly one.
 *
 *  You must free the newly allocated object yourself.
 *
 * @param key
 * @param op
 * @param value
 */
template <class T>
T *QDjangoQuerySet<T>::get(const QString &key, QDjangoWhere::Operation op, const QVariant &value) const
{
    return get(QDjangoWhere(key, op, value));
}

/** Deletes all objects in the QDjangoQuerySet.
 */
template <class T>
void QDjangoQuerySet<T>::remove()
{
    sqlDelete();
}

/** Returns a QDjangoQuerySet that will automatically "follow" foreign-key
 *  relationships, selecting that additional related-object data when it
 *  executes its query.
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::selectRelated() const
{
    QDjangoQuerySet<T> other;
    other.m_where = m_where;
    other.m_selectRelated = true;
    return other;
}

/** Returns the number or objects in the QDjangoQuerySet.
 */
template <class T>
int QDjangoQuerySet<T>::size()
{
    sqlFetch();
    return m_properties.size();
}

/** Return the QDjangoWhere expressing the WHERE clause of the
 * QDjangoQuerySet.
 */
template <class T>
QDjangoWhere QDjangoQuerySet<T>::where() const
{
    return m_where;
}

#endif
