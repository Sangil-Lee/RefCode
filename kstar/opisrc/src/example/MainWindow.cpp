#include "MainWindow.h"

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
	attachWidget();
}

void
MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("KWT test", "KWT", 0, QApplication::UnicodeUTF8));
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
	pattach = new AttachChannelAccess("../ui/example.ui",1);
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
