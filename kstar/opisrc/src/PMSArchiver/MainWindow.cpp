#include <ios>
#include <iostream>
#include <fstream>
#include <epicsTimeHelper.h>
#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;
fstream outstream("./data.txt", ios::in);
static bool viewer(void *arg, const char *name, size_t , size_t , const CtrlInfo &,
             DbrType , DbrCount , const RawValue::Data *value)
{
	MainWindow *pMain = (MainWindow*)arg;
	if(pMain == 0) return false;

	char timebuf[30];
	epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &(value->stamp));
	//qDebug("Time:%s, PVName(%s:%f)", timebuf, name, value->value);
	//outstream << "Loop:" << pMain->mloop << ", Time: " << timebuf << ", PVName: ("<<name <<":" <<value->value<<")" << endl;

	int curpage = pMain->CurrentPage();

	if(pMain->mloop == 0)
	{
		pMain->AddEPICSTimeStamp(curpage, value->stamp);
	};
	pMain->AddPVAndValue(curpage, name, value->value);

    return true;
}

MainWindow::MainWindow():mloop(0),marraycount(0)
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
	displayDocklist<<"PMS_In_Board_Archive"<<"PMS_Out_Board_Archive";
	//page1index = page2index = 0;

	makeUI();
	createConnects();
	registerPushButtonsEvent();
#if 1
	attachAllWidget();
	setDefaultIndex(0) ;
	connectToArchiver();
