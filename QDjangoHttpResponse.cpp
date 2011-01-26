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

#include <QDateTime>
#include <QHttpResponseHeader>
#include <QTcpSocket>

#include "QDjangoHttpResponse.h"

//#define DEBUG_HTTP

// maximum request body size is 10 MB
#define MAX_BODY_SIZE (10 * 1024 * 1024)

class QDjangoHttpResponsePrivate
{
public:    
    QHttpResponseHeader header;
    QByteArray body;
    bool ready;
};

/** Constructs a new HTTP response.
 */
QDjangoHttpResponse::QDjangoHttpResponse()
    : d(new QDjangoHttpResponsePrivate)
{
    d->ready = true;
    setHeader("Content-Length", "0");
    setStatusCode(QDjangoHttpResponse::OK);
}

/** Destroys the HTTP response.
 */
QDjangoHttpResponse::~QDjangoHttpResponse()
{
    delete d;
}

QByteArray QDjangoHttpResponse::body() const
{
    return d->body;
}

void QDjangoHttpResponse::setBody(const QByteArray &body)
{
    d->body = body;
    setHeader("Content-Length", QString::number(d->body.size()));
}

QString QDjangoHttpResponse::header(const QString &key) const
{
    return d->header.value(key);
}

void QDjangoHttpResponse::setHeader(const QString &key, const QString &value)
{
    d->header.setValue(key, value);
}

bool QDjangoHttpResponse::isReady() const
{
    return d->ready;
}

void QDjangoHttpResponse::setReady(bool ready)
{
    if (ready == d->ready)
        return;

    d->ready = ready;
    if (d->ready)
        emit readyRead();
}

int QDjangoHttpResponse::statusCode() const
{
    return d->header.statusCode();
}

void QDjangoHttpResponse::setStatusCode(int code)
{
    switch(code)
    {
    case OK:
        d->header.setStatusLine(code, "OK");
        break;
    case MovedPermanently:
        d->header.setStatusLine(code, "Moved Permanently");
        break;
    case Found:
        d->header.setStatusLine(code, "Found");
        break;
    case NotModified:
        d->header.setStatusLine(code, "Not Modified");
        break;
    case BadRequest:
        d->header.setStatusLine(code, "Bad Request");
        break;
    case AuthorizationRequired:
        d->header.setStatusLine(code, "Authorization Required");
        break;
    case NotFound:
        d->header.setStatusLine(code, "Not Found");
        break;
    case MethodNotAllowed:
        d->header.setStatusLine(code, "Method Not Allowed");
        break;
    case InternalServerError:
        d->header.setStatusLine(code, "Internal Server Error");
        break;
    default:
        d->header.setStatusLine(code);
        break;
    }
}

