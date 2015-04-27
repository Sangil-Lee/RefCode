#include "MainWindow.h"

MainWindow::MainWindow(const QString optype):moptype(optype)
{
	init();
}

MainWindow::~MainWindow()
{
	qDebug("~MainWindow is called");
}

void MainWindow::init()
{
	makeUI();

	// createStatusBar()
	statusBar()->showMessage(tr("Ready"));
	attachWidget();
	setGeometry(0,0,850,460);
	setMaximumSize(850,460);
	setMinimumSize(850,460);

	createActions();
	createConnects();
}

void MainWindow::makeUI()
{
	//setObjectName (QString::fromUtf8("heatingmainwidget"));
	//centralwidget = new QWidget(this);
	//centralwidget->setObjectName (QString::fromUtf8("centralwidget"));
	//centralwidget->setWindowTitle(QApplication::translate("Heating Permission", "Central", 0, QApplication::UnicodeUTF8));

	setWindowTitle(QApplication::translate("Heating Permission", "Heating Permission", 0, QApplication::UnicodeUTF8));
}

void MainWindow::createActions()
{
	if(centralwidget == NULL) return;
}

void MainWindow::createConnects()
{
	if(centralwidget == NULL) return;
	pbNBIA = centralwidget->findChild<QPushButton *>("PBNBIA");
	if( pbNBIA ) connect(pbNBIA, SIGNAL(clicked()), this, SLOT(PBAction()));

	pbNBIB = centralwidget->findChild<QPushButton *>("PBNBIB");
	if( pbNBIB ) connect(pbNBIB, SIGNAL(clicked()), this, SLOT(PBAction()));

	pbECH110 = centralwidget->findChild<QPushButton *>("PBECH110");
	if( pbECH110 ) connect(pbECH110, SIGNAL(clicked()), this, SLOT(PBAction()));

	pbECH170 = centralwidget->findChild<QPushButton *>("PBECH170");
	if( pbECH170 ) connect(pbECH170, SIGNAL(clicked()), this, SLOT(PBAction()));

	pbLHCD = centralwidget->findChild<QPushButton *>("PBLHCD");
	if( pbLHCD ) connect(pbLHCD, SIGNAL(clicked()), this, SLOT(PBAction()));

	pbICRF = centralwidget->findChild<QPushButton *>("PBICRF");
	if( pbICRF ) connect(pbICRF, SIGNAL(clicked()), this, SLOT(PBAction()));
}
void MainWindow::PBAction()
{
	QPushButton *pBut = qobject_cast<QPushButton *>(sender());
	//qDebug("%s", pBut -> objectName().toStdString().c_str());
	QString pbName = pBut -> objectName();
#if 0
	pvVariable *pVar = 0;
	if(pbName.compare("PBNBIA") == 0 )
	{
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_NB11");
	}
	else if(pbName.compare("PBNBIB") == 0 )
	{
		qDebug("***(%s)", pBut -> objectName().toStdString().c_str());
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_NB12");
	}
	else if(pbName.compare("PBECH110") == 0 )
	{
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_EC");
	}
	else if(pbName.compare("PBECH170") == 0 )
	{
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_EC1");
	}
	else if(pbName.compare("PBLHCD") == 0 )
	{
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_LH");
	}
	else if(pbName.compare("PBICRF") == 0 )
	{
		pVar = msys->newVariable("DEVOPI01_HEAT_INT_RDY_IC");
	}
	msys->pend(0.5);
	pvValue    value;
	value.shortVal[0] = 0;
	pVar->put(pvTypeSHORT, 1, &value);
	epicsThreadSleep(0.05);
#else
	chid channel;
	int status = 0;
	int value = 0;
	string chname;
	if(pbName.compare("PBNBIA") == 0 )
	{
		chname = "NB11_KSTAR_OPR_RDY";
	}
	else if(pbName.compare("PBNBIB") == 0 )
	{
		chname = "NB12_KSTAR_OPR_RDY";
	}
	else if(pbName.compare("PBECH110") == 0 )
	{
		chname = "ECH_KSTAR_OPR_RDY";
	}
	else if(pbName.compare("PBECH170") == 0 )
	{
		chname = "EC1_KSTAR_OPR_RDY";
	}
	else if(pbName.compare("PBLHCD") == 0 )
	{
		chname = "LH1_KSTAR_OPR_RDY";
	}
	else if(pbName.compare("PBICRF") == 0 )
	{
		chname = "ICRF_KSTAR_OPR_RDY";
	}

	status = ca_search_and_connect(chname.c_str(), &channel, NULL, NULL);
	status = ca_pend_io ( 1.0 );
	SEVCHK (status, 0);
	ca_put(DBR_INT, channel, &value);
	ca_flush_io();
	epicsThreadSleep(0.05);
	ca_poll();
	ca_clear_channel(channel);
#endif
}

void MainWindow::attachWidget()
{
	if(moptype.isEmpty()==true)
		pattach = new AttachChannelAccess("/usr/local/opi/ui/HeatControl_NO.ui", 1);
	else if(moptype.compare("CPO",Qt::CaseInsensitive) == 0)
		pattach = new AttachChannelAccess("/usr/local/opi/ui/HeatControl_CPO.ui", 1);
	else if(moptype.compare("HO", Qt::CaseInsensitive) == 0)
		pattach = new AttachChannelAccess("/usr/local/opi/ui/HeatControl_HO.ui", 1);

	centralwidget = pattach->GetWidget();
	if(centralwidget == NULL) return;

	setCentralWidget(centralwidget);
}

void MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
