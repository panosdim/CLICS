#ifndef DBHANDLER_H
#define DBHANDLER_H

#include "clicsitem.h"
#include <QSqlDatabase>
#include <QDate>

class DBHandler
{
public:
    DBHandler(const QString& path);
    ~DBHandler();

    bool saveClicsItem(const ClicsItem &item);
    QList<ClicsItem> getWeeklyClicsItems(const QDate& date);
    QSqlDatabase database() const;
private:
    QSqlDatabase m_db;
};

#endif // DBHANDLER_H
