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

#include <QObject>

/** Tests for the User class.
 */
class TestUser: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void create();
    void remove();
    void removeFilter();
    void removeLimit();
    void get();
    void filter();
    void filterLike();
    void exclude();
    void limit();
    void subLimit();
    void values();
    void valuesList();
    void cleanup();
    void cleanupTestCase();

private:
    void loadFixtures();
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

