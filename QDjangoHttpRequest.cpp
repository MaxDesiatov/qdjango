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

#include <QIODevice>

#include "QDjangoHttpRequest.h"
#include "QDjangoHttpRequest_p.h"

// maximum request body size is 10 MB
#define MAX_BODY_SIZE (10 * 1024 * 1024)

void QDjangoHttpRequestPrivate::readFromSocket(QIODevice *socket)
{
    /* Read header */
    while (!headerReceived && socket->canReadLine())
    {
        QByteArray line = socket->readLine();
        buffer += line;
        if (line == "\r\n")
        {
            header = QHttpRequestHeader(QString::fromUtf8(buffer));
            buffer.clear();
            int bytes = header.value("Content-Length").toInt();
            if (bytes < 0 || bytes > MAX_BODY_SIZE)
            {
                qWarning("Invalid Content-Length");
                valid = false;
            } else {
                bytesRemaining = bytes;
            }
            headerReceived = true;
        }
    }
    if (!headerReceived)
        return;

    /* Read body */
    if (bytesRemaining > 0)
    {
        const QByteArray chunk = socket->read(bytesRemaining);
        buffer += chunk;
        bytesRemaining -= chunk.size();
    }
}

/** Constructs a new HTTP request.
 */
QDjangoHttpRequest::QDjangoHttpRequest()
    : d(new QDjangoHttpRequestPrivate)
{
    d->bytesRemaining = 0;
    d->headerReceived = false;
    d->valid = true;
}

/** Destroys the HTTP request.
 */
QDjangoHttpRequest::~QDjangoHttpRequest()
{
    delete d;
}

/** Returns the raw body of the HTTP request.
 */
QByteArray QDjangoHttpRequest::body() const
{
    return d->buffer;
}

/** Returns the specified HTTP request header.
 *
 * \param key
 */
QString QDjangoHttpRequest::header(const QString &key) const
{
    return d->header.value(key);
}

/** Returns the HTTP request's method (e.g. GET, POST).
 */
QString QDjangoHttpRequest::method() const
{
    return d->header.method();
}

/** Returns the HTTP request's path.
 */
QString QDjangoHttpRequest::path() const
{
    return d->header.path();
}

/** Returns true if the HTTP request is fully received, false otherwise.
 */
bool QDjangoHttpRequest::isFinished() const
{
    return d->headerReceived && !d->bytesRemaining;
}

/** Returns true if the HTTP request is valid, false otherwise.
 */
bool QDjangoHttpRequest::isValid() const
{
    return d->valid;
}

