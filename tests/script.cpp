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

#include <QtTest>

#include "QDjangoScript.h"

#include "auth/models.h"
#include "script.h"

Q_DECLARE_METATYPE(QDjangoQuerySet<User>)

void TestScript::cleanupTestCase()
{
    metaModel.dropTable();

    delete engine;
}

void TestScript::initTestCase()
{
    metaModel = QDjango::registerModel<User>();
    QCOMPARE(metaModel.isValid(), true);
    QCOMPARE(metaModel.createTable(), true);

    engine = new QScriptEngine(this);
    qScriptRegisterWhere(engine);
    qScriptRegisterModel<User>(engine);
}

void TestScript::testWhere()
{
    QScriptValue result = engine->evaluate("where = Q('username', Q.Equals, 'foobar')");
    QDjangoWhere where = engine->fromScriptValue<QDjangoWhere>(result);
    QCOMPARE(where.sql(), QLatin1String("username = ?"));

    result = engine->evaluate("where.and(Q('password', Q.Equals, 'foopass'))");
    QDjangoWhere andWhere = engine->fromScriptValue<QDjangoWhere>(result);
    QCOMPARE(andWhere.sql(), QLatin1String("username = ? AND password = ?"));

    result = engine->evaluate("where.or(Q('password', Q.Equals, 'foopass'))");
    QDjangoWhere orWhere = engine->fromScriptValue<QDjangoWhere>(result);
    QCOMPARE(orWhere.sql(), QLatin1String("username = ? OR password = ?"));
}

void TestScript::testModel()
{
    // create model instance
    QScriptValue result = engine->evaluate("user = User();");
    User *user = qobject_cast<User*>(result.toQObject());
    QVERIFY(user != 0);

    // set properties
    engine->evaluate("user.username = 'foobar';");
    QCOMPARE(user->username(), QLatin1String("foobar"));
}

