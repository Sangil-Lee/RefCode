#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow()//:archiver(), archiver2(),archiver3(),archiver4(),signaldb()
{
	init();
}

MainWindow::~MainWindow()
{
	qDebug("~MainWindow Call");
}

void
MainWindow::init()
{
	makeUI();
	createActions();
	createConnects();
	createToolbars();

//	attachAllWidget();

}


void
MainWindow::createActions()
{
	expAction = new QAction(tr("&Experiment"), this);	
	diagAction = new QAction(tr("&Diagnostic"), this);	
	macAction = new QAction(tr("&Machine"), this);	

	connect(expAction, SIGNAL(triggered()), this, SLOT(expCA()));
	connect(diagAction, SIGNAL(triggered()), this, SLOT(diagCA()));
	connect(macAction, SIGNAL(triggered()), this, SLOT(macCA()));
}
void
MainWindow::createConnects()
{

}

void
MainWindow::makeUI()
{
	//pattach = new AttachChannelAccess("/usr/local/opisrc/src/SHOT_display/ui/ShotInfoDisplay.ui",1);
	pattach = new AttachChannelAccess("/usr/local/opi/ui/UItemp24.ui",1);
	if(pattach->GetWidget())
	{
		centralwidget = pattach->GetWidget();
	}

	centralwidget->setGeometry(QRect(0,0,1024,768));
	centralwidget->setMinimumSize(QSize(1024,768));
	centralwidget->setMaximumSize(QSize(1920,1080));

    QString objName;
	objName="test";
	qDebug("ObjectName:%s", objName.toStdString().c_str());

	setCentralWidget(centralwidget);

    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));

} // setupUi

void MainWindow::createToolbars()
{
	ToolBar = addToolBar(tr("&ToolBar"));
	ToolBar->addAction(expAction);
	ToolBar->addAction(diagAction);
	ToolBar->addAction(macAction);

}

void MainWindow::attachAllWidget()
{
}

void MainWindow::setIndex(const int index)
{

}
void MainWindow::setEnableButtons(QPushButton *pobj)
{

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	QPushButton *pBut = (QPushButton*)obj;
	QString objName = pBut->objectName();

	return QMainWindow::eventFilter(obj, event);
}

void
MainWindow::showMultiplot()
{

}
void
MainWindow::showArchivesheet()
{

}
void
MainWindow::showPVListViewer()
{

}
void
MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}

void
MainWindow::showSignalDB()
{

}

void
MainWindow::showArchiverviewer()
{


}

void
MainWindow::showManual()
{

}

void
MainWindow::showAboutTMS()
{

}
void
MainWindow::setDefaultIndex(int index) 
{

}
void
MainWindow::changeText(const QString &str) 
{

}
void
MainWindow::keyPressEvent(QKeyEvent *event )
{

}

void
MainWindow::expCA()
{
	qDebug("expCA\n");
}

void
MainWindow::diagCA()
{
	qDebug("diagCA\n");
}

void
MainWindow::macCA()
{
	qDebug("macCA\n");
}
