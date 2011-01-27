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
#include <QScriptValueIterator>

#include "QDjangoScript.h"
#include "QDjangoWhere.h"


static QScriptValue newWhere(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoWhere where;
    if (context->argumentCount() == 1 && context->argument(0).isObject()) {
        QScriptValueIterator it(context->argument(0));
        while (it.hasNext()) {
            it.next();
            QString key = it.name();
            QDjangoWhere::Operation op = QDjangoWhere::Equals;
            if (key.endsWith("__lt")) {
                key.chop(4);
                op = QDjangoWhere::LessThan;
            }
            else if (key.endsWith("__lte")) {
                key.chop(5);
                op = QDjangoWhere::LessOrEquals;
            }
            else if (key.endsWith("__gt")) {
                key.chop(4);
                op = QDjangoWhere::GreaterThan;
            }
            else if (key.endsWith("__gte")) {
                key.chop(5);
                op = QDjangoWhere::GreaterOrEquals;
            }
            else if (key.endsWith("__startswith")) {
                key.chop(12);
                op = QDjangoWhere::StartsWith;
            }
            else if (key.endsWith("__endswith")) {
                key.chop(10);
                op = QDjangoWhere::EndsWith;
            }
            else if (key.endsWith("__contains")) {
                key.chop(10);
                op = QDjangoWhere::Contains;
            }
            else if (key.endsWith("__in")) {
                key.chop(4);
                op = QDjangoWhere::IsIn;
            }
            where = where && QDjangoWhere(key, op, it.value().toVariant());
        }
    } else if (context->argumentCount() == 3) {
        where = QDjangoWhere(
                   context->argument(0).toString(),
                   static_cast<QDjangoWhere::Operation>(context->argument(1).toInteger()),
                   context->argument(2).toVariant());
    }
    return engine->toScriptValue(where);
}

static QScriptValue whereAnd(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoWhere q = engine->fromScriptValue<QDjangoWhere>(context->thisObject());
    QDjangoWhere other = engine->fromScriptValue<QDjangoWhere>(context->argument(0));
    return engine->toScriptValue(q && other);
}

static QScriptValue whereOr(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoWhere q = engine->fromScriptValue<QDjangoWhere>(context->thisObject());
    QDjangoWhere other = engine->fromScriptValue<QDjangoWhere>(context->argument(0));
    return engine->toScriptValue(q || other);
}

static QScriptValue whereToString(QScriptContext *context, QScriptEngine *engine)
{
    QDjangoWhere q = engine->fromScriptValue<QDjangoWhere>(context->thisObject());
    return engine->toScriptValue("Q(" + q.sql() + ")");
}

void qScriptRegisterWhere(QScriptEngine *engine)
{
    QScriptValue whereProto = engine->newObject();
    whereProto.setProperty("and", engine->newFunction(whereAnd));
    whereProto.setProperty("or", engine->newFunction(whereOr));
    whereProto.setProperty("toString", engine->newFunction(whereToString));
    engine->setDefaultPrototype(qMetaTypeId<QDjangoWhere>(), whereProto);

    QScriptValue ctor = engine->newFunction(newWhere);
    engine->globalObject().setProperty("Q", ctor, QScriptValue::ReadOnly);
}

