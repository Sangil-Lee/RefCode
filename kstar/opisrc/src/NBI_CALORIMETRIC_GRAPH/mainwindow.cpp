#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    static_attach = new AttachChannelAccess(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void MainWindow::closeEvent(QCloseEvent *)
{
    kill(getpid(), SIGTERM);
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
    ui->pushButton_2->setStyleSheet("");
    ui->pushButton_3->setStyleSheet("");
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->pushButton->setStyleSheet("");
    ui->pushButton_2->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
    ui->pushButton_3->setStyleSheet("");
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->pushButton->setStyleSheet("");
    ui->pushButton_2->setStyleSheet("");
    ui->pushButton_3->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,255,0);");
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->caMultiplot->setYRange(ui->doubleSpinBox->value(), ui->doubleSpinBox_2->value());

}

void MainWindow::on_pushButton_5_clicked()
{
    ui->caMultiplot_2->setYRange(ui->doubleSpinBox_4->value(), ui->doubleSpinBox_3->value());
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->caMultiplot_3->setYRange(ui->doubleSpinBox_5->value(), ui->doubleSpinBox_6->value());
}

void MainWindow::on_pushButton_10_clicked()
{
    ui->caMultiplot_5->setYRange(ui->doubleSpinBox_15->value(), ui->doubleSpinBox_17->value());
}

void MainWindow::on_pushButton_12_clicked()
{
    ui->caMultiplot_6->setYRange(ui->doubleSpinBox_16->value(), ui->doubleSpinBox_13->value());
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->caMultiplot_7->setYRange(ui->doubleSpinBox_18->value(), ui->doubleSpinBox_14->value());
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->caMultiplot_9->setYRange(ui->doubleSpinBox_19->value(), ui->doubleSpinBox_20->value());
}
