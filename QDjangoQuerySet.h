/*
 * QDjango
 * Copyright (C) 2010-2011 Bolloré telecom
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

#ifndef QDJANGO_QUERYSET_H
#define QDJANGO_QUERYSET_H

#include "QDjango.h"
#include "QDjangoWhere.h"
#include "QDjangoQuerySet_p.h"

/** \brief The QDjangoQuerySet class is a template class for performing
 *   database queries.
 *
 *  The QDjangoQuerySet template class allows you to define and manipulate
 *  sets of QDjangoModel objects stored in the database.
 *
 *  You can chain filter expressions using the filter() and exclude() methods
 *  or apply limits on the number of rows using the limit() method.
 *
 *  You can retrieve database values using the values() and valuesList()
 *  methods or retrieve model instances using the get() and at() methods.
 *
 *  You can also delete sets of objects using the remove() method.
 *
 *  Behinds the scenes, the QDjangoQuerySet class uses implicit sharing to
 *  reduce memory usage and avoid needless copying of data.
 *
 * \ingroup Database
 */
template <class T>
    class QDjangoQuerySet
{
public:
    QDjangoQuerySet();
    QDjangoQuerySet(const QDjangoQuerySet<T> &other);
    ~QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet exclude(const QDjangoWhere &where) const;
    QDjangoQuerySet filter(const QDjangoWhere &where) const;
    QDjangoQuerySet limit(int pos, int length = -1) const;
    QDjangoQuerySet none() const;
    QDjangoQuerySet orderBy(const QStringList &keys) const;
    QDjangoQuerySet selectRelated() const;

    int count() const;
    QDjangoWhere where() const;

    bool remove();
    int size();
    QList< QMap<QString, QVariant> > values(const QStringList &fields = QStringList());
    QList< QList<QVariant> > valuesList(const QStringList &fields = QStringList());

    T *get(const QDjangoWhere &where, T *target = 0) const;
    T *at(int index, T *target = 0);

    QDjangoQuerySet<T> &operator=(const QDjangoQuerySet<T> &other);

private:
    QDjangoQueryBase *d;
};

/** Constructs a new queryset.
 */
template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet()
{
    d = new QDjangoQueryBase(T::staticMetaObject.className());
}

/** Constructs a copy of \a other.
 *
 * \param other
 */
template <class T>
QDjangoQuerySet<T>::QDjangoQuerySet(const QDjangoQuerySet<T> &other)
{
    other.d->counter.ref();
    d = other.d;
}

/** Destroys the queryset.
 */
template <class T>
QDjangoQuerySet<T>::~QDjangoQuerySet()
{
    if (!d->counter.deref())
        delete d;
}

/** Returns the object in the QDjangoQuerySet at the given index.
 *
 *  Returns 0 if the index is out of bounds.
 *
 *  If target is 0, a new object instance will be allocated which
 *  you must free yourself.
 *
 * \param index
 * \param target optional existing model instance.
 */
