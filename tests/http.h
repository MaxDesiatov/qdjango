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

class QDjangoHttpController;
class QDjangoHttpServer;

/** Test QDjangoServer class.
 */
class TestHttp : public QObject
{
    Q_OBJECT

private slots:
    void cleanupTestCase();
    void initTestCase();
    void testGet_data();
    void testGet();

private:
    QDjangoHttpController *httpController;
    QDjangoHttpServer *httpServer;
};

