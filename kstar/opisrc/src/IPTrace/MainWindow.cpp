#include "MainWindow.h"

MainWindow::MainWindow()
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
	setGeometry(0,0,680,635);
	setMaximumSize(680,635);
	setMinimumSize(680,635);

	createActions();
	createConnects();
}

void MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("IPTrace", "IPTrace", 0, QApplication::UnicodeUTF8));
}

void MainWindow::createActions()
{
	if(centralwidget == NULL) return;
}

void MainWindow::createConnects()
{
	if(centralwidget == NULL) return;

	mpWaveform = centralwidget->findChild<CAWaveformPut *>("CAWaveformPut");
	if(mpWaveform) 
	{
		connect(mpWaveform, SIGNAL(resetsignal()), mpWaveform,SLOT(reSet()));
		mpWaveform->MonitorOn();
	};
	 
}
#if 0
void MainWindow::PBAction()
{
	QPushButton *pBut = qobject_cast<QPushButton *>(sender());
	QString pbName = pBut -> objectName();
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
}
#endif

void MainWindow::attachWidget()
{
	pattach = new AttachChannelAccess("/usr/local/opi/ui/IPTrace.ui", 1);
	centralwidget = pattach->GetWidget();
	if(centralwidget == NULL) return;

	setCentralWidget(centralwidget);
}

void MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
