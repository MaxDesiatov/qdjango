#include <QtCore/QString>
#include <QtTest/QtTest>

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

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

QVariantMapTest::QVariantMapTest()
{
}

void QVariantMapTest::initTestCase()
{
}

void QVariantMapTest::cleanupTestCase()
{
}

void QVariantMapTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(QVariantMapTest);

#include "tst_qvariantmaptest.moc"
