#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow(const bool cap):archiver(),archiver2(),archiver3(),archiver4(),signaldb(),mcap(cap)
{
	init();

#if 0
	//Old Code. will be deleted.
	if(mcap == true) 
	{
		qDebug("StartTimer..");
		startTimer(30000);
	};
#endif
	
}

MainWindow::~MainWindow()
{
}

void
MainWindow::init()
{
	displayDocklist
	<<"VMS_vacuum_main"
	<<"VMS_cr_status"<<"VMS_cr_rtchart"<<"VMS_cr_control" 
	<<"VMS_vv_status"<<"VMS_vv_rtchart"<<"VMS_vv_control"
	<<"VMS_crp_detail"<<"VMS_jacket_heater"<<"VMS_RGA"
	<<"CLS_vacuum_status"<<"CLS_vacuum_rtchart"
	<<"CLS_tfhcs_status"<<"CLS_pfhcs_status"<<"CLS_ts_status"
	<<"CLS_heater_control"<<"CLS_tf_valve_control"<<"CLS_pf_valve_control";

	passWord
	<<""
	<<""<<""<<"2007"
	<<""<<""<<"2007"
	<<"2007"<<"2007"<<"2007"
	<<"2007"<<""
	<<""<<""<<""
	<<"2007"<<"2007"<<"2007";

	makeUI();
	createActions();
	createConnects();
	registerPushButtonsEvent();
	attachAllWidget();
   	tabWidget->setCurrentIndex(1);
	//setEnableButtons(pushButton[10]);
	//stackedWidget->setCurrentIndex(10);
	setIndex(1);

	pAccess = new AttachChannelAccess("/usr/local/opi/ui/Extra.ui", 1);
	if(pAccess->GetWidget())
		pAccess->GetWidget()->hide();


}
void
MainWindow::createActions()
{
#if 0
	menu_View->addAction(dockWidget->toggleViewAction());
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
	if (toplevel == true)
	{
		if(dockWidget->geometry().x() < 1280)
		{
			dockWidget->setGeometry(0,0,1280,1024);
		} else
		{
			dockWidget->setGeometry(1280,0,1280,1024);
		}
		stackedWidget->currentWidget()->showFullScreen();
	}
}
void
MainWindow::makeUI()
{
    setObjectName(QString::fromUtf8("MainWindow"));

    //action_Print = new QAction(this);
    //action_Print->setObjectName(QString::fromUtf8("action_Print"));

    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	//connect(action_Exit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));

    action_Multiplot = new QAction(this);
    action_Multiplot->setObjectName(QString::fromUtf8("action_Multiplot"));
	connect(action_Multiplot, SIGNAL(triggered()), this, SLOT(showMultiplot()));

    action_Archivesheet = new QAction(this);
    action_Archivesheet->setObjectName(QString::fromUtf8("action_Archivesheet"));
	connect(action_Archivesheet, SIGNAL(triggered()), this, SLOT(showArchivesheet()));

    action_Archiverviewer = new QAction(this);
    action_Archiverviewer->setObjectName(QString::fromUtf8("action_Archiverviewer"));
	connect(action_Archiverviewer, SIGNAL(triggered()), this, SLOT(showArchiverviewer()));

    action_SignalDB = new QAction(this);
    action_SignalDB->setObjectName(QString::fromUtf8("action_SignalDB"));
	connect(action_SignalDB, SIGNAL(triggered()), this, SLOT(showSignalDB()));

    action_PVListV = new QAction(this);
    action_PVListV->setObjectName(QString::fromUtf8("action_PVListV"));
	connect(action_PVListV, SIGNAL(triggered()), this, SLOT(showPVListViewer()));

#if 0
	action_Manual = new QAction(this);
	action_Manual->setObjectName(QString::fromUtf8("action_Manual"));
	connect(action_Manual, SIGNAL(triggered()), this, SLOT(showManual()));
#endif

	action_AboutVMS = new QAction(this);
	action_AboutVMS->setObjectName(QString::fromUtf8("action_AboutVMS"));
	connect(action_AboutVMS, SIGNAL(triggered()), this, SLOT(showAboutVMS()));

    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    //centralwidget->setGeometry(QRect(0, 0, 1280, 821));
	
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
    //dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
    //dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    //dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    QPalette palettedw;

    QBrush brdw1(QColor(255, 255, 55, 255));
    brdw1.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Button, brdw1);

    QBrush brdw2(QColor(255, 0, 10, 255));
    brdw2.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Mid, brdw2);

	//dockWidget->setPalette(palettedw);

    dockWidgetContents = new QWidget(dockWidget);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));

    //stackedWidget = new QStackedWidget(dockWidgetContents);
    stackedWidget = new QStackedLayout(dockWidgetContents);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    stackedWidget->setGeometry(QRect(0, 0, 1080, 821));
    dockWidget->setWidget(dockWidgetContents);

    vboxLayout->addWidget(dockWidget);

    tabWidget = new QTabWidget(centralwidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setGeometry(QRect(0, 0, 190, 821));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());