template <class T>
T *QDjangoQuerySet<T>::at(int index, T *target)
{
    T *entry = target ? target : new T;
    if (!d->sqlLoad(entry, index))
    {
        if (!target)
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
    other.d->lowMark = d->lowMark;
    other.d->highMark = d->highMark;
    other.d->needsJoin = d->needsJoin;
    other.d->orderBy = d->orderBy;
    other.d->selectRelated = d->selectRelated;
    other.d->whereClause = d->whereClause;
    return other;
}

/** Counts the number of objects in the queryset using an SQL COUNT query,
 *  or -1 if the query failed.
 *
 *  If you intend to iterate over the results, you should consider using
 *  size() instead.
 *
 * \note If the QDjangoQuerySet is already fully fetched, this simply returns
 *  the number of objects.
 */
template <class T>
int QDjangoQuerySet<T>::count() const
{
    if (d->hasResults)
        return d->properties.size();
    return d->sqlCount();
}

/** Returns a new QDjangoQuerySet containing objects for which the given key
 *  where condition is false.
 *
 *  You can chain calls to filter() and exclude() to further refine the
 *  filtering conditions.
 *
 * \param where QDjangoWhere expressing the exclude condition
 *
 * \sa filter()
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::exclude(const QDjangoWhere &where) const
{
    QDjangoQuerySet<T> other = all();
    other.d->addFilter(!where);
    return other;
}

/** Returns a new QDjangoQuerySet containing objects for which the given
 *  where condition is true.
 *
 *  You can chain calls to filter() and exclude() to progressively refine
 *  your filtering conditions.
 *
 * \param where QDjangoWhere expressing the filter condition
 *
 * \sa exclude()
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::filter(const QDjangoWhere &where) const
{
    QDjangoQuerySet<T> other = all();
    other.d->addFilter(where);
    return other;
}

/** Returns the object in the QDjangoQuerySet for which the given
 *  where condition is true.
 *
 *  Returns 0 if the number of matching object is not exactly one.
 *
 *  If target is 0, a new object instance will be allocated which
 *  you must free yourself.
 *
 * \param where QDjangoWhere expressing the lookup condition
 * \param target optional existing model instance.
 */
template <class T>
T *QDjangoQuerySet<T>::get(const QDjangoWhere &where, T *target) const
{
    QDjangoQuerySet<T> qs = filter(where);
    return qs.size() == 1 ? qs.at(0, target) : 0;
}

/** Returns a new QDjangoQuerySet containing limiting the number of
 *  records to manipulate.
 *
 *  You can chain calls to limit() to further restrict the number
 *  of returned records.
 *
 *  However, you cannot apply additional restrictions using filter(),
 *  exclude(), get(), orderBy() or remove() on the returned QDjangoQuerySet.
 *
 * \param pos offset of the records
 * \param length maximum number of records
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::limit(int pos, int length) const
{
    Q_ASSERT(pos >= 0);
    Q_ASSERT(length >= -1);

    QDjangoQuerySet<T> other = all();
    other.d->lowMark += pos;
    if (length > 0)
    {
        // calculate new high mark
        other.d->highMark = other.d->lowMark + length;
        // never exceed the current high mark
        if (d->highMark > 0 && other.d->highMark > d->highMark)
            other.d->highMark = d->highMark;
    }
    return other;
}

/** Returns an empty QDjangoQuerySet.
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::none() const
{
    QDjangoQuerySet<T> other;
    other.d->whereClause = !QDjangoWhere();
    return other;
}

/** Returns a QDjangoQuerySet whose elements are ordered using the given keys.
 *
 *  By default the elements will by in ascending order. You can prefix the key
 *  names with a "-" (minus sign) to use descending order.
 *
 * \param keys
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::orderBy(const QStringList &keys) const
{
    // it is not possible to change ordering once a limit has been set
    Q_ASSERT(!d->lowMark && !d->highMark);

    QDjangoQuerySet<T> other = all();
    other.d->orderBy << keys;
    return other;
}

/** Deletes all objects in the QDjangoQuerySet.
 *
 * \return true if deletion succeeded, false otherwise
 */
template <class T>
bool QDjangoQuerySet<T>::remove()
{
    return d->sqlDelete();
}

/** Returns a QDjangoQuerySet that will automatically "follow" foreign-key
 *  relationships, selecting that additional related-object data when it
 *  executes its query.
 */
template <class T>
QDjangoQuerySet<T> QDjangoQuerySet<T>::selectRelated() const
{
    QDjangoQuerySet<T> other = all();
    other.d->selectRelated = true;
    return other;
}

/** Returns the number of objects in the QDjangoQuerySet, or -1
 *  if the query failed.
 *
 *  If you do not plan to access the objects, you should consider using
 *  count() instead.
 */
template <class T>
int QDjangoQuerySet<T>::size()
{
    if (!d->sqlFetch())
        return -1;
    return d->properties.size();
}

/** Returns a list of property hashes for the current QDjangoQuerySet.
 *  If no \a fields are specified, all the model's declared fields are returned.
 *
 * \param fields
 */
template <class T>
QList< QMap<QString, QVariant> > QDjangoQuerySet<T>::values(const QStringList &fields)
{
    return d->sqlValues(fields);
}

/** Returns a list of property lists for the current QDjangoQuerySet.
 *  If no \a fields are specified, all the model's fields are returned in the
 *  order they where declared.
 *
 * \param fields
 */
template <class T>
QList< QList<QVariant> > QDjangoQuerySet<T>::valuesList(const QStringList &fields)
{
    return d->sqlValuesList(fields);
}

/** Returns the QDjangoWhere expressing the WHERE clause of the
 * QDjangoQuerySet.
 */
template <class T>
QDjangoWhere QDjangoQuerySet<T>::where() const
{
    return d->whereClause;
}

/** Assigns the specified queryset to this object.
 *
 * \param other
 */
template <class T>
QDjangoQuerySet<T> &QDjangoQuerySet<T>::operator=(const QDjangoQuerySet<T> &other)
{
    other.d->counter.ref();
    if (!d->counter.deref())
        delete d;
    d = other.d;
    return *this;
}

#endif
