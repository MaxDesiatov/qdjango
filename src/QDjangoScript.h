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

#ifndef QDJANGO_SCRIPT_H
#define QDJANGO_SCRIPT_H

#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

#include "QDjango.h"
#include "QDjangoQuerySet.h"
#include "QDjangoScript_p.h"

Q_DECLARE_METATYPE(QDjangoWhere)

/** \defgroup Script */

/** \brief The QDjangoScript class provides static methods for making models
 *  scriptable.
 *
 * \ingroup Script
 */
class QDjangoScript
{
public:
    template <class T>
    static void registerModel(QScriptEngine *engine);
    static void registerWhere(QScriptEngine *engine);
};

/** Makes a QDjangoModel class available to the given QScriptEngine.
 *
 * \param engine
 */
template <class T>
void QDjangoScript::registerModel(QScriptEngine *engine)
{
    QDjango::registerModel<T>();

    QScriptValue querysetProto = engine->newObject();
    querysetProto.setProperty("all", engine->newFunction(QDjangoQuerySet_all<T>));
    querysetProto.setProperty("at", engine->newFunction(QDjangoQuerySet_at<T>));
    querysetProto.setProperty("count", engine->newFunction(QDjangoQuerySet_count<T>));
    querysetProto.setProperty("exclude", engine->newFunction(QDjangoQuerySet_exclude<T>));
    querysetProto.setProperty("filter", engine->newFunction(QDjangoQuerySet_filter<T>));
    querysetProto.setProperty("get", engine->newFunction(QDjangoQuerySet_get<T>));
    querysetProto.setProperty("limit", engine->newFunction(QDjangoQuerySet_limit<T>));
    querysetProto.setProperty("remove", engine->newFunction(QDjangoQuerySet_remove<T>));
    querysetProto.setProperty("size", engine->newFunction(QDjangoQuerySet_size<T>));
    querysetProto.setProperty("toString", engine->newFunction(QDjangoQuerySet_toString<T>));
    engine->setDefaultPrototype(qMetaTypeId< QDjangoQuerySet<T> >(), querysetProto);

    QDjangoQuerySet<T> qs;
    QScriptValue value = engine->newQMetaObject(&T::staticMetaObject, engine->newFunction(QDjangoModel_new<T>));
    value.setProperty("objects", engine->toScriptValue(qs));
    engine->globalObject().setProperty(T::staticMetaObject.className(), value);
}

#endif
