#ifndef QDJANGO_MODEL_H
#define QDJANGO_MODEL_H

#include <QObject>

class QDjangoModel;
class QSqlDatabase;


class QDjangoModel : public QObject
{
    Q_OBJECT

public:
    QDjangoModel(QObject *parent = 0);

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
};

#endif
