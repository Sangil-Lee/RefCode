#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow():archiver(), archiver2(),archiver3(),archiver4(),signaldb()
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
	displayDocklist
	<<"ECH_Status"
	<<"ECH_Fault_List"
	<<"ECH_NIDAQcontrol"
	<<"";

	passWord
	<<""
	<<""
	<<""
	<<"";

	makeUI();
	createActions();
	createConnects();
	registerPushButtonsEvent();
	attachAllWidget();
   	tabWidget->setCurrentIndex(0);

	setIndex(0);
}
void
MainWindow::createActions()
{
	//menu_View->addAction(dockWidget->toggleViewAction());
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

#if 0
    action_Print = new QAction(this);
    action_Print->setObjectName(QString::fromUtf8("action_Print"));
#endif

#if 1
    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	//connect(action_Exit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));
#endif

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

	action_Manual = new QAction(this);
	action_Manual->setObjectName(QString::fromUtf8("action_Manual"));
	connect(action_Manual, SIGNAL(triggered()), this, SLOT(showManual()));

	action_AboutECH = new QAction(this);
	action_AboutECH->setObjectName(QString::fromUtf8("action_AboutECH"));
	connect(action_AboutECH, SIGNAL(triggered()), this, SLOT(showAboutECH()));

    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    //centralwidget->setGeometry(QRect(0, 0, 1280, 821));
	
    widget = new QWidget(centralwidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(200, 0, 1080, 821));
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
    tabWidget->setGeometry(QRect(0, 0, 200, 821));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());

#if 1
	/* TabWidget color setting */
    QPalette palettetw;

    QBrush brushtw1(QColor(60, 76, 100, 255));
    brushtw1.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Window, brushtw1);
    //palettetw.setBrush(QPalette::Inactive, QPalette::Window, brushtw1);
    //palettetw.setBrush(QPalette::Disabled, QPalette::Window, brushtw1);

    QBrush brushtw2(QColor(60, 76, 100, 255));
    brushtw2.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Base, brushtw2);
    //palettetw.setBrush(QPalette::Inactive, QPalette::Base, brushtw2);
    //palettetw.setBrush(QPalette::Disabled, QPalette::Base, brushtw2);

    palettetw.setBrush(QPalette::Active, QPalette::Button, brushtw1);
    //palettetw.setBrush(QPalette::Inactive, QPalette::Button, brushtw1);
    //palettetw.setBrush(QPalette::Disabled, QPalette::Button, brushtw1);

    QBrush brushtw3(QColor(255, 255, 255, 255));
    brushtw3.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::ButtonText, brushtw3);
    //palettetw.setBrush(QPalette::Inactive, QPalette::ButtonText, brushtw3);
    //palettetw.setBrush(QPalette::Disabled, QPalette::ButtonText, brushtw3);

    palettetw.setBrush(QPalette::Active, QPalette::WindowText, brushtw3);
    //palettetw.setBrush(QPalette::Inactive, QPalette::WindowText, brushtw3);
    //palettetw.setBrush(QPalette::Disabled, QPalette::WindowText, brushtw3);

    tabWidget->setPalette(palettetw);

#endif

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
    QFont font;
    font.setPointSize(14);
    vboxLayout0 = new QVBoxLayout(tab_0);
    vboxLayout0->setSpacing(6);
    vboxLayout0->setMargin(4);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout0"));

    QPalette paletteb;
    QBrush brushb(QColor(211, 197, 179, 255));
    brushb.setStyle(Qt::SolidPattern);
    paletteb.setBrush(QPalette::Active, QPalette::Button, brushb);
    //paletteb.setBrush(QPalette::Inactive, QPalette::Button, brushb);
    //paletteb.setBrush(QPalette::Disabled, QPalette::Button, brushb);

    QBrush brushbt(QColor(106, 88, 62, 255));
    brushbt.setStyle(Qt::SolidPattern);
    paletteb.setBrush(QPalette::Active, QPalette::ButtonText, brushbt);
    //paletteb.setBrush(QPalette::Inactive, QPalette::ButtonText, brushbt);
    //paletteb.setBrush(QPalette::Disabled, QPalette::ButtonText, brushbt);

    pushButton[0] = new QPushButton(tab_0);
    pushButton[0]->setObjectName(QString::fromUtf8("pushButton_0"));
    pushButton[0]->setFont(font);
    pushButton[0]->setText(QApplication::translate("MainWindow", "Operation", 0, QApplication::UnicodeUTF8));
    pushButton[0]->setPalette(paletteb);
    vboxLayout0->addWidget(pushButton[0]);

    pushButton[1] = new QPushButton(tab_0);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font);
    pushButton[1]->setText(QApplication::translate("MainWindow", "Interlock", 0, QApplication::UnicodeUTF8));
    pushButton[1]->setPalette(paletteb);
    vboxLayout0->addWidget(pushButton[1]);

    pushButton[2] = new QPushButton(tab_0);
    pushButton[2]->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton[2]->setFont(font);
    pushButton[2]->setText(QApplication::translate("MainWindow", "DAQ", 0, QApplication::UnicodeUTF8));
    pushButton[2]->setPalette(paletteb);
    vboxLayout0->addWidget(pushButton[2]);

