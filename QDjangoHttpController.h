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
class QUrl;

QString httpDateTime(const QDateTime &dt);
QDateTime httpDateTime(const QString &str);

/** \brief The QDjangoHttpController class is the base class for HTTP request handlers.
 */
class QDjangoHttpController
{
public:
    /** \brief Responds to an HTTP request.
     *
     * Reimplement this method when creating a subclass of QDjangoHttpController.
     */
    virtual QDjangoHttpResponse *respondToRequest(const QDjangoHttpRequest &request) = 0;

    // common responses
    static QDjangoHttpResponse *serveBadRequest(const QDjangoHttpRequest &request);
    static QDjangoHttpResponse *serveInternalServerError(const QDjangoHttpRequest &request);
    static QDjangoHttpResponse *serveNotFound(const QDjangoHttpRequest &request);
    static QDjangoHttpResponse *serveRedirect(const QDjangoHttpRequest &request, const QUrl &url, bool permanent = false);
    static QDjangoHttpResponse *serveStatic(const QDjangoHttpRequest &request, const QString &filePath, const QDateTime &expires = QDateTime());

private:
    static QDjangoHttpResponse *serveError(const QDjangoHttpRequest &request, int code, const QString &text);
};

#endif
