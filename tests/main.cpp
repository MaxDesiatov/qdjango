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
    bool result = user.createTable();
//    QCOMPARE(result, true);
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
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass"));

    // update
    user.setPassword("foopass2");
    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    other = users.get("username", "foouser");
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass2"));

    User user2;
    user2.setUsername("baruser");
    user2.setPassword("barpass");
    QCOMPARE(user2.save(), true);
    QCOMPARE(users.all().size(), 2);

    other = users.get("username", "baruser");
    QCOMPARE(other->pk(), QVariant(2));
    QCOMPARE(other->username(), QString::fromLatin1("baruser"));
    QCOMPARE(other->password(), QString::fromLatin1("barpass"));
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
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass"));
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
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass"));

    qs = qs.filter("password", "foopass");
    QCOMPARE(qs.size(), 1);
}

void TestModel::cleanup()
{
    User user;
    user.database().exec("DELETE FROM user");
}

void TestModel::cleanupTestCase()
{
    User user;
    user.database().exec("DROP TABLE user");
}

void TestWhere::simpleWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QCOMPARE(queryId.sql(), QString::fromLatin1("id = :id"));

    QDjangoWhere queryNotId("id", QDjangoWhere::NotEquals, 1);
    QCOMPARE(queryNotId.sql(), QString::fromLatin1("id != :id"));
}

void TestWhere::andWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    QDjangoWhere queryAnd = queryId && queryUsername;
    QCOMPARE(queryAnd.sql(), QString::fromLatin1("id = :id AND username = :username"));
}

void TestWhere::orWhere()
{
    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    QDjangoWhere queryAnd = queryId || queryUsername;
    QCOMPARE(queryAnd.sql(), QString::fromLatin1("id = :id OR username = :username"));
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

    TestWhere testQuery;
    QTest::qExec(&testQuery);

    TestModel test;
    QTest::qExec(&test);

    db.close();
    return EXIT_SUCCESS;
};

