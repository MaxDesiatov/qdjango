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

#include "models.h"
#include "tests.h"

void TestShares::initTestCase()
{
    QCOMPARE(File().createTable(), true);
}

void TestShares::testFile()
{
    // create a file
    File file;
    file.setDate(QDateTime(QDate(2010, 6, 1), QTime(10, 5, 14)));
    file.setPath("foo/bar.txt");
    file.setSize(1234);
    QCOMPARE(file.save(), true);

    // update the file
    file.setSize(5678);
    QCOMPARE(file.save(), true);
}

void TestShares::cleanup()
{
    QDjangoQuerySet<File>().remove();
}

void TestShares::cleanupTestCase()
{
    QCOMPARE(File().dropTable(), true);
}


