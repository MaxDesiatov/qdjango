/*
 * QDjango
 * Copyright (C) 2010 Bolloré telecom
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

#ifndef QDJANGO_SHARES_MODELS_H
#define QDJANGO_SHARES_MODELS_H

#include <QDateTime>

#include "QDjangoModel.h"

class File : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QDateTime date READ date WRITE setDate)
    Q_PROPERTY(qint64 size READ size WRITE setSize)

public:
    File(QObject *parent = 0);

    QDateTime date() const;
    void setDate(const QDateTime &date);

    QString path() const;
    void setPath(const QString &path);

    qint64 size() const;
    void setSize(qint64 size);

private:
    QString m_path;
    QDateTime m_date;
    qint64 m_size;
};

#endif
