#include <iostream>
#include "capvviewer.h"

static void printChidInfo(chid chid, char *message)
{
	printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
	printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
	printf("exceptionCallback called\n");
	chid        chid = args.chid;
	//long      stat = args.stat; /* Channel access status code*/
	const char  *channel;
	static char *noname = "unknown";
	channel = (chid ? ca_name(chid) : noname);
	if(chid) printChidInfo(chid,"exceptionCallback");

};


CAPVViewer::CAPVViewer():striptoolprc()
{
	init();
	getUiObject();
	registerSignal();
	startStriptool();
}
CAPVViewer::~CAPVViewer()
{
	striptoolprc.kill();
}

void
CAPVViewer::init()
{
	QFile file ("/usr/local/opi/ui/CAPVViewer.ui");
	if (!file.exists())
	{
		qDebug("Ui File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->show();
	m_pwidget->move(0,0);
	file.close();

	ca_context_create(ca_disable_preemptive_callback);

	// 1 second
	(void)startTimer(1000);
	//QTimer::singleShot(1000, this, SLOT(getValue()));
}

void CAPVViewer::getUiObject()
{
	mall_label = m_pwidget->findChildren<QLabel *>();
	mall_ledit = m_pwidget->findChildren<QLineEdit *>();
	mptab = m_pwidget->findChild<QTabWidget*>("tabWidget");

	QString sleall,slball, sle1st,slb1st,slb1stunit;
	for(int i = 0;i<MAXALL;i++)
	{
		sleall = QString("leAll%1").arg(i);
		slball = QString("lbAll%1").arg(i);
		sle1st=QString("le1st%1").arg(i);
		slb1st=QString("lb1st%1").arg(i);
		slb1stunit=QString("lb1stunit%1").arg(i);
		mleall[i] = m_pwidget->findChild<QLineEdit *>(sleall);
		mlball[i] = m_pwidget->findChild<QLabel *>(slball);
		mle1st[i] = m_pwidget->findChild<QLineEdit *>(sle1st);
		mlb1st[i] = m_pwidget->findChild<QLabel *>(slb1st);
		mlb1stunit[i]= m_pwidget->findChild<QLabel *>(slb1stunit);
	};
	mcbLeindex = m_pwidget->findChild<QComboBox *>("cbLeindex");
	mrExp = m_pwidget->findChild<QRadioButton *>("rbExp");
	mrNum = m_pwidget->findChild<QRadioButton *>("rbNum");
	mrAuto= m_pwidget->findChild<QRadioButton *>("rbAuto");
	mspPrec = m_pwidget->findChild<QSpinBox *>("spPrec");
	mbApply = m_pwidget->findChild<QPushButton *>("pbApply");
}

void CAPVViewer::registerSignal()
{ 
	connect(mbApply, SIGNAL(clicked()), this, SLOT(applyDispalyformat()) );
}
void CAPVViewer::applyDispalyformat()
{
	int index = mcbLeindex->currentIndex(); 
	if( index == 0 )
	{
	}
	else
	{
	};
}
//void CAPVViewer::getValue()
void CAPVViewer::timerEvent(QTimerEvent *)
{ 
}
QString 
CAPVViewer::displayLegend(const int index, const char *svalue)
{
	double pvvalue = QString(svalue).toDouble();
	QString strval;
	switch(index)
	{
		case 1:
			strval = QString("%1").arg(pvvalue, 0, 'E', 3);
			break;
		case 2:
			strval = QString("%1").arg(pvvalue, 0, 'f', 3);
			break;
		default:
			strval = QString("%1").arg(pvvalue, 0, 'g');
			break;
	};

	return strval;
}
void 
CAPVViewer::startStriptool()
{
#if 0
	QString program = "PVListviewer";
	if (striptoolprc.state() != QProcess::Running )
	{
		striptoolprc.start(program);
	};
#endif
}
