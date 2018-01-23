#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clicsitem.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dbh(QCoreApplication::applicationDirPath() + "/clics.db3")
{
    ui->setupUi(this);

    // Database connection
    if (!m_dbh.isOpen()) {
        ui->statusBar->showMessage("Could not open database file" + m_dbh.databaseName());
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

    //insert data
    weekTable->setItem(0, 0, new QTableWidgetItem("14-017\n4704 TBE0"));
    weekTable->item(0,0)->setTextAlignment(Qt::AlignCenter);

    // Configure dateEdit
    ui->dateFrom->setDate(QDate::currentDate());
    ui->dateUntil->setDate(QDate::currentDate());

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
    // TODO: Validation check
    ClicsItem item(ui->dateFrom->date().toString("yyyy-MM-dd"),ui->cmbIAN->currentText(), ui->cmbActivity->currentText(), ui->cmbObject->currentText());
    m_dbh.saveClicsItem(item);
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
