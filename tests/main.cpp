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

#include <cstdlib>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QVariant>
#include <QtTest>

#include "QDjango.h"
#include "QDjangoWhere.h"

#include "main.h"
#include "auth/models.h"
#include "auth/tests.h"
#include "shares/models.h"
#include "shares/tests.h"
#include "http.h"
#include "script.h"

QString Object::foo() const
{
    return m_foo;
}

void Object::setFoo(const QString &foo)
{
    m_foo = foo;
}

int Object::bar() const
{
    return m_bar;
}

void Object::setBar(int bar)
{
    m_bar = bar;
}

void TestModel::initTestCase()
{
    metaModel = QDjango::registerModel<Object>();
    QCOMPARE(metaModel.isValid(), true);
    QCOMPARE(metaModel.createTable(), true);
}

void TestModel::save()
{
    Object obj;
    obj.setFoo("some string");
    obj.setBar(1234);
    QCOMPARE(metaModel.save(&obj), true);
    QCOMPARE(obj.property("id"), QVariant(1));
}

void TestModel::cleanupTestCase()
{
    metaModel.dropTable();
}

/** Test empty where clause.
 */
void TestWhere::emptyWhere()
{
    // construct empty where clause
    QDjangoWhere testQuery;
    QCOMPARE(testQuery.isAll(), true);
    QCOMPARE(testQuery.isNone(), false);
    CHECKWHERE(testQuery, QString(), QVariantList());

    // negate the where clause
    testQuery = !QDjangoWhere();
    QCOMPARE(testQuery.isAll(), false);
    QCOMPARE(testQuery.isNone(), true);
    CHECKWHERE(testQuery, QLatin1String("1 != 0"), QVariantList());
}

/** Test "=" comparison.
 */
void TestWhere::equalsWhere()
{
    QDjangoWhere testQuery;

    // construct an "equals" where clause
    testQuery = QDjangoWhere("id", QDjangoWhere::Equals, 1);
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);

    // negate the where clause
    testQuery = !QDjangoWhere("id", QDjangoWhere::Equals, 1);
    CHECKWHERE(testQuery, QLatin1String("id != ?"), QVariantList() << 1);
}

/** Test "!=" comparison.
 */
void TestWhere::notEqualsWhere()
{
    QDjangoWhere testQuery;

    // construct a "not equals" where clause
    testQuery = QDjangoWhere("id", QDjangoWhere::NotEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id != ?"), QVariantList() << 1);

    // negate the where clause
    testQuery = !QDjangoWhere("id", QDjangoWhere::NotEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);
}

/** Test ">" comparison.
 */
void TestWhere::greaterThan()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::GreaterThan, 1);
    CHECKWHERE(testQuery, QLatin1String("id > ?"), QVariantList() << 1);

    testQuery = !QDjangoWhere("id", QDjangoWhere::GreaterThan, 1);
    CHECKWHERE(testQuery, QLatin1String("id <= ?"), QVariantList() << 1);
}

/** Test ">=" comparison.
 */
void TestWhere::greaterOrEquals()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::GreaterOrEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id >= ?"), QVariantList() << 1);

    testQuery = !QDjangoWhere("id", QDjangoWhere::GreaterOrEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id < ?"), QVariantList() << 1);
}

/** Test "<" comparison.
 */
void TestWhere::lessThan()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::LessThan, 1);
    CHECKWHERE(testQuery, QLatin1String("id < ?"), QVariantList() << 1);

    testQuery = !QDjangoWhere("id", QDjangoWhere::LessThan, 1);
    CHECKWHERE(testQuery, QLatin1String("id >= ?"), QVariantList() << 1);
}

/** Test "<=" comparison.
 */
void TestWhere::lessOrEquals()
{
    QDjangoWhere testQuery;

    testQuery = QDjangoWhere("id", QDjangoWhere::LessOrEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id <= ?"), QVariantList() << 1);

    testQuery = !QDjangoWhere("id", QDjangoWhere::LessOrEquals, 1);
    CHECKWHERE(testQuery, QLatin1String("id > ?"), QVariantList() << 1);
}

/** Test "in" comparison.
 */
void TestWhere::isIn()
{
    QDjangoWhere testQuery = QDjangoWhere("id", QDjangoWhere::IsIn, QVariantList() << 1 << 2);
    CHECKWHERE(testQuery, QLatin1String("id IN (?, ?)"), QVariantList() << 1 << 2);
}

/** Test "startswith" comparison.
 */
void TestWhere::startsWith()
{
    QDjangoWhere testQuery = QDjangoWhere("name", QDjangoWhere::StartsWith, "abc");
    CHECKWHERE(testQuery, QLatin1String("name LIKE ? ESCAPE '\\'"), QVariantList() << "abc%");
}

/** Test "endswith" comparison.
 */
void TestWhere::endsWith()
{
    QDjangoWhere testQuery = QDjangoWhere("name", QDjangoWhere::EndsWith, "abc");
    CHECKWHERE(testQuery, QLatin1String("name LIKE ? ESCAPE '\\'"), QVariantList() << "%abc");
}

/** Test "contains" comparison.
 */
void TestWhere::contains()
{
    QDjangoWhere testQuery = QDjangoWhere("name", QDjangoWhere::Contains, "abc");
    CHECKWHERE(testQuery, QLatin1String("name LIKE ? ESCAPE '\\'"), QVariantList() << "%abc%");
}

/** Test compound where clause, using the AND operator.
 */
void TestWhere::andWhere()
{
    QDjangoWhere testQuery;

    const QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    const QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    testQuery = queryId && queryUsername;
    CHECKWHERE(testQuery, QLatin1String("id = ? AND username = ?"), QVariantList() << 1 << "foo");

    // and with "all" queryset
    testQuery = QDjangoWhere() && queryId;
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);

    testQuery = queryId && QDjangoWhere();
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);

    // and with "none" queryset
    testQuery = !QDjangoWhere() && queryId;
    QCOMPARE(testQuery.isNone(), true);
    CHECKWHERE(testQuery, QLatin1String("1 != 0"), QVariantList());

    testQuery = queryId && !QDjangoWhere();
    QCOMPARE(testQuery.isNone(), true);
    CHECKWHERE(testQuery, QLatin1String("1 != 0"), QVariantList());

    // negation
    testQuery = !(queryId && queryUsername);
    CHECKWHERE(testQuery, QLatin1String("NOT (id = ? AND username = ?)"), QVariantList() << 1 << "foo");
}

