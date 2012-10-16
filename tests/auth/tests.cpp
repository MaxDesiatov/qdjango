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

#include <QtTest/QtTest>

#include "QDjangoQuerySet.h"
#include "QDjangoWhere.h"

#include "models.h"
#include "tests.h"

/** Create database table before running tests.
 */
void TestUser::initTestCase()
{
    QCOMPARE(QDjango::registerModel<User>().createTable(), true);
}

/** Load fixtures consisting of 3 users.
 */
void TestUser::loadFixtures()
{
    User foo;
    foo.setUsername("foouser");
    foo.setPassword("foopass");
    foo.setLastLogin(QDateTime(QDate(2010, 6, 1), QTime(10, 5, 14)));
    QCOMPARE(foo.save(), true);

    User bar;
    bar.setUsername("baruser");
    bar.setPassword("barpass");
    bar.setLastLogin(QDateTime(QDate(2010, 6, 1), QTime(10, 6, 31)));
    QCOMPARE(bar.save(), true);

    User wiz;
    wiz.setUsername("wizuser");
    wiz.setPassword("wizpass");
    wiz.setLastLogin(QDateTime(QDate(2010, 6, 1), QTime(10, 7, 18)));
    QCOMPARE(wiz.save(), true);

    QCOMPARE(QDjangoQuerySet<User>().count(), 3);
    QCOMPARE(QDjangoQuerySet<User>().size(), 3);
}

void TestUser::create()
{
    const QDjangoQuerySet<User> users;
    User *other;

    // create
    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");
    user.setLastLogin(QDateTime(QDate(2010, 6, 1), QTime(10, 5, 14)));
    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    // get by id
    other = users.get(QDjangoWhere("id", QDjangoWhere::Equals, 1));
    QVERIFY(other != 0);
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    // get by pk
    other = users.get(QDjangoWhere("pk", QDjangoWhere::Equals, 1));
    QVERIFY(other != 0);
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    // get by username
    other = users.get(QDjangoWhere("username", QDjangoWhere::Equals, "foouser"));
    QVERIFY(other != 0);
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    QCOMPARE(other->lastLogin(), QDateTime(QDate(2010, 6, 1), QTime(10, 5, 14)));
    delete other;

    // update
    user.setPassword("foopass2");
    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    other = users.get(QDjangoWhere("username", QDjangoWhere::Equals, "foouser"));
    QVERIFY(other != 0);
    QCOMPARE(other->pk(), QVariant(1));
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass2"));
    delete other;

    User user2;
    user2.setUsername("baruser");
    user2.setPassword("barpass");
    user2.setLastLogin(QDateTime(QDate(2010, 6, 1), QTime(10, 6, 31)));
    QCOMPARE(user2.save(), true);
    QCOMPARE(users.all().size(), 2);

    other = users.get(QDjangoWhere("username", QDjangoWhere::Equals, "baruser"));
    QVERIFY(other != 0);
    QCOMPARE(other->pk(), QVariant(2));
    QCOMPARE(other->username(), QLatin1String("baruser"));
    QCOMPARE(other->password(), QLatin1String("barpass"));
    QCOMPARE(other->lastLogin(), QDateTime(QDate(2010, 6, 1), QTime(10, 6, 31)));
    delete other;
}

/** Test removing a single user.
 */
void TestUser::remove()
{
    const QDjangoQuerySet<User> users;

    User user;
    user.setUsername("foouser");
    user.setPassword("foopass");

    QCOMPARE(user.save(), true);
    QCOMPARE(users.all().size(), 1);

    QCOMPARE(user.remove(), true);
    QCOMPARE(users.all().size(), 0);
}

/** Test removing multiple users.
  */