#endif
}
void
MainWindow::createConnects()
{
	connect(dockWidget,SIGNAL(topLevelChanged(bool)), this, SLOT(showFullWindow(bool)) );
}
void
MainWindow::showFullWindow(bool toplevel)
{
	QPalette p;
	QPixmap bg;
    QSize normal_size;
	QSize full_size;

	//normal_size = QSize(1040,821);
	//full_size = QSize(1280,1024);

	if (toplevel == true)
	{
		if(dockWidget->geometry().x() < 1280)
		{
			dockWidget->setGeometry(0,0,1280,1024);
		} else
		{
			dockWidget->setGeometry(1280,0,1280,1024);
		}
		//dockWidget->setGeometry(0,0,1280,1024);
		//stackedWidget->currentWidget()->showFullScreen();
		stackedWidget->currentWidget()->setGeometry(0,0,1280,1000);
		full_size = stackedWidget->currentWidget()->size();

		if (stackedWidget->currentWidget()->palette().brush(QPalette::Window).style() == Qt::TexturePattern) 
		{
			bg = stackedWidget->currentWidget()->palette().brush(QPalette::Window).texture();
			bg = bg.scaled(full_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			p.setBrush(QPalette::Window, bg);
			stackedWidget->currentWidget()->setPalette(p);
		}
	} else
	{
		dockWidget->setGeometry(0,0,1080,821);
		stackedWidget->currentWidget()->setGeometry(0,0,1080,800);
		normal_size = stackedWidget->currentWidget()->size();

		if (stackedWidget->currentWidget()->palette().brush(QPalette::Window).style() == Qt::TexturePattern) {
		bg = stackedWidget->currentWidget()->palette().brush(QPalette::Window).texture();
		bg = bg.scaled(normal_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		p.setBrush(QPalette::Window, bg);
		stackedWidget->currentWidget()->setPalette(p);
		}
	}
}
void
MainWindow::makeUI()
{
    setObjectName(QString::fromUtf8("MainWindow"));
#if 1
    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	//connect(action_Exit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));
#endif

    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
	
    widget = new QWidget(centralwidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(190, 0, 1080, 821));
    widget->setMinimumSize(QSize(400, 0));
    vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(0);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    dockWidget = new QDockWidget(widget);
    dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
    dockWidget->setSizePolicy(sizePolicy);
    dockWidgetContents = new QWidget(dockWidget);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));

    vdockLayout = new QVBoxLayout(widget);
    vdockLayout->setSpacing(0);
    vdockLayout->setMargin(0);
    vdockLayout->setObjectName(QString::fromUtf8("vdockLayout"));

    stackedWidget = new QStackedWidget(dockWidgetContents);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    stackedWidget->setGeometry(QRect(0, 0, 1080, 821));
    dockWidget->setWidget(dockWidgetContents);

    vboxLayout->addWidget(dockWidget);

	dockWidgetContents->setLayout(vdockLayout);
	vdockLayout->addWidget(stackedWidget);

    tabWidget = new QTabWidget(centralwidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setGeometry(QRect(0, 0, 190, 821));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());

    tabWidget->setSizePolicy(sizePolicy1);
    tabWidget->setMaximumSize(QSize(16777215, 16777215));
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setElideMode(Qt::ElideNone);

    tab_0 = new QWidget();
    tab_0->setObjectName(QString::fromUtf8("tab_0"));
    QFont font;
    font.setPointSize(14);
    vboxLayout0 = new QVBoxLayout(tab_0);
    vboxLayout0->setSpacing(6);
    vboxLayout0->setMargin(9);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout0"));

    pushButton[0] = new QPushButton(tab_0);
    pushButton[0]->setObjectName(QString::fromUtf8("pushButton_0"));
    pushButton[0]->setFont(font);
    pushButton[0]->setText(QApplication::translate("MainWindow", "PMS In Board", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[0]);

    pushButton[1] = new QPushButton(tab_0);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font);
    pushButton[1]->setText(QApplication::translate("MainWindow", "PMS Out Board", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[1]);

    QSpacerItem *spacerItem;
    //spacerItem = new QSpacerItem(31, 61, QSizePolicy::Minimum, QSizePolicy::Expanding);
    spacerItem = new QSpacerItem(31, 61, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout0->addItem(spacerItem);

	//Add Code.
    archiveList = new QComboBox(tab_0);
    archiveList->setObjectName(QString::fromUtf8("archiveList"));
    vboxLayout0->addWidget(archiveList);

    Start_dateTime = new QDateTimeEdit(tab_0);
    Start_dateTime->setObjectName(QString::fromUtf8("Start_dateTime"));
    Start_dateTime->setDisplayFormat("yyyy/MM/dd hh:mm:ss");
    Start_dateTime->setDateTime(QDateTime(QDate(2012,5,29), QTime(12,0,0)));
    vboxLayout0->addWidget(Start_dateTime);

    End_dateTime = new QDateTimeEdit(tab_0);
    End_dateTime->setObjectName(QString::fromUtf8("End_dateTime"));
    End_dateTime->setDisplayFormat("yyyy/MM/dd hh:mm:ss");
    End_dateTime->setDateTime(QDateTime(QDate(2012,5,29), QTime(13,0,0)));
    vboxLayout0->addWidget(End_dateTime);

    RButton = new QPushButton(tab_0);
    RButton->setObjectName(QString::fromUtf8("RButton"));
    RButton->setText("OK");
	if(RButton!=0) connect(RButton, SIGNAL(clicked()), this, SLOT(getChannelValues()));
    //RButton->setGeometry(QRect(980, 20, 80, 27));
    vboxLayout0->addWidget(RButton);


    currentTimeLabel = new QLabel(tab_0);
    currentTimeLabel->setObjectName(QString::fromUtf8("currentTimeLabel"));
    currentTimeLabel->setText("Current Index Time");
    //currentTimeLabel->setGeometry(QRect(760, 7, 161, 21));
    vboxLayout0->addWidget(currentTimeLabel);

    timeSlider = new QSlider(tab_0);
    timeSlider->setObjectName(QString::fromUtf8("timeSlider"));
    //timeSlider->setGeometry(QRect(760, 24, 201, 16));
    timeSlider->setValue(-1);
    timeSlider->setOrientation(Qt::Horizontal);
	connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(readValue(int)));
    vboxLayout0->addWidget(timeSlider);

	//--> Splitter
    splitter = new QSplitter(tab_0);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);

    incButtonDouble = new QPushButton(splitter);
    incButtonDouble->setObjectName(QString::fromUtf8("incButtonDouble"));
    QSizePolicy sizePol(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePol.setHeightForWidth(incButtonDouble->sizePolicy().hasHeightForWidth());
    incButtonDouble->setSizePolicy(sizePol);
    incButtonDouble->setText("<<");
	connect(incButtonDouble,SIGNAL(clicked()), this, SLOT(decDouble()));
    splitter->addWidget(incButtonDouble);

    incButton = new QPushButton(splitter);
    incButton->setObjectName(QString::fromUtf8("incButton"));
    sizePol.setHeightForWidth(incButton->sizePolicy().hasHeightForWidth());
    incButton->setSizePolicy(sizePol);
    incButton->setText("<");
	connect(incButton,SIGNAL(clicked()), this, SLOT(decrease()));
    splitter->addWidget(incButton);

    decButton = new QPushButton(splitter);
    decButton->setObjectName(QString::fromUtf8("decButton"));
    sizePol.setHeightForWidth(decButton->sizePolicy().hasHeightForWidth());
    decButton->setSizePolicy(sizePol);
    decButton->setText(">");
	connect(decButton,SIGNAL(clicked()), this, SLOT(increase()));
    splitter->addWidget(decButton);

    decButtonDouble = new QPushButton(splitter);
    decButtonDouble->setObjectName(QString::fromUtf8("decButtonDouble"));
    sizePol.setHeightForWidth(decButtonDouble->sizePolicy().hasHeightForWidth());
    decButtonDouble->setSizePolicy(sizePol);
    decButtonDouble->setText(">>");
	connect(decButtonDouble,SIGNAL(clicked()), this, SLOT(incDouble()));
    splitter->addWidget(decButtonDouble);

    vboxLayout0->addWidget(splitter);
	//<-- Splitter

    splitter2 = new QSplitter(tab_0);
    splitter2->setObjectName(QString::fromUtf8("splitter2"));
    splitter2->setOrientation(Qt::Horizontal);

    stopButton = new QPushButton(splitter2);
    stopButton->setObjectName(QString::fromUtf8("stopButton"));
    sizePol.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
    stopButton->setSizePolicy(sizePol);
    stopButton->setText("Stop");
	connect(stopButton,SIGNAL(clicked()), this, SLOT(timerStop()));
    splitter2->addWidget(stopButton);

    startButton = new QPushButton(splitter2);
    startButton->setObjectName(QString::fromUtf8("startButton"));
    sizePol.setHeightForWidth(startButton->sizePolicy().hasHeightForWidth());
    startButton->setSizePolicy(sizePol);
    startButton->setText("Run");
	connect(startButton,SIGNAL(clicked()), this, SLOT(timerRun()));
    splitter2->addWidget(startButton);

    vboxLayout0->addWidget(splitter2);

    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "PMS Panel", 0, QApplication::UnicodeUTF8));

    menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 1280, 30));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menu_Util = new QMenu(menubar);
    menu_Util->setObjectName(QString::fromUtf8("menu_Util"));

	menu_Help = new QMenu(menubar);
	menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
    setMenuBar(menubar);

    QLabel *slabel1 = new QLabel("Set your mouse on the dynamic value to read PVNAME.");
    slabel1->setAlignment(Qt::AlignHCenter);
    slabel1->setMinimumSize(slabel1->sizeHint());
    slabel1->setFrameStyle(QFrame::Panel | QFrame::Plain);

    QFrame *sframe = new QFrame();
    QVBoxLayout *svlayout = new QVBoxLayout(sframe);
    svlayout->setSpacing(1);
    svlayout->setMargin(2);

    statusBar()->addWidget(sframe,1);
    
    toolBar = new QToolBar(this);
    toolBar->setObjectName(QString::fromUtf8("toolBar"));
    QPalette palette;
    QBrush brush(QColor(108, 147, 255, 100));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush);
    QBrush brush1(QColor(44, 51, 91, 100));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, brush1);
    QBrush brush2(QColor(108, 147, 255, 100));
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
    QBrush brush3(QColor(44, 51, 91, 100));
    brush3.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush3);
    QBrush brush4(QColor(44, 51, 91, 100));
    brush4.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush4);
    QBrush brush5(QColor(44, 51, 91, 100));
    brush5.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush5);
    toolBar->setPalette(palette);
    toolBar->setOrientation(Qt::Horizontal);
    addToolBar(static_cast<Qt::ToolBarArea>(4), toolBar);

    menubar->addAction(menu_File->menuAction());
    menubar->addAction(menu_Util->menuAction());
	menubar->addSeparator();
    menubar->addAction(menu_Help->menuAction());


    menu_File->addAction(action_Exit);

    QSize size(1280, 1024);
    size = size.expandedTo(minimumSizeHint());
    resize(size);
    tabWidget->setCurrentIndex(0);
    QMetaObject::connectSlotsByName(this);

    msgframe = new QFrame(centralwidget);
    msgframe->setObjectName(QString::fromUtf8("msgframe"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    msgframe->setGeometry(QRect(19, 820, 1255, 90));
    //msgframe->setSizePolicy(sizePolicy4);
    //msgframe->setGeometry(QRect(18, 880, 1254, 70));
    //msgframe->setMinimumSize(QSize(1164, 90));
    //msgframe->setFrameShape(QFrame::StyledPanel);
    //msgframe->setFrameShadow(QFrame::Raised);

	setCentralWidget(centralwidget);

	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 

	// Set Object Text.
	setWindowTitle(QApplication::translate("MainWindow", "PMS (Plant Monitoring System)", 0, QApplication::UnicodeUTF8));
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "PMS Data Retrieval", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    menu_Util->setTitle(QApplication::translate("MainWindow", "&Util", 0, QApplication::UnicodeUTF8));
    menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));

} // makeUI

