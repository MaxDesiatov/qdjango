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

#include <QDebug>
#include <QScriptExtensionPlugin>
#include <QScriptValue>
#include <QScriptEngine>

#include "script.h"
#include "tests/models.h"

Q_DECLARE_METATYPE(QDjangoQuerySet<User>)
Q_DECLARE_METATYPE(QDjangoQuerySet<Group>)

class QDjangoScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList QDjangoScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qdjango") << QLatin1String("qdjango.test");
    return list;
}

void QDjangoScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qdjango.test"))
    {
        QScriptValue extensionObject = engine->globalObject();
        qScriptRegisterModel<User>(engine);
        qScriptRegisterModel<Group>(engine);
    }
}

Q_EXPORT_PLUGIN2(qtscript_qdjango, QDjangoScriptPlugin)
Q_EXPORT_STATIC_PLUGIN2(qtscript_qdjango, QDjangoScriptPlugin)
