#include "MainWindow.h"

QPalette palette1;
QPalette palette2;
QBrush fontbrush1(QColor(255, 255, 255, 255));	//font white
QBrush backbrush1(QColor(0, 150, 0, 255));		//background green

QBrush fontbrush2(QColor(0, 0, 0, 255));		//font black
QBrush backbrush2(QColor(255, 255, 255, 255));	//background white

MainWindow::MainWindow()
{
	ui.setupUi(this);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_POLORIZATION_Show()));
	connect(ui.pushButton_TC, SIGNAL(clicked()), this, SLOT(slot_TCMONITORING_Show()));

#if 0
	connect(ui.calbKshotstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_Shot_Stat_Change1(bool)));
	connect(ui.calbLshotstat, SIGNAL(valueChanged(bool)), this, SLOT(slot_Shot_Stat_Change2(bool)));
	connect(ui.calbKshot, SIGNAL(valueChanged(double)), this, SLOT(slot_Shot_Number_Change1(double)));
#endif

	//set label text
	palette1.setBrush(QPalette::Active, QPalette::WindowText, fontbrush1);
	palette1.setBrush(QPalette::Inactive, QPalette::WindowText, fontbrush1);

	//set background
	palette1.setBrush(QPalette::Active, QPalette::Window, backbrush1);
	palette1.setBrush(QPalette::Inactive, QPalette::Window, backbrush1);

	//set label text
	palette2.setBrush(QPalette::Active, QPalette::WindowText, fontbrush2);
	palette2.setBrush(QPalette::Inactive, QPalette::WindowText, fontbrush2);

	//set background
	palette2.setBrush(QPalette::Active, QPalette::Window, backbrush2);
	palette2.setBrush(QPalette::Inactive, QPalette::Window, backbrush2);

	connect(ui.calbCM_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_CM_Stat_Change1(bool)));
	connect(ui.calbCM_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_CM_Stat_Change2(bool)));
	connect(ui.calbTLCM_1, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change1(bool)));
	connect(ui.calbTLCM_2, SIGNAL(valueChanged(bool)), this, SLOT(slot_TLCM_Stat_Change2(bool)));
	connect(ui.calbOPKstar, SIGNAL(valueChanged(bool)), this, SLOT(slot_OP_Stat_Change1(bool)));
	connect(ui.calbOPDummy, SIGNAL(valueChanged(bool)), this, SLOT(slot_OP_Stat_Change2(bool)));
	
	connect(ui.calbL1, SIGNAL(valueChanged(bool)), this, SLOT(slot_L1_Stat_Change(bool)));
	connect(ui.calbL2, SIGNAL(valueChanged(bool)), this, SLOT(slot_L2_Stat_Change(bool)));
	connect(ui.calbL3, SIGNAL(valueChanged(bool)), this, SLOT(slot_L3_Stat_Change(bool)));
	connect(ui.calbL4, SIGNAL(valueChanged(bool)), this, SLOT(slot_L4_Stat_Change(bool)));
	
	ui.lbKSTARLogo->setPixmap(QPixmap(QString::fromUtf8("/usr/local/opi/images/ECCD_kstar.gif")));

	ui.calbKshotstat->hide();
	ui.calbLshotstat->hide();
	ui.calbKshot->show();
	ui.calbLshot->show();

	insertStackedWidgetPage();

	slot_POLORIZATION_Show ();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}

void MainWindow::slot_POLORIZATION_Show (void)
{
	ui.stackedWidget_main->setCurrentIndex(STACKED_POLARIZATION);
	setpushButton(0);
}

void MainWindow::slot_TCMONITORING_Show (void)
{
	ui.stackedWidget_main->setCurrentIndex(STACKED_TCMONITORING);
	setpushButton(1);
}

void MainWindow::setpushButton(int inum){
	QFont font;
	font.setPointSize(11);
	font.setBold(false);

	ui.pushButton->setFont(font);
	ui.pushButton_TC->setFont(font);

	ui.CALBfaIntl_1_0->show();
	ui.CALBfaIntl_1_1->show();
	ui.CALBfaIntl_1_2->show();
	ui.CALBfaIntl_1_3->show();
	ui.CALBfaIntl_1_4->show();
	ui.CALBfaIntl_1_5->show();

	ui.CALBslIntl_1_0->show();
	ui.CALBslIntl_1_1->show();
	ui.CALBslIntl_1_2->show();
	ui.CALBslIntl_1_3->show();
	ui.CALBslIntl_1_4->show();
	ui.CALBslIntl_1_5->show();

	font.setBold(true);

	switch (inum) {
		case 0:
			ui.pushButton->setFont(font);	
			break;
		case 1:
			ui.pushButton_TC->setFont(font);	
			break;
	}
}

