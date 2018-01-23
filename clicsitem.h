#ifndef CLICSITEM_H
#define CLICSITEM_H

#include <QString>

class ClicsItem
{
public:
    QString date() const;
    void setDate(const QString &date);

    QString ian() const;
    void setIan(const QString &ian);

    QString activity() const;
    void setActivity(const QString &activity);

    QString object() const;
    void setObject(const QString &object);

    ClicsItem(const QString &date, const QString &ian, const QString &activity, const QString &object);
private:
    QString m_date;
    QString m_ian;
    QString m_activity;
    QString m_object;
};

#endif // CLICSITEM_H
