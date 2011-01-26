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

#include <QDebug>
#include <QScriptEngine>
#include <QScriptValue>

#include "QDjangoScript.h"
#include "QDjangoWhere.h"

static QScriptValue newWhere(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() == 3) {
        return engine->toScriptValue(QDjangoWhere(
                       context->argument(0).toString(),
                       static_cast<QDjangoWhere::Operation>(context->argument(1).toInteger()),
                       context->argument(2).toVariant()));
    }
    return engine->toScriptValue(QDjangoWhere());
}

static QScriptValue whereToString(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoWhere q = engine->fromScriptValue<QDjangoWhere>(context->thisObject());
    return engine->toScriptValue("Q(" + q.sql() + ")");
}

void qScriptRegisterWhere(QScriptEngine *engine)
{
    QScriptValue whereProto = engine->newObject();
    whereProto.setProperty("toString", engine->newFunction(whereToString));
    engine->setDefaultPrototype(qMetaTypeId<QDjangoWhere>(), whereProto);

    QScriptValue ctor = engine->newFunction(newWhere);
    ctor.setProperty("Equals", QScriptValue(engine, QDjangoWhere::Equals), QScriptValue::ReadOnly);
    ctor.setProperty("NotEquals", QScriptValue(engine, QDjangoWhere::NotEquals), QScriptValue::ReadOnly);
    ctor.setProperty("GreaterThan", QScriptValue(engine, QDjangoWhere::GreaterThan), QScriptValue::ReadOnly);
    ctor.setProperty("LessThan", QScriptValue(engine, QDjangoWhere::LessThan), QScriptValue::ReadOnly);
    ctor.setProperty("GreaterOrEquals", QScriptValue(engine, QDjangoWhere::GreaterOrEquals), QScriptValue::ReadOnly);
    ctor.setProperty("LessOrEquals", QScriptValue(engine, QDjangoWhere::LessOrEquals), QScriptValue::ReadOnly);
    ctor.setProperty("StartsWith", QScriptValue(engine, QDjangoWhere::StartsWith), QScriptValue::ReadOnly);
    ctor.setProperty("EndsWith", QScriptValue(engine, QDjangoWhere::EndsWith), QScriptValue::ReadOnly);
    ctor.setProperty("IsIn", QScriptValue(engine, QDjangoWhere::IsIn), QScriptValue::ReadOnly);
    engine->globalObject().setProperty("Q", ctor, QScriptValue::ReadOnly);
}

