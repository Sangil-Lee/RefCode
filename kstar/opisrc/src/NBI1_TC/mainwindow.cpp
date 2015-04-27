#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <signal.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *titlebarWidget1 = ui->dockWidget->titleBarWidget();
    QWidget *EmptyWidget1 = new QWidget();
    ui->dockWidget->setTitleBarWidget(EmptyWidget1);

    neu11_pattach = NULL;
    neu12_pattach = NULL;
    bls_pattach = NULL;
    bd_pattach = NULL;
    bm_pattach = NULL;
    cal_pattach = NULL;
    baf_pattach = NULL;
    shi_pattach = NULL;
    id_pattach = NULL;
    tile_pattach = NULL;

    up_pattach = NULL;
    up_pattach = new AttachChannelAccess(ui->widget);

    if(neu11_pattach==NULL){
        neu11_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_Neutral1-1.ui",1);
        if(neu11_pattach) {
            neu11_widget = neu11_pattach->GetWidget();

        }
        ui->dockWidget->setWidget(neu11_widget);
    }



    //ui->horizontalLayout->insertWidget(0, p_widget, 0, 0);
    //ui->horizontalLayout->addWidget(p_widget, 0,0 );
}

/*
    AttachChannelAccess *bls_pattach;
    AttachChannelAccess *bd_pattach;
    AttachChannelAccess *bm_pattach;
    AttachChannelAccess *cal_pattach;
    AttachChannelAccess *baf_pattach;
    AttachChannelAccess *shi_pattach;
    AttachChannelAccess *id_pattach;
    AttachChannelAccess *neu11_pattach;
    AttachChannelAccess *neu12_pattach;
    AttachChannelAccess *tile_pattach;
  */

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

void MainWindow::on_btTCne1_clicked()
{
    if(neu11_pattach==NULL)
    {
        neu11_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_Neutral1-1.ui",1);
        if(neu11_pattach)
        {
            neu11_widget = neu11_pattach->GetWidget();
        }

    }
    ui->dockWidget->setWidget(neu11_widget);
}

void MainWindow::on_btTCne2_clicked()
{
    if(neu12_pattach==NULL)
    {
        neu12_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_Neutral1-2.ui",1);
        if(neu12_pattach)
        {
            neu12_widget = neu12_pattach->GetWidget();
        }       
    }
    ui->dockWidget->setWidget(neu12_widget);
}

void MainWindow::on_btTCbm_clicked()
{
    if(bm_pattach==NULL)
    {
        bm_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_Bending1.ui",1);
        if(bm_pattach)
        {
            bm_widget = bm_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(bm_widget);
}

void MainWindow::on_btTCid_clicked()
{
    if(id_pattach==NULL)
    {
        id_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_IonDump.ui",1);
        if(id_pattach)
        {
            id_widget = id_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(id_widget);
}

void MainWindow::on_btTCcal_clicked()
{
    if(cal_pattach==NULL)
    {
        cal_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_Calorimeter.ui",1);
        if(cal_pattach)
        {
            cal_widget = cal_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(cal_widget);
}

void MainWindow::on_btTCbls_clicked()
{
    if(bls_pattach==NULL)
    {
        bls_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_BLScraper.ui",1);
        if(bls_pattach)
        {
            bls_widget = bls_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(bls_widget);
}

void MainWindow::on_btTCbd_clicked()
{
    if(bd_pattach==NULL)
    {
        bd_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_BeamDuct.ui",1);
        if(bd_pattach)
        {
            bd_widget = bd_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(bd_widget);
}

void MainWindow::on_btTCcs_clicked()
{
    if(shi_pattach==NULL)
    {
        shi_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_CryoShield.ui",1);
        if(shi_pattach)
        {
            shi_widget = shi_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(shi_widget);
}

void MainWindow::on_btTCcb_clicked()
{
    if(baf_pattach==NULL)
    {
        baf_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_CryoBaffle.ui",1);
        if(baf_pattach)
        {
            baf_widget = baf_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(baf_widget);
}

void MainWindow::on_btTCtp_clicked()
{
    if(tile_pattach==NULL)
    {
        tile_pattach= new AttachChannelAccess("/usr/local/opi/ui/NBI1_TileProtector.ui",1);
        if(tile_pattach)
        {
            tile_widget = tile_pattach->GetWidget();
        }
    }
    ui->dockWidget->setWidget(tile_widget);
}