#if 0
    pushButton[3] = new QPushButton(tab_0);
    pushButton[3]->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton[3]->setFont(font);
    pushButton[3]->setText(QApplication::translate("MainWindow", "Waveform Graph2", 0, QApplication::UnicodeUTF8));
    pushButton[3]->setPalette(paletteb);
    vboxLayout0->addWidget(pushButton[3]);
#endif

#if 0
    spacerItem0 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout0->addItem(spacerItem0);
#endif
    frame = new QFrame(tab_0);
    frame->setObjectName(QString::fromUtf8("frame"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy2.setHorizontalStretch(1);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
    frame->setSizePolicy(sizePolicy2);
    frame->setMinimumSize(QSize(16, 704));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    vboxLayout0->addWidget(frame);
	QVBoxLayout *vfboxLayout = new QVBoxLayout(frame);
	vfboxLayout->setSpacing(0);
	vfboxLayout->setMargin(0);
	vfboxLayout->setObjectName(QString::fromUtf8("vfboxLayout"));

	QUiLoader m_loader;
	QFile *file = new QFile("/usr/local/opi/ui/ECH_Menu_Area.ui");
	file->open(QFile::ReadOnly);
	QWidget *m_widget = m_loader.load(file);
	file->close();
	vfboxLayout->addWidget(m_widget);

	AttachChannelAccess *pattachECHMenu = new AttachChannelAccess(frame);

/* TG remove 20130704
    tab_1 = new QWidget();
    tab_1->setObjectName(QString::fromUtf8("tab_1"));
    vboxLayout1 = new QVBoxLayout(tab_1);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(4);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

    pushButton[3] = new QPushButton(tab_1);
    pushButton[3]->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton[3]->setFont(font);
    pushButton[3]->setText(QApplication::translate("MainWindow", "DAQ WavePattern", 0, QApplication::UnicodeUTF8));
    pushButton[3]->setPalette(paletteb);
    vboxLayout1->addWidget(pushButton[3]);

	spacerItem1 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
	vboxLayout1->addItem(spacerItem1);


    frame2 = new QFrame(tab_1);
    frame2->setObjectName(QString::fromUtf8("frame2"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy3.setHorizontalStretch(1);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(frame2->sizePolicy().hasHeightForWidth());
    frame2->setSizePolicy(sizePolicy3);
    frame2->setMinimumSize(QSize(16, 704));
    frame2->setFrameShape(QFrame::StyledPanel);
    frame2->setFrameShadow(QFrame::Raised);

    vboxLayout1->addWidget(frame2);
	QVBoxLayout *vfboxLayout1 = new QVBoxLayout(frame2);
	vfboxLayout1->setSpacing(0);
	vfboxLayout1->setMargin(0);
	vfboxLayout1->setObjectName(QString::fromUtf8("vfboxLayout1"));
	QUiLoader m_loader1;
	QFile *file1 = new QFile("/usr/local/opi/ui/ECH_Menu_Area.ui");
	file1->open(QFile::ReadOnly);
	QWidget *m_widget1 = m_loader1.load(file1);
	file1->close();
	vfboxLayout1->addWidget(m_widget1);

	AttachChannelAccess *pattachECHSubMenu = new AttachChannelAccess(frame2);

*/

	/*
    spacerItem1 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    vfboxLayout->addItem(spacerItem1);

	QUiLoader m_loader2;
	QFile *file2 = new QFile("/usr/local/opi/ui/ECH_Menu_Area2.ui");
	file2->open(QFile::ReadOnly);
	QWidget *m_widget2 = m_loader2.load(file2);
	file2->close();
	vfboxLayout->addWidget(m_widget2);
	*/




    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "ECH Main panels", 0, QApplication::UnicodeUTF8));
//    tabWidget->addTab(tab_1, QApplication::translate("MainWindow", "ECH Sub panels", 0, QApplication::UnicodeUTF8));

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
    //menubar->addAction(menu_View->menuAction());
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
	menu_Help->addAction(action_AboutECH);

