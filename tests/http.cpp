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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtTest>
#include <QUrl>

#include "QDjangoHttpServer.h"

#include "http.h"

void TestHttp::initTestCase()
{
    httpServer = new QDjangoHttpServer(this);
    QCOMPARE(httpServer->listen(QHostAddress::LocalHost, 8123), true);
}

void TestHttp::testGet()
{
    QNetworkAccessManager network;
    QNetworkReply *reply = network.get(QNetworkRequest(QUrl("http://127.0.0.1:8123/")));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QCOMPARE(reply->error(), QNetworkReply::ContentNotFoundError);
}

