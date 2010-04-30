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

#include <cstdlib>

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QtTest/QtTest>

#include "queryset.h"
#include "where.h"

#include "main.h"
#include "models.h"

void TestModel::initTestCase()
{
    User user;
    user.createTable();
}

void TestModel::createUser()
{
    const QDjangoQuerySet<User> users;
    User *other;

    // create
    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");
    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    other = users.get("username", "foouser");
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));

    // update
    user.setPassword("foopass2");
    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    other = users.get("username", "foouser");
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass2"));

    User user2;
    user2.setUsername("baruser");
    user2.setPassword("barpass");
    QCOMPARE(user2.save(), true);
    QCOMPARE(users.all().size(), 2);

    other = users.get("username", "baruser");
    QCOMPARE(other->pk(), QVariant(2));
    QCOMPARE(other->username(), QLatin1String("baruser"));
    QCOMPARE(other->password(), QLatin1String("barpass"));
}

void TestModel::removeUser()
{
    const QDjangoQuerySet<User> users;

    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");

    bool result = user.save();
    QCOMPARE(result, true);
    QCOMPARE(users.all().size(), 1);

    result = user.remove();
    QCOMPARE(result, true);
    QCOMPARE(users.all().size(), 0);
}

void TestModel::getUser()
{
    const QDjangoQuerySet<User> users;

    User foo;
    foo.setUsername("foouser");
    foo.setPassword("foopass");
    foo.save();

    User bar;
    bar.setUsername("baruser");
    bar.setPassword("barpass");
    bar.save();

    QCOMPARE(users.all().size(), 2);

    User *other = users.get("username", "foouser");
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
}

void TestModel::filterUsers()
{
    const QDjangoQuerySet<User> users;

    User foo;
    foo.setUsername("foouser");
    foo.setPassword("foopass");
    foo.save();

    User bar;
    bar.setUsername("baruser");
    bar.setPassword("barpass");
    bar.save();

    QCOMPARE(users.all().size(), 2);

    QDjangoQuerySet<User> qs = users.filter("username", "bar");
    QCOMPARE(qs.size(), 0);

    qs = users.filter("username", "foouser");
    QCOMPARE(qs.size(), 1);
    User *other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));

    qs = qs.filter("password", "foopass");
    QCOMPARE(qs.size(), 1);
}

void TestModel::cleanup()
{
    QDjangoModel::database().exec("DELETE FROM user");
}

void TestModel::cleanupTestCase()
{
    QDjangoModel::database().exec("DROP TABLE user");
}

void TestWhere::quoting()
{
    QCOMPARE(QDjango::quote("foo"), QLatin1String("`foo`"));
    QCOMPARE(QDjango::unquote("`foo`"), QLatin1String("foo"));
}

void TestWhere::simpleWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QCOMPARE(queryId.sql(), QLatin1String("id = :id"));

    QDjangoWhere queryNotId("id", QDjangoWhere::NotEquals, 1);
    QCOMPARE(queryNotId.sql(), QLatin1String("id != :id"));
}

void TestWhere::andWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    QDjangoWhere queryAnd = queryId && queryUsername;
    QCOMPARE(queryAnd.sql(), QLatin1String("id = :id AND username = :username"));
}

void TestWhere::orWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    QDjangoWhere queryAnd = queryId || queryUsername;
    QCOMPARE(queryAnd.sql(), QLatin1String("id = :id OR username = :username"));
}

void TestRelated::initTestCase()
{
    User user;
    user.createTable();
    Message message;
    message.createTable();
}

void TestRelated::testRelated()
{
    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");
    QCOMPARE(user.save(), true);

    Message message;
    message.setUserId(user.pk().toInt());
    message.setText("test message");
    QCOMPARE(message.save(), true);

    User *msgUser = message.user();
    QCOMPARE(msgUser->username(), QLatin1String("foouser"));
    QCOMPARE(msgUser->password(), QLatin1String("foopass"));
}

void TestRelated::cleanupTestCase()
{
    QDjangoModel::database().exec("DROP TABLE user");
    QDjangoModel::database().exec("DROP TABLE message");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // open database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        fprintf(stderr, "Could not access database\n");
        return EXIT_FAILURE;
    }
    QDjangoModel::setDatabase(db);

    // declare models
    qDjangoRegisterModel<User>();
    qDjangoRegisterModel<Group>();
    qDjangoRegisterModel<Message>();

    TestWhere testWhere;
    QTest::qExec(&testWhere);

    TestModel testModel;
    QTest::qExec(&testModel);

    TestRelated testRelated;
    QTest::qExec(&testRelated);

    db.close();
    return EXIT_SUCCESS;
};

