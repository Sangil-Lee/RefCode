#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TitleClear();


    main_pattach = new AttachChannelAccess(this);

    mainopiform = new panel_mainopiform;    
    ui->dockWidget->setWidget(mainopiform);
    ui->dockWidget->widget()->close();
    cssetform = new panel_cssetform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(cssetform);
    pfopiform = new panel_pfopiform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfopiform);
    pfsetform = new panel_pfsetform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfsetform);;
    tfdispform = new panel_tfdispform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfdispform);
    tfopiform = new panel_tfopiform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfopiform);
    tfsetform = new panel_tfsetform;
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfsetform);

    maindispform_pattach = NULL;
    csdispform_pattach = NULL;
    pfdispform_pattach = NULL;
    tftestform_pattach = NULL;
    cstestform_pattach = NULL;
    pftestform_pattach = NULL;
    tfdispnewpvform_pattach = NULL;
    cssetnewpvform_pattach = NULL;
    pfsetnewpvform_pattach = NULL;
    tfsetnewpvform_pattach = NULL;

    connect(tfdispform,SIGNAL(OnClickNewPv()),this,SLOT(OnClickTFDispNewPvForm()));
    connect(tfsetform,SIGNAL(OnClickNewPv()),this,SLOT(OnClickTFSetNewPvForm()));
    connect(cssetform,SIGNAL(OnClickNewPv()),this,SLOT(OnClickCSSetNewPvForm()));
    connect(pfsetform,SIGNAL(OnClickNewPv()),this,SLOT(OnClickPFSetNewPvForm()));

    setWindowState(windowState() /*| Qt::WindowFullScreen*/);
    setWindowTitle("KSTAR Quench Detection System");

    on_pushButton_clicked();
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

void MainWindow::TitleClear()
{
    QWidget *titlebarWidget1 = ui->dockWidget->titleBarWidget();
    QWidget *EmptyWidget1 = new QWidget();

    ui->dockWidget->setTitleBarWidget(EmptyWidget1);
    delete titlebarWidget1;
}


void MainWindow::on_pushButton_clicked()
{

    if(maindispform_pattach == NULL)
    {
       maindispform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_maindispform.ui",1);
       if(maindispform_pattach)
            maindispform_widget = maindispform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();    
    ui->dockWidget->setWidget(maindispform_widget);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfdispform);
}

void MainWindow::on_pushButton_3_clicked()
{

    if(csdispform_pattach == NULL)
    {
       csdispform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_csdispform.ui",1);
       if(csdispform_pattach)
            csdispform_widget = csdispform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(csdispform_widget);
}

void MainWindow::on_pushButton_4_clicked()
{

    if(pfdispform_pattach == NULL)
    {
       pfdispform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_pfdispform.ui",1);
       if(pfdispform_pattach)
            pfdispform_widget = pfdispform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfdispform_widget);
}

void MainWindow::on_pushButton_6_clicked()
{

    if(tftestform_pattach == NULL)
    {
       tftestform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_tftestform.ui",1);
       if(tftestform_pattach)
            tftestform_widget = tftestform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tftestform_widget);
}

void MainWindow::on_pushButton_7_clicked()
{

    if(cstestform_pattach == NULL)
    {
       cstestform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_cstestform.ui",1);
       if(cstestform_pattach)
            cstestform_widget = cstestform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(cstestform_widget);
}

void MainWindow::on_pushButton_5_clicked()
{

    if(pftestform_pattach == NULL)
    {
       pftestform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_pftestform.ui",1);
       if(pftestform_pattach)
            pftestform_widget = pftestform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pftestform_widget);
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfsetform);
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(cssetform);
}

void MainWindow::on_pushButton_10_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfsetform);
}

void MainWindow::on_pushButton_16_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(mainopiform);
}

void MainWindow::on_pushButton_14_clicked()
{
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfopiform);
}

void MainWindow::on_pushButton_15_clicked()
{    
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfopiform);
}

void MainWindow::OnClickTFDispNewPvForm()
{

    if(tfdispnewpvform_pattach == NULL)
    {
       tfdispnewpvform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_tfdispnewpvform.ui",1);
       if(tfdispnewpvform_pattach)
            tfdispnewpvform_widget = tfdispnewpvform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfdispnewpvform_widget);
}
void MainWindow::OnClickCSSetNewPvForm()
{

    if(cssetnewpvform_pattach == NULL)
    {
       cssetnewpvform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_cssetnewpvform.ui",1);
       if(cssetnewpvform_pattach)
            cssetnewpvform_widget = cssetnewpvform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(cssetnewpvform_widget);
}
void MainWindow::OnClickPFSetNewPvForm()
{

    if(pfsetnewpvform_pattach == NULL)
    {
       pfsetnewpvform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_pfsetnewpvform.ui",1);
       if(pfsetnewpvform_pattach)
            pfsetnewpvform_widget = pfsetnewpvform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(pfsetnewpvform_widget);
}
void MainWindow::OnClickTFSetNewPvForm()
{

    if(tfsetnewpvform_pattach == NULL)
    {
       tfsetnewpvform_pattach = new AttachChannelAccess("/usr/local/opi/ui/panel_tfsetnewpvform.ui",1);
       if(tfsetnewpvform_pattach)
            tfsetnewpvform_widget = tfsetnewpvform_pattach->GetWidget();
    }
    ui->dockWidget->widget()->close();
    ui->dockWidget->setWidget(tfsetnewpvform_widget);
}

void MainWindow::on_pushButton_11_clicked()
{
#if 0 /* OMY */
    if(tfgrpform_pattach == NULL){
        tfgrpform_pattach = new AttachChannelAccess("./panel_tfgrpform.ui",1);
        if(tfgrpform_pattach)
            tfgrpform_widget->setCentralWidget(tfgrpform_pattach->GetWidget());
    }

    tfgrpform_widget->show();
#endif
}

void MainWindow::on_pushButton_12_clicked()
{
#if 0
    if(csgrpform_pattach == NULL){
        csgrpform_pattach = new AttachChannelAccess("./panel_csgrpform.ui",1);
        if(csgrpform_pattach)
            csgrpform_widget->setCentralWidget(csgrpform_pattach->GetWidget());
    }

    csgrpform_widget->show();
#endif
}

void MainWindow::on_pushButton_13_clicked()
{
#if 0
    if(pfgrpform_pattach == NULL){
        pfgrpform_pattach = new AttachChannelAccess("./panel_pfgrpform.ui",1);
        if(pfgrpform_pattach)
            pfgrpform_widget->setCentralWidget(pfgrpform_pattach->GetWidget());
    }

    pfgrpform_widget->show();
#endif
}
