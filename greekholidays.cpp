#include "greekholidays.h"

QDate getOrthodoxEaster(int year) {
    QDate oed;

    int r1 = year % 4;
    int r2 = year % 7;
    int r3 = year % 19;
    int r4 = (19 * r3 + 15) % 30;
    int r5 = (2 * r1 + 4 * r2 + 6 * r4 + 6) % 7;
    int days = r5 + r4 + 13;

    if (days > 39) {
        days = days - 39;
        oed = QDate(year, 5, days);
    } else if (days > 9) {
        days = days - 9;
        oed = QDate(year, 4, days);
    } else {
        days = days + 22;
        oed = QDate(year, 3, days);
    }
    return oed;
}

namespace GreekHolidays
{
bool isHoliday(QDate date) {
    int year = date.year();
    QList<QDate> holidays = getHolidays(year);
    return holidays.contains(date);
}

QList<QDate> getHolidays(int year) {
    QList<QDate> holidays;
    QDate newYearEve = QDate(year, 1, 1);
    QDate epiphany = QDate(year, 1, 6);
    QDate easter = getOrthodoxEaster(year);
    QDate cleanMonday = easter.addDays(-48);
    QDate independenceDay = QDate(year, 3, 25);
    QDate goodFriday = easter.addDays(-2);
    QDate easterMonday = easter.addDays(1);
    QDate labourDay = QDate(year, 5, 1);
    QDate whitMonday = easter.addDays(50);
    QDate assumption = QDate(year, 8, 15);
    QDate ochiDay = QDate(year, 10, 28);
    QDate christmas = QDate(year, 12, 25);
    QDate glorifying = QDate(year, 12, 26);
    holidays.append(newYearEve);
    holidays.append(epiphany);
    holidays.append(cleanMonday);
    holidays.append(independenceDay);
    holidays.append(goodFriday);
    holidays.append(easterMonday);
    holidays.append(labourDay);
    holidays.append(whitMonday);
    holidays.append(assumption);
    holidays.append(ochiDay);
    holidays.append(christmas);
    holidays.append(glorifying);

    return holidays;
}
}
