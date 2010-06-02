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

#include <QSqlQuery>
#include <QStringList>

#include "QDjango.h"
#include "QDjangoModel.h"
#include "QDjangoWhere.h"

/** Constructs an empty QDjangoWhere, which expresses no constraint.
 */
QDjangoWhere::QDjangoWhere()
    :  m_operation(None), m_combine(NoCombine), m_negate(false)
{
}

/** Constructs a QDjangoWhere expressing a constraint on a database column.
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

/** Negates the current QDjangoWhere.
 */
QDjangoWhere QDjangoWhere::operator!() const
{
    QDjangoWhere result;
    result.m_key = m_key;
    result.m_placeholder = m_placeholder;
    result.m_data = m_data;
    result.m_combine = m_combine;
    result.m_negate = !m_negate;
    if (m_children.isEmpty())
    {
        switch (m_operation)
        {
        case None:
        case IsIn:
            result.m_operation = m_operation;
            break;
        case Equals:
            // simplify !(a = b) to a != b
            result.m_operation = NotEquals;
            result.m_negate = m_negate;
            break;
        case NotEquals:
            // simplify !(a != b) to a = b
            result.m_operation = Equals;
            result.m_negate = m_negate;
            break;
        case GreaterThan:
            // simplify !(a > b) to a <= b
            result.m_operation = LessOrEquals;
            result.m_negate = m_negate;
            break;
        case LessThan:
            // simplify !(a < b) to a >= b
            result.m_operation = GreaterOrEquals;
            result.m_negate = m_negate;
            break;
        case GreaterOrEquals:
            // simplify !(a >= b) to a < b
            result.m_operation = LessThan;
            result.m_negate = m_negate;
            break;
        case LessOrEquals:
            // simplify !(a <= b) to a > b
            result.m_operation = GreaterThan;
            result.m_negate = m_negate;
            break;
        }
    } else {
        result.m_children = m_children;
        result.m_operation = m_operation;
    }
    
    return result;
}

/** Combines the current QDjangoWhere with another QDjangoWhere using a logical AND.
 */
QDjangoWhere QDjangoWhere::operator&&(const QDjangoWhere &other) const
{
    if (isAll() || other.isNone())
        return other;
    else if (isNone() || other.isAll())
        return *this;

    QDjangoWhere result;
    result.m_combine = AndCombine;
    result.m_children << *this << other;
    return result;
}

/** Combines the current QDjangoWhere with another QDjangoWhere using a logical OR.
 */
QDjangoWhere QDjangoWhere::operator||(const QDjangoWhere &other) const
{
    if (isAll() || other.isNone())
        return *this;
    else if (isNone() || other.isAll())
        return other;

    QDjangoWhere result;
    result.m_combine = OrCombine;
    result.m_children << *this << other;
    return result;
}

/** Bind the values associated with this QDjangoWhere to an SQL query.
 */
void QDjangoWhere::bindValues(QSqlQuery &query) const
{
    if (m_operation == QDjangoWhere::IsIn)
    {
        const QList<QVariant> values = m_data.toList();
        for (int i = 0; i < values.size(); i++)
            query.bindValue(QString("%1_%2").arg(m_placeholder).arg(i), values[i]);
    }
    else if (m_operation != QDjangoWhere::None)
        query.bindValue(m_placeholder, m_data);
    else
        foreach (const QDjangoWhere &child, m_children)
            child.bindValues(query);
}

/** Returns true if the current QDjangoWhere does not express any constraint.
 */
bool QDjangoWhere::isAll() const
{
    return m_combine == NoCombine && m_operation == None && m_negate == false;
}

/** Returns true if the current QDjangoWhere expressed an impossible constraint.
 */
bool QDjangoWhere::isNone() const
{
    return m_combine == NoCombine && m_operation == None && m_negate == true;
}

/** Resolves field names to database columns using the given model.
 *
 * @param model
 */
bool QDjangoWhere::resolve(const QDjangoModel *model)
{
    // resolve column
    if (m_operation != None)
    {
        m_key = model->databaseColumn(m_key);
        QStringList bits;
        foreach (const QString &bit, m_key.split('.'))
            bits << QDjango::unquote(bit);
        m_placeholder = ":" + bits.join("_");
    }

    // recurse into children
    for (int i = 0; i < m_children.size(); i++)
        if (!m_children[i].resolve(model))
            return false;
    return true;
}

/** Returns the SQL code corresponding for the current QDjangoWhere.
 */
QString QDjangoWhere::sql() const
{
    switch (m_operation)
    {
        case Equals:
            return m_key + " = " + m_placeholder;
        case NotEquals:
            return m_key + " != " + m_placeholder;
        case GreaterThan:
            return m_key + " > " + m_placeholder;
        case LessThan:
            return m_key + " < " + m_placeholder;
        case GreaterOrEquals:
            return m_key + " >= " + m_placeholder;
        case LessOrEquals:
            return m_key + " <= " + m_placeholder;
        case IsIn:
        {
            QStringList bits;
            for (int i = 0; i < m_data.toList().size(); i++)
                bits << QString("%1_%2").arg(m_placeholder).arg(i);
            return m_key + " IN (" + bits.join(", ") + ")";
        }
        case None:
            if (m_combine == NoCombine)
            {
                return m_negate ? QString("1 != 0") : QString();
            } else {
                QStringList bits;
                foreach (const QDjangoWhere &child, m_children)
                {
                    QString atom = child.sql();
                    if (child.m_children.isEmpty())
                        bits << atom;
                    else
                        bits << QString("(%1)").arg(atom);
                }
                QString combined;
                if (m_combine == AndCombine)
                    combined = bits.join(" AND ");
                else if (m_combine == OrCombine)
                    combined = bits.join(" OR ");
                if (m_negate)
                    combined = QString("NOT (%1)").arg(combined);
                return combined;
            }
    }
    return QString();
}