/** Test compound where clause, using the OR operator.
 */
void TestWhere::orWhere()
{
    QDjangoWhere testQuery;

    const QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    const QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foo");

    testQuery = queryId || queryUsername;
    CHECKWHERE(testQuery, QLatin1String("id = ? OR username = ?"), QVariantList() << 1 << "foo");

    // or with "all" queryset
    testQuery = QDjangoWhere() || queryId;
    QCOMPARE(testQuery.isAll(), true);
    CHECKWHERE(testQuery, QString(), QVariantList());

    testQuery = queryId || QDjangoWhere();
    QCOMPARE(testQuery.isAll(), true);
    CHECKWHERE(testQuery, QString(), QVariantList());

    // or with "none" queryset
    testQuery = !QDjangoWhere() || queryId;
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);

    testQuery = queryId || !QDjangoWhere();
    CHECKWHERE(testQuery, QLatin1String("id = ?"), QVariantList() << 1);

    // negation
    testQuery = !(queryId || queryUsername);
    CHECKWHERE(testQuery, QLatin1String("NOT (id = ? OR username = ?)"), QVariantList() << 1 << "foo");
}

/** Test compound where clause, using both the AND and the OR operators.
 */
void TestWhere::complexWhere()
{
    QDjangoWhere testQuery;

    const QDjangoWhere queryId("id", QDjangoWhere::Equals, 1);
    const QDjangoWhere queryUsername("username", QDjangoWhere::Equals, "foouser");
    const QDjangoWhere queryPassword("password", QDjangoWhere::Equals, "foopass");

    testQuery = (queryId || queryUsername) && queryPassword;
    CHECKWHERE(testQuery, QLatin1String("(id = ? OR username = ?) AND password = ?"), QVariantList() << 1 << "foouser" << "foopass");

    testQuery = queryId || (queryUsername && queryPassword);
    CHECKWHERE(testQuery, QLatin1String("id = ? OR (username = ? AND password = ?)"), QVariantList() << 1 << "foouser" << "foopass");
}

/** Display program usage.
 */
static void usage()
{
    fprintf(stderr, "Usage: qdjango-test [-d <driver>] [-n <database>] [-u <user>] [-p <password>]\n");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // initialise options
    QString databaseDriver = "QSQLITE";
    QString databaseName = ":memory:";
    QString databaseUser;
    QString databasePassword;
    int count = 1;

    // parse command line arguments
    if (!(argc % 2))
    {
        usage();
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-c") && i < argc - 1)
            count = QString::fromLocal8Bit(argv[++i]).toInt();
        else if (!strcmp(argv[i], "-d") && i < argc - 1)
            databaseDriver = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-n") && i < argc - 1)
            databaseName = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-p") && i < argc - 1)
            databasePassword = QString::fromLocal8Bit(argv[++i]);
        else if (!strcmp(argv[i], "-u") && i < argc - 1)
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
    QDjango::setDatabase(db);

    // declare models
    QDjango::registerModel<User>();
    QDjango::registerModel<Group>();
    QDjango::registerModel<Message>();
    QDjango::registerModel<UserGroups>();

    QDjango::registerModel<File>();

    // run tests
    int errors = 0;

    for (int i = 0; i < count; ++i)
    {
        TestWhere testWhere;
        errors += QTest::qExec(&testWhere);

        TestModel testModel;
        errors += QTest::qExec(&testModel);

        TestUser testUser;
        errors += QTest::qExec(&testUser);

        TestRelated testRelated;
        errors += QTest::qExec(&testRelated);

        TestShares testShares;
        errors += QTest::qExec(&testShares);

        TestHttp testHttp;
        errors += QTest::qExec(&testHttp);

        TestScript testScript;
        errors += QTest::qExec(&testScript);
    }

    if (errors)
    {
        qWarning() << "Total failed tests:" << errors;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};

