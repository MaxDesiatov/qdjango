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

#include <QSqlDriver>

#include "QDjango.h"
#include "QDjangoQuerySet.h"

QDjangoCompiler::QDjangoCompiler(const QString &modelName)
{
    database = QDjango::database();
    driver = database.driver();
    baseModel = QDjango::metaModel(modelName);
}

QString QDjangoCompiler::referenceModel(const QString &modelPath)
{
    if (modelPath.isEmpty())
        return driver->escapeIdentifier(baseModel.m_table, QSqlDriver::TableName);

    if (modelRefs.contains(modelPath))
        return modelRefs.value(modelPath);

    const QString modelRef = "T" + QString::number(modelRefs.size());
    modelRefs.insert(modelPath, modelRef);
    return modelRef;
}

QString QDjangoCompiler::databaseColumn(const QString &name)
{
    QDjangoMetaModel model = baseModel;
    QString modelPath;
    QStringList bits = name.split("__");
    while (bits.size() > 1) {
        const QByteArray fk = bits.first().toLatin1();
        if (!model.m_foreignFields.contains(fk))
            break;

        model = QDjango::metaModel(model.m_foreignFields[fk]);

        // store reference
        if (!modelPath.isEmpty())
            modelPath += "__";
        modelPath += bits.first();
        referenceModel(modelPath);

        bits.takeFirst();
    }

    QString fieldName = bits.join("__");
    if (fieldName == QLatin1String("pk"))
        fieldName = model.m_primaryKey;

    return referenceModel(modelPath) + "." + driver->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QStringList QDjangoCompiler::fieldNames(bool recurse, QDjangoMetaModel *metaModel, const QString &modelPath)
{
    QStringList fields;
    if (!metaModel)
        metaModel = &baseModel;

    // store reference
    const QString tableName = referenceModel(modelPath);
    foreach (const QDjangoMetaField &field, metaModel->m_localFields)
        fields << tableName + "." + driver->escapeIdentifier(field.name, QSqlDriver::FieldName);
    if (!recurse)
        return fields;

    // recurse for foreign keys
    const QString pathPrefix = modelPath.isEmpty() ? QString() : (modelPath + "__");
    foreach (const QByteArray &fkName, metaModel->m_foreignFields.keys()) {
        QDjangoMetaModel metaForeign = QDjango::metaModel(metaModel->m_foreignFields[fkName]);
        fields += fieldNames(recurse, &metaForeign, pathPrefix + fkName);
    }
    return fields;
}

void QDjangoCompiler::resolve(QDjangoWhere &where)
{
    // resolve column
    if (where.m_operation != QDjangoWhere::None)
        where.m_key = databaseColumn(where.m_key);

    // recurse into children
    for (int i = 0; i < where.m_children.size(); i++)
        resolve(where.m_children[i]);
}

QDjangoQuerySetPrivate::QDjangoQuerySetPrivate(const QString &modelName)
    : counter(1),
    hasResults(false),
    lowMark(0),
    highMark(0),
    selectRelated(false),
    m_modelName(modelName)
{
}

QStringList QDjangoQuerySetPrivate::fieldNames(const QSqlDatabase &db, const QDjangoMetaModel &metaModel, QString &from, bool &needsJoin) const
{
    QStringList fields;
    foreach (const QDjangoMetaField &field, metaModel.m_localFields)
        fields.append(metaModel.databaseColumn(db, field.name));
    if (!selectRelated && !needsJoin)
        return fields;

    // recurse for foreign keys
    foreach (const QByteArray &fkName, metaModel.m_foreignFields.keys())
    {
        QDjangoMetaModel metaForeign = QDjango::metaModel(metaModel.m_foreignFields[fkName]);
        from += QString(" INNER JOIN %1 ON %2 = %3")
            .arg(metaForeign.databaseTable(db))
            .arg(metaForeign.databaseColumn(db, "pk"))
            .arg(metaModel.databaseColumn(db, fkName + "_id"));
        if (selectRelated)
            fields += fieldNames(db, metaForeign, from, needsJoin);
    }
    return fields;
}

void QDjangoQuerySetPrivate::addFilter(const QDjangoWhere &where)
{
    // it is not possible to add filters once a limit has been set
    Q_ASSERT(!lowMark && !highMark);

    whereClause = whereClause && where;
}

void QDjangoQuerySetPrivate::resolve(QDjangoWhere &where, const QSqlDatabase &db, const QDjangoMetaModel &model, bool &needsJoin) const
{
    // resolve column
    if (where.m_operation != QDjangoWhere::None)
        where.m_key = model.databaseColumn(db, where.m_key, &needsJoin);

    // recurse into children
    for (int i = 0; i < where.m_children.size(); i++)
        resolve(where.m_children[i], db, model, needsJoin);
}

QDjangoWhere QDjangoQuerySetPrivate::resolvedWhere(const QSqlDatabase &db) const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    bool needsJoin = false;
    QDjangoWhere resolvedWhere(whereClause);
    resolve(resolvedWhere, db, metaModel, needsJoin);
    return resolvedWhere;
}

