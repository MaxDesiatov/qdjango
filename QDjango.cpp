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

#include <QDebug>
#include <QMetaProperty>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "QDjango.h"
#include "QDjangoModel.h"

/*! \mainpage
 *
 * QDjango is a simple yet powerful Object Relation Mapper (ORM) built
 * on top of the Qt library. Where possible it tries to follow django's
 * ORM API, hence its name.
 *
 * @see QDjango
 * @see QDjangoModel
 * @see QDjangoWhere
 * @see QDjangoQuerySet
 */


static QMap<QString, QDjangoModel*> registry;

static void sqlDebug(const QSqlQuery &query)
{
    qDebug() << "SQL query" << query.lastQuery();
    QMapIterator<QString, QVariant> i(query.boundValues());
    while (i.hasNext()) {
        i.next();
        qDebug() << "   " << i.key().toAscii().data() << "="
                 << i.value().toString().toAscii().data();
    }
}

bool sqlExec(QSqlQuery &query)
{
#ifdef QDJANGO_DEBUG_SQL
    sqlDebug(query);
    if (!query.exec())
    {
        qWarning() << "SQL error" << query.lastError();
        return false;
    }
    return true;
#else
    return query.exec();
#endif
}

/** Creates the database tables for all registered models.
 */
void QDjango::createTables()
{
    foreach (const QString &key, registry.keys())
        registry[key]->createTable();
}

/** Registers a QDjangoModel.
 */
bool QDjango::registerModel(QDjangoModel *model)
{
    const QString name = model->metaObject()->className();
    if (registry.contains(name))
        return false;
    qDebug() << "Registering model" << name;
    registry.insert(name, model);
    return true;
}

/** Returns the QDjangoModel with the given name.
 */
const QDjangoModel *QDjango::model(const QString &name)
{
    return registry.value(name);
}

/** Returns the SQL used to declare a field as auto-increment.
 */
QString QDjango::autoIncrementSql()
{
    const QString driverName = QDjangoModel::database().driverName();
    if (driverName == "QSQLITE" || driverName == "QSQLITE2")
        return QLatin1String(" AUTOINCREMENT");
    else if (driverName == "QMYSQL")
        return QLatin1String(" AUTO_INCREMENT");
    else
        return QString();
}

/** Returns the empty SQL limit clause.
 */
QString QDjango::noLimitSql()
{
    const QString driverName = QDjangoModel::database().driverName();
    if (driverName == "QSQLITE" || driverName == "QSQLITE2")
        return QLatin1String(" LIMIT -1");
    else if (driverName == "QMYSQL")
        // 2^64 - 1, as recommended by the MySQL documentation
        return QLatin1String(" LIMIT 18446744073709551615");
    else
        return QString();
}

/** Quotes a database table or column name.
 */
QString QDjango::quote(const QString &name)
{
    return "`" + name + "`";
}

/** Unquotes a database table or column name.
 */
QString QDjango::unquote(const QString &quoted)
{
    if (quoted.startsWith("`") && quoted.endsWith("`"))
        return quoted.mid(1, quoted.size() - 2);
    return quoted;
}

