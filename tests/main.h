#include <QObject>

class TestWhere : public QObject
{
    Q_OBJECT

private slots:
    void simpleWhere();
    void andWhere();
    void orWhere();
};

class TestModel: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void createUser();
    void removeUser();
    void getUser();
    void filterUsers();
    void cleanup();
    void cleanupTestCase();
};

