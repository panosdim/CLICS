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
    QList<QDate> getSickLeaves(QDate date);
    QList<QDate> getAnnualLeaves(QDate date);
    QSqlDatabase database() const;
    QStringList getIanCodes();
    QStringList getActivityCodes(const QString ian);
    QStringList getObjectCodes(const QString activity);
    QString getDesc(const QString object);
private:
    QSqlDatabase m_db;
};

#endif // DBHANDLER_H
