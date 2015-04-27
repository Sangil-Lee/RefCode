#include <QtGui>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "MenuWindow.h"


void childHandler (int signo)
{
	int  	status;
	pid_t	pid;		// child process ID
	while (1) 
	{
		if ((pid = waitpid (-1, &status, WNOHANG)) > 0) 
		{
			if (WIFSTOPPED(status)) {
				qDebug("child #%d stopped", pid);
			} 
			else if (WIFSIGNALED(status)) {
				qDebug("child #%d signalled (%d)", pid, WTERMSIG(status));
			} 
			else if (WIFEXITED(status)) {
				qDebug("child #%d exited", pid);
			} 
			else {
				qDebug("child #%d caused SIGCHLD", pid);
			}
		}
		else 
		{
			if (pid == -1 && errno != ECHILD) 
			{
				// bad return from wait()
				qDebug("wait() failed");
			};
			break;
		};
	};
}

int MenuWindow::initSignalHandler ()
{
	struct sigaction	action;

	sigemptyset (&action.sa_mask);
	action.sa_handler	= childHandler;
	action.sa_flags		= 0;

	return sigaction(SIGCHLD, &action, NULL);
}

MenuWindow::MenuWindow()
{
	init();
}

void MenuWindow::init()
{

	hostinfo = new QHostInfo();
	hostname = hostinfo->localHostName();

	pcslist<<"opi05";
	tsslist<<"opi03"<<"opi04";
	ccslist<<"opi03"<<"opi04";
	mpslist<<"opi05"<<"opi06";
	qdslist<<"opi11";
	echlist<<"opi11";
	ddslist<<"opi10"<<"opi17"<<"opi18";


    createIconGroupBox();
    createActions();
    createTrayIcon();
	//initSignalHandler ();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconGroupBox1);
    mainLayout->addWidget(iconGroupBox2);
    mainLayout->addWidget(iconGroupBox3);
    mainLayout->addWidget(iconGroupBox4);
    setLayout(mainLayout);

	//QIcon icon("/usr/local/opi/images/kooka.xpm");
    //trayIcon->setIcon(icon);
    //trayIcon->show();

	//iconlist <<"images/heart.svg"<<"images/trash.svg";

	applist<<"TMS"<<"VMS"<<"SCS"<<"OPIapp";

    setWindowTitle(tr("KSTAR-MenuWindow"));
    resize(400, 300);
}

void MenuWindow::setVisible(bool visible)
{
    //minimizeAction->setEnabled(visible);
    //maximizeAction->setEnabled(!visible);
    restoreAction->setEnabled(!visible);
    QWidget::setVisible(visible);
}

void MenuWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void MenuWindow::execution(const QString message, const QStringList hostlist)
{

	if (hostlist.contains(NULL) || hostlist.contains(hostname))
	{

		qDebug("MenuWindow:message:%s", message.toStdString().c_str());
	    qDebug("Host name(%s) is in the ExecutableHostList", 
			hostname.toStdString().c_str());
		MenuButton *menubutton = qobject_cast<MenuButton *>(sender());

		trayIcon->setIcon(menubutton->icon());
		trayIcon->show();

		if (trayIcon->isVisible()) 
		{
			hide();
		};
		processManager(message);
	}
	else
	{
	    qDebug("Host name(%s) is not in the ExecutableHostList", 
			hostname.toStdString().c_str());
	}
}

void MenuWindow::processManager(const QString program)
{
	//Process Management 
	size_t processcount = vec_process.size();
	qDebug ("processcnt:%d, program:%s", processcount, program.toStdString().c_str());
	
#if 1
	myProcess = new QProcess(this);
	myProcess->start(program);
#endif

	vec_process.push_back(myProcess);
}

void MenuWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) 
	{
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}

