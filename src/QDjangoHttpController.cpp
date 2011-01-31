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
#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include "QDjangoHttpController.h"
#include "QDjangoHttpRequest.h"
#include "QDjangoHttpResponse.h"

QString httpDateTime(const QDateTime &dt)
{
    if (dt.isValid())
        return dt.toUTC().toString("ddd, dd MMM yyyy HH:mm:ss") + " GMT";
    return QString();
}

QDateTime httpDateTime(const QString &str)
{
    QDateTime dt = QDateTime::fromString(str.left(25), "ddd, dd MMM yyyy HH:mm:ss");
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

QDjangoHttpResponse *QDjangoHttpController::serveError(const QDjangoHttpRequest &request, int code, const QString &text)
{
    QDjangoHttpResponse *response = new QDjangoHttpResponse;
    response->setHeader("Content-Type", "text/html; charset=utf-8");
    response->setStatusCode(code);
    response->setBody(QString("<html>"
        "<head><title>Error</title></head>"
        "<body><p>%1</p></body>"
        "</html>").arg(text).toUtf8());
    return response;
}

/** Respond to a malformed HTTP request.
 *
 * \param request
 */
QDjangoHttpResponse *QDjangoHttpController::serveBadRequest(const QDjangoHttpRequest &request)
{
    return serveError(request, QDjangoHttpResponse::BadRequest, "Your browser sent a malformed request.");
}

/** Respond to an HTTP \a request with an internal server error.
 *
 * \param request
 */
QDjangoHttpResponse *QDjangoHttpController::serveInternalServerError(const QDjangoHttpRequest &request)
{
    return serveError(request, QDjangoHttpResponse::InternalServerError, "An internal server error was encountered.");
}

/** Respond to an HTTP \a request with a not found error.
 *
 * \param request
 */
QDjangoHttpResponse *QDjangoHttpController::serveNotFound(const QDjangoHttpRequest &request)
{
    return serveError(request, QDjangoHttpResponse::NotFound, "The document you requested was not found.");
}

/** Respond to an HTTP \a request with a redirect.
 *
 * \param request
 * \param url The URL to which the user is redirected.
 * \param permanent Whether the redirect is permanent.
 */
QDjangoHttpResponse *QDjangoHttpController::serveRedirect(const QDjangoHttpRequest &request, const QUrl &url, bool permanent)
{
    const QString urlString = url.toString();
    QDjangoHttpResponse *response = serveError(request, permanent ? QDjangoHttpResponse::MovedPermanently : QDjangoHttpResponse::Found,
        QString("You are being redirect to <a href=\"%1\">%2</a>").arg(urlString, urlString));
    response->setHeader("Location", urlString.toUtf8());
    return response;
}

/** Respond to an HTTP \a request for a static file.
 *
 * \param request
 * \param docPath The path to the document, such that it can be opened using a QFile.
 * \param expires An optional expiry date.
 */
QDjangoHttpResponse *QDjangoHttpController::serveStatic(const QDjangoHttpRequest &request, const QString &docPath, const QDateTime &expires)
{
    QFileInfo info(docPath);
    if (!info.isFile())
        return serveNotFound(request);
    const QString fileName = info.fileName();

    QDjangoHttpResponse *response = new QDjangoHttpResponse;
    response->setStatusCode(QDjangoHttpResponse::OK);

    // determine last modified date
    QDateTime lastModified = info.lastModified();
    if (docPath.startsWith(":/"))
        lastModified = QFileInfo(qApp->applicationFilePath()).lastModified();
    if (lastModified.isValid())
        response->setHeader("Last-Modified", httpDateTime(lastModified));

    // cache expiry
    if (expires.isValid())
        response->setHeader("Expires", httpDateTime(expires));

    // handle if-modified-since
    const QDateTime ifModifiedSince = httpDateTime(request.header("If-Modified-Since"));
    if (lastModified.isValid() && ifModifiedSince.isValid() && lastModified <= ifModifiedSince)
    {
        response->setStatusCode(304);
        return response;
    }

    // determine content type
    QString mimeType("application/octet-stream");
    if (fileName.endsWith(".css"))
        mimeType = "text/css";
    else if (fileName.endsWith(".html"))
        mimeType = "text/html";
    else if (fileName.endsWith(".js"))
        mimeType = "application/javascript";
    else if (fileName.endsWith(".png"))
        mimeType = "image/png";
    response->setHeader("Content-Type", mimeType);

    // read contents
    QFile file(docPath);
    if (!file.open(QIODevice::ReadOnly)) {
        delete response;
        return serveInternalServerError(request);
    }
    response->setBody(file.readAll());
    return response;
}

