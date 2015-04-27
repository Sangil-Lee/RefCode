#include <iostream>
#include "striptoollegend.h"


StripToolLegend::StripToolLegend():msgqname("/Stripmsgq"), striptoolprc()
{
	init();
	getUiObject();
	registerSignal();
	startStriptool();
}
StripToolLegend::~StripToolLegend()
{
    /* closing the queue -- mq_close() */
    mq_close(m_msgqID);
	striptoolprc.kill();
    if(m_msgqID ==-1)	mq_unlink(msgqname.c_str());
}

void
StripToolLegend::init()
{
	QFile file ("/usr/local/opi/ui/StripToolLegend_harf.ui");
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

    mq_unlink(msgqname.c_str());
    m_msgqID = mq_open(msgqname.c_str(), O_RDWR|O_CREAT|O_EXCL|O_NONBLOCK, S_IRWXU|S_IRWXG, NULL);
    if (m_msgqID == -1) 
	{
        qDebug("In mq_open()");
        exit(1);
    };

	printf("msgID:%d\n", m_msgqID);
    /* getting the attributes from the queue        --  mq_getattr() */
    mq_getattr(m_msgqID, &m_msgqAttr);
    qDebug("Queue:\n\t- stores at most %ld messages\n\t- large at most %ld bytes each\n\t- currently holds %ld messages\n", m_msgqAttr.mq_maxmsg, m_msgqAttr.mq_msgsize, m_msgqAttr.mq_curmsgs);

	// 1 second
	(void)startTimer(1000);
	//QTimer::singleShot(1000, this, SLOT(getValue()));
}

void StripToolLegend::getUiObject()
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
		indexinfo.type = 0;
		indexinfo.precision = -1;
		vecIndex.push_back(indexinfo);
	};
	mcbLeindex = m_pwidget->findChild<QComboBox *>("cbLeindex");
	mrExp = m_pwidget->findChild<QRadioButton *>("rbExp");
	mrNum = m_pwidget->findChild<QRadioButton *>("rbNum");
	mrAuto= m_pwidget->findChild<QRadioButton *>("rbAuto");
	mspPrec = m_pwidget->findChild<QSpinBox *>("spPrec");
	mbApply = m_pwidget->findChild<QPushButton *>("pbApply");
}

