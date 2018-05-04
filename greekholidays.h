#ifndef GREEKHOLIDAYS_H
#define GREEKHOLIDAYS_H

#include <QDate>

namespace GreekHolidays
{
QList<QDate> getHolidays(int year);
bool isHoliday(QDate date);
}
#endif // GREEKHOLIDAYS_H
