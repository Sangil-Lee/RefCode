#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow(const bool cap):mcap(cap)
{
	init();

	if(mcap == true) 
	{
		qDebug("StartTimer..");
		startTimer(30000);
	};
}

MainWindow::~MainWindow()
{
}

void
MainWindow::init()
{
	displayDocklist
	<<"icp"
	<<"hds_vibrate";

	makeUI();
	createConnects();
	registerPushButtonsEvent();
	attachAllWidget();
   	tabWidget->setCurrentIndex(1);
	setIndex(0);
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

    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
    connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));

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

    QPalette palettedw;

    QBrush brdw1(QColor(255, 255, 55, 255));
    brdw1.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Button, brdw1);

    QBrush brdw2(QColor(255, 0, 10, 255));
    brdw2.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Mid, brdw2);

    dockWidgetContents = new QWidget(dockWidget);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));

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

    pushButton[1] = new QPushButton(tab_0);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font0);
    pushButton[1]->setText(QApplication::translate("MainWindow", "HDS VIBRATE", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[1]);

    vboxLayout0->setSpacing(5);
    vboxLayout0->setMargin(5);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout1"));

    spacerItem0 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout0->addItem(spacerItem0);

    spacerItem0 = new QSpacerItem(20, 450, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout0->addItem(spacerItem0);

    frame1 = new QFrame(tab_0);
    frame1->setObjectName(QString::fromUtf8("frame1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(frame1->sizePolicy().hasHeightForWidth());
    frame1->setSizePolicy(sizePolicy2);
    frame1->setMinimumSize(QSize(16, 240));
    frame1->setFrameShape(QFrame::StyledPanel);
    frame1->setFrameShadow(QFrame::Raised);

    vboxLayout0->addWidget(frame1);
    QVBoxLayout *vf1boxLayout = new QVBoxLayout(frame1);
    vf1boxLayout->setSpacing(0);
    vf1boxLayout->setMargin(0);
    vf1boxLayout->setObjectName(QString::fromUtf8("vf1boxLayout"));

    QUiLoader m_loader1;

    QFile *file1 = new QFile("/usr/local/opi/ui/ICP_legend.ui");
    file1->open(QFile::ReadOnly);
    QWidget *m_widget1 = m_loader1.load(file1);
    file1->close();
    vf1boxLayout->addWidget(m_widget1);

    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "Main panels", 0, QApplication::UnicodeUTF8));

    menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 1280, 30));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
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
    menu_File->addAction(action_Exit);

    QFrame *tbframe = new QFrame();
    toolBar->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(920, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(100, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	CAGraphic *ioc1HB = new CAGraphic(this);
	ioc1HB->setLineWidth(2);
	ioc1HB->setMinimumSize(QSize(20,20));
	ioc1HB->setMaximumSize(QSize(20,20));
	ioc1HB->setFillColor(QColor("white"));
	ioc1HB->setLineColor(QColor("black"));
	ioc1HB->setPvname("NB1_MAIN_HeartBeat");
	ioc1HB->setFillColor(QColor("gray"));
	ioc1HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc1HB->setObjectName("CAGraphic_CRHB");
	ioc1HB->setToolTip("NBI1 Heart Beat");

    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":/images/kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addItem(tbspacer2);
    tblayout->addWidget(ioc1HB);
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
    msgframe->setGeometry(QRect(19, 820, 1255, 90));

	vboxLayout3 = new QVBoxLayout(msgframe);
	vboxLayout3->setSpacing(0);
	vboxLayout3->setMargin(0);

	QHBoxLayout *vhLayout = new QHBoxLayout();
	vhLayout->setSpacing(0);
	vhLayout->setMargin(0);

	textEdit = new QTextEdit(this);
	textEdit->setObjectName(QString::fromUtf8("textEdit"));
	textEdit->setFontPointSize(12);
	textEdit->setAutoFormatting(QTextEdit::AutoAll);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	vboxLayout3->addWidget(textEdit);

    	setCentralWidget(centralwidget);

	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setDefaultIndex(int))); 

	setWindowTitle(QApplication::translate("MainWindow", "KSTAR ICP", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/VMS.xpm")));
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));

    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "Main panels", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));

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
	}
}

bool MainWindow::displayDockWidget(const int index, QPushButton *pobj)
{
	if ( index >= DISPLAYTOT || index < DISPLAYTYPE ) return false;

	QString displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(index)+".ui";
    dockWidget->setWindowTitle(displayDocklist.at(index));
	setEnableButtons(pobj);

	if(stackedWidget->widget(index) != 0)
	{
		setIndex(index);
	} 
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
	QPushButton *pBut = (QPushButton*)obj;
	QString objName = pBut->objectName();
	int index = 0;

	if(objName.compare("pushButton_0") == 0) index = 0;
	else if(objName.compare("pushButton_1") == 0) index = 1;
	else index = -1;

	if(event->type()==QEvent::MouseButtonPress && pBut -> isEnabled() && (index < 2 || index >= 0))
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if(mouseEvent->buttons() & Qt::LeftButton)
		{
			displayDockWidget(index, pBut);
			return true;
		}	
	}

	return QMainWindow::eventFilter(obj, event);
}

void
MainWindow::closeEvent(QCloseEvent *)
{
	::exit(0);
}

void
MainWindow::setDefaultIndex(int index) 
{
	if (index == 0)
	{
		setEnableButtons(pushButton[0]);
		//stackedWidget->setCurrentIndex(0);
		setIndex(0);
	}
	else if (index == 1)
	{
		setEnableButtons(pushButton[1]);
		//stackedWidget->setCurrentIndex(1);
		setIndex(1);
	}
	else if (index == 2)
	{
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
void 
MainWindow::timerEvent(QTimerEvent *event)
{
}
