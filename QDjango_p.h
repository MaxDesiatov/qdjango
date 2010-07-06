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

#ifndef QDJANGO_P_H
#define QDJANGO_P_H

#include <QMap>
#include <QObject>
#include <QSqlDatabase>
#include <QStringList>
#include <QVariant>

class QDjangoModel;

/** \internal
 */
class QDjangoMetaField
{
public:
    QDjangoMetaField();

    QByteArray name;
    QVariant::Type type;
    bool autoIncrement;
    bool index;
    int maxLength;
    bool primaryKey;
    QString foreignName;
    QString foreignModel;
};

/** \internal
 */
class QDjangoMetaModel
{
public:
    QDjangoMetaModel(const QDjangoModel *model = 0);
    QString databaseColumn(const QString &name, bool *needsJoin = 0) const;

    bool createTable() const;
    bool dropTable() const;

    void load(QObject *model, const QMap<QString, QVariant> &props) const;
    bool remove(QObject *model) const;
    bool save(QObject *model) const;

    QByteArray primaryKey() const;

private:
    QList<QDjangoMetaField> m_localFields;
    QMap<QString, QString> m_foreignFields;
    QByteArray m_primaryKey;
    QByteArray m_table;

    friend class QDjangoQueryBase;
};

/** \internal
 */
class QDjangoWatcher : public QObject
{
    Q_OBJECT

public:
    QDjangoWatcher(QObject *parent = 0);
    QSqlDatabase reference;
    QMap<QThread*, QSqlDatabase> copies;
    qint64 connectionId;

private slots:
    void threadFinished();
};

#endif
