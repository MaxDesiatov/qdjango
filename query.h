#ifndef QDJANGO_QUERY_H
#define QDJANGO_QUERY_H

#include <QVariant>

class QDjangoQuery
{
public:
    enum Operation
    {
        None,
        Equals,
        NotEquals,
        // Contains,
    };

    QDjangoQuery();
    QDjangoQuery(const QString &key, QDjangoQuery::Operation operation, QVariant data);

    QDjangoQuery operator&&(const QDjangoQuery &other) const;
    QDjangoQuery operator||(const QDjangoQuery &other) const;

private:
    enum Combine
    {
        NoCombine,
        AndCombine,
        OrCombine,
    };

    QString m_key;
    QDjangoQuery::Operation m_operation;
    QVariant m_data;

    QList<QDjangoQuery> m_children;
    QDjangoQuery::Combine m_combine;
};

#endif
