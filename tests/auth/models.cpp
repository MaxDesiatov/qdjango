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

#include "QDjangoQuerySet.h"

#include "models.h"

User::User(QObject *parent)
    : QDjangoModel(parent),
    m_isActive(true),
    m_isStaff(false),
    m_isSuperUser(false)
{
    // declare field options
    setFieldOption("username", MaxLengthOption, 30);
    setFieldOption("first_name", MaxLengthOption, 30);
    setFieldOption("last_name", MaxLengthOption, 30);
    setFieldOption("password", MaxLengthOption, 128);

    // initialise dates
    m_dateJoined = QDateTime::currentDateTime();
    m_lastLogin = QDateTime::currentDateTime();
}

QString User::username() const
{
    return m_username;
}

void User::setUsername(const QString &username)
{
    m_username = username;
}

QString User::firstName() const
{
    return m_firstName;
}

void User::setFirstName(const QString &firstName)
{
    m_firstName = firstName;
}

QString User::lastName() const
{
    return m_lastName;
}

void User::setLastName(const QString &lastName)
{
    m_lastName = lastName;
}

QString User::email() const
{
    return m_email;
}

void User::setEmail(const QString &email)
{
    m_email = email;
}

QString User::password() const
{
    return m_password;
}

void User::setPassword(const QString &password)
{
    m_password = password;
}

bool User::isActive() const
{
    return m_isActive;
}

void User::setIsActive(bool isActive)
{
    m_isActive = isActive;
}

bool User::isStaff() const
{
    return m_isStaff;
}

void User::setIsStaff(bool isStaff)
{
    m_isStaff = isStaff;
}

bool User::isSuperUser() const
{
    return m_isSuperUser;
}

void User::setIsSuperUser(bool isSuperUser)
{
    m_isSuperUser = isSuperUser;
}

QDateTime User::dateJoined() const
{
    return m_dateJoined;
}

void User::setDateJoined(const QDateTime &dateJoined)
{
    m_dateJoined = dateJoined;
}

QDateTime User::lastLogin() const
{
    return m_lastLogin;
}

void User::setLastLogin(const QDateTime &lastLogin)
{
    m_lastLogin = lastLogin;
}

QString Group::name() const
{
    return m_name;
}

void Group::setName(const QString &name)
{
    m_name = name;
}

UserGroups::UserGroups(QObject *parent)
    : QDjangoModel(parent), m_userId(0), m_groupId(0)
{
    addForeignKey("user", "user_id", new User);
    addForeignKey("group", "group_id", new Group);
}

User *UserGroups::user() const
{
    return qobject_cast<User*>(foreignKey("user"));
}

int UserGroups::userId() const
{
    return m_userId;
}

void UserGroups::setUserId(int userId)
{
    m_userId = userId;
}

Group *UserGroups::group() const
{
    return qobject_cast<Group*>(foreignKey("group"));
}

int UserGroups::groupId() const
{
    return m_groupId;
}

void UserGroups::setGroupId(int groupId)
{
    m_groupId = groupId;
}

Message::Message(QObject *parent)
    : QDjangoModel(parent), m_userId(0)
{
    addForeignKey("user", "user_id", new User);
}

/** Returns the User associated with this Message.
 */
User *Message::user() const
{
    return qobject_cast<User*>(foreignKey("user"));
}

int Message::userId() const
{
    return m_userId;
}

void Message::setUserId(int userId)
{
    m_userId = userId;
}

QString Message::text() const
{
    return m_text;
}

void Message::setText(const QString &text)
{
    m_text = text;
}