void TestUser::removeFilter()
{
    loadFixtures();

    // remove "foouser" and "baruser"
    const QDjangoQuerySet<User> users;
    QDjangoQuerySet<User> qs = users.filter(QDjangoWhere("username", QDjangoWhere::IsIn, QStringList() << "foouser" << "baruser"));
    QCOMPARE(qs.remove(), true);

    // check remaining user
    qs = users.all();
    QCOMPARE(qs.size(), 1);
    User *other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("wizuser"));
    delete other;
}

/** Test removing multiple users with a LIMIT clause.
 */
void TestUser::removeLimit()
{
    loadFixtures();

    // FIXME : remove the first two entries fails
    const QDjangoQuerySet<User> users;
    QCOMPARE(users.limit(0, 2).remove(), false);
    QCOMPARE(users.all().size(), 3);
}

/** Test retrieving a single user.
 */
void TestUser::get()
{
    loadFixtures();

    // get an inexistent user
    const QDjangoQuerySet<User> users;
    User *other = users.get(QDjangoWhere("username", QDjangoWhere::Equals, "does_not_exist"));
    QVERIFY(other == 0);

    // get multiple users
    other = users.get(QDjangoWhere());
    QVERIFY(other == 0);

    // get an existing user
    other = users.get(QDjangoWhere("username", QDjangoWhere::Equals, "foouser"));
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;
}

/** Test filtering users with a "=" comparison.
 */
void TestUser::filter()
{
    loadFixtures();

    // all users
    const QDjangoQuerySet<User> users;
    QDjangoQuerySet<User> qs = users.all();
    QCOMPARE(qs.where().sql(), QLatin1String(""));
    QCOMPARE(qs.size(), 3);

    // invalid username
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::Equals, "doesnotexist"));
    QCOMPARE(qs.size(), 0);

    // valid username
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::Equals, "foouser"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` = ?"));
    QCOMPARE(qs.size(), 1);
    User *other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    // chain filters
    qs = qs.filter(QDjangoWhere("password", QDjangoWhere::Equals, "foopass"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` = ? AND `user`.`password` = ?"));
    QCOMPARE(qs.size(), 1);

    // username in list
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::IsIn, QList<QVariant>() << "foouser" << "wizuser"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` IN (?, ?)"));
    QCOMPARE(qs.size(), 2);

    // two tests on username
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::Equals, "foouser") ||
                      QDjangoWhere("username", QDjangoWhere::Equals, "baruser"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` = ? OR `user`.`username` = ?"));
    QCOMPARE(qs.size(), 2);
}

/** Test filtering users with a "like" condition.
 */
void TestUser::filterLike()
{
    loadFixtures();

    // username starts with "foo"
    const QDjangoQuerySet<User> users;
    QDjangoQuerySet<User> qs = users.filter(QDjangoWhere("username", QDjangoWhere::StartsWith, "foo"));
    QCOMPARE(qs.size(), 1);
    User *other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    // username ends with "ouser"
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::EndsWith, "ouser"));
    QCOMPARE(qs.size(), 1);
    other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    // username contains "ou"
    qs = users.filter(QDjangoWhere("username", QDjangoWhere::Contains, "ou"));
    QCOMPARE(qs.size(), 1);
    other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;
}

/** Test excluding users with an "=" condition.
 */
