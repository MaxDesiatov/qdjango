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
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

/** \brief The QDjangoMetaField class holds the database schema for a field.
 *
 * \internal
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
    QString foreignModel;
};

/** \brief The QDjangoMetaModel class holds the database schema for a model.
 *
 *  It manages table creation and deletion operations as well as row
 *  serialisation, deserialisation and deletion operations.
 *
 * \internal
 */
class QDjangoMetaModel
{
public:
    QDjangoMetaModel(const QObject *model = 0);

    QString databaseColumn(const QString &name, bool *needsJoin = 0) const;
    QString databaseTable() const;

    bool isValid() const;

    bool createTable() const;
    bool dropTable() const;

    void load(QObject *model, const QMap<QString, QVariant> &props) const;
    bool remove(QObject *model) const;
    bool save(QObject *model) const;

    QObject *foreignKey(const QObject *model, const char *name) const;
    void setForeignKey(QObject *model, const char *name, QObject *value) const;

    QByteArray primaryKey() const;

private:
    QList<QDjangoMetaField> m_localFields;
    QMap<QByteArray, QString> m_foreignFields;
    QByteArray m_primaryKey;
    QByteArray m_table;

    friend class QDjangoQueryBase;
};

/** \brief The QDjangoDatabase class represents a set of connections to a
 *  database.
 *
 * \internal
 */
class QDjangoDatabase : public QObject
{
    Q_OBJECT

public:
    QDjangoDatabase(QObject *parent = 0);

    QSqlDatabase reference;
    QMutex mutex;
    QMap<QThread*, QSqlDatabase> copies;
    qint64 connectionId;

private slots:
    void threadFinished();
};

class QDjangoQuery : public QSqlQuery
{
public:
    QDjangoQuery(QSqlDatabase db) : QSqlQuery(db)
    {
    };

#ifdef QDJANGO_DEBUG_SQL
    bool exec(QDjangoQuery &query)
    {
        qDebug() << "SQL query" << lastQuery();
        QMapIterator<QString, QVariant> i(boundValues());
        while (i.hasNext()) {
            i.next();
            qDebug() << "   " << i.key().toAscii().data() << "="
                     << i.value().toString().toAscii().data();
        }
        if (!QSqlQuery::exec())
        {
            qWarning() << "SQL error" << lastError();
            return false;
        }
        return true;
    }
#endif
};

#endif