void MainWindow::registerPushButtonsEvent()
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	qDebug("Button Size:%d", allPButtons.size());

	for (int i = 0; i < allPButtons.size(); ++i) 
	{
		QPushButton *pushbutton = (QPushButton*)allPButtons.at(i);
		objName = pushbutton->objectName();
		if(objName.contains("pushButton"))
		{
			pushbutton -> installEventFilter(this);
			qDebug("ObjectName:%s", objName.toStdString().c_str());
		}
	};
}

void MainWindow::attachAllWidget()
{
	QString displayname;
	QUiLoader loader;
	//const int singletime = 1;
	QFile file;
	for(int i = 0; i < displayDocklist.size();i++)
	{
		displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(i)+".ui";
		//pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i, singletime );
		file.setFileName(displayname);
		file.open(QFile::ReadOnly);
		pwidget = loader.load(&file);
		vecACHAcc.push_back(pwidget);
		if (!pwidget)
		{
			 QWidget *page = new QWidget();
			 QPushButton *pbut = new QPushButton(page);
			 pbut -> setGeometry(0,0,180,40);
			 char display[30];
			 sprintf(display,"%s: %d",displayname.toStdString().c_str(), i);
			 pbut -> setText(display);
			 stackedWidget->addWidget(page);
		}
		else
		{
			pwidget->setAutoFillBackground (true);
			stackedWidget->addWidget(pwidget);
		};
	};
}

