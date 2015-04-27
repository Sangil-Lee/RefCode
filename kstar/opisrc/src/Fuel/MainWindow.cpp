#include "MainWindow.h"
//#include "kstar_errMdef.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow():archiver(), archiver2(),archiver3(),archiver4(),signaldb()
{
	init();
}

MainWindow::~MainWindow()
{
}

void
MainWindow::init()
{
	displayDocklist
	<<"VMS_vv_status"
	<<"Fuel_main"
	<<"Fuel_piezo_status"
	<<"Fuel_DAQ"
	<<"GDC_main" 
	<<"Fuel_SMBI_MGI" 
	<<"Fuel_SMBI_DAQ"
	<<"Fuel_MGI_status"
//Added by linupark for TAB[[	
	<<"Fuel_SMBI_Integrator"
	<<"";
//]]

	passWord
	<<""
	<<"2007"
	<<"2007"
	<<"2007"
	<<"2007"
	<<"2007"
	<<"2007"
	<<"2011"
//Added by linupark for TAB[[		
	<<"2012"
	<<"";
//]]
	makeUI();
	createActions();
	createConnects();
	registerPushButtonsEvent();
	attachAllWidget();
        tabWidget->setCurrentIndex(0);
	setIndex(0);
	//setEnableButtons(pushButton[2]);
	//stackedWidget->setCurrentIndex(0);

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
		dockWidget->setGeometry(0,0,1280,1024);
		stackedWidget->currentWidget()->showFullScreen();
	}
}
void
MainWindow::makeUI()
{
    setObjectName(QString::fromUtf8("MainWindow"));

#if 0
    action_Print = new QAction(this);
    action_Print->setObjectName(QString::fromUtf8("action_Print"));
#endif

    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
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

	action_AboutTMS = new QAction(this);
	action_AboutTMS->setObjectName(QString::fromUtf8("action_AboutTMS"));
	connect(action_AboutTMS, SIGNAL(triggered()), this, SLOT(showAboutTMS()));

    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    //centralwidget->setGeometry(QRect(0, 0, 1280, 821));
	
    widget = new QWidget(centralwidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    //Modified by linupark in 20120713[[
    //widget->setGeometry(QRect(190, 0, 1080, 821));
    widget->setGeometry(QRect(190, 0, 1080, 886));
    //]]
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
    dockWidgetContents = new QWidget(dockWidget);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));

    stackedWidget = new QStackedWidget(dockWidgetContents);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    //Modified by linupark in 20120713[[
    //stackedWidget->setGeometry(QRect(0, 0, 1080, 821));
    stackedWidget->setGeometry(QRect(0, 0, 1080, 886));
    //]]
    dockWidget->setWidget(dockWidgetContents);

    vboxLayout->addWidget(dockWidget);

    tabWidget = new QTabWidget(centralwidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    //tabWidget->setGeometry(QRect(0, 0, 190, 821));
    tabWidget->setGeometry(QRect(0, 0, 190, 886));
    //]]
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
    vboxLayout0->setMargin(6);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout0"));

    pushButton[0] = new QPushButton(tab_0);
    pushButton[0]->setObjectName(QString::fromUtf8("pushButton_0"));
    pushButton[0]->setFont(font0);
    pushButton[0]->setText(QApplication::translate("MainWindow", "VV Status", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[0]);

    pushButton[1] = new QPushButton(tab_0);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font0);
    pushButton[1]->setText(QApplication::translate("MainWindow", "Fuel & GDC", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[1]);

    pushButton[2] = new QPushButton(tab_0);
    pushButton[2]->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton[2]->setFont(font0);
    pushButton[2]->setText(QApplication::translate("MainWindow", "Piezo Status", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[2]);

    pushButton[3] = new QPushButton(tab_0);
    pushButton[3]->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton[3]->setFont(font0);
    pushButton[3]->setText(QApplication::translate("MainWindow", "Fuel DAQ", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[3]);

    pushButton[4] = new QPushButton(tab_0);
    pushButton[4]->setObjectName(QString::fromUtf8("pushButton_4"));
    pushButton[4]->setFont(font0);
    pushButton[4]->setText(QApplication::translate("MainWindow", "GDC Control", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[4]);

    pushButton[5] = new QPushButton(tab_0);
    pushButton[5]->setObjectName(QString::fromUtf8("pushButton_5"));
    pushButton[5]->setFont(font0);
    pushButton[5]->setText(QApplication::translate("MainWindow", "SMBI MGI", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[5]);

    pushButton[6] = new QPushButton(tab_0);
    pushButton[6]->setObjectName(QString::fromUtf8("pushButton_6"));
    pushButton[6]->setFont(font0);
    pushButton[6]->setText(QApplication::translate("MainWindow", "SMBI DAQ", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[6]);

    pushButton[7] = new QPushButton(tab_0);
    pushButton[7]->setObjectName(QString::fromUtf8("pushButton_7"));
    pushButton[7]->setFont(font0);
    pushButton[7]->setText(QApplication::translate("MainWindow", "MGI Status", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[7]);


    //spacerItem0 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //vboxLayout0->addItem(spacerItem0);

	frame0 = new QFrame(tab_0);
	frame0->setObjectName(QString::fromUtf8("frame0"));
	frame0->setContentsMargins(0,0,0,0);
	frame0->setMinimumSize(QSize(145, 50));
	frame0->setMaximumSize(QSize(180, 145));
	frame0->setMinimumSize(QSize(200, 50));
	frame0->setMaximumSize(QSize(200, 145));
	frame0->setFrameShape(QFrame::StyledPanel);
    	frame0->setFrameShadow(QFrame::Raised);
	vboxLayout0->addWidget(frame0);

	QVBoxLayout *vbox0 = new QVBoxLayout(frame0);
	vbox0->setSpacing(0);
        vbox0->setMargin(0);
        vbox0->setObjectName(QString::fromUtf8("vbox0"));

	QUiLoader m_loader0;
        QFile *file0 = new QFile("/usr/local/opi/ui/Fuel_remote.ui");
        file0->open(QFile::ReadOnly);
        QWidget *m_widget0 = m_loader0.load(file0);
        file0->close();
        vbox0->addWidget(m_widget0);

	AttachChannelAccess *pattachVMSRM = new AttachChannelAccess(frame0);

    frame = new QFrame(tab_0);
    frame->setObjectName(QString::fromUtf8("frame"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
    frame->setSizePolicy(sizePolicy2);
    frame->setMinimumSize(QSize(16, 200));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    vboxLayout0->addWidget(frame);
	QVBoxLayout *vfboxLayout = new QVBoxLayout(frame);
	vfboxLayout->setSpacing(0);
	vfboxLayout->setMargin(0);
	vfboxLayout->setObjectName(QString::fromUtf8("vfboxLayout"));

	QUiLoader m_loader;

	QFile *file = new QFile("/usr/local/opi/ui/Fuel_legend.ui");
	file->open(QFile::ReadOnly);
	QWidget *m_widget = m_loader.load(file);
	file->close();
	vfboxLayout->addWidget(m_widget);

//Added by linupark for TAB[[
	tab_1 = new QWidget();
	tab_1->setObjectName(QString::fromUtf8("tab_1"));
	QFont font;
	font.setPointSize(14);
	vboxLayout1 = new QVBoxLayout(tab_1);
	vboxLayout1->setSpacing(6);
	vboxLayout1->setMargin(9);
	vboxLayout1->setAlignment(Qt::AlignTop);
	vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

	pushButton[8] = new QPushButton(tab_1);
	pushButton[8]->setObjectName(QString::fromUtf8("pushButton_8"));
	pushButton[8]->setFont(font);
	pushButton[8]->setText(QApplication::translate("MainWindow", "Operation", 0, QApplication::UnicodeUTF8));
	vboxLayout1->addWidget(pushButton[8]);

	pushButton[9] = new QPushButton(tab_1);
	pushButton[9]->setObjectName(QString::fromUtf8("pushButton_9"));
	pushButton[9]->setFont(font);
	pushButton[9]->setText(QApplication::translate("MainWindow", "DAQ", 0, QApplication::UnicodeUTF8));
	vboxLayout1->addWidget(pushButton[9]);
//]]

    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "TMS Main panels", 0, QApplication::UnicodeUTF8));
//Added by linupark for TAB[[

	tabWidget->addTab(tab_1, QApplication::translate("MainWindow", "Total Operation", 0, QApplication::UnicodeUTF8));
//]]
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
#if 0
    menu_File->addAction(action_Print);
#endif
    menu_File->addAction(action_Exit);

    menu_Util->addAction(action_Multiplot);
    menu_Util->addAction(action_Archivesheet);
    menu_Util->addAction(action_Archiverviewer);
    menu_Util->addAction(action_SignalDB);
    menu_Util->addAction(action_PVListV);

#if 0
	menu_Help->addAction(action_Manual);
#endif
	menu_Help->addAction(action_AboutTMS);

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

    QSpacerItem *tbspacer = new QSpacerItem(930, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	CAGraphic *ioc1HB = new CAGraphic();
	ioc1HB->setLineWidth(2);
	ioc1HB->setMinimumSize(QSize(20,20));
	ioc1HB->setMaximumSize(QSize(20,20));
	ioc1HB->setFillColor(QColor("white"));
	ioc1HB->setLineColor(QColor("black"));
	ioc1HB->setPvname("FUEL_HEART_BEAT");
	ioc1HB->setFillColor(QColor("gray"));
	ioc1HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc1HB->setObjectName("CAGraphic_ioc1HB");

	CAGraphic *ioc2HB = new CAGraphic();
	ioc2HB->setLineWidth(2);
	ioc2HB->setMinimumSize(QSize(20,20));
	ioc2HB->setMaximumSize(QSize(20,20));
	ioc2HB->setFillColor(QColor("white"));
	ioc2HB->setLineColor(QColor("black"));
	ioc2HB->setPvname("FUEL_DAQ_HEARTBEAT");
	ioc2HB->setFillColor(QColor("gray"));
	ioc2HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc2HB->setObjectName("CAGraphic_ioc2HB");

	CAGraphic *ioc3HB = new CAGraphic();
	ioc3HB->setLineWidth(2);
	ioc3HB->setMinimumSize(QSize(20,20));
	ioc3HB->setMaximumSize(QSize(20,20));
	ioc3HB->setFillColor(QColor("white"));
	ioc3HB->setLineColor(QColor("black"));
	ioc3HB->setPvname("FUEL_LTU_HeartBeat");
	ioc3HB->setFillColor(QColor("gray"));
	ioc3HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc3HB->setObjectName("CAGraphic_ioc3HB");

	CAGraphic *ioc4HB = new CAGraphic();
	ioc4HB->setLineWidth(2);
	ioc4HB->setMinimumSize(QSize(20,20));
	ioc4HB->setMaximumSize(QSize(20,20));
	ioc4HB->setFillColor(QColor("white"));
	ioc4HB->setLineColor(QColor("black"));
	ioc4HB->setPvname("SMBI_MGI_HEART_BEAT");
	ioc4HB->setFillColor(QColor("gray"));
	ioc4HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc4HB->setObjectName("CAGraphic_ioc4HB");

	CAGraphic *ioc5HB = new CAGraphic();
	ioc5HB->setLineWidth(2);
	ioc5HB->setMinimumSize(QSize(20,20));
	ioc5HB->setMaximumSize(QSize(20,20));
	ioc5HB->setFillColor(QColor("white"));
	ioc5HB->setLineColor(QColor("black"));
	ioc5HB->setPvname("SMBI_DAQ_HEARTBEAT");
	ioc5HB->setFillColor(QColor("gray"));
	ioc5HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc5HB->setObjectName("CAGraphic_ioc5HB");

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock();
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("FUEL_TIME.RVAL");
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
	tblayout->addWidget(ioc5HB);
    tblayout->addItem(tbspacer2);
    tblayout->addWidget(logo);
	AttachChannelAccess *pattachTB = new AttachChannelAccess(tbframe);

    QSize size(1280, 1024);
    size = size.expandedTo(minimumSizeHint());
    resize(size);
    tabWidget->setCurrentIndex(0);
    QMetaObject::connectSlotsByName(this);

    //Modified by linupark in 20120713[[
    //msgframe = new QFrame(centralwidget);
    //msgframe->setObjectName(QString::fromUtf8("msgframe"));
    //]]
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
    //Modified by linupark in 20120713[[
    //msgframe->setGeometry(QRect(19, 820, 1255, 90));


	//vboxLayout2 = new QVBoxLayout(msgframe);
	//vboxLayout2->setSpacing(0);
	//vboxLayout2->setMargin(0);
   //]]

	QHBoxLayout *vhLayout = new QHBoxLayout();
	vhLayout->setSpacing(0);
	vhLayout->setMargin(0);

	CADisplayer *ioc1_interlock = new CADisplayer();
	ioc1_interlock->setMinimumSize(QSize(160,20));
	ioc1_interlock->setMaximumSize(QSize(160,20));
	ioc1_interlock->setPvname("FUEL_IOC_INTERLOCK");
	ioc1_interlock->setObjectName("CADisplayer_ioc1_interlock");
	ioc1_interlock->setVisible(false);
	vhLayout->addWidget(ioc1_interlock);

	CADisplayer *ioc2_interlock = new CADisplayer();
	ioc2_interlock->setMinimumSize(QSize(160,20));
	ioc2_interlock->setMaximumSize(QSize(160,20));
	ioc2_interlock->setPvname("GDC_IOC_INTERLOCK");
	ioc2_interlock->setObjectName("CADisplayer_ioc2_interlock");
	ioc2_interlock->setVisible(false);
	vhLayout->addWidget(ioc2_interlock);
    //Modified by linupark in 20120713[[
	//vboxLayout2->addLayout(vhLayout);
    //]]
    /*
	textEdit = new QTextEdit(this);
	textEdit->setObjectName(QString::fromUtf8("textEdit"));
	//textEdit->setGeometry(QRect(0, 0, 1000, 50)); 
	//textEdit->setGeometry(QRect(16, 900, 1000, 70)); 
	textEdit->setFontPointSize(12);
	textEdit->setAutoFormatting(QTextEdit::AutoAll);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    */
    //Modified by linupark in 20120713[[
	//vboxLayout2->addWidget(textEdit);
	//AttachChannelAccess *pattach_msg = new AttachChannelAccess(msgframe);
    //]]

    	setCentralWidget(centralwidget);

	//QObject::connect(ioc1_interlock, SIGNAL(valueChanged(QString)), this,  SLOT(changeText(QString))); 
	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 


	// Set Object Text.
	setWindowTitle(QApplication::translate("MainWindow", "Fuel & GDC (KSTAR Fuel & Glow Discharge System)", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/Fuel.xpm")));
#if 0
    action_Print->setText(QApplication::translate("MainWindow", "&Print", 0, QApplication::UnicodeUTF8));
#endif
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));
    action_Multiplot->setText(QApplication::translate("MainWindow", "&Multiplot", 0, QApplication::UnicodeUTF8));
    action_Archivesheet->setText(QApplication::translate("MainWindow", "&Archivesheet", 0, QApplication::UnicodeUTF8));
    action_Archiverviewer->setText(QApplication::translate("MainWindow", "a&Rchiveviewer", 0, QApplication::UnicodeUTF8));
    action_SignalDB->setText(QApplication::translate("MainWindow", "&SignalDB", 0, QApplication::UnicodeUTF8));
    action_PVListV->setText(QApplication::translate("MainWindow", "&PVListviewer", 0, QApplication::UnicodeUTF8));
#if 0
    action_Manual->setText(QApplication::translate("MainWindow", "ma&Nual", 0, QApplication::UnicodeUTF8));
#endif
    action_AboutTMS->setText(QApplication::translate("MainWindow", "About &TMS", 0, QApplication::UnicodeUTF8));
    //tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "Fuel & GDC panels", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "Individual", 0, QApplication::UnicodeUTF8));

//Added by linupark tof TAB[[
	tabWidget->setTabText(tabWidget->indexOf(tab_1), QApplication::translate("MainWindow", "Total Operation", 0, QApplication::UnicodeUTF8));
//]]
	
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
                        stackedWidget->addWidget(pattach->GetWidget());
                }
	};
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
		if ( objName.compare(pobj->objectName()) == 0 ) pushbutton -> setEnabled(false);
		// shbaek modified else if condition to enable CAPushbutton::setFalseActive [2009.11.02]
		// else if (pushbutton->isEnabled() != false) pushbutton -> setEnabled(true);
		// ------------------------------------------
		// shbaek modified if clauses [2009.11.21]
		else if (objName.contains("caPushButton")) {}
		else pushbutton -> setEnabled(true);
	};

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
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
				if(pword.getStatus()== true) displayDockWidget(index, pBut);
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
}
void
MainWindow::showPVListViewer()
{
	PVListViewer *plist = new PVListViewer();
}
void
MainWindow::closeEvent(QCloseEvent *)
{
	archiver.kill();
	archiver2.kill();
	archiver3.kill();
	archiver4.kill();
	signaldb.kill();
	kill(getpid(), SIGTERM);
}

void
MainWindow::showSignalDB()
{
	QString program = "konqueror";
	QStringList arguments;
	arguments << "http://172.17.100.204/signalDB";
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
	qDebug("Commands: %s %s %s", program.toStdString().c_str(), 
			arguments[0].toStdString().c_str(), 
			arguments[1].toStdString().c_str());
#if 1
	if (archiver.state() != QProcess::Running )
	{
		archiver.start(program, arguments);
	}
	else if (archiver2.state() != QProcess::Running )
	{
		archiver2.start(program, arguments);
	}
	else if (archiver3.state() != QProcess::Running )
	{
		archiver3.start(program, arguments);
	}
	else if (archiver4.state() != QProcess::Running )
	{
		archiver4.start(program, arguments);
	}
#else
	if ( myProcess->state() != QProcess::Running )
	{
		myProcess = new QProcess(this);
		myProcess->start(program, arguments);
	};
#endif
}

void
MainWindow::showManual()
{
	QString path = "/usr/local/opi/doc/index.html";  
	if (!assistant)
			assistant = new QAssistantClient("");
	assistant->showPage(path);
}

void
MainWindow::showAboutTMS()
{
	QMessageBox *mbox = new QMessageBox();
	mbox->setIconPixmap(QPixmap("/usr/local/opi/images/Fuel.xpm"));
	mbox->setWindowTitle("About KSTAR Fuel");
	mbox->setText("KSTAR Fuel Operator Interface");
	mbox->setDetailedText("It is developed to control the KSTAR Fueling and Glow Discharge System.");
	mbox->exec();

}
void
MainWindow::setDefaultIndex(int index) 
{
	if (index == 0)
	{
		setEnableButtons(pushButton[0]);
		setIndex(0);
		//stackedWidget->setCurrentIndex(0);
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
	if ((modifier==0x14 || modifier == 0x4)&& key == Qt::Key_P)
	{
		QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
		QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
		QString format = "jpg";
		mpixmap = QPixmap::grabWidget(this);
		mpixmap.save(filename, format.toAscii());
	};
}