int QDjangoQuerySetPrivate::sqlCount() const
{
    QSqlDatabase db = QDjango::database();
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build query
    bool needsJoin = false;
    QDjangoWhere resolvedWhere(whereClause);
    resolve(resolvedWhere, db, metaModel, needsJoin);

    QString from = metaModel.databaseTable(db);
    fieldNames(db, metaModel, from, needsJoin);
    QString sql = "SELECT COUNT(*) FROM " + from;
    QString where = resolvedWhere.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit(db);
    QDjangoQuery query(db);
    query.prepare(sql);
    resolvedWhere.bindValues(query);

    // execute query
    if (!query.exec() || !query.next())
        return -1;
    return query.value(0).toInt();
}

bool QDjangoQuerySetPrivate::sqlDelete()
{
    // DELETE on an empty queryset doesn't need a query
    if (whereClause.isNone())
        return true;

    // FIXME : it is not possible to remove entries once a limit has been set
    // because SQLite does not support limits on DELETE unless compiled with the
    // SQLITE_ENABLE_UPDATE_DELETE_LIMIT option
    if (lowMark || highMark)
        return false;

    QSqlDatabase db = QDjango::database();
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build query
    bool needsJoin = false;
    QDjangoWhere resolvedWhere(whereClause);
    resolve(resolvedWhere, db, metaModel, needsJoin);

    QString from = metaModel.databaseTable(db);
    fieldNames(db, metaModel, from, needsJoin);
    QString sql = "DELETE FROM " + from;
    QString where = resolvedWhere.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit(db);
    QDjangoQuery query(db);
    query.prepare(sql);
    resolvedWhere.bindValues(query);

    // execute query
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

bool QDjangoQuerySetPrivate::sqlFetch()
{
    if (hasResults || whereClause.isNone())
        return true;

    QSqlDatabase db = QDjango::database();
    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build query
    bool needsJoin = false;
    QDjangoWhere resolvedWhere(whereClause);
    resolve(resolvedWhere, db, metaModel, needsJoin);

    QString from = metaModel.databaseTable(db);
    QStringList fields = fieldNames(db, metaModel, from, needsJoin);
    QString sql = "SELECT " + fields.join(", ") + " FROM " + from;
    QString where = resolvedWhere.sql();
    if (!where.isEmpty())
        sql += " WHERE " + where;
    sql += sqlLimit(db);
    QDjangoQuery query(db);
    query.prepare(sql);
    resolvedWhere.bindValues(query);

    // execute query
    if (!query.exec())
        return false;

    // store results
    while (query.next())
    {
        QVariantList props;
        for (int i = 0; i < fields.size(); ++i)
            props << query.value(i);
        properties.append(props);
    }
    hasResults = true;
    return true;
}

QString QDjangoQuerySetPrivate::sqlLimit(const QSqlDatabase &db) const
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
        bits.append(QString("%1 %2").arg(metaModel.databaseColumn(db, field), order));
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

bool QDjangoQuerySetPrivate::sqlLoad(QObject *model, int index)
{
    if (!sqlFetch())
        return false;

    if (index < 0 || index >= properties.size())
    {
        qWarning("QDjangoQuerySet out of bounds");
        return false;
    }

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);
    int pos = 0;
    metaModel.load(model, properties.at(index), pos);
    return true;
}

QList<QVariantMap> QDjangoQuerySetPrivate::sqlValues(const QStringList &fields)
{
    QList<QVariantMap> values;
    if (!sqlFetch())
        return values;

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build field list
    QMap<QString, int> fieldPos;
    if (fields.isEmpty()) {
        for (int i = 0; i < metaModel.m_localFields.size(); ++i)
            fieldPos.insert(QString::fromAscii(metaModel.m_localFields[i].name), i);
    } else {
        foreach (const QString &name, fields) {
            int pos = 0;
            foreach (const QDjangoMetaField &field, metaModel.m_localFields) {
                if (field.name == name)
                    break;
                pos++;
            }
            Q_ASSERT_X(pos < metaModel.m_localFields.size(), "QDjangoQuerySet<T>::values", "unknown field requested");
            fieldPos.insert(name, pos);
        }
    }

    // extract values
    foreach (const QVariantList &props, properties) {
        QVariantMap map;
        QMap<QString, int>::const_iterator i;
        for (i = fieldPos.constBegin(); i != fieldPos.constEnd(); ++i)
            map[i.key()] = props[i.value()];
        values.append(map);
    }
    return values;
}

QList<QVariantList> QDjangoQuerySetPrivate::sqlValuesList(const QStringList &fields)
{
    QList<QVariantList> values;
    if (!sqlFetch())
        return values;

    const QDjangoMetaModel metaModel = QDjango::metaModel(m_modelName);

    // build field list
    QList<int> fieldPos;
    if (fields.isEmpty()) {
        for (int i = 0; i < metaModel.m_localFields.size(); ++i)
            fieldPos << i;
    } else {
        foreach (const QString &name, fields) {
            int pos = 0;
            foreach (const QDjangoMetaField &field, metaModel.m_localFields) {
                if (field.name == name)
                    break;
                pos++;
            }
            Q_ASSERT_X(pos < metaModel.m_localFields.size(), "QDjangoQuerySet<T>::valuesList", "unknown field requested");
            fieldPos << pos;
        }
    }

    // extract values
    foreach (const QVariantList &props, properties) {
        QVariantList list;
        foreach (int pos, fieldPos)
            list << props.at(pos);
        values.append(list);
    }
    return values;
}

