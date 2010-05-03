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

#ifndef QDJANGO_SCRIPT_H
#define QDJANGO_SCRIPT_H

#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

#include "queryset.h"

template <class T>
static QScriptValue querySetAll(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    return engine->toScriptValue(qs.all());
}

template <class T>
static QScriptValue querySetAt(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    //QDjangoQuerySet<T> qs = context->thisObject().toVariant().value< QDjangoQuerySet<T> >();
    int index = context->argument(0).toInteger();
    return engine->newQObject(qs.at(index), QScriptEngine::ScriptOwnership);
}

template <class T>
static QScriptValue querySetExclude(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    QString key = context->argument(0).toString();
    QVariant val = context->argument(1).toVariant();
    return engine->toScriptValue(qs.exclude(key, val));
}

template <class T>
static QScriptValue querySetFilter(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    QString key = context->argument(0).toString();
    QVariant val = context->argument(1).toVariant();
    return engine->toScriptValue(qs.filter(key, val));
}

template <class T>
static QScriptValue querySetGet(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    QString key = context->argument(0).toString();
    QVariant val = context->argument(1).toVariant();
    return engine->newQObject(qs.get(key, val), QScriptEngine::ScriptOwnership);
}

template <class T>
static QScriptValue querySetSize(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    return QScriptValue(engine, qs.size());
}

template <class T>
static QScriptValue querySetToString(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoQuerySet<T> qs = engine->fromScriptValue< QDjangoQuerySet<T> >(context->thisObject());
    return QScriptValue(engine, QString("QuerySet<%1>(%2)").arg(T::staticMetaObject.className(), qs.where().sql()));
}

template <class T>
static QScriptValue newModel(QScriptContext *context, QScriptEngine *engine)
{
    return engine->newQObject(new T, QScriptEngine::ScriptOwnership);
}

template <class T>
void qScriptRegisterModel(QScriptEngine *engine)
{
    qDjangoRegisterModel<T>();

    QScriptValue querysetProto = engine->newObject();
    querysetProto.setProperty("all", engine->newFunction(querySetAll<T>));
    querysetProto.setProperty("at", engine->newFunction(querySetAt<T>));
    querysetProto.setProperty("exclude", engine->newFunction(querySetExclude<T>));
    querysetProto.setProperty("filter", engine->newFunction(querySetFilter<T>));
    querysetProto.setProperty("get", engine->newFunction(querySetGet<T>));
    querysetProto.setProperty("size", engine->newFunction(querySetSize<T>));
    querysetProto.setProperty("toString", engine->newFunction(querySetToString<T>));
    engine->setDefaultPrototype(qMetaTypeId< QDjangoQuerySet<T> >(), querysetProto);

    QDjangoQuerySet<T> qs;
    QScriptValue value = engine->newQMetaObject(&T::staticMetaObject, engine->newFunction(newModel<T>));
    value.setProperty("objects", engine->toScriptValue(qs));
    engine->globalObject().setProperty(T::staticMetaObject.className(), value);
}

#endif
