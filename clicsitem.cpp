#include "clicsitem.h"

ClicsItem::ClicsItem(const QString &date, const QString &ian, const QString &activity, const QString &object)
{
    m_date = date;
    m_ian = ian;
    m_activity = activity;
    m_object = object;
}

QString ClicsItem::date() const
{
    return m_date;
}

void ClicsItem::setDate(const QString &date)
{
    m_date = date;
}

QString ClicsItem::ian() const
{
    return m_ian;
}

void ClicsItem::setIan(const QString &ian)
{
    m_ian = ian;
}

QString ClicsItem::activity() const
{
    return m_activity;
}

void ClicsItem::setActivity(const QString &activity)
{
    m_activity = activity;
}

QString ClicsItem::object() const
{
    return m_object;
}

void ClicsItem::setObject(const QString &object)
{
    m_object = object;
}
