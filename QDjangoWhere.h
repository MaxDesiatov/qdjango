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

#ifndef QDJANGO_QUERY_H
#define QDJANGO_QUERY_H

#include <QVariant>

class QDjangoModel;
class QSqlQuery;

/** \brief The QDjangoWhere class expresses an SQL constraint.
 *
 * The QDjangoWhere class is used to build SQL WHERE statements. In its
 * simplest form a QDjangoWhere expresses a constraint on a column value.
 *
 * QDjangoWhere instances can be negated using the "!" unary operator
 * or combined using the "&&" and "||" boolean operators.
 */
class QDjangoWhere
{
public:
    /** A comparison operation on a database column value. */
    enum Operation
    {
        /** No comparison, always returns true. */
        None,
        /** Returns true if the column value is equal to the given value. */
        Equals,
        /** Returns true if the column value is not equal to the given value. */
        NotEquals,
        /** Returns true if the column value is greater than the given value. */
        GreaterThan,
        /** Returns true if the column value is less than the given value. */
        LessThan,
        /** Returns true if the column value is greater or equal to the given value. */
        GreaterOrEquals,
        /** Returns true if the column value is less or equal to the given value. */
        LessOrEquals,
        /** Returns true if the column value starts with the given value (strings only). */
        StartsWith,
        /** Returns true if the column value ends with the given value (strings only). */
        EndsWith,
        /** Returns true if the column value contains the given value (strings only). */
        Contains,
        /** Returns true if the column value is one of the given values. */
        IsIn,
    };

    QDjangoWhere();
    QDjangoWhere(const QString &key, QDjangoWhere::Operation operation, QVariant value);

    QDjangoWhere operator!() const;
    QDjangoWhere operator&&(const QDjangoWhere &other) const;
    QDjangoWhere operator||(const QDjangoWhere &other) const;

    void bindValues(QSqlQuery &query) const;
    bool isAll() const;
    bool isNone() const;
    bool resolve(const QDjangoModel *model, bool *needsJoin);
    QString sql() const;

private:
    enum Combine
    {
        NoCombine,
        AndCombine,
        OrCombine,
    };

    QString m_key;
    QDjangoWhere::Operation m_operation;
    QVariant m_data;

    QList<QDjangoWhere> m_children;
    QDjangoWhere::Combine m_combine;
    bool m_negate;
};

#endif
