/*
 * QDjango
 * Copyright (C) 2010 Bolloré telecom
 * See AUTHORS file for a full list of contributors.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QDJANGO_MODEL_H
#define QDJANGO_MODEL_H

#include <QObject>

class QDjangoModel;
class QSqlDatabase;
class QSqlQuery;

void sqlDebug(const QSqlQuery &query);
bool sqlExec(QSqlQuery &query);

#define Q_MODEL(x) QDjango::model(x::staticMetaObject.className())

class QDjango
{
public:
    static void createTables();

    static const QDjangoModel *model(const QString &name);
    static bool registerModel(QDjangoModel *model);
};

template <class T>
void qDjangoRegisterModel()
{
    if (!QDjango::model(T::staticMetaObject.className()))
    {
        T *model = new T;
        if (!QDjango::registerModel(model))
            delete model;
    }
}

class QDjangoModel : public QObject
{
    Q_OBJECT

public:
    QDjangoModel(QObject *parent = 0);

    QVariant pk() const;
    void setPk(const QVariant &pk);

    bool createTable() const;
    bool dropTable() const;

    static QSqlDatabase &database();
    static void setDatabase(QSqlDatabase *database);

    QString databaseTable() const;
    QString databasePkName() const;
    void setPrimaryKey(const QString &key);

    QStringList databaseFields() const;

public slots:
    bool remove();
    bool save();

private:
    int m_id;
    QString m_pkName;
};

#endif
