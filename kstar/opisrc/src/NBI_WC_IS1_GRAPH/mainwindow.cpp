#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    static_attach = new AttachChannelAccess(this);

    //ui->caColorCheckBox->setText("ARC-1");
    //ui->caColorCheckBox_2->setText("ARC-2");
    //ui->caColorCheckBox_3->setText("ARC-3");
    //ui->caColorCheckBox_4->setText("FIL-1");
    //ui->caColorCheckBox_5->setText("FIL-2");
    //ui->caColorCheckBox->
    //ui->label->setStyle();

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
