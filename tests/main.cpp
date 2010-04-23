#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QtTest/QtTest>

#include "queryset.h"

#include "main.h"
#include "user.h"

void TestModel::initTestCase()
{
    User user;
    user.database().exec("DROP TABLE user");
}

void TestModel::createTable()
{
    User user;
    bool result = user.createTable();
    QCOMPARE(result, true);
}

void TestModel::createUser()
{
    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");

    bool result = user.save();
    QCOMPARE(result, true);
    QCOMPARE(QDjangoQuerySet<User>().size(), 1);

    User *other = QDjangoQuerySet<User>().get("username", "foouser");
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass"));
}

void TestModel::removeUser()
{
    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");

    bool result = user.save();
    QCOMPARE(result, true);
    QCOMPARE(QDjangoQuerySet<User>().size(), 1);

    result = user.remove();
    QCOMPARE(result, true);
    QCOMPARE(QDjangoQuerySet<User>().size(), 0);
}

void TestModel::getUser()
{
    User foo;
    foo.setUsername("foouser");
    foo.setPassword("foopass");
    foo.save();

    User bar;
    bar.setUsername("baruser");
    bar.setPassword("barpass");
    bar.save();

    QCOMPARE(QDjangoQuerySet<User>().size(), 2);

    User *other = QDjangoQuerySet<User>().get("username", "foouser");
    QCOMPARE(other->username(), QString::fromLatin1("foouser"));
    QCOMPARE(other->password(), QString::fromLatin1("foopass"));
}

void TestModel::filterUsers()
{
    User foo;
    foo.setUsername("foouser");
    foo.setPassword("foopass");
    foo.save();

    User bar;
    bar.setUsername("baruser");
    bar.setPassword("barpass");
    bar.save();

    const QDjangoQuerySet<User> UserQuery;
    QCOMPARE(UserQuery.size(), 2);

    QDjangoQuerySet<User> qs;

    qs = UserQuery.filter("foo", "bar");
    QCOMPARE(qs.size(), 0);

    qs = UserQuery.filter("username", "foouser");
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/tmp/foo.sqlite");
    Q_ASSERT(db.open());

    User user;
    user.setDatabase(&db);

    TestModel test;
    QTest::qExec(&test);

    db.close();
    return 0;
};

