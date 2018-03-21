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
        ui->statusBar->showMessage("Could not open database file " + m_dbh.database().databaseName());
        ui->btnSave->setEnabled(false);
        ui->btnClear->setEnabled(false);
        ui->cmbIAN->setEnabled(false);
        ui->cmbActivity->setEnabled(false);
        ui->cmbObject->setEnabled(false);
        ui->dateFrom->setEnabled(false);
        ui->dateUntil->setEnabled(false);
        ui->calendarWidget->setEnabled(false);
        return;
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
    setHolidays(ui->dateFrom->calendarWidget());
    setHolidays(ui->dateUntil->calendarWidget());
    connect(ui->dateFrom->calendarWidget(), &QCalendarWidget::currentPageChanged, this, &MainWindow::on_dateFrom_currentPageChanged);
    connect(ui->dateUntil->calendarWidget(), &QCalendarWidget::currentPageChanged, this, &MainWindow::on_dateUntil_currentPageChanged);

    // Configure comboBox
    QStringList m_IAN;
    m_IAN<<"04-001"<<"14-017";
    ui->cmbIAN->insertItems(0, m_IAN);
    ui->cmbIAN->setCurrentText("");

    // Configure Status Bar
    connect(ui->statusBar, &QStatusBar::messageChanged, this, &MainWindow::showReady);
    ui->statusBar->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSave_clicked()
{
    // Validation check
    if (ui->cmbIAN->currentText() == "" || ui->cmbActivity->currentText() == "" || ui->cmbObject->currentText() == "") {
        ui->statusBar->showMessage("Please fill IAN Activity and Object", 4000);
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
        ui->statusBar->showMessage("Clics items saved successfully.", 4000);
    } else {
        ui->statusBar->showMessage("Some items failed to saved in database please try again.");
    }

    // Clear form
    ui->cmbIAN->clearEditText();
    ui->cmbActivity->clearEditText();
    ui->cmbObject->clearEditText();
    ui->calendarWidget->selectionChanged();
}

void MainWindow::on_cmbIAN_currentTextChanged(const QString &arg1)
{
    ui->cmbActivity->clear();
    if (arg1 == "04-001"){
        ui->cmbActivity->insertItem(0, "9007");
        ui->cmbActivity->insertItem(1, "9008");
        ui->cmbActivity->insertItem(2, "9010");
        ui->cmbActivity->insertItem(3, "9013");
    }

    if (arg1 == "14-017"){
        ui->cmbActivity->insertItem(0, "4704");
    }
}

void MainWindow::on_cmbActivity_currentTextChanged(const QString &arg1)
{
    ui->cmbObject->clear();
    if (arg1 == "9007"){
        ui->cmbObject->insertItem(0, "0007");
        ui->statusBar->showMessage("Annual Leave");
    }

    if (arg1 == "9008"){
        ui->cmbObject->insertItem(0, "0008");
        ui->statusBar->showMessage("Sick Leave");
    }

    if (arg1 == "9010"){
        ui->cmbObject->insertItem(0, "0010");
        ui->statusBar->showMessage("Bank Holiday");
    }

    if (arg1 == "9013"){
        ui->cmbObject->insertItem(0, "0013");
        ui->statusBar->showMessage("Parental Leave");
    }

    if (arg1 == "4704"){
        ui->cmbObject->insertItem(0, "TBE0");
    }
}

void MainWindow::on_calendarWidget_selectionChanged()
{
    QTableWidget* weekTable;
    weekTable = ui->tableWidget;
    QDate date = ui->calendarWidget->selectedDate();
    QList<ClicsItem> items = m_dbh.getWeeklyClicsItems(date);
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

    // Color as selected whole week
    int dayofweek = date.dayOfWeek();
    const QDate MondayDate = date.addDays(- (dayofweek - 1));
    const QDate FridayDate = MondayDate.addDays(4);

    QBrush brush;
    QColor color = QColor("dodgerblue");
    brush.setColor( color );
    QTextCharFormat cf = ui->calendarWidget->dateTextFormat( date );
    cf.setBackground( brush );
    QDate iter = MondayDate;
    while (iter <= FridayDate) {
        ui->calendarWidget->setDateTextFormat( iter, cf );
        iter = iter.addDays(1);
    }

    // Clear previous Selection
    if (!m_Monday.isNull() && m_Monday != MondayDate) {
         brush.setColor( Qt::white );
         cf.setBackground( brush );
         QDate iter = m_Monday;
         while (iter <= m_Friday) {
             ui->calendarWidget->setDateTextFormat( iter, cf );
             iter = iter.addDays(1);
         }
    }

    m_Monday = MondayDate;
    m_Friday = FridayDate;

    ui->dateFrom->setDate(MondayDate);
    ui->dateUntil->setDate(FridayDate);
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
    ui->dateUntil->setMinimumDate(date);
}

void MainWindow::on_dateUntil_dateChanged(const QDate &date)
{
    ui->dateFrom->setMaximumDate(date);
    // Needed to Fix wierd bug that dateFrom date is set
    // to previous week Friday instead of Monday
    ui->dateFrom->setDate(m_Monday);
}

void MainWindow::showReady(const QString &message)
{
    if (message == "") {
        ui->statusBar->showMessage("Ready");
    }
}

void MainWindow::on_btnClear_clicked()
{
    // Clear form
    ui->cmbIAN->clearEditText();
    ui->cmbActivity->clearEditText();
    ui->cmbObject->clearEditText();
    ui->calendarWidget->selectionChanged();
    ui->statusBar->showMessage("Ready");
}
