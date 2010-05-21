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

#include "QDjango.h"
#include "QDjangoModel.h"
#include "QDjangoQuerySet.h"

QDjangoQueryBase::QDjangoQueryBase(const QString &modelName)
    : m_haveResults(false), m_modelName(modelName)
{
}

QStringList QDjangoQueryBase::fieldNames(const QDjangoModel *model, QString &from)
{
    QStringList fields;
    foreach (const QString &field, model->databaseFields())
        fields.append(model->databaseColumn(field));
    if (!m_selectRelated)
        return fields;

    // recurse for foreign keys
    foreach (const QString &fk, model->m_foreignModels.keys())
    {
        const QDjangoModel *foreign = model->m_foreignModels[fk];
        fields += fieldNames(foreign, from);
        from += QString(" INNER JOIN %1 ON %2 = %3")
            .arg(QDjango::quote(foreign->databaseTable()))
            .arg(foreign->databaseColumn(foreign->m_pkName))
            .arg(model->databaseColumn(fk));
    }
    return fields;
}

void QDjangoQueryBase::addFilter(const QString &key, QDjangoWhere::Operation op, const QVariant &value)
{
    const QDjangoModel *model = QDjango::model(m_modelName);
    QDjangoWhere q(model->databaseColumn(key), op, value);
    if (m_where.isEmpty())
        m_where = q;
    else
        m_where = m_where && q;
}

void QDjangoQueryBase::sqlDelete()
{
    // delete entries
    const QDjangoModel *model = QDjango::model(m_modelName);
    QString from = QDjango::quote(model->databaseTable());
    QString sql = "DELETE FROM " + from;
    if (!m_where.isEmpty())
        sql += " WHERE " + m_where.sql();
    QSqlQuery query(sql, QDjangoModel::database());
    if (!m_where.isEmpty())
        m_where.bindValues(query);
    sqlExec(query);

    // invalidate cache
    if (m_haveResults)
    {
        m_properties.clear();
        m_haveResults = false;
    }
}

void QDjangoQueryBase::sqlFetch()
{
    if (m_haveResults)
        return;

    // build query
    const QDjangoModel *model = QDjango::model(m_modelName);
    QString from = QDjango::quote(model->databaseTable());
    QStringList fields = fieldNames(model, from);
    QString sql = "SELECT " + fields.join(", ") + " FROM " + from;
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

bool QDjangoQueryBase::sqlLoad(QDjangoModel *model, int index)
{
    sqlFetch();

    if (index < 0 | index >= m_properties.size())
    {
        qWarning("QDjangoQuerySet out of bounds");
        return false;
    }

    model->databaseLoad(m_properties.at(index));
    return true;
}