void MenuWindow::messageClicked()
{
    QMessageBox::information(0, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void MenuWindow::createIconGroupBox()
{
    iconGroupBox1 = new QGroupBox(tr("Vacuum pumping stage"));
    iconGroupBox2 = new QGroupBox(tr("Cool-down stage"));
    iconGroupBox3 = new QGroupBox(tr("Magnet current charging stage"));
    iconGroupBox4 = new QGroupBox(tr("Plasma creation stage"));

#if 0
	QPixmap pixmap("/usr/local/opi/images/kooka.xpm");
	QIcon tms_icon(pixmap);
	QIcon vms_icon("/usr/local/opi/images/trash.svg");
	QIcon cls_icon("/usr/local/opi/images/bad.svg");
#endif
	QPixmap tms_pixmap("/usr/local/opi/images/TMS.xpm");
	QIcon tms_icon(tms_pixmap);

	QPixmap pcs_pixmap("/usr/local/opi/images/PCS.xpm");
	QIcon pcs_icon(pcs_pixmap);

	QPixmap qds_pixmap("/usr/local/opi/images/QDS.xpm");
	QIcon qds_icon(qds_pixmap);

	QPixmap vms_pixmap("/usr/local/opi/images/VMS.xpm");
	QIcon vms_icon(vms_pixmap);

	QPixmap scs_pixmap("/usr/local/opi/images/SCS.xpm");
	QIcon scs_icon(scs_pixmap);

	QPixmap fuel_pixmap("/usr/local/opi/images/Fuel.xpm");
	QIcon fuel_icon(fuel_pixmap);

	QPixmap dds_pixmap("/usr/local/opi/images/Diag.xpm");
	QIcon dds_icon(dds_pixmap);

	QPixmap icrh_pixmap("/usr/local/opi/images/ICRH.xpm");
	QIcon icrh_icon(icrh_pixmap);

#if 0
	QIcon cls_icon("/usr/local/opi/images/bad.svg");
#endif

    tmsButton = new MenuButton("TMS", tms_icon, tr(""), this);
	tmsButton->setText("TMS/HDS/HRS");

    pcsButton = new MenuButton("medm -x /home/kstar/PCS_medm/PCSDisplay200803.adl", pcs_icon, tr(""), this, pcslist);
	pcsButton->setText("PCS");

    qdsButton = new MenuButton("QDS", qds_icon, tr(""), this, qdslist);
	qdsButton->setText("QDS");

    vmsButton = new MenuButton("VMS", vms_icon, tr(""), this);
	vmsButton->setText("VMS/CLS");

    scsButton = new MenuButton("SCS", scs_icon, tr(""), this);
	scsButton->setText("SCS");

    fuelButton = new MenuButton("Fuel", fuel_icon, tr(""), this);
	fuelButton->setText("Fuel/GDC");

    ddsButton = new MenuButton("DDS", dds_icon, tr(""), this, ddslist);
	ddsButton->setText("DDS");

    icrhButton = new MenuButton("ICRH", icrh_icon, tr(""), this);
	icrhButton->setText("ICRH");

	tmsButton->setIconSize(QSize(64,64));
	pcsButton->setIconSize(QSize(64,64));
	qdsButton->setIconSize(QSize(64,64));
	vmsButton->setIconSize(QSize(64,64));
	scsButton->setIconSize(QSize(64,64));
	fuelButton->setIconSize(QSize(64,64));
	ddsButton->setIconSize(QSize(64,64));
	icrhButton->setIconSize(QSize(64,64));

	//iconButton->setFlat(true);
	//iconButton2->setFlat(true);
    iconLayout1 = new QGridLayout;
    iconLayout1->addWidget(vmsButton, 0, 0);
    iconLayout1->addWidget(fuelButton, 0, 1);
	iconLayout1->setAlignment(Qt::AlignLeft);
    iconGroupBox1->setLayout(iconLayout1);

    iconLayout2 = new QGridLayout;
    iconLayout2->addWidget(tmsButton, 0, 0);
    iconLayout2->addWidget(qdsButton, 0, 1);
	iconLayout2->setAlignment(Qt::AlignLeft);
	iconGroupBox2->setLayout(iconLayout2);

    iconLayout3 = new QGridLayout;
    iconLayout3->addWidget(pcsButton, 0, 0);
	iconLayout3->setAlignment(Qt::AlignLeft);
	iconGroupBox3->setLayout(iconLayout3);

    iconLayout4 = new QGridLayout;
    iconLayout4->addWidget(scsButton, 0, 0);
    iconLayout4->addWidget(ddsButton, 0, 1);
    iconLayout4->addWidget(icrhButton, 0, 2);
	iconLayout4->setAlignment(Qt::AlignLeft);
	iconGroupBox4->setLayout(iconLayout4);
}

void MenuWindow::createActions()
{
    //minimizeAction = new QAction(tr("Mi&nimize"), this);
    //connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    //maximizeAction = new QAction(tr("Ma&ximize"), this);
    //connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Show"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(menuExit()));
    //connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MenuWindow::menuExit()
{
	vector<QProcess *>::iterator veciter;

	for(veciter=vec_process.begin(); veciter != vec_process.end(); ++veciter)
	{
		QProcess *process = static_cast<QProcess*> (*veciter);
		qDebug("processID:%d", process->pid() );
		if (process->pid() == 0) continue;
		process->close();
	};
	qApp->quit();
}

void MenuWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    //trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
