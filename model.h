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
    void setPk(const QVariant &pk);

    bool createTable() const;
    bool dropTable() const;

    static QSqlDatabase &database();
    static void setDatabase(QSqlDatabase *database);

    QString databaseTable() const;
    QString databasePkName() const;
    void setPrimaryKey(const QString &key);

    QStringList databaseFields() const;

    bool remove();
    bool save();

private:
    int m_id;
    QString m_pkName;
};

#endif
