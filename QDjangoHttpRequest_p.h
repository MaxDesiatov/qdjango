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

#ifndef QDJANGO_HTTP_REQUEST_P_H
#define QDJANGO_HTTP_REQUEST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qjango API.
//

#include <QHttpRequestHeader>

class QDjangoHttpRequestPrivate
{
public:
    void readFromSocket(QIODevice *socket);

    QByteArray buffer;
    qint64 bytesRemaining;
    QHttpRequestHeader header;
    bool headerReceived;
    bool valid;
};

#endif
