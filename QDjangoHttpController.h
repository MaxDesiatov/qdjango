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

#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <QDateTime>
#include <QString>

class QDjangoHttpRequest;
class QDjangoHttpResponse;

QString httpDateTime(const QDateTime &dt);
QDateTime httpDateTime(const QString &str);

/** The QDjangoHttpController class is the base class for HTTP request handlers.
 */
class QDjangoHttpController
{
public:
    virtual QDjangoHttpResponse *respondToRequest(const QDjangoHttpRequest &request) = 0;

    // common error responses
    static QDjangoHttpResponse *serveBadRequest(const QDjangoHttpRequest &request);
    static QDjangoHttpResponse *serveInternalServerError(const QDjangoHttpRequest &request);
    static QDjangoHttpResponse *serveNotFound(const QDjangoHttpRequest &request);

protected:
    static QDjangoHttpResponse *serveError(const QDjangoHttpRequest &request, int code, const QString &text);
    QDjangoHttpResponse *serveRedirect(const QDjangoHttpRequest &request, const QString &url) const;
    QDjangoHttpResponse *serveStatic(const QDjangoHttpRequest &request, const QString &filePath, const QDateTime &expires = QDateTime()) const;
};

#endif
