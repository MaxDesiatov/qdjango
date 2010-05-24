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

#include "QDjangoQuerySet.h"

#include "models.h"

QString User::username() const
{
    return m_username;
}

void User::setUsername(const QString &username)
{
    m_username = username;
}

QString User::password() const
{
    return m_password;
}

void User::setPassword(const QString &password)
{
    m_password = password;
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
    addForeignKey("user_id", new User);
    addForeignKey("group_id", new Group);
}

User *UserGroups::user() const
{
    return qobject_cast<User*>(foreignKey("user_id"));
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
    return qobject_cast<Group*>(foreignKey("group_id"));
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
    addForeignKey("user_id", new User);
}

User *Message::user() const
{
    return qobject_cast<User*>(foreignKey("user_id"));
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

