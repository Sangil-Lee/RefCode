#include <QDebug>

#include "mainwindow.h"
#include "ui_nb1_vac.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)    
{
    ui->setupUi(this);
    m_pAttachMain = new AttachChannelAccess(this);
    m_pAttachView = new AttachChannelAccess(ui->stackedWidget_3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *)
{
}

void MainWindow::closeEvent(QCloseEvent *)
{
    kill(getpid(), SIGTERM);
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
