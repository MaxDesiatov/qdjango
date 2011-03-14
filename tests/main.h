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

#include "QDjango.h"
#include "QDjangoModel.h"

#include <QObject>

#define CHECKWHERE(_where, s, v) { \
    QDjangoQuery _sql_query(QDjango::database()); \
    const QVariantList _sql_values = v; \
    _where.bindValues(_sql_query); \
    QCOMPARE(_where.sql(), s); \
    QCOMPARE(_sql_query.boundValues().size(), _sql_values.size()); \
    for(int _i = 0; _i < _sql_values.size(); ++_i) QCOMPARE(_sql_query.boundValue(_i), _sql_values[_i]); \
    }

class Object : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString foo READ foo WRITE setFoo)
    Q_PROPERTY(int bar READ bar WRITE setBar)
    Q_PROPERTY(int wiz READ wiz WRITE setWiz)

    Q_CLASSINFO("__meta__", "db_table=foo_table")
    Q_CLASSINFO("foo", "max_length=255")
    Q_CLASSINFO("bar", "db_index=true")
    Q_CLASSINFO("wiz", "ignore_field=true")

public:
    Object(QObject *parent = 0);

    QString foo() const;
    void setFoo(const QString &foo);

    int bar() const;
    void setBar(int bar);

    int wiz() const;
    void setWiz(int wiz);

private:
    QString m_foo;
    int m_bar;
    int m_wiz;
};

class Item : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    Item(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

private:
    QString m_name;
};

class Owner : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Item* item1 READ item1 WRITE setItem1)
    Q_PROPERTY(Item* item2 READ item2 WRITE setItem2)

public:
    Owner(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    Item *item1() const;
    void setItem1(Item *item1);

    Item *item2() const;
    void setItem2(Item *item2);

private:
    QString m_name;
};

class tst_QDjangoCompiler : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void fieldNames();
    void fieldNamesRecursive();
};

/** Test QDjangoMetaModel class.
 */
class tst_QDjangoMetaModel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void options();
    void save();
    void cleanupTestCase();

private:
    QDjangoMetaModel metaModel;
};

/** Test QDjangoModel class.
 */
class tst_QDjangoModel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void filterRelated();
    void selectRelated();
    void cleanupTestCase();
};

/** Test QDjangoWhere class.
 */
class tst_QDjangoWhere : public QObject
{
    Q_OBJECT

private slots:
    void emptyWhere();
    void equalsWhere();
    void notEqualsWhere();
    void greaterThan();
    void greaterOrEquals();
    void lessThan();
    void lessOrEquals();
    void isIn();
    void startsWith();
    void endsWith();
    void contains();
    void andWhere();
    void orWhere();
    void complexWhere();
};

