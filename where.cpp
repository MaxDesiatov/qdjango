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

#include "where.h"

QDjangoWhere::QDjangoWhere()
    :  m_operation(None), m_combine(NoCombine)
{
}

QDjangoWhere::QDjangoWhere(const QString &key, QDjangoWhere::Operation operation, QVariant data)
    :  m_key(key), m_operation(operation), m_data(data), m_combine(NoCombine)
{
}

QDjangoWhere QDjangoWhere::operator&&(const QDjangoWhere &other) const
{
    QDjangoWhere result;
    result.m_combine = AndCombine;
    result.m_children << *this << other;
    return result;
}

QDjangoWhere QDjangoWhere::operator||(const QDjangoWhere &other) const
{
    QDjangoWhere result;
    result.m_combine = OrCombine;
    result.m_children << *this << other;
    return result;
}

void QDjangoWhere::bindValues(QSqlQuery &query) const
{
    if (m_operation != QDjangoWhere::None)
        query.bindValue(":" + m_key, m_data);
    else
        foreach (const QDjangoWhere &child, m_children)
            child.bindValues(query);
}

bool QDjangoWhere::isEmpty() const
{
    return m_combine == NoCombine && m_operation == None;
}

QString QDjangoWhere::sql() const
{
    if (m_operation == Equals)
        return m_key + " = :" + m_key;
    else if (m_combine != NoCombine)
    {
        QStringList bits;
        foreach (const QDjangoWhere &child, m_children)
            bits << child.sql();
        if (m_combine == AndCombine)
            return bits.join(" AND ");
        else if (m_combine == OrCombine)
            return bits.join(" OR ");
    }
    return "";
}

