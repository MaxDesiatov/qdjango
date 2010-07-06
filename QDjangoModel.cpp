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

#include <QDebug>
#include <QStringList>

#include "QDjango.h"
#include "QDjango_p.h"
#include "QDjangoModel.h"
#include "QDjangoQuerySet.h"

/** Construct a new QDjangoModel.
 *
 * \param parent
 */
QDjangoModel::QDjangoModel(QObject *parent)
    : QObject(parent)
{
}

/** Returns primary key for this QDjangoModel.
 */
QVariant QDjangoModel::pk() const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return property(metaModel.primaryKey());
}

/** Sets the primary key for this QDjangoModel.
 *
 * \param pk
 */
void QDjangoModel::setPk(const QVariant &pk)
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    setProperty(metaModel.primaryKey(), pk);
}

/** Creates the database table for this QDjangoModel.
 */
bool QDjangoModel::createTable() const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return metaModel.createTable();
}

/** Drops the database table for this QDjangoModel.
 */
bool QDjangoModel::dropTable() const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return metaModel.dropTable();
}

/** Declares a foreign-key pointing to a QDjangoModel.
 *
 * \param name
 * \param model
 */
void QDjangoModel::addForeignKey(const QString &name, QObject *model)
{
    model->setParent(this);
    setProperty(name.toLatin1() + "_ptr", qVariantFromValue(model));
}

/** Retrieves the QDjangoModel pointed to by the given foreign-key.
 *
 * \param name
 */
QObject *QDjangoModel::foreignKey(const QString &name) const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    const QString foreignClass = metaModel.foreignModel(name.toLatin1());
    const QDjangoMetaModel foreignMeta = QDjango::metaModel(foreignClass);

    // if the foreign object was not loaded yet, do it now
    QObject *foreign = property(name.toLatin1() + "_ptr").value<QObject*>();
    const QVariant foreignPk = property(name.toLatin1() + "_id");
    if (foreign->property(foreignMeta.primaryKey()) != foreignPk)
    {
        QDjangoQueryBase qs(foreignClass);
        qs.addFilter(QDjangoWhere("pk", QDjangoWhere::Equals, foreignPk));
        qs.sqlFetch();
        if (qs.m_properties.size() != 1 || !qs.sqlLoad(foreign, 0))
            return 0;
    }
    return foreign;
}

/** Sets the QDjangoModel pointed to by the given foreign-key.
 *
 * \param name
 * \param model
 */
void QDjangoModel::setForeignKey(const QString &name, QObject *model)
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    const QString foreignClass = metaModel.foreignModel(name.toLatin1());
    const QDjangoMetaModel foreignMeta = QDjango::metaModel(foreignClass);

    QObject *old = property(name.toLatin1() + "_ptr").value<QObject*>();
    if (old)
        delete old;

    setProperty(name.toLatin1() + "_id", model->property(foreignMeta.primaryKey()));
    model->setParent(this);
    setProperty(name.toLatin1() + "_ptr", qVariantFromValue(model));
}

/** Deletes the QDjangoModel from the database.
 *
 * \return true if deletion succeeded, false otherwise
 */
bool QDjangoModel::remove()
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return metaModel.remove(this);
}

/** Saves the QDjangoModel to the database.
 *
 * \return true if saving succeeded, false otherwise
 */
bool QDjangoModel::save()
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return metaModel.save(this);
}

