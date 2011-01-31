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

#include "QDjango.h"
#include "QDjangoQuerySet.h"

QDjangoQueryBase::QDjangoQueryBase(const QString &modelName)
    : counter(1),
    hasResults(false),
    lowMark(0),
    highMark(0),
    needsJoin(false),
    selectRelated(false),
    m_modelName(modelName)
{
}

QStringList QDjangoQueryBase::fieldNames(const QDjangoMetaModel &metaModel, QString &from)
{
    QStringList fields;
    foreach (const QDjangoMetaField &field, metaModel.m_localFields)
        fields.append(metaModel.databaseColumn(field.name));
    if (!selectRelated && !needsJoin)
        return fields;

    // recurse for foreign keys
    foreach (const QByteArray &fkName, metaModel.m_foreignFields.keys())
    {
        QDjangoMetaModel metaForeign = QDjango::metaModel(metaModel.m_foreignFields[fkName]);
        from += QString(" INNER JOIN %1 ON %2 = %3")
            .arg(metaForeign.databaseTable())
            .arg(metaForeign.databaseColumn("pk"))
            .arg(metaModel.databaseColumn(fkName + "_id"));
        if (selectRelated)
            fields += fieldNames(metaForeign, from);
    }
    return fields;
}

void QDjangoQueryBase::addFilter(const QDjangoWhere &where)
{
    // it is not possible to add filters once a limit has been set
    Q_ASSERT(!lowMark && !highMark);

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QDjangoWhere q(where);
    q.resolve(metaModel, &needsJoin);
    whereClause = whereClause && q;
}

int QDjangoQueryBase::sqlCount() const
{
    // prepare query
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QString sql = "SELECT COUNT(*) FROM " + metaModel.databaseTable();
    QString where = whereClause.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit();
    QDjangoQuery query(QDjango::database());
    query.prepare(sql);
    whereClause.bindValues(query);

    // execute query
    if (!query.exec() || !query.next())
        return -1;
    return query.value(0).toInt();
}

bool QDjangoQueryBase::sqlDelete()
{
    // DELETE on an empty queryset doesn't need a query
    if (whereClause.isNone())
        return true;

    // FIXME : it is not possible to remove entries once a limit has been set
    // because SQLite does not support limits on DELETE unless compiled with the
    // SQLITE_ENABLE_UPDATE_DELETE_LIMIT option
    if (lowMark || highMark)
        return false;

    // delete entries
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QString from = metaModel.databaseTable();
    QString sql = "DELETE FROM " + from;
    QString where = whereClause.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit();
    QDjangoQuery query(QDjango::database());
    query.prepare(sql);
    whereClause.bindValues(query);
    if (!query.exec())
        return false;

    // invalidate cache
    if (hasResults)
    {
        properties.clear();
        hasResults = false;
    }
    return true;
}

bool QDjangoQueryBase::sqlFetch()
{
    if (hasResults || whereClause.isNone())
        return true;

    // build query
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QString from = metaModel.databaseTable();
    QStringList fields = fieldNames(metaModel, from);
    QString sql = "SELECT " + fields.join(", ") + " FROM " + from;
    QString where = whereClause.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit();
    QDjangoQuery query(QDjango::database());
    query.prepare(sql);
    whereClause.bindValues(query);

    // execute query
    if (!query.exec())
        return false;

    // store results
    while (query.next())
    {
        QVariantMap props;
        for (int i = 0; i < fields.size(); ++i)
            props.insert(fields[i], query.value(i));
        properties.append(props);
    }
    hasResults = true;
    return true;
}

QString QDjangoQueryBase::sqlLimit() const
{
    QString limit;

    // order
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QStringList bits;
    QString field;
    foreach (field, orderBy)
    {
        QString order = "ASC";
        if (field.startsWith("-"))
        {
            order = "DESC";
            field = field.mid(1);
        } else if (field.startsWith("+")) {
            field = field.mid(1);
        }
        bits.append(QString("%1 %2").arg(metaModel.databaseColumn(field), order));
    }
    if (!bits.isEmpty())
        limit += " ORDER BY " + bits.join(", ");

    // limits
    if (highMark > 0)
        limit += QString(" LIMIT %1").arg(highMark - lowMark);
    if (lowMark > 0)
    {
        // no-limit is backend specific
        if (highMark <= 0)
            limit += QDjango::noLimitSql();
        limit += QString(" OFFSET %1").arg(lowMark);
    }
    return limit;
}

bool QDjangoQueryBase::sqlLoad(QObject *model, int index)
{
    if (!sqlFetch())
        return false;

    if (index < 0 || index >= properties.size())
    {
        qWarning("QDjangoQuerySet out of bounds");
        return false;
    }

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    metaModel.load(model, properties.at(index));
    return true;
}

QList<QVariantMap> QDjangoQueryBase::sqlValues(const QStringList &fields)
{
    QList<QVariantMap> values;
    if (!sqlFetch())
        return values;

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build field list
    QStringList fieldNames;
    if (fields.isEmpty())
        foreach (const QDjangoMetaField &field, metaModel.m_localFields)
            fieldNames << field.name;
    else
        fieldNames = fields;

    foreach (const QVariantMap &props, properties)
    {
        QVariantMap map;
        foreach (const QString &field, fieldNames)
        {
            const QString key = metaModel.databaseColumn(field);
            map[field] = props[key];
        }
        values.append(map);
    }
    return values;
}

QList<QVariantList> QDjangoQueryBase::sqlValuesList(const QStringList &fields)
{
    QList<QVariantList> values;
    if (!sqlFetch())
        return values;

    // build field list
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    QStringList fieldNames;
    if (fields.isEmpty())
        foreach (const QDjangoMetaField &field, metaModel.m_localFields)
            fieldNames << field.name;
    else
        fieldNames = fields;

    foreach (const QVariantMap &props, properties)
    {
        QVariantList list;
        foreach (const QString &field, fieldNames)
        {
            const QString key = metaModel.databaseColumn(field);
            list << props.value(key);
        }
        values.append(list);
    }
    return values;
}

