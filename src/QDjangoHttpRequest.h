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

#ifndef QDJANGO_HTTP_REQUEST_H
#define QDJANGO_HTTP_REQUEST_H

#include <QString>

class QDjangoHttpRequest;
class QDjangoHttpRequestPrivate;

/** \brief The QDjangoHttpRequest class represents an HTTP request.
 *
 * \ingroup Http
 */
class QDjangoHttpRequest
{
public:
    QDjangoHttpRequest();
    ~QDjangoHttpRequest();

    QByteArray body() const;
    QString header(const QString &key) const;
    QString method() const;
    QString path() const;

    bool isFinished() const;
    bool isValid() const;

private:
    Q_DISABLE_COPY(QDjangoHttpRequest)
    QDjangoHttpRequestPrivate* const d;
    friend class QDjangoHttpConnection;
};

#endif
