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

bool DBHandler::isOpen() const
{
    return m_db.isOpen();
}

QString DBHandler::databaseName() const
{
    return m_db.databaseName();
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
