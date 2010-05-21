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

#include "QDjangoModel.h"

class User : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

private:
    QString m_username;
    QString m_password;
};

class Group : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QString name() const;
    void setName(const QString &name);

private:
    QString m_name;
};

class UserGroups : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(int user_id READ userId WRITE setUserId)
    Q_PROPERTY(int group_id READ groupId WRITE setGroupId)

public:
    UserGroups(QObject *parent = 0);

    User *user() const;
    int userId() const;
    void setUserId(int userId);

    Group *group() const;
    int groupId() const;
    void setGroupId(int groupId);

private:
    int m_userId;
    int m_groupId;
};

class Message : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(int user_id READ userId WRITE setUserId)
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    Message(QObject *parent = 0);

    User *user() const;
    int userId() const;
    void setUserId(int userId);

    QString text() const;
    void setText(const QString &text);

private:
    int m_userId;
    QString m_text;
};

