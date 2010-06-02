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

#include <QObject>

class TestWhere : public QObject
{
    Q_OBJECT

private slots:
    void quoting();
    void emptyWhere();
    void equalsWhere();
    void notEqualsWhere();
    void greaterThan();
    void greaterOrEquals();
    void lessThan();
    void lessOrEquals();
    void andWhere();
    void orWhere();
    void complexWhere();
};

class TestModel: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void createUser();
    void removeUser();
    void removeUsers();
    void getUser();
    void filterUsers();
    void excludeUsers();
    void limit();
    void subLimit();
    void values();
    void valuesList();
    void cleanup();
    void cleanupTestCase();
};

class TestRelated : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testGroups();
    void testRelated();
    void filterRelated();
    void cleanup();
    void cleanupTestCase();
};

