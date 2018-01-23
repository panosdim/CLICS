#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbhandler.h"
#include <QMainWindow>
#include <QTableWidget>

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

private:
    Ui::MainWindow *ui;
    DBHandler m_dbh;
};

#endif // MAINWINDOW_H
