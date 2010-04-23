#include "query.h"

QDjangoQuery::QDjangoQuery()
    :  m_operation(None), m_combine(NoCombine)
{
}

QDjangoQuery::QDjangoQuery(const QString &key, QDjangoQuery::Operation operation, QVariant data)
    :  m_key(key), m_operation(operation), m_data(data), m_combine(NoCombine)
{
}

QDjangoQuery QDjangoQuery::operator&&(const QDjangoQuery &other) const
{
    QDjangoQuery result;
    result.m_combine = AndCombine;
    result.m_children << *this << other;
    return result;
}

QDjangoQuery QDjangoQuery::operator||(const QDjangoQuery &other) const
{
    QDjangoQuery result;
    result.m_combine = OrCombine;
    result.m_children << *this << other;
    return result;
}

