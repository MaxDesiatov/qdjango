#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtSql>

#include <QDjango.h>

class Model : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap prop READ prop WRITE setProp)

public:
    Model()
    {}

    QVariantMap prop() const { return _prop; }
    void setProp(const QVariantMap prop) { _prop = prop; }

private:
    QVariantMap _prop;
};

class QVariantMapTest : public QObject
{
    Q_OBJECT

public:
    QVariantMapTest();

private:
    QSqlDatabase _db;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

QVariantMapTest::QVariantMapTest() :
    _db(QSqlDatabase::addDatabase("QSQLITE"))
{}

void QVariantMapTest::initTestCase()
{
    _db.setDatabaseName("test.db");
    QVERIFY(_db.open());

    QDjango::setDatabase(_db);
}

void QVariantMapTest::cleanupTestCase()
{
}

void QVariantMapTest::testCase1()
{
    QVariantMap map;


}

QTEST_APPLESS_MAIN(QVariantMapTest);

#include "tst_qvariantmaptest.moc"