#if 0
    QPalette palettetw;
    QBrush brushtw1(QColor(0, 0, 55, 200));
    brushtw1.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Window, brushtw1);
    QBrush brushtw2(QColor(96, 96, 129, 200));
    brushtw2.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Base, brushtw2);
    QBrush brushtw3(QColor(100, 100, 100, 255));
    brushtw3.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::WindowText, brushtw3);
    //QBrush brushtw4(QColor(255, 255, 255, 50));
    QBrush brushtw4(QColor(55, 55, 55, 100));
    brushtw4.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Button, brushtw4);
	/*
    QBrush brushtw5(QColor(255, 55, 55, 100));
    brushtw5.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Inactive, QPalette::Button, brushtw5);
	*/
    tabWidget->setPalette(palettetw);
#endif
    tabWidget->setSizePolicy(sizePolicy1);
    tabWidget->setMaximumSize(QSize(16777215, 16777215));
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setElideMode(Qt::ElideNone);

    tab_0 = new QWidget();
    tab_0->setObjectName(QString::fromUtf8("tab_0"));
    QFont font0;
    font0.setPointSize(16);
    vboxLayout0 = new QVBoxLayout(tab_0);
    vboxLayout0->setSpacing(6);
    vboxLayout0->setMargin(9);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout0"));

    pushButton[0] = new QPushButton(tab_0);
    pushButton[0]->setObjectName(QString::fromUtf8("pushButton_0"));
    pushButton[0]->setFont(font0);
    pushButton[0]->setText(QApplication::translate("MainWindow", "Main", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[0]);

    tab_1 = new QWidget();
    tab_1->setObjectName(QString::fromUtf8("tab_1"));

    QFont font1;
    font1.setPointSize(16);
    vboxLayout1 = new QVBoxLayout(tab_1);
    vboxLayout1->setSpacing(5);
    vboxLayout1->setMargin(5);
    vboxLayout1->setAlignment(Qt::AlignTop);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

    pushButton[1] = new QPushButton(tab_1);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font1);
    pushButton[1]->setText(QApplication::translate("Cryostat Status", "CR Status ", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[1]);

    pushButton[2] = new QPushButton(tab_1);
    pushButton[2]->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton[2]->setFont(font1);
    pushButton[2]->setText(QApplication::translate("Cryostat R-T Chart", "CR R-T Chart", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[2]);

    pushButton[3] = new QPushButton(tab_1);
    pushButton[3]->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton[3]->setFont(font1);
    pushButton[3]->setText(QApplication::translate("Cryostat Control", "CR Control", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[3]);

    spacerItem0 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout1->addItem(spacerItem0);

    pushButton[4] = new QPushButton(tab_1);
    pushButton[4]->setObjectName(QString::fromUtf8("pushButton_4"));
    pushButton[4]->setFont(font1);
    pushButton[4]->setText(QApplication::translate("MainWindow", "VV Status", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[4]);

    pushButton[5] = new QPushButton(tab_1);
    pushButton[5]->setObjectName(QString::fromUtf8("pushButton_5"));
    pushButton[5]->setFont(font1);
    pushButton[5]->setText(QApplication::translate("MainWindow", "VV R-T Chart", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[5]);

    pushButton[6] = new QPushButton(tab_1);
    pushButton[6]->setObjectName(QString::fromUtf8("pushButton_6"));
    pushButton[6]->setFont(font1);
    pushButton[6]->setText(QApplication::translate("MainWindow", "VV Control", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[6]);

    spacerItem1 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout1->addItem(spacerItem1);

    pushButton[7] = new QPushButton(tab_1);
    pushButton[7]->setObjectName(QString::fromUtf8("pushButton_7"));
    pushButton[7]->setFont(font1);
    pushButton[7]->setText(QApplication::translate("MainWindow", "CRP Status", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[7]);

    pushButton[8] = new QPushButton(tab_1);
    pushButton[8]->setObjectName(QString::fromUtf8("pushButton_8"));
    pushButton[8]->setFont(font1);
    pushButton[8]->setText(QApplication::translate("MainWindow", "Jacket Heater", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[8]);

    pushButton[9] = new QPushButton(tab_1);
    pushButton[9]->setObjectName(QString::fromUtf8("pushButton_9"));
    pushButton[9]->setFont(font1);
    pushButton[9]->setText(QApplication::translate("MainWindow", "VMS RGA", 0, QApplication::UnicodeUTF8));
    vboxLayout1->addWidget(pushButton[9]);

    spacerItem2 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout1->addItem(spacerItem2);

    frame0 = new QFrame(tab_1);
    frame0->setObjectName(QString::fromUtf8("frame0"));
	frame0->setContentsMargins(0,0,0,0);
    frame0->setMinimumSize(QSize(120, 90));
    frame0->setMaximumSize(QSize(180, 120));
    frame0->setFrameShape(QFrame::StyledPanel);
    frame0->setFrameShadow(QFrame::Raised);
    vboxLayout1->addWidget(frame0);

	QVBoxLayout *vbox0 = new QVBoxLayout(frame0);
	vbox0->setSpacing(0);
	vbox0->setMargin(0);
	vbox0->setObjectName(QString::fromUtf8("vbox0"));

	QUiLoader m_loader0;
	QFile *file0 = new QFile("/usr/local/opi/ui/VMS_remote.ui");
	file0->open(QFile::ReadOnly);
	QWidget *m_widget0 = m_loader0.load(file0);
	file0->close();
	vbox0->addWidget(m_widget0);

	AttachChannelAccess *pattachVMSRM = new AttachChannelAccess(frame0);

    frame1 = new QFrame(tab_1);
    frame1->setObjectName(QString::fromUtf8("frame1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(frame1->sizePolicy().hasHeightForWidth());
    frame1->setSizePolicy(sizePolicy2);
    frame1->setMinimumSize(QSize(16, 240));
    frame1->setFrameShape(QFrame::StyledPanel);
    frame1->setFrameShadow(QFrame::Raised);

    vboxLayout1->addWidget(frame1);
	QVBoxLayout *vf1boxLayout = new QVBoxLayout(frame1);
	vf1boxLayout->setSpacing(0);
	vf1boxLayout->setMargin(0);
	vf1boxLayout->setObjectName(QString::fromUtf8("vf1boxLayout"));

	QUiLoader m_loader1;

	QFile *file1 = new QFile("/usr/local/opi/ui/VMS_legend.ui");
	file1->open(QFile::ReadOnly);
	QWidget *m_widget1 = m_loader1.load(file1);
	file1->close();
	vf1boxLayout->addWidget(m_widget1);

    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    vboxLayout2 = new QVBoxLayout(tab_2);
    vboxLayout2->setSpacing(5);
    vboxLayout2->setMargin(5);
    vboxLayout2->setAlignment(Qt::AlignTop);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));

    pushButton[10] = new QPushButton(tab_2);
    pushButton[10]->setObjectName(QString::fromUtf8("pushButton_10"));
    pushButton[10]->setFont(font1);
    pushButton[10]->setText(QApplication::translate("MainWindow", "VPS Status", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[10]);

    pushButton[11] = new QPushButton(tab_2);
    pushButton[11]->setObjectName(QString::fromUtf8("pushButton_11"));
    pushButton[11]->setFont(font1);
    pushButton[11]->setText(QApplication::translate("MainWindow", "VPS R-T Chart", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[11]);
    spacerItem3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout2->addItem(spacerItem3);

    pushButton[12] = new QPushButton(tab_2);
    pushButton[12]->setObjectName(QString::fromUtf8("pushButton_12"));
    pushButton[12]->setFont(font1);
    pushButton[12]->setText(QApplication::translate("MainWindow", "TF HCS Status", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[12]);

    pushButton[13] = new QPushButton(tab_2);
    pushButton[13]->setObjectName(QString::fromUtf8("pushButton_13"));
    pushButton[13]->setFont(font1);
    pushButton[13]->setText(QApplication::translate("MainWindow", "PF HCS Status", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[13]);

    pushButton[14] = new QPushButton(tab_2);
    pushButton[14]->setObjectName(QString::fromUtf8("pushButton_14"));
    pushButton[14]->setFont(font1);
    pushButton[14]->setText(QApplication::translate("MainWindow", "T/S Status", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[14]);


    spacerItem4 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout2->addItem(spacerItem4);

    pushButton[15] = new QPushButton(tab_2);
    pushButton[15]->setObjectName(QString::fromUtf8("pushButton_15"));
    pushButton[15]->setFont(font1);
    pushButton[15]->setText(QApplication::translate("MainWindow", "Heater Control", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[15]);

    pushButton[16] = new QPushButton(tab_2);
    pushButton[16]->setObjectName(QString::fromUtf8("pushButton_16"));
    pushButton[16]->setFont(font1);
    pushButton[16]->setText(QApplication::translate("MainWindow", "TF V/V Control", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[16]);


	pushButton[17] = new QPushButton(tab_2);
	pushButton[17]->setObjectName(QString::fromUtf8("pushButton_17"));
	pushButton[17]->setFont(font1);
	pushButton[17]->setText(QApplication::translate("MainWindow", "PF V/V Control", 0, QApplication::UnicodeUTF8));
	vboxLayout2->addWidget(pushButton[17]);
						
#if 0
    pushButton[15] = new QPushButton(tab_2);
    pushButton[15]->setObjectName(QString::fromUtf8("pushButton_15"));
    pushButton[15]->setFont(font1);
    pushButton[15]->setText(QApplication::translate("MainWindow", "Alarm Status", 0, QApplication::UnicodeUTF8));
    vboxLayout2->addWidget(pushButton[15]);
#endif

    spacerItem5 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout2->addItem(spacerItem5);

    frame2 = new QFrame(tab_2);
    frame2->setObjectName(QString::fromUtf8("frame2"));
	frame2->setContentsMargins(0,0,0,0);
    frame2->setSizePolicy(sizePolicy2);
    frame2->setMinimumSize(QSize(150, 160));
    frame2->setMaximumSize(QSize(180, 160));
    frame2->setFrameShape(QFrame::StyledPanel);
    frame2->setFrameShadow(QFrame::Raised);
    vboxLayout2->addWidget(frame2);

	QVBoxLayout *vbox = new QVBoxLayout(frame2);
	vbox->setSpacing(0);
	vbox->setMargin(0);
	vbox->setObjectName(QString::fromUtf8("vbox"));

	QUiLoader m_loader01;
	QFile *file01 = new QFile("/usr/local/opi/ui/CLS_remote.ui");
	file01->open(QFile::ReadOnly);
	QWidget *m_widget01 = m_loader01.load(file01);
	file01->close();
	vbox->addWidget(m_widget01);

	AttachChannelAccess *pattachRM = new AttachChannelAccess(frame2);



    frame3 = new QFrame(tab_2);
    frame3->setObjectName(QString::fromUtf8("frame3"));
    frame3->setSizePolicy(sizePolicy2);
    frame3->setMinimumSize(QSize(150, 240));
    frame3->setFrameShape(QFrame::StyledPanel);
    frame3->setFrameShadow(QFrame::Raised);

    vboxLayout2->addWidget(frame3);
	QVBoxLayout *vf2boxLayout = new QVBoxLayout(frame3);
	vf2boxLayout->setSpacing(0);
	vf2boxLayout->setMargin(0);
	vf2boxLayout->setObjectName(QString::fromUtf8("vf2boxLayout"));

	QUiLoader m_loader2;

	QFile *file2 = new QFile("/usr/local/opi/ui/CLS_legend.ui");
	file2->open(QFile::ReadOnly);
	QWidget *m_widget2 = m_loader2.load(file2);
	file2->close();
	vf2boxLayout->addWidget(m_widget2);


    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "Main panels", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tab_1, QApplication::translate("MainWindow", "VMS Sub panels", 0, QApplication::UnicodeUTF8));
    tabWidget->addTab(tab_2, QApplication::translate("MainWindow", "CLS Sub panels", 0, QApplication::UnicodeUTF8));
    //setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 1280, 30));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menu_Util = new QMenu(menubar);
    menu_Util->setObjectName(QString::fromUtf8("menu_Util"));
#if 0
    menu_View = new QMenu(menubar);
    menu_View->setObjectName(QString::fromUtf8("menu_View"));
#endif
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
#if 0
    menubar->addAction(menu_View->menuAction());
#endif
	menubar->addSeparator();
    menubar->addAction(menu_Help->menuAction());
    //menu_File->addAction(action_Print);
    menu_File->addAction(action_Exit);

    menu_Util->addAction(action_Multiplot);
    menu_Util->addAction(action_Archivesheet);
    menu_Util->addAction(action_Archiverviewer);
    menu_Util->addAction(action_SignalDB);
    menu_Util->addAction(action_PVListV);

#if 0
	menu_Help->addAction(action_Manual);
#endif
	menu_Help->addAction(action_AboutVMS);

	/*
    toolBar->addSeparator();
    toolBar->addAction(action_Print);
	*/

    QFrame *tbframe = new QFrame();
    toolBar->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(920, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	CAGraphic *ioc1HB = new CAGraphic(this);
	ioc1HB->setLineWidth(2);
	ioc1HB->setMinimumSize(QSize(20,20));
	ioc1HB->setMaximumSize(QSize(20,20));
	ioc1HB->setFillColor(QColor("white"));
	ioc1HB->setLineColor(QColor("black"));
	//ioc1HB->setPvname("VMS_CR_HT_BT_HO");
	ioc1HB->setPvname("VMS_CR_HEART_BEAT");
	ioc1HB->setFillColor(QColor("gray"));
	ioc1HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc1HB->setObjectName("CAGraphic_CRHB");
	ioc1HB->setToolTip("Cryostat Heart Beat");

	CAGraphic *ioc2HB = new CAGraphic(this);
	ioc2HB->setLineWidth(2);
	ioc2HB->setMinimumSize(QSize(20,20));
	ioc2HB->setMaximumSize(QSize(20,20));
	ioc2HB->setFillColor(QColor("white"));
	ioc2HB->setLineColor(QColor("black"));
	//ioc2HB->setPvname("VMS_VV_HT_BT_HO");
	ioc2HB->setPvname("VMS_VV_HEART_BEAT");
	ioc2HB->setFillColor(QColor("gray"));
	ioc2HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc2HB->setObjectName("CAGraphic_VVHB");
	ioc2HB->setToolTip("Vacuum Vessel Heart Beat");

	CAGraphic *ioc3HB = new CAGraphic(this);
	ioc3HB->setLineWidth(2);
	ioc3HB->setMinimumSize(QSize(20,20));
	ioc3HB->setMaximumSize(QSize(20,20));
	ioc3HB->setFillColor(QColor("white"));
	ioc3HB->setLineColor(QColor("black"));
	ioc3HB->setPvname("CLS_VPS_HEART_BEAT");
	ioc3HB->setFillColor(QColor("gray"));
	ioc3HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc3HB->setObjectName("CAGraphic_ioc3HB");
	ioc3HB->setToolTip("CLS VPS Heart Beat");

	CAGraphic *ioc4HB = new CAGraphic(this);
	ioc4HB->setLineWidth(2);
	ioc4HB->setMinimumSize(QSize(20,20));
	ioc4HB->setMaximumSize(QSize(20,20));
	ioc4HB->setFillColor(QColor("white"));
	ioc4HB->setLineColor(QColor("black"));
	ioc4HB->setPvname("CLS_HCS_HEART_BEAT");
	ioc4HB->setFillColor(QColor("gray"));
	ioc4HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc4HB->setObjectName("CAGraphic_ioc4HB");
	ioc4HB->setToolTip("CLS HCS Heart Beat");

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock(this);
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("CLS_TIME.RVAL");
	wclock1->setFont(clockfont);
	wclock1->setObjectName("CAWclock_wclock1");
	
    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":/images/kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);
	tblayout->addWidget(ioc1HB);
	tblayout->addWidget(ioc2HB);
	tblayout->addWidget(ioc3HB);
	tblayout->addWidget(ioc4HB);
    tblayout->addWidget(logo);
	AttachChannelAccess *pattachTB = new AttachChannelAccess(tbframe);

    QSize size(1280, 1024);
    size = size.expandedTo(minimumSizeHint());
    resize(size);
    tabWidget->setCurrentIndex(0);
    QMetaObject::connectSlotsByName(this);

    msgframe = new QFrame(centralwidget);
    msgframe->setObjectName(QString::fromUtf8("msgframe"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    //sizePolicy3.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
    //msgframe->setSizePolicy(sizePolicy3);
    msgframe->setGeometry(QRect(19, 820, 1255, 90));
    //msgframe->setMinimumSize(QSize(1164, 90));
    //msgframe->setFrameShape(QFrame::StyledPanel);
    //msgframe->setFrameShadow(QFrame::Raised);


	vboxLayout3 = new QVBoxLayout(msgframe);
	vboxLayout3->setSpacing(0);
	vboxLayout3->setMargin(0);

	QHBoxLayout *vhLayout = new QHBoxLayout();
	vhLayout->setSpacing(0);
	vhLayout->setMargin(0);

	CADisplayer *ioc1_interlock = new CADisplayer();
	ioc1_interlock->setMinimumSize(QSize(160,20));
	ioc1_interlock->setMaximumSize(QSize(160,20));
	ioc1_interlock->setPvname("VMS_IOC1_INTERLOCK");
	ioc1_interlock->setObjectName("CADisplayer_ioc1_interlock");
	ioc1_interlock->setVisible(false);
	vhLayout->addWidget(ioc1_interlock);

	textEdit = new QTextEdit(this);
	textEdit->setObjectName(QString::fromUtf8("textEdit"));
	//textEdit->setGeometry(QRect(0, 0, 1000, 50)); 
	//textEdit->setGeometry(QRect(16, 900, 1000, 70)); 
	textEdit->setFontPointSize(12);
	textEdit->setAutoFormatting(QTextEdit::AutoAll);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	vboxLayout3->addWidget(textEdit);

	AttachChannelAccess *pattach_msg = new AttachChannelAccess(msgframe);
    	setCentralWidget(centralwidget);

	QObject::connect(ioc1_interlock, SIGNAL(valueChanged(QString)), this,  SLOT(changeText(QString))); 

	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 


	// Set Object Text.
	setWindowTitle(QApplication::translate("MainWindow", "KSTAR Vacuum Monitoring System & Current Lead System", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/VMS.xpm")));
    //action_Print->setText(QApplication::translate("MainWindow", "&Print", 0, QApplication::UnicodeUTF8));
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));
    action_Multiplot->setText(QApplication::translate("MainWindow", "&Multiplot", 0, QApplication::UnicodeUTF8));
    action_Archivesheet->setText(QApplication::translate("MainWindow", "&Archivesheet", 0, QApplication::UnicodeUTF8));
    action_Archiverviewer->setText(QApplication::translate("MainWindow", "a&Rchiveviewer", 0, QApplication::UnicodeUTF8));
    action_SignalDB->setText(QApplication::translate("MainWindow", "&SignalDB", 0, QApplication::UnicodeUTF8));
    action_PVListV->setText(QApplication::translate("MainWindow", "&PVListviewr", 0, QApplication::UnicodeUTF8));
#if 0
    action_Manual->setText(QApplication::translate("MainWindow", "ma&Nual", 0, QApplication::UnicodeUTF8));
#endif
    action_AboutVMS->setText(QApplication::translate("MainWindow", "About &VMS", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "Main panels", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_1), QApplication::translate("MainWindow", "VMS sub panels", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "CLS sub panels", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    menu_Util->setTitle(QApplication::translate("MainWindow", "&Util", 0, QApplication::UnicodeUTF8));
#if 0
    menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
#endif
    menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));

} // setupUi

void MainWindow::registerPushButtonsEvent()
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	for (int i = 0; i < allPButtons.size(); ++i) 
	//for (int i = 0; i < displayDocklist.size(); ++i) 
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
	for(int i = 0; i < displayDocklist.size();i++)
	{
		displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(i)+".ui";
		AttachChannelAccess *pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i);
		vecACHAcc.push_back(pattach);
		if (!pattach->GetWidget())
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
			QWidget *w = pattach->GetWidget();
			w->setAutoFillBackground (true);
			stackedWidget->addWidget(pattach->GetWidget());
		}
	}
}

bool MainWindow::displayDockWidget(const int index, QPushButton *pobj)
{
	if ( index >= DISPLAYTOT || index < DISPLAYTYPE ) return false;

	QString displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(index)+".ui";
    dockWidget->setWindowTitle(displayDocklist.at(index));
	setEnableButtons(pobj);

#if 1
	// for stackwidget
	if(stackedWidget->widget(index) != 0)
	{
		//stackedWidget->setCurrentIndex(index);
		//return true;
		setIndex(index);
	}; 
#else
	//for DockWidget
	AttachChannelAccess *pattach = new AttachChannelAccess(displayname.toStdString().c_str());
	qDebug("DisplayUI:%s",displayname.toStdString().c_str());
	if (!pattach->GetWidget())	return false;
	dockWidget->setWidget(pattach->GetWidget());
#endif
	return true;
}

void MainWindow::setIndex(const int index)
{
    stackedWidget->setCurrentIndex(index);
    AttachChannelAccess *pattach = 0;
    for (size_t i = 0; i < vecACHAcc.size(); i++)
    {
        pattach = vecACHAcc.at(i);
        if (!pattach->GetWidget()) continue;
        if ( i == index ) pattach->SetUiCurIndex(index);
        else pattach->SetUiCurIndex(-1);
    }
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
		if ( objName.compare(pobj->objectName()) == 0 ) pushbutton -> setEnabled(false);
		else if (objName.contains("caPushButton")) {}
		else pushbutton -> setEnabled(true);
#endif
	};

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	qDebug("---------------------------------------------MainWindow::eventFilter");
	QPushButton *pBut = (QPushButton*)obj;
	QString objName = pBut->objectName();
	int index = 0;
	if ( objName.compare("pushButton_0") == 0 ) index = DISPLAYTYPE;
	else if( objName.compare("pushButton_1") == 0 ) index = DISPLAYTYPE1;
	else if( objName.compare("pushButton_2") == 0 ) index = DISPLAYTYPE2;
	else if( objName.compare("pushButton_3") == 0 ) index = DISPLAYTYPE3;
	else if( objName.compare("pushButton_4") == 0 ) index = DISPLAYTYPE4;
	else if( objName.compare("pushButton_5") == 0 ) index = DISPLAYTYPE5;
	else if( objName.compare("pushButton_6") == 0 ) index = DISPLAYTYPE6;
	else if( objName.compare("pushButton_7") == 0 ) index = DISPLAYTYPE7;
	else if( objName.compare("pushButton_8") == 0 ) index = DISPLAYTYPE8;
	else if( objName.compare("pushButton_9") == 0 ) index = DISPLAYTYPE9;
	else if( objName.compare("pushButton_10") == 0 ) index = DISPLAYTYPE10;
	else if( objName.compare("pushButton_11") == 0 ) index = DISPLAYTYPE11;
	else if( objName.compare("pushButton_12") == 0 ) index = DISPLAYTYPE12;
	else if( objName.compare("pushButton_13") == 0 ) index = DISPLAYTYPE13;
	else if( objName.compare("pushButton_14") == 0 ) index = DISPLAYTYPE14;
	else if( objName.compare("pushButton_15") == 0 ) index = DISPLAYTYPE15;
	else if( objName.compare("pushButton_16") == 0 ) index = DISPLAYTYPE16;
	else if( objName.compare("pushButton_17") == 0 ) index = DISPLAYTYPE17;
	/* else if( objName.compare("pushButton_16") == 0 ) index = DISPLAYTYPE16; */
	else index = -1;

	QString passwd = passWord.at(index);
	if(event->type()==QEvent::MouseButtonPress && pBut -> isEnabled() && (index < DISPLAYTOT || index >= DISPLAYTYPE))
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->buttons() & Qt::LeftButton) 
		{
			if(passwd.compare("") != 0)
			{
				//qDebug("Password: %s", passwd.toStdString().c_str()); 
				Password pword(passwd, new QWidget());
				if(pword.getStatus()== true) 
				{
					displayDockWidget(index, pBut);
				} else {
					return false;
				}
			}
			else
			{
				displayDockWidget(index, pBut);
			}
			return true;
		}
		return false;
	} 

	return QMainWindow::eventFilter(obj, event);
}

void
MainWindow::showMultiplot()
{
	MultiplotMainWindow *pmainWindow = new MultiplotMainWindow;
	pmainWindow->resize(1280,900);
}
void
MainWindow::showArchivesheet()
{
#if 0
	ArchiveSheet *psheet = new ArchiveSheet(2010,21);
	psheet->setWindowIcon(QPixmap(":/images/interview.png"));
	psheet->resize(1280, 1024);
	psheet->show();
#endif
}
void
MainWindow::showPVListViewer()
{
	//PVListViewer *plist = new PVListViewer();
}
void
MainWindow::closeEvent(QCloseEvent *)
{
	archiver.kill();
	archiver2.kill();
	archiver3.kill();
	archiver4.kill();
	signaldb.kill();
	//kill(getpid(), SIGTERM);

	::exit(0);
}

void
MainWindow::showSignalDB()
{
	QString program = "konqueror";
	QStringList arguments;
	arguments << "http://172.17.100.204/signalDB/";
	qDebug("Commands: %s %s %s", program.toStdString().c_str(), 
			arguments[0].toStdString().c_str()); 
	if (signaldb.state() != QProcess::Running )
	{
		signaldb.start(program, arguments);
	};
}

void
MainWindow::showArchiverviewer()
{
	QString program = "java";
	QStringList arguments;
	arguments << "-jar" << "/usr/local/opi/bin/archiveviewer.jar";
	qDebug("Commands: %s %s %s", program.toStdString().c_str(), arguments[0].toStdString().c_str(), arguments[1].toStdString().c_str());
#if 1
	if(archiver.state() != QProcess::Running)
	{
		archiver.start(program, arguments);
	}
	else if(archiver2.state() != QProcess::Running)
	{
		archiver2.start(program, arguments);
	}
	else if(archiver3.state() != QProcess::Running)
	{
		archiver3.start(program, arguments);
	}
	else if(archiver4.state() != QProcess::Running)
	{
		archiver4.start(program, arguments);
	}
#else
	if (myProcess->state() != QProcess::Running )
	{
		myProcess = new QProcess(this);
		myProcess->start(program, arguments);
	};
#endif

}

void
MainWindow::showManual()
{
	QString path = QApplication::applicationDirPath() + "/doc/index.html";  
	if (!assistant)
			assistant = new QAssistantClient("");
	assistant->showPage(path);
}

void
MainWindow::showAboutVMS()
{
	QMessageBox *mbox = new QMessageBox();
	mbox->setIconPixmap(QPixmap("/usr/local/opi/images/VMS.xpm"));
	mbox->setWindowTitle("About KSTAR VMS");
	mbox->setText("KSTAR VMS Operator Interface");
	mbox->setDetailedText("It is the operator interface for the KSTAR Vacuum Pumping System and Current Lead System.\n[Release 2.3.0]");
	mbox->exec();

}
void
MainWindow::setDefaultIndex(int index) 
{
	if (index == 0)
	{
		setEnableButtons(pushButton[0]);
		//stackedWidget->setCurrentIndex(0);
		setIndex(0);
		killTimer(mtimerid);
	}
	else if (index == 1)
	{
		setEnableButtons(pushButton[1]);
		//stackedWidget->setCurrentIndex(1);
		setIndex(1);
		killTimer(mtimerid);
	}
	else if (index == 2)
	{
		QWidget *pWidget = pAccess->GetWidget();
		CAPopUp *pPopupAB = pWidget->findChild<CAPopUp *>("CAPopUpAB");
		if(pPopupAB!=0) pPopupAB->setShow(0);

		//qDebug("HDS Page***");
		mtimerid = startTimer(1000);

		setEnableButtons(pushButton[12]);
		//stackedWidget->setCurrentIndex(7);
		setIndex(12);
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
#if 0
void 
MainWindow::timerEvent(QTimerEvent *event)
{
	QString format = "jpg";
	//QPixmap pixmap = QPixmap::grabWidget(stackedWidget->currentWidget());
	QPixmap pixmap = QPixmap::grabWindow(winId());

	QString fullpath = QString("vms.jpg");
	pixmap.save(fullpath, format.toAscii());

	uploadFile(fullpath);

}
#endif
int MainWindow::uploadFile(const QString filename)
{
	QString cmd="ncftpput -u kstar -p kstar2007 203.230.119.122 /usr/local/tomcat5/webapps/kstarweb/screenshot/ " + filename;
	FILE *fp = popen(cmd.toStdString().c_str(), "r");
	pclose(fp);
	return 0;
}

void
MainWindow::keyPressEvent(QKeyEvent *event )
{
	int key = event->key();
	quint32 modifier = event->nativeModifiers();
	if ((modifier==0x14 || modifier == 0x4)&& key == Qt::Key_P)
	{
		QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
		QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
		QString format = "jpg";
		//mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
		mpixmap = QPixmap::grabWidget(this);
		mpixmap.save(filename, format.toAscii());
	};
}

void MainWindow::timerEvent(QTimerEvent * /*event*/)
{
	QWidget *pWidget = pAccess->GetWidget();
	CAPopUp *pPopupAB = pWidget->findChild<CAPopUp *>("CAPopUpAB");
	if(pPopupAB!=0) pPopupAB->Show();
}

