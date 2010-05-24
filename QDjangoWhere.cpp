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

#include <QSqlQuery>
#include <QStringList>

#include "QDjango.h"
#include "QDjangoModel.h"
#include "QDjangoWhere.h"

QDjangoWhere::QDjangoWhere()
    :  m_operation(None), m_combine(NoCombine), m_negate(false)
{
}

/** Construct a QDjangoWhere expressing a test on a database column.
 *
 * @param key
 * @param operation
 * @param value
 */
QDjangoWhere::QDjangoWhere(const QString &key, QDjangoWhere::Operation operation, QVariant value)
    :  m_key(key), m_operation(operation), m_data(value), m_combine(NoCombine)
{
    QStringList bits;
    foreach (const QString &bit, m_key.split('.'))
        bits << QDjango::unquote(bit);
    m_placeholder = ":" + bits.join("_");
}

/** Negate the current QDjangoWhere.
 */
QDjangoWhere QDjangoWhere::operator!() const
{
    QDjangoWhere result;
    result.m_key = m_key;
    result.m_placeholder = m_placeholder;
    result.m_data = m_data;
    result.m_combine = m_combine;
    if (m_children.isEmpty())
    {
        switch (m_operation)
        {
        case None:
            result.m_operation = None;
            break;
        case Equals:
            result.m_operation = NotEquals;
            break;
        case NotEquals:
            result.m_operation = Equals;
            break;
        }
        result.m_negate = m_negate;
    } else {
        result.m_children = m_children;
        result.m_operation = m_operation;
        result.m_negate = !m_negate;
    }
    
    return result;
}

/** Combines the current QDjangoWhere with another QDjangoWhere using a logical AND.
 */
QDjangoWhere QDjangoWhere::operator&&(const QDjangoWhere &other) const
{
    QDjangoWhere result;
    result.m_combine = AndCombine;
    result.m_children << *this << other;
    return result;
}

/** Combines the current QDjangoWhere with another QDjangoWhere using a logical OR.
 */
QDjangoWhere QDjangoWhere::operator||(const QDjangoWhere &other) const
{
    QDjangoWhere result;
    result.m_combine = OrCombine;
    result.m_children << *this << other;
    return result;
}

/** Bind the values associated with this QDjangoWhere to an SQL query.
 */
void QDjangoWhere::bindValues(QSqlQuery &query) const
{
    if (m_operation != QDjangoWhere::None)
        query.bindValue(m_placeholder, m_data);
    else
        foreach (const QDjangoWhere &child, m_children)
            child.bindValues(query);
}

/** Returns true if the current QDjangoWhere does not express any constraint.
 */
bool QDjangoWhere::isEmpty() const
{
    return m_combine == NoCombine && m_operation == None;
}

/** Returns the SQL code corresponding for the current QDjangoWhere.
 */
QString QDjangoWhere::sql() const
{
    if (m_operation == Equals)
        return m_key + " = " + m_placeholder;
    else if (m_operation == NotEquals)
        return m_key + " != " + m_placeholder;
    else if (m_combine != NoCombine)
    {
        QStringList bits;
        foreach (const QDjangoWhere &child, m_children)
            bits << child.sql();
        QString combined;
        if (m_combine == AndCombine)
            combined = bits.join(" AND ");
        else if (m_combine == OrCombine)
            combined = bits.join(" OR ");
        if (m_negate)
            combined = QString("NOT (%1)").arg(combined);
        return combined;
    }
    return "";
}

