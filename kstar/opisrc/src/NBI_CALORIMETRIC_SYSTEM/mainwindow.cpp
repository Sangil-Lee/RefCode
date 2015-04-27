#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QWidget *titlebarWidget1 = ui->dockWidget->titleBarWidget();
    QWidget *EmptyWidget1 = new QWidget();

    ui->dockWidget->setTitleBarWidget(EmptyWidget1);

    delete titlebarWidget1;
    calorimetric = NULL;

    if(calorimetric == NULL)
        calorimetric = new panel_calorimetric;

    ui->dockWidget->setWidget(calorimetric);

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

}

void MainWindow::on_pushButton_2_clicked()
{

}