void StripToolLegend::registerSignal()
{ 
	connect(mbApply, SIGNAL(clicked()), this, SLOT(applyDispalyformat()) );
}
void StripToolLegend::applyDispalyformat()
{
	int index = mcbLeindex->currentIndex(); 
	if( index == 0 )
	{
		for(int i = 0; i<MAXALL;i++)
		{
			if ( mrAuto->isChecked() == true )
			{
				vecIndex.at(i).type = 0;
				vecIndex.at(i).precision = -1;
			}
			else if ( mrExp->isChecked() == true )
			{
				vecIndex.at(i).type = 1;
				vecIndex.at(i).precision = -1;
			}
			else if ( mrNum->isChecked() == true) 
			{
				vecIndex.at(i).type = 2;
				vecIndex.at(i).precision = mspPrec->value();
			};
		}
	}
	else
	{
		if ( mrAuto->isChecked() == true )
		{
			vecIndex.at(index-1).type = 0;
			vecIndex.at(index-1).precision = -1;
		}
		else if ( mrExp->isChecked() == true )
		{
			vecIndex.at(index-1).type = 1;
			vecIndex.at(index-1).precision = -1;
		}
		else if ( mrNum->isChecked() == true) 
		{
			vecIndex.at(index-1).type = 2;
			vecIndex.at(index-1).precision = mspPrec->value();
		};
	};
}
//void StripToolLegend::getValue()
void StripToolLegend::timerEvent(QTimerEvent *)
{ 
	int msgsz;
	unsigned int sender;
	QColor	color;
	QPalette palette;
	QString	spvname;
	QString	spvimsi;
	QString	spvunit;
	int red, green, blue, index;
	//qDebug("%d", 0x000FFFFF);
#if 0
    msgsz=mq_receive(m_msgqID, (char*)&stripinfo.nopv, sizeof(short)*MAXALL, &sender);
	//qDebug("MsgSize:%d, sender:%d", msgsz, sender);
	for(int i = 0; i < MAXALL;i++)
	{
		qDebug("nopv[%d]:%d", i, stripinfo.nopv[i]);
		if( stripinfo.nopv[i] == 0 )
		{
			mlball[i]->setText("");
			mleall[i]->setText("");
			mlb1st[i]->setText("");
			mle1st[i]->setText("");
			mlb1stunit[i]->setText("");
		};
	};
#endif
	bool breadmsg = false;
	int count = 0;
    while((msgsz=mq_receive(m_msgqID, (char*)&stripinfo, maxmsgsize, &sender))>0) 
	{
		//printf("Recive[%d]:%s:%s, unit:%s, R:%d,G:%d,B:%d\n",stripinfo.index, stripinfo.pvname,
				//stripinfo.svalue, stripinfo.egu, stripinfo.r, stripinfo.g, stripinfo.b);
		red =   (stripinfo.r*255)/65535;
		green = (stripinfo.g*255)/65535;
		blue =  (stripinfo.b*255)/65535;
		color.setRgb(red,green, blue);
		palette.setBrush(QPalette::Active, QPalette::WindowText, color);
		palette.setBrush(QPalette::Inactive, QPalette::WindowText, color);
		spvimsi = stripinfo.pvname;
		spvname = spvimsi.insert(12,"\n");
		spvunit = QString("%1").arg(stripinfo.egu);
		//QString strvalue = displayLegend(stripinfo.index, QString(stripinfo.svalue));
		QString strvalue = displayLegend(stripinfo.index, stripinfo.svalue);
		index = stripinfo.index;
		mlball[index]->setPalette(palette);
		//mlball[index]->setText(stripinfo.pvname);
		mlball[index]->setText(spvname);
		mlb1st[index]->setPalette(palette);
		mlb1st[index]->setText(spvname);
		mleall[index]->setText(strvalue+spvunit);
		mle1st[index]->setText(strvalue);
		mlb1stunit[index]->setText(spvunit);
		breadmsg=true;
	};
#if 1
	if(breadmsg==true)
	{
		unsigned int pvbit = stripinfo.pvbit;
		mpvcount = bitCount(pvbit);
		breadmsg=false;
		for(int i = 0; i< 10; i++)
		{
			if( bitLGet(pvbit, i)==1)
			{
				mlball[i]->setText("");
				mlb1st[i]->setText("");
				mleall[i]->setText("");
				mle1st[i]->setText("");
				mlb1stunit[i]->setText("");
			};
		}
	}
#endif
}
QString 
StripToolLegend::displayLegend(const int index, const char *svalue)
{
	double pvvalue = QString(svalue).toDouble();
	QString strval;
	//qDebug("index:%d, type:%d", index, vecIndex.at(index).type);
	switch(vecIndex.at(index).type)
	{
		case 1:
			strval = QString("%1").arg(pvvalue, 0, 'E', 3);
			break;
		case 2:
			strval = QString("%1").arg(pvvalue, 0, 'f', vecIndex.at(index).precision);
			break;
		default:
			strval = QString("%1").arg(pvvalue, 0, 'g');
			break;
	};
		//strval = QString("%1").arg(pvvalue, 0, 'E', 3);

	return strval;
}
void 
StripToolLegend::startStriptool()
{
#if 1
	QString program = "StripTool";
	if (striptoolprc.state() != QProcess::Running )
	{
		striptoolprc.start(program);
	};
#endif
}
void StripToolLegend::bitOR ( unsigned int &src, unsigned int org )
{
	src |=org;
}
void StripToolLegend::bitAND ( unsigned int &src, unsigned int org )
{
	src &=~org;
}
void StripToolLegend::bitRSet ( unsigned int &src, const int pos, int state )
{
	//Left->Right bit set
	unsigned int mask = ~0u-( ~0u >> 1 );
	//print_binary(mask);
	for(int i=0;i<pos;i++)
	{
		mask = mask>>1;
	};
	//print_binary(mask);
	if(state>=1)
		src|=mask;
	else
		src&=~mask;
}
void StripToolLegend::bitLSet ( unsigned int &src, const int pos, int state )
{
	//Right->Left bit set
	//unsigned int mask = ~0u-( ~0u >> 1 );
	unsigned int mask = 0x00000001;
	//print_binary(mask);
	for(int i=0;i<pos;i++)
	{
		mask = mask<<1;
	};
	//print_binary(mask);
	if(state>=1)
		src|=mask;
	else
		src&=~mask;
}
int StripToolLegend::bitLGet(unsigned int &src, const int pos)
{
	//Right->Left bit set
	//unsigned int mask = ~0u-( ~0u >> 1 );
	unsigned int mask = 0x00000001;
	//print_binary(mask);
	for(int i=0;i<pos;i++)
	{
		mask = mask<<1;
	};
	return (((src&mask) == mask)?1:0);
}
int StripToolLegend::bitRGet(unsigned int &src, const int pos)
{
	//Right->Left bit set
	//unsigned int mask = ~0u-( ~0u >> 1 );
	unsigned int mask = ~0u-( ~0u >> 1 );
	//print_binary(mask);
	for(int i=0;i<pos;i++)
	{
		mask = mask>>1;
	};
	return (((src&mask) == mask)?1:0);
}
void StripToolLegend::print_binary( unsigned int u )
{
	unsigned int mask = ~0u - ( ~0u >> 1 );
	while ( mask != 0 )
	{
		if ( ( u & mask ) != 0 ) printf ("1");
		else printf ("0");
		mask = mask >> 1;
	};	
	printf ("\n");
};
int StripToolLegend::bitCount(unsigned int &src)
{
	unsigned int mask = ~0u - ( ~0u >> 1 );
	int count = 0;
	while ( mask != 0 )
	{
		if ( ( src & mask ) != 0 ) count++;
		mask = mask >> 1;
	};	
	return count;
};
