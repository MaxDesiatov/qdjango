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

#ifndef QDJANGO_QUERY_H
#define QDJANGO_QUERY_H

#include <QVariant>

class QSqlQuery;

/** QDjangoWhere expresses a constraint for a database operation.
 *
 * It is used to construct SQL WHERE statements.
 */
class QDjangoWhere
{
public:
    /** A comparison operation. */
    enum Operation
    {
        /** No comparison, always returns true. */
        None,
        /** Returns true if the two values are equal. */
        Equals,
        /** Returns true if the two values are not equal */
        NotEquals,
    };

    QDjangoWhere();
    QDjangoWhere(const QString &key, QDjangoWhere::Operation operation, QVariant value);

    QDjangoWhere operator!() const;
    QDjangoWhere operator&&(const QDjangoWhere &other) const;
    QDjangoWhere operator||(const QDjangoWhere &other) const;

    void bindValues(QSqlQuery &query) const;
    bool isEmpty() const;
    QString sql() const;

private:
    enum Combine
    {
        NoCombine,
        AndCombine,
        OrCombine,
    };

    QString m_key;
    QString m_placeholder;
    QDjangoWhere::Operation m_operation;
    QVariant m_data;

    QList<QDjangoWhere> m_children;
    QDjangoWhere::Combine m_combine;
    bool m_negate;
};

#endif
