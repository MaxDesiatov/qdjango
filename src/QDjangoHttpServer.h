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

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <QTcpServer>

class QDjangoHttpController;
class QDjangoHttpRequest;
class QDjangoHttpResponse;
class QDjangoHttpServer;
class QDjangoHttpServerPrivate;

/** \defgroup Http */

/** \brief The QDjangoHttpServer class represents an HTTP server.
 *
 * \ingroup Http
 */
class QDjangoHttpServer : public QTcpServer
{
    Q_OBJECT

public:
    QDjangoHttpServer(QObject *parent = 0);
    ~QDjangoHttpServer();

    QDjangoHttpController *controller() const;
    void setController(QDjangoHttpController *controller);

signals:
    /** This signal is emitted when a request completes.
     */
    void requestFinished(QDjangoHttpRequest *request, QDjangoHttpResponse *response);

protected:
    void incomingConnection(int socketDescriptor);

private:
    Q_DISABLE_COPY(QDjangoHttpServer)
    QDjangoHttpServerPrivate* const d;
};

#endif
