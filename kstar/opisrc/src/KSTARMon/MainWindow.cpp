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
	setWindowTitle(QApplication::translate("KSTAR Monitoring Tool", mUifile.toStdString().c_str(), 0, QApplication::UnicodeUTF8));
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
#if 1
	QString uifilepath = "/usr/local/opi/ui/"+mUifile;
	//QString uifilepath = "../ui/"+mUifile;
	qDebug("uifile:%s",uifilepath.toStdString().c_str());
	pattach = new AttachChannelAccess(uifilepath, 1, true);
	if(pattach->GetWidget())
	{
		pattach->SetUiCurIndex(1);
		centralwidget = pattach->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("Pattach", "Pattach", 0, QApplication::UnicodeUTF8));
		setGeometry(centralwidget->geometry());
		setCentralWidget(centralwidget);
	};
#endif
}

/*
 * It's very important that
 * this application should kill process itself to protect segmentation fault.
 */
void
MainWindow::closeEvent(QCloseEvent *)
{
	if(pattach) 
	{
		delete pattach;
		pattach = 0;
	};
}


