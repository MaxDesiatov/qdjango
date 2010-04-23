#ifndef QDJANGO_MODEL_H
#define QDJANGO_MODEL_H

#include <QObject>

class QDjangoModel;
class QDjangoModelPrivate;
class QSqlDatabase;

/*
class QDjangoManager
{
public:
    QDjangoManager(QDjangoModel *model);
    QDjangoQuerySet all();
    QDjangoQuerySet exclude();
    QDjangoQuerySet filter();
    QDjangoModel *get(const QVariant &pk);

    int size();

private:
    QDjangoModel *m_model;
};
*/

class QDjangoModel : public QObject
{
    Q_OBJECT

public:
    QDjangoModel(QObject *parent = 0);
    //QDjangoManager *objects();

    QVariant pk() const;

    bool createTable() const;
    bool dropTable() const;

    static QSqlDatabase &database();
    static void setDatabase(QSqlDatabase *database);

    QString databaseTable() const;
    QString databasePkName() const;
    QStringList databaseFields() const;

    bool remove();
    bool save();

private:
    QDjangoModelPrivate *d;
};

#endif
