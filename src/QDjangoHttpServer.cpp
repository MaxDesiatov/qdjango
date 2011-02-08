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
#include <QDateTime>
#include <QDebug>
#include <QTcpSocket>

#include "QDjangoHttpController.h"
#include "QDjangoHttpRequest.h"
#include "QDjangoHttpRequest_p.h"
#include "QDjangoHttpResponse.h"
#include "QDjangoHttpResponse_p.h"
#include "QDjangoHttpServer.h"
#include "QDjangoHttpServer_p.h"

//#define DEBUG_HTTP

// maximum request body size is 10 MB
#define MAX_BODY_SIZE (10 * 1024 * 1024)

class QDjangoHttpConnectionPrivate
{
public:
    bool closeAfterResponse;
    QDjangoHttpServer *server;
    QTcpSocket *socket;
    QDjangoHttpRequest *pendingRequest;
    QList<QDjangoHttpResponse*> pendingResponses;
    int requestCount;
};

/** Constructs a new HTTP connection.
 */
QDjangoHttpConnection::QDjangoHttpConnection(int socketDescriptor, QDjangoHttpServer *server)
    : QObject(server),
    d(new QDjangoHttpConnectionPrivate)
{
    bool check;

    d->closeAfterResponse = false;
    d->pendingRequest = 0;
    d->requestCount = 0;
    d->server = server;
    d->socket = new QTcpSocket(this);
    d->socket->setSocketDescriptor(socketDescriptor);

    check = connect(d->socket, SIGNAL(bytesWritten(qint64)),
                    this, SLOT(bytesWritten(qint64)));
    Q_ASSERT(check);

    check = connect(d->socket, SIGNAL(disconnected()),
                    this, SIGNAL(closed()));
    Q_ASSERT(check);

    check = connect(d->socket, SIGNAL(readyRead()),
                    this, SLOT(handleData()));
    Q_ASSERT(check);
}

/** Destroys the HTTP connection.
 */
QDjangoHttpConnection::~QDjangoHttpConnection()
{
    if (d->pendingRequest)
        delete d->pendingRequest;
    foreach (QDjangoHttpResponse *response, d->pendingResponses)
        delete response;
    delete d;
}

/** When bytes have been written, check whether we need to close
 *  the connection.
 *
 * @param bytes
 */
void QDjangoHttpConnection::bytesWritten(qint64 bytes)
{
    if (!d->socket->bytesToWrite()) {
        if (!d->pendingResponses.isEmpty()) {
            writeResponse();
        } else if (d->closeAfterResponse) {
#ifdef DEBUG_HTTP
            qDebug("Closing connection");
#endif
            d->socket->close();
            emit closed();
        }
    }
}

/** Handle incoming data on the socket.
 */
void QDjangoHttpConnection::handleData()
{
    /* Receive request */
    QDjangoHttpRequest *request = d->pendingRequest ? d->pendingRequest : new QDjangoHttpRequest;
    request->d->readFromSocket(d->socket);
    if (!request->isFinished())
    {
        d->pendingRequest = request;
        return;
    }
    d->pendingRequest = 0;

#ifdef DEBUG_HTTP
    qDebug("Handling request %i", d->requestCount++);
#endif

    /* Process request */
    bool keepAlive = request->d->header.majorVersion() >= 1 && request->d->header.minorVersion() >= 1;
    if (request->header("Connection").toLower() == QLatin1String("keep-alive"))
        keepAlive = true;
    else if (request->header("Connection").toLower() == QLatin1String("close"))
        keepAlive = false;

    QDjangoHttpController *controller = d->server->controller();
    QDjangoHttpResponse *response = 0;
    if (!request->isValid())
        response = QDjangoHttpController::serveBadRequest(*request);
    else if (!controller)
        response = QDjangoHttpController::serveNotFound(*request);
    else
        response = controller->respondToRequest(*request);
    d->pendingResponses << response;
    delete request;

    /* Store keep-alive flag */
    if (!keepAlive)
        d->closeAfterResponse = true;

    connect(response, SIGNAL(ready()), this, SLOT(writeResponse()));
    writeResponse();
}

void QDjangoHttpConnection::writeResponse()
{
    while (!d->pendingResponses.isEmpty()) {
        QDjangoHttpResponse *response = d->pendingResponses.first();
        if (!response->isReady())
            return;

        /* Finalise response */
        response->setHeader("Date", httpDateTime(QDateTime::currentDateTime()));
        response->setHeader("Server", QString("%1/%2").arg(qApp->applicationName(), qApp->applicationVersion()));
        response->setHeader("Connection", d->closeAfterResponse ? "close" : "keep-alive");

        /* Send response */
        d->socket->write(response->d->header.toString().toUtf8());
        d->socket->write(response->d->body);

        /* Destroy response */
        d->pendingResponses.removeAll(response);
        response->deleteLater();
    }
}

/** \internal
 */
class QDjangoHttpServerPrivate
{
public:
    int connectionCount;
    QDjangoHttpController *requestHandler;
};

/** Constructs a new HTTP server.
 */
QDjangoHttpServer::QDjangoHttpServer(QObject *parent)
    : QTcpServer(parent),
    d(new QDjangoHttpServerPrivate)
{
    d->connectionCount = 0;
    d->requestHandler = 0;
}

/** Destroys the HTTP server.
 */
QDjangoHttpServer::~QDjangoHttpServer()
{
    delete d;
}

/** Handles the creation of a new HTTP connection.
 *
 * \param socketDescriptor
 */
void QDjangoHttpServer::incomingConnection(int socketDescriptor)
{
    QDjangoHttpConnection *connection = new QDjangoHttpConnection(socketDescriptor, this);

#ifdef DEBUG_HTTP
    qDebug("Handling connection %i", d->connectionCount++);
#endif

    bool check;
    check = connect(connection, SIGNAL(closed()),
                    connection, SLOT(deleteLater()));
    Q_ASSERT(check);
}

/** Returns the controller which serves requests received by the server.
 */
QDjangoHttpController *QDjangoHttpServer::controller() const
{
    return d->requestHandler;
}

/** Sets the controller which serves requests received by the server.
 *
 * \param controller
 */
void QDjangoHttpServer::setController(QDjangoHttpController *controller)
{
    d->requestHandler = controller;
}