void TestUser::exclude()
{
    loadFixtures();
    const QDjangoQuerySet<User> users;

    QDjangoQuerySet<User> qs = users.all();
    QCOMPARE(qs.where().sql(), QLatin1String(""));
    QCOMPARE(users.all().size(), 3);

    qs = users.exclude(QDjangoWhere("username", QDjangoWhere::Equals, "doesnotexist"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` != ?"));
    QCOMPARE(qs.size(), 3);

    qs = users.exclude(QDjangoWhere("username", QDjangoWhere::Equals, "baruser"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` != ?"));
    QCOMPARE(qs.size(), 2);
    User *other = qs.at(0);
    QVERIFY(other != 0);
    QCOMPARE(other->username(), QLatin1String("foouser"));
    QCOMPARE(other->password(), QLatin1String("foopass"));
    delete other;

    qs = qs.exclude(QDjangoWhere("password", QDjangoWhere::Equals, "barpass"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` != ? AND `user`.`password` != ?"));
    QCOMPARE(qs.size(), 2);
}

/** Test limiting user results.
 */
void TestUser::limit()
{
    const QDjangoQuerySet<User> users;

    for (int i = 0; i < 10; i++)
    {
        User user;
        user.setUsername(QString("foouser%1").arg(i));
        user.setPassword(QString("foopass%1").arg(i));
        QCOMPARE(user.save(), true);
    }

    // all results
    QDjangoQuerySet<User> qs = users.limit(0, -1);
    QCOMPARE(qs.size(), 10);

    // all results from offset 1
    qs = users.limit(1, -1);
    QCOMPARE(qs.size(), 9);
    User *other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser1"));
    delete other;
    other = qs.at(8);
    QCOMPARE(other->username(), QLatin1String("foouser9"));
    delete other;

    // 5 results from offset 0
    qs = users.limit(0, 5);
    QCOMPARE(qs.size(), 5);
    other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser0"));
    delete other;
    other = qs.at(4);
    QCOMPARE(other->username(), QLatin1String("foouser4"));
    delete other;

    // 6 results from offset 1
    qs = users.limit(1, 8);
    QCOMPARE(qs.size(), 8);
    other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser1"));
    delete other;
    other = qs.at(7);
    QCOMPARE(other->username(), QLatin1String("foouser8"));
    delete other;
}

/** Test chaining limit statements.
 */
void TestUser::subLimit()
{
    const QDjangoQuerySet<User> users;

    for (int i = 0; i < 10; i++)
    {
        User user;
        user.setUsername(QString("foouser%1").arg(i));
        user.setPassword(QString("foopass%1").arg(i));
        QCOMPARE(user.save(), true);
    }

    // base query : 8 results from offset 1
    QDjangoQuerySet<User> refQs = users.limit(1, 8);

    // all sub-results from offset 2
    QDjangoQuerySet<User> qs = refQs.limit(2, -1);
    QCOMPARE(qs.size(), 6);
    User *other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser3"));
    delete other;
    other = qs.at(5);
    QCOMPARE(other->username(), QLatin1String("foouser8"));
    delete other;

    // 4 sub-results from offset 0
    qs = refQs.limit(0, 4);
    QCOMPARE(qs.size(), 4);
    other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser1"));
    delete other;
    other = qs.at(3);
    QCOMPARE(other->username(), QLatin1String("foouser4"));
    delete other;

    // 3 sub-results from offset 2
    qs = refQs.limit(2, 3);
    QCOMPARE(qs.size(), 3);
    other = qs.at(0);
    QCOMPARE(other->username(), QLatin1String("foouser3"));
    delete other;
    other = qs.at(2);
    QCOMPARE(other->username(), QLatin1String("foouser5"));
    delete other;
}

/** Test retrieving maps of values.
 */
void TestUser::values()
{
    loadFixtures();

    const QDjangoQuerySet<User> users;

    // FIXME : test last_login
    QList< QMap<QString, QVariant> > map = users.all().values();
    QCOMPARE(map.size(), 3);
    QCOMPARE(map[0].keys(), QList<QString>() << "date_joined" << "email" << "first_name" << "id" << "is_active" << "is_staff" << "is_superuser" << "last_login" << "last_name" << "password" << "username");
    QCOMPARE(map[0]["username"], QVariant("foouser"));
    QCOMPARE(map[0]["password"], QVariant("foopass"));
    QCOMPARE(map[1].keys(), QList<QString>() << "date_joined" << "email" << "first_name" << "id" << "is_active" << "is_staff" << "is_superuser" << "last_login" << "last_name" << "password" << "username");
    QCOMPARE(map[1]["username"], QVariant("baruser"));
    QCOMPARE(map[1]["password"], QVariant("barpass"));
    QCOMPARE(map[2].keys(), QList<QString>() << "date_joined" << "email" << "first_name" << "id" << "is_active" << "is_staff" << "is_superuser" << "last_login" << "last_name" << "password" << "username");
    QCOMPARE(map[2]["username"], QVariant("wizuser"));
    QCOMPARE(map[2]["password"], QVariant("wizpass"));

    // FIXME : test last_login
    map = users.all().values(QStringList() << "username" << "password");
    QCOMPARE(map.size(), 3);
    QCOMPARE(map[0].keys(), QList<QString>() << "password" << "username");
    QCOMPARE(map[0]["username"], QVariant("foouser"));
    QCOMPARE(map[0]["password"], QVariant("foopass"));
    QCOMPARE(map[1].keys(), QList<QString>() << "password" << "username");
    QCOMPARE(map[1]["username"], QVariant("baruser"));
    QCOMPARE(map[1]["password"], QVariant("barpass"));
    QCOMPARE(map[2].keys(), QList<QString>() << "password" << "username");
    QCOMPARE(map[2]["username"], QVariant("wizuser"));
    QCOMPARE(map[2]["password"], QVariant("wizpass"));
}

/** Test retrieving lists of values.
 */
void TestUser::valuesList()
{
    loadFixtures();

    const QDjangoQuerySet<User> users;

    // FIXME : test last_login
    QList< QList<QVariant> > list = users.all().valuesList();
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0].size(), 11);
    QCOMPARE(list[0][1], QVariant("foouser"));
    QCOMPARE(list[0][5], QVariant("foopass"));
    QCOMPARE(list[1].size(), 11);
    QCOMPARE(list[1][1], QVariant("baruser"));
    QCOMPARE(list[1][5], QVariant("barpass"));
    QCOMPARE(list[2].size(), 11);
    QCOMPARE(list[2][1], QVariant("wizuser"));
    QCOMPARE(list[2][5], QVariant("wizpass"));

    // FIXME : test last_login
    list = users.all().valuesList(QStringList() << "username" << "password");
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0].size(), 2);
    QCOMPARE(list[0][0], QVariant("foouser"));
    QCOMPARE(list[0][1], QVariant("foopass"));
    QCOMPARE(list[1].size(), 2);
    QCOMPARE(list[1][0], QVariant("baruser"));
    QCOMPARE(list[1][1], QVariant("barpass"));
    QCOMPARE(list[2].size(), 2);
    QCOMPARE(list[2][0], QVariant("wizuser"));
    QCOMPARE(list[2][1], QVariant("wizpass"));
}

/** Clear database table after each test.
 */
void TestUser::cleanup()
{
    QCOMPARE(QDjangoQuerySet<User>().remove(), true);
}

/** Drop database table after running tests.
 */
void TestUser::cleanupTestCase()
{
    QCOMPARE(QDjango::registerModel<User>().dropTable(), true);
}

/** Create database tables before running tests.
 */
void TestRelated::initTestCase()
{
    QCOMPARE(QDjango::registerModel<User>().createTable(), true);
    QCOMPARE(QDjango::registerModel<Group>().createTable(), true);
    QCOMPARE(QDjango::registerModel<Message>().createTable(), true);
    QCOMPARE(QDjango::registerModel<UserGroups>().createTable(), true);
}

/** Set and get foreign key on a Message object.
 */
void TestRelated::testRelated()
{
    const QDjangoQuerySet<Message> messages;
    // load fixtures
    QVariant userPk;
    {
        User *user = new User;
        user->setUsername("foouser");
        user->setPassword("foopass");
        QCOMPARE(user->save(), true);
        userPk = user->pk();

        Message message;
        message.setUser(user);
        message.setText("test message");
        QCOMPARE(message.save(), true);
    }

    // retrieve message, then user (2 SQL queries)
    Message *uncached = messages.get(
        QDjangoWhere("id", QDjangoWhere::Equals, 1));
    QVERIFY(uncached != 0);
    QCOMPARE(uncached->property("user_id"), userPk);

    // check related user
    User *uncachedUser = uncached->user();
    QVERIFY(uncachedUser != 0);
    QCOMPARE(uncachedUser->pk(), userPk);
    QCOMPARE(uncachedUser->username(), QLatin1String("foouser"));
    QCOMPARE(uncachedUser->password(), QLatin1String("foopass"));
    delete uncached;

    // retrieve message and user (1 SQL query)
    Message *cached = messages.selectRelated().get(
        QDjangoWhere("id", QDjangoWhere::Equals, 1));
    QVERIFY(cached != 0);
    QCOMPARE(cached->property("user_id"), userPk);

    // check related user
    User *cachedUser = cached->user();
    QVERIFY(cachedUser != 0);
    QCOMPARE(cachedUser->pk(), userPk);
    QCOMPARE(cachedUser->username(), QLatin1String("foouser"));
    QCOMPARE(cachedUser->password(), QLatin1String("foopass"));
    delete cached;
}

/** Perform filtering on a foreign field.
 */
void TestRelated::filterRelated()
{
    const QDjangoQuerySet<Message> messages;
    // load fixtures
    QVariant userPk;
    {
        User *user = new User;
        user->setUsername("foouser");
        user->setPassword("foopass");
        QCOMPARE(user->save(), true);
        userPk = user->pk();

        Message message;
        message.setUser(user);
        message.setText("test message");
        QCOMPARE(message.save(), true);
    }

    // perform filtering
    QDjangoQuerySet<Message> qs = messages.filter(
        QDjangoWhere("user__username", QDjangoWhere::Equals, "foouser"));
    QCOMPARE(qs.where().sql(), QLatin1String("`user`.`username` = ?"));
    QCOMPARE(qs.size(), 1);

    Message *msg = qs.at(0);
    QVERIFY(msg != 0);
    QCOMPARE(msg->text(), QLatin1String("test message"));
    QCOMPARE(msg->property("user_id"), userPk);
    delete msg;
}

/** Test many-to-many relationships using an intermediate table.
 */
void TestRelated::testGroups()
{
    const QDjangoQuerySet<UserGroups> userGroups;

    User *user = new User;
    user->setUsername("foouser");
    user->setPassword("foopass");
    QCOMPARE(user->save(), true);

    Group *group = new Group;
    group->setName("foogroup");
    QCOMPARE(group->save(), true);

    UserGroups userGroup;
    userGroup.setUser(user);
    userGroup.setGroup(group);
    QCOMPARE(userGroup.save(), true);
    
    UserGroups *ug = userGroups.selectRelated().get(
        QDjangoWhere("id", QDjangoWhere::Equals, 1));
    QVERIFY(ug != 0);
    QCOMPARE(ug->property("user_id"), user->pk());
    QCOMPARE(ug->property("group_id"), group->pk());
    delete ug;
}

/** Clear database tables after each test.
 */
void TestRelated::cleanup()
{
    QCOMPARE(QDjangoQuerySet<User>().remove(), true);
    QCOMPARE(QDjangoQuerySet<Group>().remove(), true);
    QCOMPARE(QDjangoQuerySet<Message>().remove(), true);
    QCOMPARE(QDjangoQuerySet<UserGroups>().remove(), true);
}

/** Drop database tables after running tests.
 */
void TestRelated::cleanupTestCase()
{
    QCOMPARE(QDjango::registerModel<User>().dropTable(), true);
    QCOMPARE(QDjango::registerModel<Group>().dropTable(), true);
    QCOMPARE(QDjango::registerModel<Message>().dropTable(), true);
    QCOMPARE(QDjango::registerModel<UserGroups>().dropTable(), true);
}

