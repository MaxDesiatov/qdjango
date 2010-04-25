#include "model.h"

class User : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

private:
    QString m_username;
    QString m_password;
};