bool MainWindow::displayDockWidget(const int index, QPushButton *pobj)
{
	if ( index >= DISPLAYTOT || index < DISPLAYTYPE ) return false;

	QString displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(index)+".ui";
    dockWidget->setWindowTitle(displayDocklist.at(index));
	setEnableButtons(pobj);

	// for stackwidget
	if(stackedWidget->widget(index) != 0)
	{
		setIndex(index);
	}; 
	return true;
}
void MainWindow::setIndex(const size_t page)
{
	stackedWidget->setCurrentIndex(page);
#if 0
	//timeSlider->setValue(0);
	if(page == 0)
		timeSlider->setValue(page1index);
	else
		timeSlider->setValue(page2index);
#endif
}
void MainWindow::setEnableButtons(QPushButton *pobj)
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	for (int i = 0; i < allPButtons.size(); ++i) 
	{
		QPushButton *pushbutton = (QPushButton*)allPButtons.at(i);
		objName = pushbutton->objectName();
#if 0
		if ( objName.compare(pobj->objectName()) == 0 ) pushbutton -> setEnabled(false);
		else pushbutton -> setEnabled(true);
#else
		if(objName.compare(pobj->objectName()) == 0) pushbutton->setEnabled(false);
		else if(objName.contains("caPushButton")){}
		else pushbutton->setEnabled(true);
#endif
	};

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	QPushButton *pBut = (QPushButton*)obj;
	QString objName = pBut->objectName();
	int index = 0;
	if ( objName.compare("pushButton_0") == 0 ) index = DISPLAYTYPE;
	else if( objName.compare("pushButton_1") == 0 ) index = DISPLAYTYPE1;
	else index = -1;

	if(event->type()==QEvent::MouseButtonPress && pBut -> isEnabled() && (index < DISPLAYTOT || index >= DISPLAYTYPE))
	{    
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->buttons() & Qt::LeftButton) 
		{
			displayDockWidget(index, pBut);
			return true;
		};
		return false;
	};

	return QMainWindow::eventFilter(obj, event);
}

