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

#ifndef QDJANGO_HTTP_RESPONSE_H
#define QDJANGO_HTTP_RESPONSE_H

#include <QObject>

class QDjangoHttpResponsePrivate;

/** The QDjangoHttpResponse class represents an HTTP response.
 */
class QDjangoHttpResponse : public QObject
{
    Q_OBJECT

public:
    enum HttpStatus {
        OK                      = 200,
        MovedPermanently        = 301,
        Found                   = 302,
        NotModified             = 304,
        BadRequest              = 400,
        AuthorizationRequired   = 401,
        NotFound                = 404,
        MethodNotAllowed        = 405,
        InternalServerError     = 500,
    };

    QDjangoHttpResponse();
    ~QDjangoHttpResponse();

    QByteArray body() const;
    void setBody(const QByteArray &body);

    QString header(const QString &key) const;
    void setHeader(const QString &key, const QString &value);

    bool isReady() const;
    void setReady(bool ready);

    int statusCode() const;
    void setStatusCode(int code);

signals:
    void readyRead();

private:
    Q_DISABLE_COPY(QDjangoHttpResponse)
    QDjangoHttpResponsePrivate* const d;
    friend class QDjangoHttpConnection;
};

#endif
