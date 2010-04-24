/*
 * boc-utils
 * Copyright (C) 2008-2009 Bolloré telecom
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

#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include "model.h"
#include "tests/user.h"

static QScriptValue newUser(QScriptContext *context, QScriptEngine *engine)
{
    return engine->newQObject(new User, QScriptEngine::ScriptOwnership);
}

class eu_bolloretelecom_qdjango_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList eu_bolloretelecom_qdjango_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qdjango");
    return list;
}

void eu_bolloretelecom_qdjango_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qdjango")) {
        QScriptValue extensionObject = engine->globalObject();

        extensionObject.setProperty("User", engine->newQMetaObject(&User::staticMetaObject, engine->newFunction(newUser)));
    } else {
        Q_ASSERT_X(false, "eu_bolloretelecom_qdjango::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(eu_bolloretelecom_qdjango_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_eu_bolloretelecom_qdjango, eu_bolloretelecom_qdjango_ScriptPlugin)
