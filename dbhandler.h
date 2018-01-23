#ifndef DBHANDLER_H
#define DBHANDLER_H

#include "clicsitem.h"
#include <QSqlDatabase>

class DBHandler
{
public:
    DBHandler(const QString& path);
    ~DBHandler();

    bool isOpen() const;
    bool saveClicsItem(const ClicsItem &item);
    QString databaseName() const;
private:
    QSqlDatabase m_db;
};

#endif // DBHANDLER_H
