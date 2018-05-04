#include "dbhandler.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QFile>

DBHandler::DBHandler(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (QFile::exists(path)) {
        m_db.open();
    } else {
        qDebug() << "Error: connection with database fail";
    }
}

DBHandler::~DBHandler()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

QSqlDatabase DBHandler::database() const
{
    return m_db;
}

bool DBHandler::saveClicsItem(const ClicsItem& item)
{
    bool success = false;

    QSqlQuery queryAdd;
    queryAdd.prepare("REPLACE INTO clics (date, ian, activity, object) VALUES (:date, :ian, :activity, :object)");
    queryAdd.bindValue(":date", item.date());
    queryAdd.bindValue(":ian", item.ian());
    queryAdd.bindValue(":activity", item.activity());
    queryAdd.bindValue(":object", item.object());

    if(queryAdd.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "add clics item failed: " << queryAdd.lastError();
    }


    return success;
}

QList<ClicsItem> DBHandler::getWeeklyClicsItems(const QDate& date) {
    int dayofweek = date.dayOfWeek();
    const QDate MondayDate = date.addDays(- (dayofweek - 1));
    const QDate FridayDate = MondayDate.addDays(4);
    QList<ClicsItem> result;

    // Initialize result
    for (int i = 0; i < 5; i++) {
        result.insert(i, ClicsItem("-", "-", "-", "-"));
    }

    QSqlQuery querySelect;
    querySelect.prepare("SELECT * FROM clics WHERE `date` >= date(:Monday) AND `date` <= date(:Friday)");
    querySelect.bindValue(":Monday", MondayDate.toString("yyyy-MM-dd"));
    querySelect.bindValue(":Friday", FridayDate.toString("yyyy-MM-dd"));

    if(querySelect.exec())
    {
        while (querySelect.next()) {
            QString date = querySelect.value(0).toString();
            QString ian = querySelect.value(1).toString();
            QString activity = querySelect.value(2).toString();
            QString object = querySelect.value(3).toString();
            int index = MondayDate.daysTo(QDate::fromString(date, "yyyy-MM-dd"));
            result.replace(index, ClicsItem(date, ian, activity, object));
        }
    }
    else
    {
        qDebug() << "select weekly clics items failed: " << querySelect.lastError();
    }

    return result;
}

QList<QDate> DBHandler::getSickLeaves(QDate date) {
    QList<QDate> result;
    QSqlQuery querySelect;
    const QDate startMonth = date.addDays(1 - date.day());
    const QDate endMonth = date.addDays(date.daysInMonth() - date.day());

    querySelect.prepare("SELECT `date` FROM clics WHERE `date` >= date(:startMonth) AND `date` <= date(:endMonth) AND `object` == '0008'");
    querySelect.bindValue(":startMonth", startMonth.toString("yyyy-MM-dd"));
    querySelect.bindValue(":endMonth", endMonth.toString("yyyy-MM-dd"));

    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result.append(QDate::fromString(querySelect.value(0).toString(), "yyyy-MM-dd"));
        }
    }
    else
    {
        qDebug() << "get sick leaves items failed: " << querySelect.lastError();
    }

    return result;
}

QList<QDate> DBHandler::getAnnualLeaves(QDate date) {
    QList<QDate> result;
    QSqlQuery querySelect;
    const QDate startMonth = date.addDays(1 - date.day());
    const QDate endMonth = date.addDays(date.daysInMonth() - date.day());

    querySelect.prepare("SELECT `date` FROM clics WHERE `date` >= date(:startMonth) AND `date` <= date(:endMonth) AND `object` == '0007'");
    querySelect.bindValue(":startMonth", startMonth.toString("yyyy-MM-dd"));
    querySelect.bindValue(":endMonth", endMonth.toString("yyyy-MM-dd"));

    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result.append(QDate::fromString(querySelect.value(0).toString(), "yyyy-MM-dd"));
        }
    }
    else
    {
        qDebug() << "get annual leaves items failed: " << querySelect.lastError();
    }

    return result;
}

QStringList DBHandler::getIanCodes() {
    QStringList result;
    QSqlQuery querySelect;

    querySelect.prepare("SELECT DISTINCT ian FROM codes");
    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result << querySelect.value(0).toString();
        }
    }
    else
    {
        qDebug() << "get ian codes failed: " << querySelect.lastError();
    }

     return result;
}

QStringList DBHandler::getActivityCodes(const QString ian) {
    QStringList result;
    QSqlQuery querySelect;

    querySelect.prepare("SELECT activity FROM codes  WHERE `ian` == :IAN");
    querySelect.bindValue(":IAN", ian);
    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result << querySelect.value(0).toString();
        }
    }
    else
    {
        qDebug() << "get activity codes failed: " << querySelect.lastError();
    }

     return result;
}

QStringList  DBHandler::getObjectCodes(const QString activity) {
    QStringList result;
    QSqlQuery querySelect;

    querySelect.prepare("SELECT object FROM codes  WHERE `activity` == :ACTIVITY");
    querySelect.bindValue(":ACTIVITY", activity);
    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result << querySelect.value(0).toString();
        }
    }
    else
    {
        qDebug() << "get activity codes failed: " << querySelect.lastError();
    }

     return result;
}

QString  DBHandler::getDesc(const QString object) {
    QString result;
    QSqlQuery querySelect;

    querySelect.prepare("SELECT desc FROM codes  WHERE `object` == :OBJECT");
    querySelect.bindValue(":OBJECT", object);
    if(querySelect.exec())
    {
        while (querySelect.next()) {
            result = querySelect.value(0).toString();
        }
    }
    else
    {
        qDebug() << "get activity codes failed: " << querySelect.lastError();
    }

     return result;
}
