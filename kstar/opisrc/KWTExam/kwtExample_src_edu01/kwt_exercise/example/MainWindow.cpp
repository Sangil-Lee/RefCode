#include "MainWindow.h"


MainWindow::MainWindow()
{
	mUifile="";
	init();
}

MainWindow::MainWindow(QString uifilepath):mUifile(uifilepath)
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
	setStatusBar(statusBar());
	attachWidget();
}

void
MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("KWT test", "KWT", 0, QApplication::UnicodeUTF8));
	centralwidget = new QWidget(this);
	centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
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
#if 0
	pattach = new AttachChannelAccess("../ui/controlmaster.ui", 1);
#endif
#if 1
	pattach = new AttachChannelAccess(mUifile, 1);
	if(pattach->GetWidget())
	{
		//pattach->SetUiCurIndex(1);
		centralwidget = pattach->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("Pattach", "Pattach", 0, QApplication::UnicodeUTF8));
	}
	setCentralWidget(centralwidget);
#endif
}

/*
 * It's very important that
 * this application should kill process itself to protect segmentation fault.
 */
void
MainWindow::closeEvent(QCloseEvent *)
{
	kill (getpid(), SIGTERM);
}


