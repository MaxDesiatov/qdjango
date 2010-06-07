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

#ifndef QDJANGO_QUERYSET_P_H
#define QDJANGO_QUERYSET_P_H

#include <QMap>
#include <QStringList>

#include "QDjangoWhere.h"

class QDjangoModel;

/** \internal
 */
class QDjangoQueryBase
{
public:
    QDjangoQueryBase(const QString &modelName);

protected:
    void addFilter(const QDjangoWhere &where);
    void sqlDelete();
    void sqlFetch();
    QString sqlLimit() const;
    bool sqlLoad(QDjangoModel *model, int index);
    QList< QMap<QString, QVariant> > sqlValues(const QStringList &fields);
    QList< QList<QVariant> > sqlValuesList(const QStringList &fields);

    int m_lowMark;
    int m_highMark;
    QDjangoWhere m_where;
    bool m_needsJoin;
    QList< QMap<QString, QVariant> > m_properties;
    bool m_selectRelated;

private:
    QStringList fieldNames(const QDjangoModel *model, QString &from);

    bool m_haveResults;
    QString m_modelName;

friend class QDjangoModel;
};

#endif