#if 1
    QFrame *tbframe = new QFrame();
    toolBar->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(1000, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	//CAGraphic *ioc1HB = new CAGraphic();
	ioc1HB = new CAGraphic();
	ioc1HB->setLineWidth(2);
	ioc1HB->setMinimumSize(QSize(20,20));
	ioc1HB->setMaximumSize(QSize(20,20));
	ioc1HB->setFillColor(QColor("white"));
	ioc1HB->setLineColor(QColor("black"));
	ioc1HB->setFillMode(StaticGraphic::Solid);
	ioc1HB->setPvname("ECH_HEARTBEAT");
	ioc1HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc1HB->setObjectName("CAGraphic_ioc1HB");
	ioc1HB->setToolTip("ECH IOC HEART BEAT");

	//CAGraphic *ioc2HB = new CAGraphic();
/*
	ioc2HB = new CAGraphic();
	ioc2HB->setLineWidth(2);
	ioc2HB->setMinimumSize(QSize(20,20));
	ioc2HB->setMaximumSize(QSize(20,20));
	ioc2HB->setFillColor(QColor("white"));
	ioc2HB->setLineColor(QColor("black"));
	ioc2HB->setFillMode(StaticGraphic::Solid);
	ioc2HB->setPvname("ECH_LTU_HEARTBEAT");
	ioc2HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc2HB->setObjectName("CAGraphic_ioc2HB");
	ioc2HB->setToolTip("ECH LTU HEART BEAT");
*/

    font.setPointSize(12);
    //CAWclock *wclock1 = new CAWclock();
    wclock1 = new CAWclock();
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("ECH_IOC_WCLOCK.RVAL");
	wclock1->setFont(font);
	wclock1->setObjectName("CAWclock_wclock1");
	
    //QLabel *logo = new QLabel("KSTAR logo");
    logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":/images/kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);
	tblayout->addWidget(ioc1HB);
//	tblayout->addWidget(ioc2HB);
    tblayout->addItem(tbspacer2);
    tblayout->addWidget(logo);
	AttachChannelAccess *pattachTB = new AttachChannelAccess(tbframe);
#endif

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
    sizePolicy4.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
//TG    msgframe->setGeometry(QRect(19, 820, 1255, 90));
    //msgframe->setSizePolicy(sizePolicy4);
    msgframe->setGeometry(QRect(10, 880, 1254, 70));
    //msgframe->setMinimumSize(QSize(1164, 90));
    //msgframe->setFrameShape(QFrame::StyledPanel);
    //msgframe->setFrameShadow(QFrame::Raised);

	vboxLayout2 = new QVBoxLayout(msgframe);
	vboxLayout2->setSpacing(0);
	vboxLayout2->setMargin(0);


/*
	QUiLoader m_loader2;
	QFile *file2 = new QFile("/usr/local/opi/ui/ECH_message.ui");
	file2->open(QFile::ReadOnly);
	QWidget *m_widget2 = m_loader2.load(file2);
	file2->close();
	vboxLayout2->addWidget(m_widget2);
*/

	QHBoxLayout *vhLayout = new QHBoxLayout();
	vhLayout->setSpacing(0);
	vhLayout->setMargin(0);

	CADisplayer *ioc1_interlock = new CADisplayer();
	ioc1_interlock->setMinimumSize(QSize(160,20));
	ioc1_interlock->setMaximumSize(QSize(160,20));
	ioc1_interlock->setPvname("ECH_IOC1_INTERLOCK");
	ioc1_interlock->setObjectName("CADisplayer_ioc1_interlock");
	ioc1_interlock->setVisible(false);
	vhLayout->addWidget(ioc1_interlock);

	CADisplayer *ioc2_interlock = new CADisplayer();
	ioc2_interlock->setMinimumSize(QSize(160,20));
	ioc2_interlock->setMaximumSize(QSize(160,20));
	ioc2_interlock->setPvname("ECH_IOC2_INTERLOCK");
	ioc2_interlock->setObjectName("CADisplayer_ioc2_interlock");
	ioc2_interlock->setVisible(false);
	vhLayout->addWidget(ioc2_interlock);
	vboxLayout2->addLayout(vhLayout);

	textEdit = new QTextEdit(this);
	textEdit->setObjectName(QString::fromUtf8("textEdit"));
	//textEdit->setGeometry(QRect(0, 0, 1000, 50)); 
	//textEdit->setGeometry(QRect(16, 900, 1000, 70)); 
	textEdit->setFontPointSize(12);
	textEdit->setAutoFormatting(QTextEdit::AutoAll);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	vboxLayout2->addWidget(textEdit);

	//AttachChannelAccess *pattach_msg = new AttachChannelAccess(msgframe);
	setCentralWidget(centralwidget);

	QObject::connect(ioc1_interlock, SIGNAL(valueChanged(QString)), this,  SLOT(changeText(QString))); 

	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 


	// Set Object Text.
	setWindowTitle(QApplication::translate("MainWindow", "ECH (KSTAR 84GHz ECH System)", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/ECH.xpm")));
#if 0
    action_Print->setText(QApplication::translate("MainWindow", "&Print", 0, QApplication::UnicodeUTF8));
#endif
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));
    action_Multiplot->setText(QApplication::translate("MainWindow", "&Multiplot", 0, QApplication::UnicodeUTF8));
    action_Archivesheet->setText(QApplication::translate("MainWindow", "&Archivesheet", 0, QApplication::UnicodeUTF8));
    action_Archiverviewer->setText(QApplication::translate("MainWindow", "a&Rchiveviewer", 0, QApplication::UnicodeUTF8));
    action_SignalDB->setText(QApplication::translate("MainWindow", "&SignalDB", 0, QApplication::UnicodeUTF8));
    action_PVListV->setText(QApplication::translate("MainWindow", "&PVListviewer", 0, QApplication::UnicodeUTF8));
    action_Manual->setText(QApplication::translate("MainWindow", "ma&Nual", 0, QApplication::UnicodeUTF8));
    action_AboutECH->setText(QApplication::translate("MainWindow", "About &ECH", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "ECH main panels", 0, QApplication::UnicodeUTF8));
//    tabWidget->setTabText(tabWidget->indexOf(tab_1), QApplication::translate("MainWindow", "ECH sub panels", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    menu_Util->setTitle(QApplication::translate("MainWindow", "&Util", 0, QApplication::UnicodeUTF8));
    //menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
    menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));

} // setupUi

