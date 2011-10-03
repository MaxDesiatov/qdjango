#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtSql>

#include <QDjango.h>
#include <QDjangoQuerySet.h>

class Model : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap prop READ prop WRITE setProp)

public:
    Model() {}

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
    QDjangoMetaModel _meta;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

QVariantMapTest::QVariantMapTest() :
    _db(QSqlDatabase::addDatabase("QSQLITE")),
    _meta(QDjango::registerModel<Model>())
{}

void QVariantMapTest::initTestCase()
{
    QDir().remove("test.db");

    _db.setDatabaseName("test.db");
    QVERIFY(_db.open());

    QDjango::setDatabase(_db);
    QDjango::createTables();
}

void QVariantMapTest::cleanupTestCase()
{
}

void QVariantMapTest::testCase1()
{
    QVariantMap map;
    map["123"] = 123;
    map["string"] = "string";
    QVariantList list;
    list << 1 << 2 << 3;
    map["list"] = list;

    Model model;
    model.setProp(map);

    _meta.save(&model);

    QDjangoQuerySet<Model> models;
    QVERIFY(1 == models.count());
    Model *testModel = models.at(0);
    QVERIFY(123 == testModel->prop()["123"].toInt());
    QVERIFY("string" == testModel->prop()["string"].toString());
    QVariantList testList;
    testList << 1 << 2 << 3;
    QVERIFY(testList == testModel->prop()["list"].toList());
    delete testModel;
}

QTEST_APPLESS_MAIN(QVariantMapTest);

#include "tst_qvariantmaptest.moc"
