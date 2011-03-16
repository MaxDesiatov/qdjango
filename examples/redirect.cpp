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

#include <QCoreApplication>

#include "QDjangoHttpController.h"
#include "QDjangoHttpResponse.h"
#include "QDjangoHttpServer.h"

class RedirectController : public QDjangoHttpController
{
public:
    QDjangoHttpResponse* respondToRequest(const QDjangoHttpRequest &request)
    {
        QDjangoHttpResponse *response = new QDjangoHttpResponse;
        response->setStatusCode(QDjangoHttpResponse::Found);
        response->setHeader("Location", "http://www.google.com/");
        return response;
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    RedirectController controller;

    QDjangoHttpServer server;
    server.setController(&controller);
    server.listen(QHostAddress::Any, 8091);

    return app.exec();
}

