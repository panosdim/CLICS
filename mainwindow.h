#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbhandler.h"
#include "webdav.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QCalendarWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnSave_clicked();
    void on_cmbIAN_currentTextChanged(const QString &arg1);
    void on_cmbActivity_currentTextChanged(const QString &arg1);
    void on_cmbObject_currentTextChanged(const QString &arg1);
    void on_calendarWidget_selectionChanged();
    void on_calendarWidget_currentPageChanged(int year, int month);
    void on_dateFrom_currentPageChanged(int year);
    void on_dateUntil_currentPageChanged(int year);
    void on_dateFrom_dateChanged(const QDate &date);
    void on_dateUntil_dateChanged(const QDate &date);
    void showReady(const QString &message);
    void on_btnClear_clicked();
    void DBSyncFinished();

public slots:
    void closing();

private:
    Ui::MainWindow *ui;
    DBHandler m_dbh;
    WebDAV m_webdav;
    QDate m_Monday;
    QDate m_Friday;
    bool m_SyncNeeded;
    void setHolidays(QCalendarWidget *calendar, int year = QDate::currentDate().year());
    void setSickLeaves(QCalendarWidget *calendar, QDate date = QDate::currentDate());
    void setAnnualLeaves(QCalendarWidget *calendar, QDate date = QDate::currentDate());
};

#endif // MAINWINDOW_H
