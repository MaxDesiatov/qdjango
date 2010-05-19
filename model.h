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

#include <QMap>
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

    static QString quote(const QString &name);
    static QString unquote(const QString &quoted);
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

    static QSqlDatabase database();
    static void setDatabase(QSqlDatabase database);

    QString databaseColumn(const QString &name) const;
    void databaseLoad(const QMap<QString, QVariant> &props);
    QString databaseTable() const;
    QString databasePkName() const;
    QStringList databaseFields() const;
    QMap<QString,QString> foreignKeys() const;

public slots:
    bool createTable() const;
    bool dropTable() const;

    bool remove();
    bool save();

protected:
    void addForeignKey(const QString &field, const QString &modelName);
    QDjangoModel *foreignModel(const QString &field);

private:
    int m_id;
    QString m_pkName;
    QMap<QString,QString> m_foreignKeys;
    QMap<QString,QDjangoModel*> m_foreignCache;
};

#endif
