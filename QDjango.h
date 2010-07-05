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

#ifndef QDJANGO_H
#define QDJANGO_H

#include <QMap>

class QSqlDatabase;
class QSqlQuery;
class QString;

class QDjangoModel;
class QDjangoMetaModel;

bool sqlExec(QSqlQuery &query);

/** \brief The QDjango class provides a set of static functions.
 *
 *  It is used to access registered QDjangoModel classes.
 */
class QDjango
{
public:
    static bool createTables();
    static bool dropTables();

    static QSqlDatabase database();
    static void setDatabase(QSqlDatabase database);

    template <class T>
    static bool registerModel();

    // backend specific
    static QString autoIncrementSql();
    static QString noLimitSql();
    static QString quote(const QString &name);
    static QString unquote(const QString &quoted);

private:
    static bool registerModel(const QDjangoModel *model);
    static QDjangoMetaModel metaModel(const QString &name);

    friend class QDjangoModel;
    friend class QDjangoMetaModel;
    friend class QDjangoQueryBase;
};

/** Register a QDjangoModel class with QDjango.
 */
template <class T>
bool QDjango::registerModel()
{
    T model;
    return registerModel(&model);
}

#endif
