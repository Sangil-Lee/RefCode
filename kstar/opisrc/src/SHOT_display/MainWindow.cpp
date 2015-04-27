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
	createToolbars();
	//setStatusBar(statusBar());
	#if 0
	QMainWindow *qm = new QMainWindow();
	qm = (QMainWindow *) stackedWidget->currentWidget();
	setStatusBar(qm->statusBar());
	#endif
	setStatusBar(((QMainWindow *) stackedWidget->currentWidget())->statusBar());
}


void
MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("SHOT Display", "SHOT Display", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/sd_icon.png")));
	centralwidget = new QWidget(this);
	centralwidget->setObjectName (QString::fromUtf8("centralwidget"));
	centralwidget->setWindowTitle(QApplication::translate("KWT test", "Central", 0, QApplication::UnicodeUTF8));

	centralwidget->setGeometry(QRect(0,0,1280,950));
	centralwidget->setMinimumSize(QSize(1280,950));
	centralwidget->setMaximumSize(QSize(1920,1280));

	attachWidget();
	setCentralWidget(centralwidget);


}

void
MainWindow::createActions()
{
	expAction = new QAction(tr("&Experiment"), this);
    macAction = new QAction(tr("&Machine"), this);

    connect(expAction, SIGNAL(triggered()), this, SLOT(expCA()));
    connect(macAction, SIGNAL(triggered()), this, SLOT(macCA()));
}

void
MainWindow::createConnects()
{

}

void
MainWindow::createToolbars()
{
	ToolBar = addToolBar(tr("&ToolBar"));
    ToolBar->addAction(expAction);
    ToolBar->addAction(macAction);
}

void
MainWindow::expCA()
{
	stackedWidget->setCurrentIndex(0);
	setStatusBar(((QMainWindow *) stackedWidget->currentWidget())->statusBar());
}

void
MainWindow::macCA()
{
	stackedWidget->setCurrentIndex(1);
	setStatusBar(((QMainWindow *) stackedWidget->currentWidget())->statusBar());
}

void
MainWindow::attachWidget()
{
	QStringList uilist;
	QString displayname;
	uilist<<"ExperimentsStatus"<<"SD_Machine_Status";

	hbox = new QHBoxLayout();
	stackedWidget = new QStackedWidget(centralwidget);
	hbox->addWidget(stackedWidget);
	centralwidget->setLayout(hbox);

	for(int i = 0; i < uilist.size();i++)
	{
		displayname = QString("/usr/local/opi/ui/")+uilist.at(i)+".ui";
		pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i);
		vecACHAcc.push_back(pattach);
		if (!pattach->GetWidget())
		{
			qDebug("%s isn't exist.", displayname.toStdString().c_str());
		}
		else
		{
			QWidget *w = pattach->GetWidget();
			w->setAutoFillBackground (true);
			stackedWidget->addWidget(pattach->GetWidget());
		};
	};

	stackedWidget->setCurrentIndex(0);

}

/*
 * It's very important that 
 * this application should kill process itself to protext segmentation fault.
 */
void 
MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
