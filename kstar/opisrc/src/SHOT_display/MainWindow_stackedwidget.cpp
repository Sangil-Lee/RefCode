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
//[[linupark added
	displayDocklist
		<<"ExperimentsStatus"
		<<"DiagnosticsOperationStatus"
		<<"MachinesStatus";
//]]linupark added

	makeUI();
	createActions();
	createConnects();
	createToolbars();
	setStatusBar(statusBar());
#ifdef ATTACHALL


	attachAllWidget();

	setIndex(0);
#else

	attachWidget();
#endif
}

void
MainWindow::makeUI()
{
	setWindowTitle(QApplication::translate("KWT test", "KWT", 0, QApplication::UnicodeUTF8));
	centralwidget = new QWidget(this);
	centralwidget->setObjectName (QString::fromUtf8("centralwidget"));
	centralwidget->setWindowTitle(QApplication::translate("KWT test", "Central", 0, QApplication::UnicodeUTF8));

	centralwidget->setGeometry(QRect(0,0,1024,768));
	centralwidget->setMinimumSize(QSize(1024,768));
	centralwidget->setMaximumSize(QSize(1920,1080));
	
#ifdef ATTACHALL	
	stackedWidget = new QStackedWidget();
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    stackedWidget->setGeometry(QRect(0, 0, 1920, 1080));
#endif

	setCentralWidget(centralwidget);

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
	QObject::connect(centralwidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 

}

void
MainWindow::createToolbars()
{
	ToolBar = addToolBar(tr("&ToolBar"));
    ToolBar->addAction(expAction);
    ToolBar->addAction(diagAction);
    ToolBar->addAction(macAction);
}

void
MainWindow::expCA()
{
#ifdef ATTACHALL
	setIndex(0);
#else
    qDebug("expCA\n");

	if(pattach1->GetWidget()) 
	{
		qDebug("pattach1 exists.");
	} else
	{
		pattach1 = new AttachChannelAccess("/usr/local/opi/ui/ExperimentsStatus.ui",1);
		qDebug("pattach1 not exists.\n");
	}
	if(pattach1->GetWidget())
	{
		centralwidget = pattach1->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("ExperimentsStatus", "ExperimentsStatus", 0, QApplication::UnicodeUTF8));
		qDebug("pattach1 set\n");
	}
#endif	
}

void
MainWindow::diagCA()
{
#ifdef ATTACHALL
		setIndex(1);
#else
    qDebug("diagCA\n");
	#if 0
	if(pattach2->GetWidget()) 
	{
		qDebug("pattach2 exists. (1)");
	}
	else
	{
		pattach2 = new AttachChannelAccess("/usr/local/opi/ui/DiagnosticsOperationStatus.ui",1);
	}
	#endif

	pattach2 = new AttachChannelAccess("/usr/local/opi/ui/DiagnosticsOperationStatus.ui",1);
		qDebug("pattach2 exists. (2)");

	if(pattach2->GetWidget())
	{
		centralwidget = pattach2->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("DiagnosticsOperationStatus", "DiagnosticsOperationStatus", 0, QApplication::UnicodeUTF8));
	}

		qDebug("pattach2 exists. (3)");
#endif		
}

void
MainWindow::macCA()
{
#ifdef ATTACHALL
		setIndex(2);
#else

    qDebug("macCA\n");
	pattach3 = new AttachChannelAccess("/usr/local/opi/ui/MachineStatus.ui",1);

	if(pattach3->GetWidget())
	{
		centralwidget = pattach3->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("MachinesStatus", "MachinesStatus", 0, QApplication::UnicodeUTF8));
	}
#endif	
}

void
MainWindow::attachWidget()
{
	pattach1 = new AttachChannelAccess("/usr/local/opi/ui/ExperimentsStatus.ui",1);
	qDebug("attachWidget\n");
	if(pattach1->GetWidget())
	{
		centralwidget = pattach1->GetWidget();
		centralwidget->setWindowTitle(QApplication::translate("ExperimentsStatus", "ExperimentsStatus", 0, QApplication::UnicodeUTF8));
		qDebug("pattach1 set\n");
	}
		
}

void MainWindow::attachAllWidget()
{
	QString displayname;
	qDebug("attachAllWidget\n");
    char display[100];
	for(int i = 0; i < displayDocklist.size();i++)
	{
		qDebug("i=%d\n",i);
		displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(i)+".ui";
		
		qDebug("display1 =%d\n",i);
	    sprintf(display,"%s: %d",displayname.toStdString().c_str(), i);
		strcpy(display, displayname.toStdString().c_str());
		qDebug("display2 =%s\n",display);
		pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i);

		qDebug("display3 =%d\n",i);
		vecACHAcc.push_back(pattach);
		if (!pattach->GetWidget())
		{

		qDebug("display4 =%d\n",i);
			 QWidget *page = new QWidget();
 //			 char display[30];
			 sprintf(display,"%s: %d",displayname.toStdString().c_str(), i);
			 stackedWidget->addWidget(page);
		}
		else
		{
		qDebug("display5 =%d\n",i);
			QWidget *w = pattach->GetWidget();
			w->setAutoFillBackground (true);
			stackedWidget->addWidget(pattach->GetWidget());
		};
	};
	qDebug("display end \n");
}

void MainWindow::setIndex(const int index)
{
	stackedWidget->setCurrentIndex(index);
	AttachChannelAccess *pattach = 0;
	qDebug("setIndex=%d\n",index);

	for (size_t i = 0; i < vecACHAcc.size(); i++)
	{
		qDebug("setIndex i=%d, for\n",i);
		pattach = vecACHAcc.at(i);
		if (!pattach->GetWidget()) continue;
		if ( i == index )
		{
			qDebug("setIndex pattach->SetUiCurIndex(%d) for\n",index);
			pattach->SetUiCurIndex(index);
			qDebug("setIndex pattach->GetWidget for\n");
			centralwidget = pattach->GetWidget();
		}
		else pattach->SetUiCurIndex(-1);
	}
	
	
}
void MainWindow::setDefaultIndex(int index) 
{
	qDebug("setDefaultIndex\n");

	if (index == 0)
	{
		setIndex(0);
		//stackedWidget->setCurrentIndex(0);
	}
	else if (index == 1)
	{
		setIndex(2);
		//stackedWidget->setCurrentIndex(1);
	}
	else if (index == 2)
	{
		setIndex(2);
		//stackedWidget->setCurrentIndex(2);
	}
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
