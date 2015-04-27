#include "MainWindow.h"

extern char ui_name[256];

MainWindow::MainWindow()
{
	init();
}

MainWindow::~MainWindow()
{
	qDebug("~MainWindow is called");
}

void
MainWindow::init()
{
	makeUI();
	createActions();
	createConnects();

	// createStatusBar()
	statusBar()->showMessage(tr("Ready"));

	attachWidget();
}

void
MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("KWT", "NBI-1 RGA Monitoring - KSTAR", 0, QApplication::UnicodeUTF8));
	centralwidget = new QWidget(this);
	centralwidget->setObjectName (QString::fromUtf8("centralwidget"));
	centralwidget->setWindowTitle(QApplication::translate("KWT test", "Central", 0, QApplication::UnicodeUTF8));
}

void
MainWindow::createActions()
{
}

void
MainWindow::createConnects()
{
}

void
MainWindow::attachWidget()
{
	pattach = new AttachChannelAccess(ui_name, 1);
	pattach->SetUiCurIndex(1);
	if(pattach->GetWidget())
	{
		centralwidget = pattach->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("Pattach", "Pattach", 0, QApplication::UnicodeUTF8));
	}
	setCentralWidget(centralwidget);
}

void 
MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
