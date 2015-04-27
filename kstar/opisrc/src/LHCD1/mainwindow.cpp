#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include "caMultiplot.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *dockEmptyTitlebar1 = new QWidget();
    QWidget *dockEmptyTitlebar2 = new QWidget();
    QWidget *dockTitleBar1 = ui->dockWidget->titleBarWidget();
    QWidget *dockTitleBar2 = ui->dockWidget_2->titleBarWidget();

    ui->dockWidget->setTitleBarWidget(dockEmptyTitlebar1);
    ui->dockWidget_2->setTitleBarWidget(dockEmptyTitlebar2);
    delete dockTitleBar1;
    delete dockTitleBar2;

    makeUI();

    on_up_pannel_load();
    on_pushButton_1_clicked();

    //m_pTimer = new QTimer();
    //m_pTimer->setInterval(1000);
    //connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));

   //m_pTimer->start();
}

void MainWindow::OnTimeOut()
{
/*
    on_pushButton_clicked();
    m_pTimer->stop();
    delete m_pTimer;
*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeUI()
{
    a_pattach= NULL;
    powersupply_pattach= NULL;
    c_pattach= NULL;
    interlock_pattach=NULL;
    vswr_pattach=NULL;
    graph_pattach=NULL;
    ltudaq_pattach=NULL;
    up_pattach= NULL;
    calorimetric_pattach=NULL;
    graph2_pattach=NULL;
    monitor_pattach=NULL;
    klystron_pattach = NULL;
    watercooling_pattach = NULL;
    tc_pattach = NULL;
    antenna_pattach = NULL;
    mdsnode_pattach = NULL;
    oper_pattach = NULL;	
    graph_pattach_popup=NULL;
    graph2_pattach_popup=NULL;

    klystron = NULL;
    //oper = NULL;

     m_klystrongraph = new QMainWindow();
     m_klystrongraph->setGeometry(30,30,1078,844);
     m_antennagraph = new QMainWindow();
     m_antennagraph->setGeometry(30,30,1078,844);
    //
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
void MainWindow::on_up_pannel_load()
{
    if(up_pattach==NULL){
        up_pattach= new AttachChannelAccess("/usr/local/opi/ui/LH1_up.ui",1);
        if(up_pattach)
            up_widget = up_pattach->GetWidget();
    }

    ui->dockWidget->setWidget(up_widget);
}

void MainWindow::on_pushButton_1_clicked()
{
    if(oper_pattach == NULL){
     oper_pattach= new AttachChannelAccess("/usr/local/opi/ui/LH1_oper.ui",1);
        if(oper_pattach)
            oper_widget = oper_pattach->GetWidget();
    }
    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(oper_widget);
        //oper = new panel_oper;
    //ui->dockWidget_2->widget()->close();
    //ui->dockWidget_2->setWidget(oper);

    ui->pushButton_1->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked()
{ 
    if(powersupply_pattach == NULL){
        powersupply_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_powersupply.ui",1);
        if(powersupply_pattach)
            powersupply_widget = powersupply_pattach->GetWidget();
    }

    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(powersupply_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);

}

void MainWindow::on_pushButton_3_clicked()
{
    if(klystron == NULL)
        klystron = new panel_rfcon_klystron;

    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(klystron);


    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *)
{

    qDebug()<<getpid();
    kill (getpid(), SIGTERM);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(interlock_pattach == NULL){
     interlock_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_interlock.ui",1);
     if(interlock_pattach)
         interlock_widget  =interlock_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(interlock_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(false);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}
/*
void MainWindow::on_pushButton_6_clicked()
{
    if(klystron_pattach == NULL){
        klystron_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_klystron.ui",1);

        if(klystron_pattach)
            klystron_widget = klystron_pattach->GetWidget();
    }
    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(klystron_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}

void MainWindow::on_pushButton_7_clicked()
{
    if(graph_pattach == NULL){
     graph_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_graph.ui",1);
     if(graph_pattach)
         graph_widget  =graph_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(graph_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}
*/
void MainWindow::on_pushButton_4_clicked()
{
    if(ltudaq_pattach == NULL){
     ltudaq_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_ltudaq.ui",1);
     if(ltudaq_pattach)
         ltudaq_widget  =ltudaq_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(ltudaq_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}
/*
void MainWindow::on_pushButton_8_clicked()
{
    if(calorimetric_pattach == NULL){
     calorimetric_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_calorimetric.ui",1);
     if( calorimetric_pattach)
          calorimetric_widget  = calorimetric_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget( calorimetric_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(false);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);

}

void MainWindow::on_pushButton_9_clicked()
{
    if(graph2_pattach == NULL){
     graph2_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_graphAn.ui",1);
     if(graph2_pattach)
         graph2_widget  =graph2_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(graph2_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}

void MainWindow::on_pushButton_10_clicked()
{
    if(monitor_pattach == NULL){
     monitor_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_gasfeeding.ui",1);
     if(monitor_pattach)
         monitor_widget  =monitor_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(monitor_widget);

     ui->pushButton_1->setEnabled(true);
     ui->pushButton_2->setEnabled(true);
     ui->pushButton_3->setEnabled(true);
     ui->pushButton_4->setEnabled(true);
     ui->pushButton_5->setEnabled(true);
     ui->pushButton_6->setEnabled(true);
     ui->pushButton_7->setEnabled(true);
     ui->pushButton_8->setEnabled(true);
     ui->pushButton_9->setEnabled(true);
     ui->pushButton_10->setEnabled(false);
     ui->pushButton_11->setEnabled(true);
     ui->pushButton_14->setEnabled(true);
     ui->pushButton_15->setEnabled(true);
     ui->pushButton_12->setEnabled(true);

}
*/
void MainWindow::on_pushButton_11_clicked()
{
    if(watercooling_pattach == NULL){
     watercooling_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_watercooling.ui",1);
     if(watercooling_pattach)
         watercooling_widget  = watercooling_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(watercooling_widget);

     ui->pushButton_1->setEnabled(true);
     ui->pushButton_2->setEnabled(true);
     ui->pushButton_3->setEnabled(true);
     ui->pushButton_4->setEnabled(true);
     ui->pushButton_5->setEnabled(true);
     //ui->pushButton_6->setEnabled(true);
     //ui->pushButton_7->setEnabled(true);
     //ui->pushButton_8->setEnabled(true);
     //ui->pushButton_9->setEnabled(true);
     //ui->pushButton_10->setEnabled(true);
     ui->pushButton_11->setEnabled(false);
     ui->pushButton_14->setEnabled(true);
     //ui->pushButton_15->setEnabled(true);
     ui->pushButton_12->setEnabled(true);
}

void MainWindow::on_pushButton_14_clicked()
{
    if(tc_pattach == NULL){
     tc_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_tc.ui",1);
     if(tc_pattach)
         tc_widget  = tc_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
     ui->dockWidget_2->setWidget(tc_widget);

    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    //ui->pushButton_6->setEnabled(true);
    //ui->pushButton_7->setEnabled(true);
    //ui->pushButton_8->setEnabled(true);
    //ui->pushButton_9->setEnabled(true);
    //ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_14->setEnabled(false);
    //ui->pushButton_15->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
}
/*
void MainWindow::on_pushButton_15_clicked()
{
    if(antenna_pattach == NULL){
     antenna_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_rfcon_antenna.ui",1);
     if(antenna_pattach)
         antenna_widget  =antenna_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(antenna_widget);

     ui->pushButton_1->setEnabled(true);
     ui->pushButton_2->setEnabled(true);
     ui->pushButton_3->setEnabled(true);
     ui->pushButton_4->setEnabled(true);
     ui->pushButton_5->setEnabled(true);
     //ui->pushButton_6->setEnabled(true);
     //ui->pushButton_7->setEnabled(true);
     //ui->pushButton_8->setEnabled(true);
     //ui->pushButton_9->setEnabled(true);
     //ui->pushButton_10->setEnabled(true);
     ui->pushButton_11->setEnabled(true);
     ui->pushButton_14->setEnabled(true);
     ui->pushButton_15->setEnabled(false);
     ui->pushButton_12->setEnabled(true);
}
*/
void MainWindow::on_pushButton_12_clicked()
{
    if(mdsnode_pattach == NULL){
     mdsnode_pattach = new AttachChannelAccess("/usr/local/opi/ui/LH1_mdsnode.ui",1);
     if(mdsnode_pattach)
         mdsnode_widget  =mdsnode_pattach->GetWidget();
     }
    ui->dockWidget_2->widget()->close();
    ui->dockWidget_2->setWidget(mdsnode_widget);

     ui->pushButton_1->setEnabled(true);
     ui->pushButton_2->setEnabled(true);
     ui->pushButton_3->setEnabled(true);
     ui->pushButton_4->setEnabled(true);
     ui->pushButton_5->setEnabled(true);
     //ui->pushButton_6->setEnabled(true);
     //ui->pushButton_7->setEnabled(true);
     //ui->pushButton_8->setEnabled(true);
     //ui->pushButton_9->setEnabled(true);
     //ui->pushButton_10->setEnabled(true);
     ui->pushButton_11->setEnabled(true);
     ui->pushButton_14->setEnabled(true);
     //ui->pushButton_15->setEnabled(true);
     ui->pushButton_12->setEnabled(false);
}
