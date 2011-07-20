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

#ifndef QDJANGO_HTTP_SERVER_P_H
#define QDJANGO_HTTP_SERVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QDjango API.
//

#include <QObject>

class QDjangoHttpConnectionPrivate;
class QDjangoHttpRequest;
class QDjangoHttpResponse;
class QDjangoHttpServer;

/** \internal
 */
class QDjangoHttpConnection : public QObject
{
    Q_OBJECT

public:
    QDjangoHttpConnection(int socketDescriptor, QDjangoHttpServer *server);
    ~QDjangoHttpConnection();

signals:
    /** This signal is emitted when the connection is closed.
     */
    void closed();

    /** This signal is emitted when a request completes.
     */
    void requestFinished(QDjangoHttpRequest *request, QDjangoHttpResponse *response);

private slots:
    void bytesWritten(qint64 bytes);
    void handleData();
    void writeResponse();

private:
    Q_DISABLE_COPY(QDjangoHttpConnection)
    QDjangoHttpConnectionPrivate* const d;
};

#endif