void MainWindow::registerPushButtonsEvent()
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	/* 
		At first, allPButtons--QList-- included all buttons inherited from QPushButton. 
		And this implemented installEventFilter(this) even for CAPushButtons.
	 */
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
		pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i);
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
		};
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
	else if( objName.compare("pushButton_13") == 0 ) index = DISPLAYTYPE13;
	else if( objName.compare("pushButton_14") == 0 ) index = DISPLAYTYPE14;
	else if( objName.compare("pushButton_15") == 0 ) index = DISPLAYTYPE15;
	else if( objName.compare("pushButton_16") == 0 ) index = DISPLAYTYPE16;
	else if( objName.compare("pushButton_17") == 0 ) index = DISPLAYTYPE17;
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
#if 0
				Password pword(passwd, new QWidget());
				if(pword.getStatus()== true) displayDockWidget(index, pBut);
#endif
#if 1
				displayDockWidget(index, pBut);
#endif
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
	//MultiplotMainWindow *pmainWindow = new MultiplotMainWindow;
	//pmainWindow->resize(1280,900);
}
void
MainWindow::showArchivesheet()
{
#if 0 // we do not use this archivesheet. so remove this function at 2011.03.10 
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
	kill(getpid(), SIGTERM);
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
#if 0
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
	if (archiver.state() != QProcess::Running )
	{
		archiver.start(program, arguments);
	};
#endif
#endif
}

void
MainWindow::showManual()
{
	//QString path = QApplication::applicationDirPath() + "../doc/index.html";  
	QString path = "/usr/local/opi/doc/index.html";  
	if (!assistant)
			assistant = new QAssistantClient("");
	assistant->showPage(path);
}

void
MainWindow::showAboutECH()
{
	QMessageBox *mbox = new QMessageBox();
	mbox->setIconPixmap(QPixmap("/usr/local/opi/images/ECH.xpm"));
	mbox->setWindowTitle("About KSTAR ECH");
	mbox->setText("KSTAR ECH Operator Interface");
	mbox->setDetailedText("It is the operator interface for the KSTAR 84GHz ECH System.\n[Release 2.3.0]");
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
	else if (index == 1)
	{
		setEnableButtons(pushButton[3]);
		setIndex(DISPLAYTYPE3);
		//setIndex(1);
		//stackedWidget->setCurrentIndex(1);
	}
	else if (index == 2)
	{
		
#if 0
		setEnableButtons(pushButton[14]);
		setIndex(14);
#endif
		//stackedWidget->setCurrentIndex(14);
#if 0
		QString passwd = passWord.at(15);
		Password pword(passwd, new QWidget());
		if(passwd.compare("") != 0)
		{
			//qDebug("Password: %s", passwd.toStdString().c_str());
			if(pword.getStatus()== true)
			{
				setEnableButtons(pushButton[15]);
				setIndex(15);
			}
			else {
				qDebug("Password is not match.");
			}
		}
#endif
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
	else if ( key == Qt::Key_F3)
	{
		showPVListViewer();
	}
}

