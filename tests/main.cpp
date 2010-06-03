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

#include <cstdlib>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QtTest/QtTest>

#include "QDjango.h"
#include "QDjangoWhere.h"

#include "main.h"
#include "auth/models.h"
#include "auth/tests.h"
#include "shares/models.h"
#include "shares/tests.h"

void TestWhere::quoting()
{
    QCOMPARE(QDjango::quote("foo"), QLatin1String("`foo`"));
    QCOMPARE(QDjango::unquote("`foo`"), QLatin1String("foo"));
}

void TestWhere::emptyWhere()
{
    QDjangoWhere testQuery;
    QCOMPARE(testQuery.isAll(), true);
    QCOMPARE(testQuery.isNone(), false);
    QCOMPARE(testQuery.sql(), QString());

    testQuery = !QDjangoWhere();
    QCOMPARE(testQuery.isAll(), false);
    QCOMPARE(testQuery.isNone(), true);
    QCOMPARE(testQuery.sql(), QLatin1String("1 != 0"));
}

void TestWhere::equalsWhere()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::Equals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::Equals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id != :id"));
}

void TestWhere::notEqualsWhere()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::NotEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id != :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::NotEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));
}

void TestWhere::greaterThan()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::GreaterThan, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id > :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::GreaterThan, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id <= :id"));
}

void TestWhere::greaterOrEquals()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::GreaterOrEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id >= :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::GreaterOrEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id < :id"));
}

void TestWhere::lessThan()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::LessThan, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id < :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::LessThan, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id >= :id"));
}

void TestWhere::lessOrEquals()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::LessOrEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id <= :id"));

    testQuery = !QDjangoWhere("id", QDjangoWhere::LessOrEquals, 1);
    QCOMPARE(testQuery.sql(), QLatin1String("id > :id"));
}

void TestWhere::isIn()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::IsIn, QList<QVariant>() << 1 << 2);
    QCOMPARE(testQuery.sql(), QLatin1String("id IN (:id_0, :id_1)"));
}

void TestWhere::startsWith()
{
    QSqlQuery query;
    QDjangoWhere testQuery = QDjangoWhere("name", QDjangoWhere::StartsWith, "abc");
    testQuery.bindValues(query);
    QCOMPARE(testQuery.sql(), QLatin1String("name LIKE :name ESCAPE '\\'"));
    QCOMPARE(query.boundValue(":name"), QVariant("abc%"));
}

void TestWhere::endsWith()
{
    QSqlQuery query;
    QDjangoWhere testQuery = QDjangoWhere("name", QDjangoWhere::EndsWith, "abc");
    testQuery.bindValues(query);
    QCOMPARE(testQuery.sql(), QLatin1String("name LIKE :name ESCAPE '\\'"));
    QCOMPARE(query.boundValue(":name"), QVariant("%abc"));
}

void TestWhere::andWhere()
{
    QDjangoWhere testQuery;

    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    testQuery = queryId && queryUsername;
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id AND username = :username"));

    // and with "all" queryset
    testQuery = QDjangoWhere() && queryId;
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));

    testQuery = queryId && QDjangoWhere();
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));

    // and with "none" queryset
    testQuery = !QDjangoWhere() && queryId;
    QCOMPARE(testQuery.isNone(), true);
    QCOMPARE(testQuery.sql(), QLatin1String("1 != 0"));

    testQuery = queryId && !QDjangoWhere();
    QCOMPARE(testQuery.isNone(), true);
    QCOMPARE(testQuery.sql(), QLatin1String("1 != 0"));

    // negation
    testQuery = !(queryId && queryUsername);
    QCOMPARE(testQuery.sql(), QLatin1String("NOT (id = :id AND username = :username)"));
}

void TestWhere::orWhere()
{
    QDjangoWhere testQuery;

    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    testQuery = queryId || queryUsername;
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id OR username = :username"));

    // or with "all" queryset
    testQuery = QDjangoWhere() || queryId;
    QCOMPARE(testQuery.isAll(), true);
    QCOMPARE(testQuery.sql(), QString());

    testQuery = queryId || QDjangoWhere();
    QCOMPARE(testQuery.isAll(), true);
    QCOMPARE(testQuery.sql(), QString());

    // or with "none" queryset
    testQuery = !QDjangoWhere() || queryId;
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));

    testQuery = queryId || !QDjangoWhere();
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id"));

    // negation
    testQuery = !(queryId || queryUsername);
    QCOMPARE(testQuery.sql(), QLatin1String("NOT (id = :id OR username = :username)"));
}

void TestWhere::complexWhere()
{
    QDjangoWhere testQuery;

    QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foouser");
    QDjangoWhere queryPassword("password", QDjangoWhere::Equals, "foopass");

    testQuery = (queryId || queryUsername) && queryPassword;
    QCOMPARE(testQuery.sql(), QLatin1String("(id = :id OR username = :username) AND password = :password"));

    testQuery = queryId || (queryUsername && queryPassword);
    QCOMPARE(testQuery.sql(), QLatin1String("id = :id OR (username = :username AND password = :password)"));
}

static int usage()
{
    fprintf(stderr, "Usage: qdjango-test [-d <driver>] [-n <database>] [-u <user>] [-p <password>]\n");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QString databaseDriver = "QSQLITE";
    QString databaseName = ":memory";
    QString databaseUser;
    QString databasePassword;

    // parse command line arguments
    if (!(argc % 2))
    {
        usage();
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-d"))
            databaseDriver = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-n"))
            databaseName = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-p"))
            databasePassword = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-u"))
            databaseUser = QString::fromLocal8Bit(argv[++i]);
        else
        {
            usage();
            return EXIT_FAILURE;
        }
    }

    // open database
    QSqlDatabase db = QSqlDatabase::addDatabase(databaseDriver);
    db.setDatabaseName(databaseName);
    db.setUserName(databaseUser);
    db.setPassword(databasePassword);
    if (!db.open()) {
        fprintf(stderr, "Could not access database\n");
        return EXIT_FAILURE;
    }
    QDjangoModel::setDatabase(db);

    // declare models
    qDjangoRegisterModel<User>();
    qDjangoRegisterModel<Group>();
    qDjangoRegisterModel<Message>();
    qDjangoRegisterModel<UserGroups>();

    qDjangoRegisterModel<File>();

    // run tests
    int errors = 0;

    TestWhere testWhere;
    errors += QTest::qExec(&testWhere);

    TestModel testModel;
    errors += QTest::qExec(&testModel);

    TestRelated testRelated;
    errors += QTest::qExec(&testRelated);

    TestShares testShares;
    errors += QTest::qExec(&testShares);

    if (errors)
    {
        qWarning() << "Total failed tests:" << errors;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};

