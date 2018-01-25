#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clicsitem.h"
#include "greekholidays.h"
#include <QTextCharFormat>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dbh(QCoreApplication::applicationDirPath() + "/clics.db3")
{
    ui->setupUi(this);

    // Database connection
    if (!m_dbh.database().isOpen()) {
        ui->statusBar->showMessage("Could not open database file" + m_dbh.database().databaseName());
    } else {
        ui->statusBar->showMessage("Ready");
    }

    // Configure table
    QTableWidget* weekTable;
    QStringList tableHeader;
    weekTable = ui->tableWidget;
    weekTable->setRowCount(1);
    weekTable->setColumnCount(5);
    tableHeader<<"Monday"<<"Tuesday"<<"Wednesday"<<"Thursday"<<"Friday";
    weekTable->setHorizontalHeaderLabels(tableHeader);
    weekTable->verticalHeader()->setVisible(false);
    weekTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    weekTable->setShowGrid(false);

    // Configure calendar widget
    ui->calendarWidget->selectionChanged();
    setHolidays(ui->calendarWidget);

    // Configure dateEdit
    ui->dateFrom->setDate(QDate::currentDate());
    ui->dateUntil->setDate(QDate::currentDate());
    setHolidays(ui->dateFrom->calendarWidget());
    setHolidays(ui->dateUntil->calendarWidget());
    connect(ui->dateFrom->calendarWidget(), &QCalendarWidget::currentPageChanged, this, &MainWindow::on_dateFrom_currentPageChanged);

    // Configure comboBox
    QStringList m_IAN;
    m_IAN<<"04-001"<<"14-017";
    ui->cmbIAN->insertItems(0, m_IAN);
    ui->cmbIAN->setCurrentText("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSave_clicked()
{
    // Validation check
    if (ui->cmbIAN->currentText() == "" || ui->cmbActivity->currentText() == "" || ui->cmbObject->currentText() == "") {
        ui->statusBar->showMessage("Please fill IAN Activity and Object");
        return;
    }

    // Store in Sqlite database
    bool successFlag = true;
    QDate from = ui->dateFrom->date();
    QDate until = ui->dateUntil->date();
    while (from <= until) {
        if (from.dayOfWeek() != Qt::Saturday && from.dayOfWeek() != Qt::Sunday) {
            ClicsItem item(from.toString("yyyy-MM-dd"),ui->cmbIAN->currentText(), ui->cmbActivity->currentText(), ui->cmbObject->currentText());
            successFlag = m_dbh.saveClicsItem(item) && successFlag;
        }
        from = from.addDays(1);
    }

    if (successFlag) {
        ui->statusBar->showMessage("Clics items saved successfully.");
    } else {
        ui->statusBar->showMessage("Some items failed to saved in database please try again.");
    }

    // Clear form
    ui->cmbIAN->clearEditText();
    ui->cmbActivity->clearEditText();
    ui->cmbObject->clearEditText();
    ui->dateFrom->setDate(QDate::currentDate());
    ui->dateUntil->setDate(QDate::currentDate());
    ui->calendarWidget->selectionChanged();
}

void MainWindow::on_cmbIAN_currentTextChanged(const QString &arg1)
{
    ui->cmbActivity->clear();
    if (arg1 == "04-001"){
        ui->cmbActivity->insertItem(0, "9013");
        ui->cmbActivity->insertItem(1, "9010");
    }

    if (arg1 == "14-017"){
        ui->cmbActivity->insertItem(0, "4704");
    }
}

void MainWindow::on_cmbActivity_currentTextChanged(const QString &arg1)
{
    ui->cmbObject->clear();
    if (arg1 == "9010"){
        ui->cmbObject->insertItem(0, "0010");
    }

    if (arg1 == "9013"){
        ui->cmbObject->insertItem(0, "0013");
    }

    if (arg1 == "4704"){
        ui->cmbObject->insertItem(0, "TBE0");
    }
}

void MainWindow::on_calendarWidget_selectionChanged()
{
    QTableWidget* weekTable;
    weekTable = ui->tableWidget;
    QList<ClicsItem> items = m_dbh.getWeeklyClicsItems(ui->calendarWidget->selectedDate());
    int column = 0;

    for (ClicsItem item: items) {
        QString data;
        if (item.date() == "-") {
            data = "-";
        } else {
            data = item.ian() + "\n" + item.activity() + "-" + item.object();
        }
        //insert data into table
        weekTable->setItem(0, column, new QTableWidgetItem(data));
        weekTable->item(0, column)->setTextAlignment(Qt::AlignCenter);
        column++;
    }

    ui->statusBar->showMessage("Ready");
}

void MainWindow::on_calendarWidget_currentPageChanged(int year)
{
    if (year != QDate::currentDate().year()) {
        setHolidays(ui->calendarWidget, year);
    }
}

void MainWindow::on_dateFrom_currentPageChanged(int year)
{
    if (year != QDate::currentDate().year()) {
        setHolidays(ui->dateFrom->calendarWidget(), year);
    }
}

void MainWindow::on_dateUntil_currentPageChanged(int year)
{
    if (year != QDate::currentDate().year()) {
        setHolidays(ui->dateUntil->calendarWidget(), year);
    }
}

void MainWindow::setHolidays(QCalendarWidget* calendar, int year)
{
    QTextCharFormat format = calendar->weekdayTextFormat(Qt::Saturday);

    QList<QDate> holidays = GreekHolidays::getHolidays(year);
    for (QDate h: holidays) {
        calendar->setDateTextFormat(h, format);
    }
}

void MainWindow::on_dateFrom_dateChanged(const QDate &date)
{
    QDate until = ui->dateUntil->date();
    if (date > until) {
        ui->dateUntil->setDate(date);
    }
}

void MainWindow::on_dateUntil_dateChanged(const QDate &date)
{
    QDate from = ui->dateFrom->date();
    if (date < from) {
        ui->dateFrom->setDate(date);
    }
}