void
MainWindow::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}

void
MainWindow::setDefaultIndex(int index) 
{
	if (index == 0)
	{
		setEnableButtons(pushButton[0]);
		setIndex(0);
	}
}
void
MainWindow::changeText(const QString &str) 
{
	QString newstr="Interlock Event Number: ";
	newstr.append(str);
	newstr.append("\n");
	textEdit->insertPlainText(newstr);
	textEdit->moveCursor( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
	//textEdit->ensureCursorVisible();
}
void
MainWindow::keyPressEvent(QKeyEvent *event )
{
	int key = event->key();
	quint32 modifier = event->nativeModifiers();
	qDebug("modifier:0x%0x, key:0x%x", modifier, key);
	if ((modifier==0x14 || modifier == 0x4)&& key == Qt::Key_P)
	{
		QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
		QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
		QString format = "jpg";
		qDebug("shot:%s", filename.toStdString().c_str());
		//mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
		mpixmap = QPixmap::grabWidget(this);
		mpixmap.save(filename, format.toAscii());
	}
}

void MainWindow::getChannelValues() 
{
	QDateTime starttime = Start_dateTime->dateTime();
	QDateTime endtime = End_dateTime->dateTime();

	QString strStartTime = starttime.toString("MM/dd/yyyy hh:mm:ss");
	QString strEndTime = endtime.toString("MM/dd/yyyy hh:mm:ss");

#if 1
	epicsTime start, end;
	string2epicsTime(strStartTime.toStdString().c_str(), start);
	string2epicsTime(strEndTime.toStdString().c_str(), end);
	struct epicsTimeStamp epicstart_ts = (epicsTimeStamp)start;
	struct epicsTimeStamp epicend_ts = (epicsTimeStamp)end;
	qDebug("epicsTimeStamp-Start:sec(%d), nasec(%d)", epicstart_ts.secPastEpoch, epicstart_ts.nsec);
	qDebug("epicsTimeStamp-End:sec(%d), nasec(%d)", epicend_ts.secPastEpoch, epicend_ts.nsec);
	double totaltime = epicsTimeDiffInSeconds( &epicend_ts, &epicstart_ts );
	int timecount = (int)totaltime;

	for (size_t i = 0; i < mpage1names.size(); i++)
	{
		qDebug("PVName[%d]:%s, time:%f", i, mpage1names.at(i).c_str(), totaltime);
	}
	qDebug("KEY(%d),Start(%s), End(%s), Max(%d)", keyindex.at(archiveList->currentIndex()), strStartTime.toStdString().c_str(), strEndTime.toStdString().c_str(),
				 MAX_COUNT);
#endif

	mloop = 0;
	outstream.open("./data.txt", ios::out);
	// According timecount, automatically setup interval
#if 1
	if(CurrentPage() == 0)
	{
		vecPage1Value.clear();
		vecPage1Stamp.clear();
		m_arclient.getValues(keyindex.at(archiveList->currentIndex()), mpage1names, start, end, MAX_COUNT*10, SHEET_TYPE, viewer, this, 1, mloop, marraycount);
	}
	else
	{
		vecPage2Value.clear();
		vecPage2Stamp.clear();
		m_arclient.getValues(keyindex.at(archiveList->currentIndex()), mpage2names, start, end, MAX_COUNT*10, SHEET_TYPE, viewer, this, 1, mloop, marraycount);
	}
#endif

	//Careful Array Index - Count.
	timeSlider->setMaximum(marraycount-1);

	//PVcount, Value Array Count
#if 1
	PVandValue *pPVValue;
	qDebug("LoopCount:%d, ArrayCount:%d",mloop, marraycount);
	for(int i = 0; i <= mloop; i++)
	{
		if(CurrentPage() == 0)
		{
			pPVValue = &(vecPage1Value.at((marraycount*i)+timeSlider->value()));
			qDebug("PVName[%d]:%s(%f), %s",i, pPVValue->pvname.c_str(), pPVValue->dvalue, mpage1names.at(i).c_str());
			outstream << "PVName:" << pPVValue->pvname.c_str()<<"," <<  mpage1names.at(i).c_str() <<",PVValue: " <<pPVValue->dvalue << endl;
		}
		else
		{
			pPVValue = &(vecPage2Value.at((marraycount*i)+timeSlider->value()));
			qDebug("PVName[%d]:%s(%f), %s",i, pPVValue->pvname.c_str(), pPVValue->dvalue, mpage2names.at(i).c_str());
			outstream << "PVName:" << pPVValue->pvname.c_str()<<"," <<  mpage2names.at(i).c_str() <<",PVValue: " <<pPVValue->dvalue << endl;
		};

	};
	outstream.close();
#endif
	timeSlider->setValue(1);
}

bool MainWindow::connectToArchiver( const QString& url )
{
	bool connstatus = false;
	QDateTime curTime = QDateTime::currentDateTime();

    Start_dateTime->setDateTime(QDateTime(QDate(2012,5,29), QTime(12,0,0)));
    End_dateTime->setDateTime(QDateTime(QDate(2012,5,29), QTime(13,0,0)));

	QList<CAColorTileButton*> tilepage1 = stackedWidget->widget(0)->findChildren<CAColorTileButton*>(); 

	for( int i = 0; i < tilepage1.size();i++)
	{
		QVariant var = tilepage1.at(i)->property("pvname");
		QString strpvname = var.toString();
		if(strpvname.compare("pvname") == 0) continue;
		qDebug("Page-1->PVName[%d]:%s", i,strpvname.toStdString().c_str());
		hash_tilepage1.insert(strpvname.toStdString().c_str(), tilepage1.at(i));
		mpage1names.push_back(strpvname.toStdString().c_str());
	};

	QList<CAColorTileButton*> tilepage2 = stackedWidget->widget(1)->findChildren<CAColorTileButton*>(); 
	for( int i = 0; i < tilepage2.size();i++)
	{
		QVariant var = tilepage2.at(i)->property("pvname");
		QString strpvname = var.toString();
		if(strpvname.compare("pvname") == 0) continue;
		qDebug("Page-2->PVName[%d]:%s", i,strpvname.toStdString().c_str());
		hash_tilepage2.insert(strpvname.toStdString().c_str(), tilepage2.at(i));
		mpage2names.push_back(strpvname.toStdString().c_str());
	};

#if 0
	epicsTime start, end;
	string2epicsTime(strStartTime.toStdString().c_str(), start);
	string2epicsTime(strEndTime.toStdString().c_str(), end);
	struct epicsTimeStamp epicstart_ts = (epicsTimeStamp)start;
	struct epicsTimeStamp epicend_ts = (epicsTimeStamp)end;
	double totaltime = epicsTimeDiffInSeconds( &epicend_ts, &epicstart_ts );
	int timecount = (int)totaltime;
#endif

    m_arclient.setURL(url.toStdString().c_str());
    stdVector<ArchiveDataClient::ArchiveInfo> archives;
	connstatus = m_arclient.getArchives(archives);

    if (connstatus)
    {
        for (size_t i=0; i<archives.size(); ++i)
		{
			// archive list filtering...
			QString filtername = archives[i].name.c_str();
			if(filtername.contains("PMS", Qt::CaseInsensitive) == false) continue;
            qDebug("Key %d: '%s' (%s)", archives[i].key, archives[i].name.c_str(),
                   archives[i].path.c_str());
			keyindex.push_back(archives[i].key);
			archiveList->addItem(archives[i].name.c_str());
		};
    };
    m_arclient.setURL(url.toStdString().c_str());

    //m_arclient.getValues(74, , start, end, count, how, viewer, this, interval);
    //m_arclient.getValues(57, mpage1names, start, end, MAX_COUNT, SHEET_TYPE, viewer, this, 1);
    //m_arclient.getValues(keyindex.at(archiveList->currentIndex()), mpage1names, start, end, MAX_COUNT, SHEET_TYPE, viewer, this, 1);

	return connstatus;
}

void MainWindow::incDouble()
{
	if(!timeSlider)	return;
	size_t incIndex = timeSlider->value() + 10;
	if(incIndex >= vecPage1Stamp.size()) incIndex=vecPage1Stamp.size(); 
	qDebug("IncDoubleIndex:%d", incIndex);

	timeSlider->setValue(incIndex);
}
void MainWindow::increase()
{
	if(!timeSlider)	return;
	size_t incIndex = timeSlider->value()+1;
	if(incIndex >= vecPage1Stamp.size()) incIndex=vecPage1Stamp.size(); 
	qDebug("IncIndex:%d", incIndex);
	timeSlider->setValue(incIndex);
}
void MainWindow::decrease()
{
	if(!timeSlider || timeSlider->value() <= 0)	return;
	size_t decIndex = timeSlider->value()-1;
	if(decIndex <=0 ) decIndex=0;
	qDebug("DecIndex:%d", decIndex);
	timeSlider->setValue(decIndex);
}
void MainWindow::decDouble()
{
	if(!timeSlider || timeSlider->value() <= 0)	return;
	size_t decIndex = timeSlider->value()-10;
	if(decIndex <=0 ) decIndex=0;
	qDebug("DecDoubleIndex:%d", decIndex);
	timeSlider->setValue(decIndex);
}

void MainWindow::readValue(int index)
{
	//QWidget *pwidget = stackedWidget->widget(0);
	if(CurrentPage() == 0)
	{
		if((size_t)index >= vecPage1Stamp.size()) index = vecPage1Stamp.size();
		//page1index = index;
	}
	else
	{
		if((size_t)index >= vecPage2Stamp.size()) index = vecPage2Stamp.size();
		//page2index = index;
	};

	PVandValue *pPVValue;
	QHash<QString,CAColorTileButton *>::iterator tileiter; 

	char timebuf[30];
	if(CurrentPage() == 0)
	{
		epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &(vecPage1Stamp.at(index)));
	}
	else
	{
		epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &(vecPage2Stamp.at(index)));
	}
	qDebug("Index(%d)-SliderValue(%d) --> %s", index,timeSlider->value(), timebuf);
	currentTimeLabel->setText(timebuf);

	for(int i = 0; i <= mloop; i++)
	{
		if (CurrentPage() == 0)
		{
			pPVValue = &(vecPage1Value.at((marraycount*i)+timeSlider->value()));
			//qDebug("PVName[%d]:%s(%f),%s",i, pPVValue->pvname.c_str(), pPVValue->dvalue, mpage1names.at(i).c_str());
			CAColorTileButton *pTileButton = hash_tilepage1[pPVValue->pvname.c_str()];
			//qDebug("PVName:%s, Tile(%p)", pTileButton->getPvname().toStdString().c_str(), pTileButton);
			pTileButton->changeValue(ECA_CONN,pPVValue->dvalue);
		}
		else
		{
			pPVValue = &(vecPage2Value.at((marraycount*i)+timeSlider->value()));
			//qDebug("PVName[%d]:%s(%f),%s",i, pPVValue->pvname.c_str(), pPVValue->dvalue, mpage2names.at(i).c_str());
			CAColorTileButton *pTileButton = hash_tilepage2[pPVValue->pvname.c_str()];
			//qDebug("PVName:%s, Tile(%p)", pTileButton->getPvname().toStdString().c_str(), pTileButton);
			pTileButton->changeValue(ECA_CONN,pPVValue->dvalue);
		};
	};
};

void MainWindow::AddEPICSTimeStamp(const int page, const epicsTimeStamp & timestamp)
{
	if(page == 0)
		vecPage1Stamp.push_back(timestamp);
	else if(page == 1)
		vecPage2Stamp.push_back(timestamp);
};

void MainWindow::AddPVAndValue(const int page, const string & pvname, const double & dvalue)
{
	pvvalue.pvname = pvname;
	pvvalue.dvalue = dvalue;
	if(page == 0)
	{
		vecPage1Value.push_back(pvvalue);
	}
	else
	{
		vecPage2Value.push_back(pvvalue);
	};
};
void MainWindow::timerStop()
{
	killTimer(mtimerID);
}
void MainWindow::timerRun()
{
	startTimer(1000);
}
void MainWindow::timerEvent(QTimerEvent *event)
{
	mtimerID = event->timerId();
	qDebug() << "Timer ID: " << mtimerID;
	increase();
}
