#include <QObject>

class TestQuery : public QObject
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
    void createTable();
    void createUser();
    void removeUser();
    void getUser();
    void filterUsers();
    void cleanup();
    void cleanupTestCase();
};