void MainWindow::slot_CM_Stat_Change1(bool bMode)
{
	bEMode = bMode;
	ui.lbCM_3->hide();
	if(bMode == true)
	{
		ui.calbCM_1->setPalette(palette1);
		ui.calbCM_1->show();
	}
	else
	{
		ui.calbCM_1->setPalette(palette2);
		ui.calbCM_1->hide();
	}
	ui.calbCM_1->setAutoFillBackground(true);
	if(bEMode == false && bPMode == false) ui.lbCM_3->show(); 
}

void MainWindow::slot_CM_Stat_Change2(bool bMode)
{
	bPMode = bMode;
	ui.lbCM_3->hide();
	if(bMode == true)
	{
		ui.calbCM_2->setFillColor(QColor("red"));
		ui.calbCM_2->show();
	}
	else
	{
		ui.calbCM_2->setPalette(palette2);
		ui.calbCM_2->hide();
	}
	ui.calbCM_2->setAutoFillBackground(true); 
	if(bEMode == false && bPMode == false) ui.lbCM_3->show();
}

void MainWindow::slot_TLCM_Stat_Change1(bool bMode)
{
	bTLRemoteMode = bMode;
	ui.lbTLCM_3->hide();
	if(bMode == true)
	{
		ui.calbTLCM_1->setPalette(palette1);
		ui.calbTLCM_1->show();
	}
	else
	{
		ui.calbTLCM_1->setPalette(palette2);
		ui.calbTLCM_1->hide();
	}
	ui.calbTLCM_1->setAutoFillBackground(true);
	if(bTLRemoteMode == false && bTLLocalMode == false) ui.lbTLCM_3->show(); 
}

void MainWindow::slot_TLCM_Stat_Change2(bool bMode)
{
	bTLLocalMode = bMode;
	ui.lbTLCM_3->hide();
	if(bMode == true)
	{
		ui.calbTLCM_2->setFillColor(QColor("red"));
		ui.calbTLCM_2->show();
	}
	else
	{
		ui.calbTLCM_2->setPalette(palette2);
		ui.calbTLCM_2->hide();
	}
	ui.calbTLCM_2->setAutoFillBackground(true); 
	if(bTLRemoteMode == false && bTLLocalMode == false) ui.lbTLCM_3->show();
}

void MainWindow::slot_OP_Stat_Change1(bool bMode)
{
	bKstarMode = bMode;
	if(bMode == false) {
		ui.calbOPKstar->setPalette(palette2);
		ui.calbOPKstar->setAutoFillBackground(true);
	}
	else {
	}
}

void MainWindow::slot_OP_Stat_Change2(bool bMode)
{
	bDummyMode = bMode;	
	if(bMode == false) {
		ui.calbOPDummy->setPalette(palette2);
		ui.calbOPDummy->setAutoFillBackground(true);
	}
	else {
	}
}

void MainWindow::slot_L1_Stat_Change(bool bMode)
{
	if(bMode == true) {
		ui.calbL1->show();
	}
	else {
		ui.calbL1->hide();
	}
}

void MainWindow::slot_L2_Stat_Change(bool bMode)
{
	if(bMode == true) {
		ui.calbL2->show();
	}
	else {
		ui.calbL2->hide();
	}
}

void MainWindow::slot_L3_Stat_Change(bool bMode)
{
	if(bMode == true) {
		ui.calbL3->show();
	}
	else {
		ui.calbL3->hide();
	}
}

void MainWindow::slot_L4_Stat_Change(bool bMode)
{
	if(bMode == true) {
		ui.calbL4->show();
	}
	else {
		ui.calbL4->hide();
	}
}

void MainWindow::insertStackedWidgetPage()
{
	AttachChannelAccess *pattach;
	QWidget *wdg;

	pattach = new AttachChannelAccess(this);

	pattach = new AttachChannelAccess("/usr/local/opi/ui/EC1_POL_Polarization.ui", 1);

	if (pattach->GetWidget()) {
		wdg = (QWidget*)ui.stackedWidget_main->widget(STACKED_POLARIZATION);
		if( !wdg ) {
			QMessageBox::information(0, "KSTAR EC1 Polarization", tr("get StackedWidget ERROR") );
			return;
		}

		ui.stackedWidget_main->removeWidget(wdg);
		ui.stackedWidget_main->insertWidget(STACKED_POLARIZATION, pattach->GetWidget());
	}

	pattach = new AttachChannelAccess("/usr/local/opi/ui/EC1_POL_TC.ui", 1);

	if (pattach->GetWidget()) {
		wdg = (QWidget*)ui.stackedWidget_main->widget(STACKED_TCMONITORING);
		if( !wdg ) {
			QMessageBox::information(0, "KSTAR EC1 Thermocouple Monitoring", tr("get StackedWidget ERROR") );
			return;
		}

		ui.stackedWidget_main->removeWidget(wdg);
		ui.stackedWidget_main->insertWidget(STACKED_TCMONITORING, pattach->GetWidget());
	}
}

