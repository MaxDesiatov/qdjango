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

/** Retrieves the QDjangoModel pointed to by the given foreign-key.
 *
 * \param name
 */
QObject *QDjangoModel::foreignKey(const QString &name) const
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    return metaModel.foreignKey(this, name.toLatin1());
}

/** Sets the QDjangoModel pointed to by the given foreign-key.
 *
 * \param name
 * \param model
 */
void QDjangoModel::setForeignKey(const QString &name, QObject *model)
{
    const QDjangoMetaModel metaModel = QDjango::metaModel(metaObject()->className());
    metaModel.setForeignKey(this, name.toLatin1(), model);
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

