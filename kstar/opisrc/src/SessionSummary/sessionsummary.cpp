#include <sys/types.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "shotsummary.h"
#include "sessionsummary.h"

#define NBI_CHOICE 4

using namespace mysqlpp;

static int  CACommand(string strcmd)
{
	FILE *fp= popen (strcmd.c_str(), "r");
	char buf[20];
	while (fgets (buf, sizeof(buf), fp) ) {};
	pclose(fp);
	return ((strcmd.compare("ShotNum")== 0||strcmd.compare("ShotType")==0||strcmd.compare("NBIFactor")==0)?atoi(buf):0);
}

template <class T> class MultipleValue
{
	public:
		MultipleValue(const T& v):value(v){};
		void operator() (T& elem) const {elem*=value;};
	private:
		T value;
};

template <class T> int caPut(const int type, const QString pvname, T &value = 0)
{
	//qDebug("----------------------------------------------------- caPut() start");

	ca_context_create(ca_disable_preemptive_callback);
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	SEVCHK(status,NULL);
	status = ca_pend_io(3.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	switch(type)
	{
		case DBR_INT:
			int intval = value;
			status = ca_put(DBR_INT, chan_id, &intval);
			break;
		case DBR_LONG:
			long longval = (long) value;
			status = ca_put(DBR_LONG, chan_id, &longval);
			break;
		case DBR_FLOAT:
			float fval = (float) value;
			status = ca_put(DBR_FLOAT, chan_id, &fval);
			break;
		case DBR_DOUBLE:
			double dval = value;
			status = ca_put(DBR_DOUBLE, chan_id, &dval);
			break;
		case DBR_CHAR:
			char cval = value;
			status = ca_put(DBR_CHAR, chan_id, &cval);
			break;
		default:
			break;
	};

	status = ca_pend_io(3.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPut timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	ca_context_destroy();
	//qDebug("----------------------------------------------------- caPut() end");
	return status;
};

SessionSummary::SessionSummary():m_con(false),mpShotResult(0),mshotno(0),mbAuto(false)
{
	init();
	getUiObject();
	registerSignal();
	hostHide(mhostname);
	emit Search();
	mpThr = new SessionSummaryThr(this);
	mpShotResult = new ShotResult();
	mpShotResult->setParent(this);
	//mpShotResult->Hide();
	qDebug("hostname:%s", mhostname.toStdString().c_str());
}
SessionSummary::~SessionSummary()
{
}

void
SessionSummary::init()
{
	QFile file ("../ui/SessionSummaryW.ui");
	//QFile file ("/usr/local/opi/ui/SessionSummaryW.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(SessionSummaryW.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	//m_pwidget->showMaximized();
	m_pwidget->show();
	file.close();
	mbstart = true;

	char hostname[32];
	gethostname(hostname, sizeof(hostname));
	mhostname = hostname;

	string dbname   = "kstarweb";
	//string dbserverhost = "172.17.100.204";
	string dbserverhost = "localhost";
	string user     = "root";
	string passwd   = "kstar2004";
	//mysqlpp::Connection con(mysqlpp::use_exceptions);
	//mysqlpp::Connection con(false);
	if ( get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
	};

	if( !mysqlkstardb.Connect_to_db(dbname, dbserverhost, user, passwd, m_con) )
	{
		cerr<< "MySQL Database Connection Failed. Check Database Server or Network." << endl;
	};
}

void SessionSummary::getUiObject()
{
	m_ptable	= m_pwidget->findChild<QTableWidget*>("tableWidget");
	mckToday	= m_pwidget->findChild<QCheckBox *>("ckToday");
	mckDesc		= m_pwidget->findChild<QCheckBox *>("ckDesc");
	mcbSort		= m_pwidget->findChild<QComboBox *>("cbSort");
	mpbBest		= m_pwidget->findChild<QPushButton *>("pbBest");
	mpbSearch	= m_pwidget->findChild<QPushButton *>("pbSearch");
	mpbRefresh 	= m_pwidget->findChild<QPushButton *>("pbRefresh");
	mpbResult  	= m_pwidget->findChild<QPushButton *>("pbResult");
	mlbToday	= m_pwidget->findChild<QLabel *>("lbToday");
	mdtStart	= m_pwidget->findChild<QDateTimeEdit *>("dtStart");
	mdtEnd		= m_pwidget->findChild<QDateTimeEdit *>("dtEnd");
	mleStart 	= m_pwidget->findChild<QLineEdit *>("leStart");
	mleEnd  	= m_pwidget->findChild<QLineEdit *>("leEnd");
	mcbModify	= m_pwidget->findChild<QComboBox *>("cbModify");
	msbShotNo 	= m_pwidget->findChild<QSpinBox *>("sbShot");
	mleSesNo  	= m_pwidget->findChild<QLineEdit *>("leSesNo");
	mleSestitle  	= m_pwidget->findChild<QLineEdit *>("leSestitle");
	mleSesleader  	= m_pwidget->findChild<QLineEdit *>("leSesleader");
	mleSescmo  	= m_pwidget->findChild<QLineEdit *>("leSescmo");
	mckDesc		->setChecked(true);

	mdtStart->hide();
	mdtEnd->hide();
	unsigned int ccs_shotno = 0;
	unsigned int mds_shotno = 0;

	//int status = GetUpdateShotNum( ccs_shotno, mds_shotno);
	//msbShotNo->setValue(ccs_shotno);

	Query query = m_con.query();
	ResUse res;
	string strquery = "select sesnum,sestitle,sesleader,sescmo from SESSIONT where sesnum=CURDATE()";
	query << strquery;
	query.parse();
	res = query.use();
	Row row;
	string sesStartDate;
	while (row = res.fetch_row())
	{
		QDateTime sesdate = QDateTime::fromString(row["sesnum"].get_string().c_str(),"yyyy-MM-dd hh:mm:ss");
		mleSesNo->setText(sesdate.date().toString("yyyy-MM-dd"));
		mleSestitle->setText(row["sestitle"].get_string().c_str());
		mleSesleader->setText(row["sesleader"].get_string().c_str());
		mleSescmo->setText(row["sescmo"].get_string().c_str());
	};

	QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	mlbToday->setText(datetime);
	//QFont font("Helvetica", 16, QFont::Bold);
	QFont font("Helvetica", 14);
	QStringList labels;
	//labels <<tr("Type")<< tr("Shot") << "Time" << tr("I_tf\n(kA)")<< tr("Ip\n(kA)") << tr("Ne\n(e19/m^2)") << tr("Te\n(eV)")
	//<< tr("Pressure\n(mbar)") << tr("ECH_P\n(kW)")<<tr("Ip_Pulse\n(msec)")<<tr("Remark")<<tr("Date");
	labels <<tr("Type")<< tr("Shot") << "Time" << tr("I_tf\n(kA)")<< tr("Ip\n(kA)") <<tr("Pulse\n(Sec)")<<
	tr("Ne\n(e19/m^-2)") << tr("Te\n(keV)") << tr("Pressure\n(mBar)") << tr("ECH_P\n(kW)")<<tr("ICRF_P\n(kW)")<<tr("NBI_P\n(kW)")<<tr("ECCD_P\n(kW)")<<tr("LHCD_P\n(kW)")<<tr("Remark")<<tr("Date");
	m_ptable->setColumnCount(labels.size());
	m_ptable->setHorizontalHeaderLabels(labels);
#if 1
	for(int i = 0; i < labels.size(); i++)
	{
		switch(i)
		{
			//Type
			case 0:
				m_ptable->horizontalHeader()->resizeSection(i, 45);
				break;
			//Shot
			case 1:
				m_ptable->horizontalHeader()->resizeSection(i, 60);
				break;
			//Time
			case 2:
				m_ptable->horizontalHeader()->resizeSection(i, 85);
				break;
			//I_tf
			case 3:
				m_ptable->horizontalHeader()->resizeSection(i, 50);
				break;
			//Ip
			case 4:
				m_ptable->horizontalHeader()->resizeSection(i, 60);
				break;
			//Pulse
			case 5:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//Ne
			case 6:
				m_ptable->horizontalHeader()->resizeSection(i, 70);
				break;
			//Te
			case 7:
				m_ptable->horizontalHeader()->resizeSection(i, 70);
				break;
			//Pressure
			case 8:
				m_ptable->horizontalHeader()->resizeSection(i, 80);
				break;
			//ECH_P
			case 9:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//ICRF_P
			case 10:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//NBI_P
			case 11:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//ECCD_P
			case 12:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//LHCD_P
			case 13:
				m_ptable->horizontalHeader()->resizeSection(i, 65);
				break;
			//Remark
			case 14:
				m_ptable->horizontalHeader()->resizeSection(i, 430);
				break;
			//Date
			case 15:
				m_ptable->horizontalHeader()->resizeSection(i, 110);
				break;
		};
#if 0
		if ( i == labels.size() - 2) 
		{
			//m_ptable->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
			m_ptable->horizontalHeader()->resizeSection(i, 480);
			continue;
		}
		m_ptable->horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
	}
#endif
	m_ptable->verticalHeader()->show();
	//m_ptable->setFixedHeight(20);
	m_ptable->setShowGrid(true);
	m_ptable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ptable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ptable->setFont(font);
	font.setPointSize(12);
	font.setBold(true);
	m_ptable->horizontalHeader()->setFont(font);
	m_ptable->horizontalHeader()->setFixedHeight(54);

	QDateTime startdatetime = QDateTime::currentDateTime();
	mdtStart->setDateTime(startdatetime);
	mdtEnd->setDateTime(startdatetime);
}

void SessionSummary::showBestIp()
{
	//BestShotList bestshotlist(this);
	BestShotList *bestshotlist = new BestShotList;
	bestshotlist -> setParent(this);
}

void SessionSummary::showResult()
{
	if(mpShotResult == NULL)
	{
		mpShotResult = new ShotResult();
		mpShotResult->setParent(this);
	}
	else
	{
		if(mpShotResult->isShow()==false) mpShotResult->Show();
	}
}

void SessionSummary::dbRecordDelete()
{
	int currow = m_ptable->currentRow();
	QTableWidgetItem *pItem = m_ptable->item(currow, 1);
	QString itemText = pItem -> text();

	//msgBox.setText("Are you sure to delete the record ?");
	if(messageBox("Are you sure to delete the record ?",APPLYCANCELBOX)==MSGNOK) return;

	try {
		qDebug("%s", itemText.toStdString().c_str());
		m_ptable->removeRow(currow);
		Query query2 = m_con.query();
		query2 << "delete from SHOTSUMMARY where shotnum=%0";
		query2.parse();
		sql_bigint shotno =itemText.toInt();
		query2.use(shotno);
		//why don't work?
		//query2 << "delete from SHOTSUMMARY where shotnum=%0:shotnum";
		//query2.def["shotnum"]=shotno;
		//query2.execute();
	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}
void SessionSummary::dbRecordModify()
{
#if 1
	int currow = m_ptable->currentRow();
	QTableWidgetItem *pItem = m_ptable->item(currow, 1);
	QString itemText = pItem -> text();

	QTableWidgetItem *pItemcomm = 0;
	QString strValue;
	sql_bigint shotno =itemText.toInt();

	try {
		Query query2 = m_con.query();
		//query2 << "update SHOTSUMMARY set comment='comment update' where shotnum=703"; //OK
		switch(mcbModify->currentIndex())
		{
			case 0:	//Remark
				pItemcomm = m_ptable->item(currow, 10);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set comment='%0' where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["comment"]=strValue.toStdString();
				break;
			case 1:	//Type
				pItemcomm = m_ptable->item(currow, 0);
				strValue = pItemcomm->text().toUpper();
				if(strValue.compare("") ==0 || strValue.compare("NO") ==0 || strValue.compare("TS") == 0 || 
					strValue.compare("CS") ==0 || strValue.compare("RS") == 0 || strValue.compare("PS") ==0 )
				{
					query2 << "update SHOTSUMMARY set type='%0' where shotnum=%1:shotnum"; //OK
					query2.parse();
					query2.def["type"]=strValue.toStdString();
				}
				else
				{
					messageBox("Shot type is possible [TS/CS/RS/PS]",OKBOX);
					return;
				}
				break;
			case 2:	//tfcurrent
				pItemcomm = m_ptable->item(currow, 3);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set tfcurrent=%0:tfcurrent where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["tfcurrent"]=(strValue.toDouble()*1000.0);
				break;
			case 3:	//plma current
				pItemcomm = m_ptable->item(currow, 4);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set plsmacurrent=%0:plsmacurrent where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["plsmacurrent"]=(strValue.toDouble()*1000.0);
				break;
			case 4:	//ipwidth;
				pItemcomm = m_ptable->item(currow, 5);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set ipwidth=%0:ipwidth where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["ipwidth"]=strValue.toDouble();
				break;
			case 5:	//edensity
				pItemcomm = m_ptable->item(currow, 6);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set edensity=%0:edensity where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["edensity"]=strValue.toDouble();
				break;
			case 6:	//etemp
				pItemcomm = m_ptable->item(currow, 7);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set etemp=%0:etemp where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["etemp"]=strValue.toDouble();
				break;
			case 7:	//airpressure
				pItemcomm = m_ptable->item(currow, 8);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set airpressure=%0:airpressure where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["airpressure"]=strValue.toDouble();
				break;
			case 8:	//echpow;
				pItemcomm = m_ptable->item(currow, 9);
				strValue = pItemcomm -> text();
				query2 << "update SHOTSUMMARY set echpow=%0:echpow where shotnum=%1:shotnum"; //OK
				query2.parse();
				query2.def["echpow"]=strValue.toDouble();
				break;
			default:
				break;
		};
		query2.def["shotnum"]=shotno;
		cout << "Query: " << query2.preview() << endl;
		mysqlpp::ResNSel sqlres = query2.execute();

		if(sqlres.success)
		{
			qDebug("Update Success:%d", (int)shotno);
		}
		else
		{
			qDebug("Update Failed:%d", (int)shotno);
		}
		//query2.use(stritemcomm.toStdString().c_str(),shotno);

	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
#endif
}

void SessionSummary::executeQuery(string)
{
}

void SessionSummary::dbSearch()
{
	try {
		QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
		if(mlbToday->text().compare(datetime)!=0) mlbToday->setText(datetime);
#if 1
		//m_ptable->clear();
		m_ptable->setRowCount(0);
		string strOrder= (mckDesc->isChecked()==true)?"DESC":"ASC";
		string strPara="";
		//qDebug("curIndex:%d",mcbSort->currentIndex());
		QString qstrStartVal =mleStart->text();
		QString	qstrEndVal = mleEnd->text();
		switch(mcbSort->currentIndex())
		{
			case 0:
				strPara="shotnum";
				break;
			case 1:
				strPara="tfcurrent";
				break;
			case 2:
				strPara="plsmacurrent";
				break;
			case 3:
				strPara="edensity";
				break;
			case 4:
				strPara="etemp";
				break;
			case 5:
				strPara="airpressure";
				break;
			case 6:
				strPara="echpow";
				break;
			case 7:
				strPara="icrhpower";
				break;
			case 8:
				strPara="nbi1power";
				break;
			case 9:
				strPara="eccdpower";
				break;
			case 10:
				strPara="lhcdpower";
				break;
			case 11:
				strPara="ipwidth";
				break;
			case 12:
				strPara="type";
				break;
			case 13:
				strPara="shotdate";
				break;
			default:
				break;
		};

		string strquery=""; 
		Query query2 = m_con.query();
		ResUse res;
		if( mckToday->isChecked() == true)
		{
			strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY where shotdate > CURDATE() order by %0 %1";
			query2 << strquery;
			query2.parse();
			res = query2.use(strPara.c_str(), strOrder.c_str());
		}
		else
		{
			if( strPara.compare("shotdate") == 0 )
			{
				strquery="select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY where UNIX_TIMESTAMP(shotdate) > UNIX_TIMESTAMP('%0') and UNIX_TIMESTAMP(shotdate) <= UNIX_TIMESTAMP('%1') order by shotdate %2";
				query2 << strquery;
				query2.parse();
				QString starttime = mdtStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");
				QString endtime	= mdtEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
				res = query2.use(starttime.toStdString().c_str(), endtime.toStdString().c_str(),strOrder.c_str());
			}
			else
			{
				if(qstrStartVal.isEmpty()==true && qstrEndVal.isEmpty()==true)
				{
					if(mbstart == true )
					{
						strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY order by %0 %1 limit 0,30";
						mbstart = false;
					}
					else
					{
						strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY order by %0 %1";
					};
					query2 << strquery;
					query2.parse();
					res = query2.use(strPara.c_str(), strOrder.c_str());
				}
				else if(qstrStartVal.isEmpty()==true)
				{
					bool check;
					double dendval = qstrEndVal.toDouble(&check);
					if(strPara.compare("tfcurrent")==0 ||strPara.compare("plsmacurrent")==0 )
					{
						dendval *= 1000;
					}
					if (check == false)
					{
						messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY where %0<=%1 order by %0 %2";
					query2 << strquery;
					query2.parse();
					res = query2.use(strPara.c_str(), dendval, strOrder.c_str());
				}
				else if(qstrEndVal.isEmpty()==true)
				{
					bool check;
					double dstartval = qstrStartVal.toDouble(&check);
					if (check == false)
					{
						messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					if(strPara.compare("tfcurrent")==0 ||strPara.compare("plsmacurrent")==0 )
					{
						dstartval *= 1000;
					}
					strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY where %0>=%1 order by %0 %2";
					query2 << strquery;
					query2.parse();
					res = query2.use(strPara.c_str(), dstartval, strOrder.c_str());
				}
				else
				{
					bool check;
					double dstartval = qstrStartVal.toDouble(&check);
					if (check == false)
					{
						messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					double dendval = qstrEndVal.toDouble(&check);
					if (check == false)
					{
						messageBox("Data Conversion Error!!",OKBOX);
						return;
					}
					if(strPara.compare("tfcurrent")==0 ||strPara.compare("plsmacurrent")==0 )
					{
						dstartval *= 1000;
						dendval *= 1000;
					};
					strquery= "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,icrhpower,nbi1power,eccdpower,lhcdpower,ipwidth,comment,type from SHOTSUMMARY where %0>=%1 and %0<=%2 order by %0 %3";
					query2 << strquery;
					query2.parse();
					res = query2.use(strPara.c_str(), dstartval, dendval, strOrder.c_str());
				}
			};
		};
		Row row;
		QTableWidgetItem *pItem = 0;
		Qt::GlobalColor qtgColor = Qt::black;
		QBrush brush;
		//int num =0;
		while (row = res.fetch_row())
		{
			//cout <<endl<< "shotnum:[" << row["shotnum"]<<"] \nshotdate:[" << row["shotdate"] << "] \ntfcur:["<<row["tfcurrent"]
			//<<"] \nplsmacurrent:[" <<row["plsmacurrent"] <<"] \nedensity:[" <<row["edensity"] <<"] \nairpressure:[" <<row["airpressure"]
			//<<"] \nechpow:[" <<row["echpow"] <<"]" << endl;
			//QString qstrnum		= QString("%1").arg(num);
			string strshottype		= row["type"].get_string();
			string strshotnum		= row["shotnum"].get_string();
			string strshotdate		= row["shotdate"].get_string();;
			string strcomment		= row["comment"].get_string();;
			//
			QDateTime datetime = QDateTime::fromString(strshotdate.c_str(),"yyyy-MM-dd hh:mm:ss");
			QString date = datetime.date().toString("yyyy-MM-dd");
			QString time = datetime.time().toString();
			double dtfcurrent		= row["tfcurrent"]/1000.0;
			double dplmacurrent		= row["plsmacurrent"]/1000.0;
			double dedensity		= row["edensity"];
			double detemp			= row["etemp"];
			double dairp			= row["airpressure"];
			double dechp			= row["echpow"];
			double dicrfp			= row["icrhpower"];
			double dnbip			= row["nbi1power"];
			dnbip *= 1000;

			double deccdp			= row["eccdpower"];
			double dlhcdp			= row["lhcdpower"];

			//double dipwidth			= row["ipwidth"]*1000.0;	// till 2010, mSec
			double dipwidth			= row["ipwidth"];	//2011, Sec

			QString qstrtfcur	= QString("%1").arg(dtfcurrent, 0, 'f', 1);
			QString qstrairp	= QString("%1").arg(dairp, 0, 'e', 1);
			//QString qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 1);
			QString qstrplmacur, qstreden, qstretemp, qstrechp, qstricrfp, qstrnbip,qstreccdp,qstrlhcdp, qstripwidth;
			if (strshottype.compare("PS")==0)
			{
				qtgColor = Qt::black;
				if(dedensity<0.1) {
					qstreden	= "--";
					qstretemp	= "--";
					if( dplmacurrent<10.0)
					{
						qstrplmacur = "--";
						qstripwidth	= "--";
					}
					else
					{
						qstrplmacur = QString("%1").arg(dplmacurrent, 0, 'f', 1);
						qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 2);
					};
				} else {
					qstrplmacur = QString("%1").arg(dplmacurrent, 0, 'f', 1);
					qstreden	= QString("%1").arg(dedensity, 0, 'f', 1);
					if(detemp == 0.0)
						qstretemp	= "--";
					else
						qstretemp	= QString("%1").arg(detemp, 0, 'f', 1);

					qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 2);
				};
				qstricrfp	= QString("%1").arg(dicrfp, 0, 'f', 1);

				// 2011 Temporary
#if 1
				qstrechp	= QString("%1").arg(dechp, 0, 'f', 1);
				qstrnbip	= QString("%1").arg(dnbip, 0, 'f', 1);
				qstreccdp	= QString("%1").arg(deccdp, 0, 'f', 1);
				qstrlhcdp	= QString("%1").arg(dlhcdp, 0, 'f', 1);
#else
				qstrechp	= "--";
				qstrnbip	= "--";
				qstreccdp	= "--";
				qstrlhcdp	= "--";
#endif
			}
			else
			{
				qstreden	= "--";
				qstretemp	= "--";
				qstrechp	= "--";
				qstricrfp	= "--";
				qstrnbip	= "--";
				qstreccdp	= "--";
				qstrlhcdp	= "--";
				qstrplmacur = "--";
				qstripwidth = "--";
				qtgColor = Qt::darkMagenta;
			};

			//m_ptable->setItem(row, 3, new QTableWidgetItem(strplmacurrent.c_str()));
			int row = m_ptable->rowCount();
			m_ptable->insertRow(row);
			m_ptable->setRowHeight(row,25);

			//pItem = new QTableWidgetItem(qstrnum);
			pItem = new QTableWidgetItem(strshottype.c_str());
			brush  = pItem->foreground();
			brush.setColor(qtgColor);
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 0, pItem);

#if 1
			pItem = new QTableWidgetItem(strshotnum.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 1, pItem);

			pItem = new QTableWidgetItem(time);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 2, pItem);

			pItem = new QTableWidgetItem(qstrtfcur);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 3, pItem);

			pItem = new QTableWidgetItem(qstrplmacur);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 4, pItem);

			//p-width
			pItem = new QTableWidgetItem(qstripwidth);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 5, pItem);

			pItem = new QTableWidgetItem(qstreden);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 6, pItem);

			pItem = new QTableWidgetItem(qstretemp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 7, pItem);

			pItem = new QTableWidgetItem(qstrairp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 8,pItem); 

			pItem = new QTableWidgetItem(qstrechp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 9, pItem);

			pItem = new QTableWidgetItem(qstricrfp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 10, pItem);

			pItem = new QTableWidgetItem(qstrnbip);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 11, pItem);

			pItem = new QTableWidgetItem(qstreccdp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 12, pItem);

			pItem = new QTableWidgetItem(qstrlhcdp);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 13, pItem);

			pItem = new QTableWidgetItem(strcomment.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 14, pItem);

			pItem = new QTableWidgetItem(date);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			pItem->setForeground(brush); 
			m_ptable->setItem(row, 15, pItem);
#endif
			//num++;
		};

		m_ptable->update();
#endif

	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}

void SessionSummary::registerSignal()
{
#if 1
	QPushButton *pbDelete	= m_pwidget->findChild<QPushButton *>("pbDelete");
	QPushButton *pbModify	= m_pwidget->findChild<QPushButton *>("pbModify");
	QPushButton	*pbMakeSes	= m_pwidget->findChild<QPushButton *>("pbMakeSes");
	QPushButton	*pbSavePDF	= m_pwidget->findChild<QPushButton *>("pbSavePDF");
	QPushButton	*pbPrint	= m_pwidget->findChild<QPushButton *>("pbPrint");
	QPushButton	*pbSesSave	= m_pwidget->findChild<QPushButton *>("pbSesSave");

	QPushButton	*pbClose	= m_pwidget->findChild<QPushButton *>("pbClose");
	connect(pbClose, SIGNAL(clicked()), qApp, SLOT(quit()));

	connect(mpbBest, SIGNAL(clicked()), this, SLOT(showBestIp()));
	connect(mpbSearch, SIGNAL(clicked()), this, SLOT(dbSearch()));
	connect(pbDelete, SIGNAL(clicked()), this, SLOT(dbRecordDelete()));
	connect(pbModify, SIGNAL(clicked()), this, SLOT(dbRecordModify()));
	connect(pbMakeSes, SIGNAL(clicked()), this, SLOT(makeSession()));
	connect(mcbSort, SIGNAL(currentIndexChanged(int)),this,SLOT(cbChanged(int)));
	connect(mpbRefresh, SIGNAL(clicked()), this, SLOT(lastshotRefresh()));
	connect(mpbResult, SIGNAL(clicked()), this, SLOT(showResult()));
	connect(pbSavePDF, SIGNAL(clicked()), this, SLOT(exportPDF()));
	connect(pbSesSave, SIGNAL(clicked()), this, SLOT(saveSES()));
	connect(pbPrint, SIGNAL(clicked()), this, SLOT(print()));
	connect(this, SIGNAL(Search()), this, SLOT(dbSearch()));
	connect(pbSesSave, SIGNAL(clicked()), this, SLOT(saveSES()));
	connect(mleSesNo, SIGNAL(returnPressed()), this, SLOT(readSES()));

	//Table changed save to mysql
	connect(m_ptable, SIGNAL(cellPressed(int, int)), this, SLOT(saveToMySQL(int,int)));
	m_ptable	= m_pwidget->findChild<QTableWidget*>("tableWidget");
#endif
}

// Not Complete.
void SessionSummary::saveToMySQL(int, int)
{
	QList<QTableWidgetItem *> list = m_ptable->selectedItems();
	QTableWidgetItem *pItem = 0;
	mPreval.clear();
	for(int i= 0; i < list.size();i++)
	{
		pItem = list.at(i);
		mPreval.push_back(pItem->text());
		qDebug("%s",pItem->text().toStdString().c_str());
	};
}

void SessionSummary::timerEvent(QTimerEvent *event)
{
	int update = updateTeData();
	if(update == 0) killTimer(event->timerId());
}

int SessionSummary::updateTeData()
{
	MDSPlusData *mdsplusdata = new MDSPlusData;
	int ece_center=mdsplusdata->getECE_Center(mshotno);
	if(ece_center < 0) 
	{
		//delete mdsplusdata;
		return -1;
	};

	qDebug("Update Te Data ShotNumber:%d, ECE_CENTER:%d", mshotno, ece_center);

	char strECECENT[3];
	sprintf(strECECENT, "%02d",ece_center); 
	strECECENT[3]='\0';

	string strECECenter = string("ECE")+string(strECECENT);
	qDebug("Update ECE_NAME ---> %s", strECECenter.c_str());

	bool check;
	mdsplusdata->setMDSDataInfo("electron",strECECenter,mshotno,check);
	if(check==MDSPlusData::FAIL)
	{
		//delete mdsplusdata;
		return -1;
	};

	int size = mdsplusdata->getNodeSize();
	double *timebase = new double[size];
	double *shotdata = new double[size];
	double *destdata = new double[size];

	if(mdsplusdata->getShotData(timebase, shotdata, size)<0)
	{
		delete timebase;
		delete shotdata;
		delete destdata;
		delete mdsplusdata;
		return -1;
	};

	unsigned int count = (size<100000)?0:100000; //2012

	smooth(destdata, shotdata, count, size);

	double tedata = *max_element(&destdata[count], &destdata[size])/1000;
	printf("After Smooth Max of [%s]:%f\n",strECECenter.c_str(), tedata);

	delete timebase;
	delete shotdata;
	delete destdata;

	qDebug("Update TF Data ShotNumber:%d", mshotno);

	string strTF = "PCITFMSRD";
	mdsplusdata->setMDSDataInfo("pcs_kstar",strTF,mshotno,check);
	if(check==MDSPlusData::FAIL)
	{
		delete mdsplusdata;
		return -1;
	};

	size = mdsplusdata->getNodeSize();

	timebase = new double[size];
	shotdata = new double[size];

	if(mdsplusdata->getShotData(timebase, shotdata, size)<0)
	{
		delete timebase;
		delete shotdata;
		delete mdsplusdata;
		return -1;
	};

	double tfdata = *max_element(&shotdata[0], &shotdata[size]);
	printf("Update TF Max of [%s]:%f\n",strTF.c_str(), tfdata);

	try
	{
		Query query = m_con.query();
		query << "update SHOTSUMMARY set etemp=%0:etemp, tfcurrent=%1:tfcurrent where shotnum=%2:shotnum"; //OK
		query.parse();
		query.def["etemp"]=tedata;
		query.def["tfcurrent"]=tfdata;
		query.def["shotnum"]=mshotno;

		cout << "Query: " << query.preview() << endl;
		mysqlpp::ResNSel sqlres = query.execute();

		if(sqlres.success)
			qDebug("Te Data Update Success:%d", (int)mshotno);
		else
			qDebug("Te Data Update Failed:%d", (int)mshotno);
	} catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
	};

	delete timebase;
	delete shotdata;
	delete mdsplusdata;

	emit Search();
	return 0;
}

void SessionSummary::ReadMDSData(int shotno, bool update)
{
#if 1
	MDSPlusData *mdsplusdata = new MDSPlusData;
	if(update == true && mshotno != 0) updateAirPressure(mdsplusdata);
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	double *destdata = 0;
	int noisecount = 0;
	int normalcount = 0;
	vector<ShotInfo> vec_shotInfo;
	ShotInfo shotinfo;
	double mxVal = 0;
	double nb11_pnb = 0.0, nb12_pnb = 0.0;
	//string strPara[]={"ITFMSRD", "PCRC03", "NE_INTER01", "B_PKR251B","ECE07", "ECH_VFWD1"};		//2008
	//string strPara[]={"PCITFMSRD", "PCRC03", "NE_INTER01", "B_PKR251A","ECE07", "ECH_VFWD1:FOO"};	//2009
	//string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE07","ECH_VFWD1:FOO","ICRF_FWD","NBI1_VG1:FOO","NBI1_IG1:FOO"};	//2010
	//string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE07","ECH_VFWD1:FOO","ICRF_FWD","NB1_PB1"};	//2010
	//string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE","ECH_VFWD1:FOO","ICRF_FWD","NB11_IG1"};	//2012
	//string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE","ECH_VFWD1:FOO","ICRF_FWD","NB11_IG1", "EC1_RFFWD1", "LH1_PLH"};	//2012
	//string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE","ECH_VFWD1","ICRF_FWD","NB11_IG1", "EC1_RFFWD1", "LH1_PLH"};	//2012
	string strPara[]={"PCITFMSRD","PCRC03","NE_INTER01","B_PKR251A","ECE","ECH_VFWD1","ICRF_FWD","NB11_PNB","NB12_PNB", "EC1_RFFWD1", "LH1_PLH"};	//2012
	int ece_center=mdsplusdata->getECE_Center(shotno);

	qDebug("ShotNumber:%d, ECE_CENTER:%d", shotno, ece_center);
	char strECECENT[3];
	sprintf(strECECENT, "%02d",ece_center); 
	strECECENT[3]='\0';
	bool check;
	vector<double> dtimebase;
	vector<double> dtimebase100k;
	//int count = 0;
	bool bstop = false;
	double preval = 0.0;
	for (int i = 0;i<11;i++)
	{
		switch(i)
		{
			case 0:
			case 1:
				mdsplusdata->setMDSDataInfo("pcs_kstar",strPara[i],shotno,check);
				break;
			case 2:
				mdsplusdata->setMDSDataInfo("processed",strPara[i],shotno,check);
				break;
			case 3:
				mdsplusdata->setMDSDataInfo("operation",strPara[i],shotno,check);
				break;
			case 4:
				strPara[i] = strPara[i]+string(strECECENT);
				qDebug("ECE_NAME ---> %s", strPara[i].c_str());
				mdsplusdata->setMDSDataInfo("electron",strPara[i],shotno,check);
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				mdsplusdata->setMDSDataInfo("heating",strPara[i],shotno,check);
				break;
		};
		if(check==MDSPlusData::FAIL) 
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			continue;
		};

		size = mdsplusdata->getNodeSize();
		timebase = new double[size];
		shotdata = new double[size];
		destdata = new double[size];
		if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			delete timebase;
			delete shotdata;
			delete destdata;
			continue;
		};

#if 0
		if(strPara[i].compare("PCITFMSRD")==0)
		{
			//In 2011, TF Current Caculation, for over 32KA
			//reason: RFM assined data type with short(2bytes) for TF Current.
			//if > 32KA, signed bit on, to solve this problem, TF Current devide by 2 in PCS.
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			mxVal *= 2;
		}
		else if(strPara[i].compare("PCRC03")==0)
#endif
		if(strPara[i].compare("PCRC03")==0)
		{
			for_each(&shotdata[0], &shotdata[size], MultipleValue<double>(-1.0));
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			//double mnVal =*min_element(&shotdata[0], &shotdata[size]);
			//if ( mxVal<fabs(mnVal) ) mxVal=mnVal;
			qDebug("Ip-Size:%d",size);
			//unsigned int count = (size<14901+500)?0:14901+500; //2010
			//unsigned int count = (size<140000+500)?0:140000+500; //2011

			//2012, 0 sec count: 15801, 15 sec count: 165802
			unsigned int count = (size<100000+500)?0:15801; //2012

			int maxsize = 0;
			if(count == 0 ) maxsize = size;
			else maxsize = 165802;
			//else maxsize = 150001;

#if 0
			ofstream outfile("./Ip_Test.txt");
			for(int i = 0; i < size; i++)
			{
				qDebug("%i  X:%f  Y:%f, Count:%d", i, timebase[i], shotdata[i],count);

				outfile <<"count: " <<i<<" X: "<<timebase[i]<<" Y: "<<shotdata[i]<<endl;
			};
			outfile.close();
			return;
#endif

#if 1
			// check 
			double dshotval = mxVal;
			if( mxVal < 0 )
			{
				//for(int i = count; i<size; i++)
				//for(int i = count; i<(count+130000); i++)
				for(int i = count; i<(count+maxsize); i++)
				{
					dshotval = fabs(shotdata[i]);
					//if(dshotval>=100.0)//0.1kA
					if(dshotval>=100000.0)//100kA	2011
					{
						if ( bstop == true ) break;
						//qDebug("%i  X:%f  Y:%f, Count:%d\n", i, timebase[i]+11, shotdata[i],count);
						//dtimebase.push_back(timebase[i]+11.0);
						dtimebase.push_back(timebase[i]);
						preval = dshotval;
						if(dshotval>=100000.0)//100kA
						{
							//dtimebase100k.push_back(timebase[i]+11.0);
							dtimebase100k.push_back(timebase[i]);
						};
					}
					//if (preval>=100.0 && shotdata[i]<0.0 && i>46000)
#if 0
					if (preval>=100.0 && dshotval<0.0)
					{
						if(count == 0) bstop = true;
						else if (i>46000) bstop = true;
					};
#endif
				};
			}
			else
			{
#if 0
				int timecount = 0;
				for(int i = 0; i < size;i++)
				{
					if(timebase[i] < 0 ) timecount++;
				};
				qDebug("TimeBase == 0 -> Count:%d", timecount);
#endif
				//for(int i = 0; i<size; i++)
				//for(int i = 61051; i<81051; i++)
				//for(int i = count; i<(count+130000); i++) // 2011
				for(int i = count; i<(count+maxsize); i++)
				{
#if 0
					//if(timebase[i]==0 )
					if(i < (count+100))
						qDebug("%i timebase[0]:%f, X:%f Y:%f, Count:%d", i,timebase[0], timebase[i], shotdata[i],count);
#endif
					dshotval = shotdata[i];
					//if(dshotval>=100.0)	//0.1kA
					if(dshotval>=100000.0)//100kA 2011
					{
						if ( bstop == true ) break;
						//qDebug("%i  X:%f  Y:%f, Count:%d\n", i, timebase[i]+11, shotdata[i],count);
						//dtimebase.push_back(timebase[i]+9.0);
						dtimebase.push_back(timebase[i]); //2010
						//preval = dshotval;
						//if (dshotval > 1000.0) normalcount++;
						if(dshotval>=100000.0)//100kA
						{
							//dtimebase100k.push_back(timebase[i]);
							//dtimebase100k.push_back(timebase[i]+9.0);
							dtimebase100k.push_back(timebase[i]); //2010
						};
					}
					else if(dshotval < 0)
					{
						if ( normalcount != 0 ) break;
						noisecount++;
					};
					//if (preval>=100.0 && shotdata[i]<0.0 && i>46000)
#if 0
					if (preval>=100.0 && dshotval<0.0)
					{
						if(count == 0) bstop = true;
						else if (i>46000) bstop = true;
					};
#endif
				};
			};
#endif
		}
#if 0
		//2008-Gyrotron
		else if( strPara[i].compare("ECH_VFWD1")==0)
		{
			//ECH_P
			// T1_ECH ~ 60msec average
			double davg = accumulate(&shotdata[5101],&shotdata[5700],0.0)/600;
			//0.113*1000000/1000);
			printf("ECH_AVG of count(60ms):%f,ECH_P(kW):%f\n",davg, davg*113);
			//mxVal= davg*0.113*1000;
			mxVal= davg*113;//0.113*1000000/1000);
		}
		//2009-Gyrotron
		else if( strPara[i].compare("ECH_VFWD1:FOO")==0)
		{
			//ECH_P
			double rmax = *max_element(&shotdata[0], &shotdata[size]);
			mxVal= rmax*62.5;
			printf("RMax:%f,ECH(kW):%f\n",rmax, mxVal);
		}
#endif
		//2010-Gyrotron
		//else if( strPara[i].compare("ECH_VFWD1:FOO")==0)
		//2012
		else if( strPara[i].compare("ECH_VFWD1")==0)
		{
			//ECH_P
#if 0
			// 2011, After Calibration, will be fixed.
			double rmax = *max_element(&shotdata[0], &shotdata[size]);
			mxVal= (rmax*98)+(39*rmax*rmax);
			printf("2011 - ECHPower: RMax:%f,ECH(kW):%f, Caculation:%s\n",rmax, mxVal,"98x+39x^2");
			//mxVal= 0.0;
#endif
			//2012
			double rmax = *max_element(&shotdata[0], &shotdata[size]);
			mxVal = ((2*rmax*rmax) + (86.6*rmax - 11.7))/1000;
			printf("2012 - ECHPower: RMax:%f,ECH(kW):%f, Caculation:%s\n",rmax, mxVal,"(2*x^2)+ 86.6*x-11.7");
		}
		//else if( strPara[i].compare("NB11_IG1")==0)	//2011
		else if( strPara[i].compare("NB11_PNB")==0)	//2011
		{
#if 0
			// 2011, After Calibration, will be fixed.
			//mxVal= 0.0;
			double rmax = *max_element(&shotdata[0], &shotdata[size]);
			float C = 0.;
			int Vg1 = CACommand("NBIFactor");
			string calc;
			switch(Vg1)
			{
				case 70:
					//Vg1 = 70; C = 0.51;
					C = 0.51;
					calc = "Vg1=70, C=0.51";
					break;
				case 80:
					//Vg1 = 80; C = 0.47;
					C = 0.47;
					calc = "Vg1=80, C=0.47";
					break;
				case 90:
					//Vg1 = 90; C = 0.43;
					C = 0.43;
					calc = "Vg1=90, C=0.43";
					break;
				case 95:
				default:
					//Vg1 = 95; C = 0.41;
					C = 0.41;
					calc = "Vg1=95, C=0.41";
					break;
			};
			mxVal= (rmax*Vg1*C)/1000;
			printf("2011 - NBIPower: RMax:%f,NBI(kW):%f, Caculation:NB11_IG1*(%s)\n",rmax, mxVal,calc.c_str());
#endif
			//2012
			//mxVal= mdsplusdata->getNBIPower(shotno);
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			nb11_pnb = mxVal;
		}
		else if( strPara[i].compare("NB12_PNB")==0)	//2011
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			nb12_pnb = mxVal;
		}
		else if( strPara[i].compare("EC1_RFFWD1")==0)	//2012
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
#if 0 
			ofstream outfile("/home/kstar/EC1_Test.txt");
			for(int i = 0; i < size; i++)
			{
				qDebug("%i  X:%f  Y:%f", i, timebase[i], shotdata[i]);
				outfile <<"Count:" <<i<<" X: "<<timebase[i]<<" Y: "<<shotdata[i]<<endl;
			};
			outfile.close();
#endif
		}
		else if( strPara[i].compare(0,3,"ECE")==0)	//2012
		{
#if 0 
			ofstream outfile("/home/kstar/ECE_ORG.txt");
			for(int i = 0; i < size; i++)
			{
				qDebug("%i  X:%f  Y:%f", i, timebase[i], shotdata[i]);
				outfile <<"Count:" <<i<<" X: "<<timebase[i]<<" Y: "<<shotdata[i]<<endl;
			};
			outfile.close();
#endif
			//2012, 0 sec count: 100000, 15 sec count: 165802
			unsigned int count = (size<200000)?0:200000; //2012

			mxVal = *max_element(&shotdata[count], &shotdata[size]);
			printf("Max of [%s]:%f\n",strPara[i].c_str(), mxVal);

			smooth(destdata, shotdata, count, size);

			mxVal = *max_element(&destdata[count], &destdata[size]);
			printf("After Smooth Max of [%s]:%f\n",strPara[i].c_str(), mxVal);

#if 0
			ofstream outfile1("/home/kstar/ECE_SMOOTH.txt");
			for(int i = 0; i < size; i++)
			{
				qDebug("%i  X:%f  Y:%f", i, timebase[i], shotdata[i]);
				outfile1 <<"Count:" <<i<<" X: "<<timebase[i]<<" Y: "<<shotdata[i]<<endl;
			};
			outfile1.close();
#endif
		}
		else if( strPara[i].compare("LH1_PLH")==0)	//2012
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			mxVal *= 0.93;
		}
		else if( strPara[i].compare("ICRF_FWD")==0)	//2012
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			mxVal *= 1000;
		}
		else
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
		};

		if(isnan(mxVal) == true) mxVal = 0.0;
		printf("Max of [%s]:%f\n",strPara[i].c_str(), mxVal);

		shotinfo.paraname = strPara[i];
		shotinfo.paramaxval = mxVal;
		vec_shotInfo.push_back(shotinfo);
		delete timebase;
		delete shotdata;
		delete destdata;
	};
	string strTime = mdsplusdata->getShotTime();
	printf("Time:%s\n", strTime.c_str());
	char strBuf[strTime.size()];
	strcpy(strBuf,strTime.c_str());
	char *pch = strtok(strBuf, " /:");
	int year = 0, mon = 0, day =0, hour=0, min=0, sec=0;
	int idx = 0;
	while(pch!=0)
	{
		switch(idx)
		{
			case 0:
				year = atoi(pch);
				break;
			case 1:
				mon = atoi(pch);
				break;
			case 2:
				day = atoi(pch);
				break;
			case 3:
				hour = atoi(pch);
				break;
			case 4:
				min = atoi(pch);
				break;
			case 5:
				sec = atoi(pch);
				break;
		};
		if(!(pch = strtok(NULL, " /:"))) continue;
		idx++;
	};

	vector<ShotInfo>::iterator veciter;
	double dMxval[vec_shotInfo.size()];
	int index =0;
	for(veciter=vec_shotInfo.begin(), index=0;veciter!=vec_shotInfo.end(); ++veciter,index++)
	{
		dMxval[index]=veciter->paramaxval;
		printf("%s,%f\n", veciter->paraname.c_str(), veciter->paramaxval);
	};

	double mxTime = 0.0;
	double mnTime = 0.0;
	if(dtimebase.empty() == false) 
	{
		mxTime = *max_element(dtimebase.begin(), dtimebase.end());
		mnTime = *min_element(dtimebase.begin(), dtimebase.end());
	};

	double timeWidth = 0.0;
	printf("NoiseCount:%d\n", noisecount);
	timeWidth = mxTime-mnTime;
	//if( timeWidth > 10) timeWidth = 0.0;

	double mxTime100k = 0.0;
	double mnTime100k = 0.0;
	if(dtimebase100k.empty() == false) 
	{
		mxTime100k = *max_element(dtimebase100k.begin(), dtimebase100k.end());
		mnTime100k = *min_element(dtimebase100k.begin(), dtimebase100k.end());
	};
	double timeWidth100k = mxTime100k-mnTime100k;
	qDebug("Ip-Pulsewidth >0.1K:maxTime:%f, minTime:%f, timewidth(sec):%f",mxTime, mnTime,timeWidth);
	qDebug("Ip-Pulsewidth >100K:maxTime100k:%f, minTime100k:%f, timewidth100k(sec):%f",mxTime100k, mnTime100k,timeWidth100k);
	qDebug("Y:%d,M:%d,D:%d,H:%d,m:%d,s:%d,ShotTime:%s",year,mon,day,hour,min,sec,strTime.c_str());
	qDebug("ICRF-Power:%f,NB11_PNB:%f, NB12_PNB:%f, ECCD-Power:%f, LHCD-Power",dMxval[6], dMxval[7], dMxval[8],dMxval[9],dMxval[10]);
	double nbi_power = nb11_pnb + nb12_pnb;
	qDebug("NBI-Power(NB11:%f+NB12:%f):%f",dMxval[7], dMxval[8],nbi_power);

	//DB Insert
	try
	{
		//Data re-calculation.
		mysqlpp::sql_datetime shotdate;
		shotdate.year = year;
		shotdate.month=mon;
		shotdate.day=day;
		shotdate.hour=hour;
		shotdate.minute=min;
		shotdate.second=sec;

		SetShotType(CACommand("ShotType"));
		mysqlpp::Query query = m_con.query();
		//SHOTSUMMARY shotsummaryinsert(shotno,shotdate,dMxval[0],dMxval[1],dMxval[2],dMxval[4],dMxval[3],dMxval[5],timeWidth,timeWidth100k,"","PS");
		string strShotType = "TS";
		if(mshottype == 0) strShotType = "PS";
		else if(mshottype == 1) strShotType = "TS";
		else if(mshottype == 2) strShotType = "CS";
		else if(mshottype == 3) strShotType = "RS";
		else if(mshottype == 4) strShotType = "PS";
		//SHOTSUMMARY shotsummaryinsert(shotno,shotdate,dMxval[0],dMxval[1],dMxval[2],0.0,dMxval[3],dMxval[5],timeWidth,timeWidth100k,"", strShotType, "2010C03");
		//SHOTSUMMARY shotsummaryinsert(shotno,shotdate,dMxval[0],dMxval[1],dMxval[2],dMxval[4]/1000,dMxval[3],dMxval[5],dMxval[6],dMxval[7],dMxval[8],dMxval[9],timeWidth,timeWidth100k,"", strShotType, "2012C03");
		SHOTSUMMARY shotsummaryinsert(shotno,shotdate,dMxval[0],dMxval[1],dMxval[2],dMxval[4]/1000,dMxval[3],dMxval[5],dMxval[6],nbi_power,dMxval[9],dMxval[10],timeWidth,timeWidth100k,"", strShotType, "2012C03");
		query.insert(shotsummaryinsert);
		query.execute();

		double ipCurrent = dMxval[1]/1000.0;
		double pulseWidth = timeWidth*1000.0; 

		qDebug("DBValue-PulseWidth:%f\n", pulseWidth);

		char cmdBuff[24];
		if(strShotType == "PS")
		{
			sprintf(cmdBuff, "caput ICS_SHOTINFO_IP %f", ipCurrent);
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_DT %f", pulseWidth);
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECHPOWER %f", dMxval[5]);
			system(cmdBuff);

			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECCDPOWER %f", dMxval[8]);
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_LHCDPOWER %f", dMxval[9]);
			system(cmdBuff);
		}
		else if(strShotType == "TS")
		{
			sprintf(cmdBuff, "caput ICS_SHOTINFO_IP 0");
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_DT 0");
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECHPOWER 0");
			system(cmdBuff);
#if 1
			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECCDPOWER 0");
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_LHCDPOWER 0");
			system(cmdBuff);
#endif
		}
		else
		{
			sprintf(cmdBuff, "caput ICS_SHOTINFO_IP 0");
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_DT 0");
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECHPOWER %f", dMxval[5]);
			system(cmdBuff);
#if 1
			sprintf(cmdBuff, "caput ICS_SHOTINFO_ECCDPOWER %f", dMxval[8]);
			system(cmdBuff);
			sprintf(cmdBuff, "caput ICS_SHOTINFO_LHCDPOWER %f", dMxval[9]);
			system(cmdBuff);
#endif
		};

		//caPut(DBR_DOUBLE, "ICS_SHOTINFO_IP", ipCurrent);
		//caPut(DBR_DOUBLE, "ICS_SHOTINFO_DT", pulseWidth);
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return;
	};

	//if(update==true) emit Search();
	emit Search();
	sendShotResult(shotno);
	delete mdsplusdata;
	if(update == true && mshottype == 0 ) startTimer(10000);
	//QTimer::singleShot(30000, this, SLOT(updateTeData()));
#endif
}

void SessionSummary::makeSession()
{
	int shotno = msbShotNo->value();
	ReadMDSData(shotno, false);
}

void SessionSummary::sendShotResult(unsigned int shotno)
{
	if(!mpShotResult) return;
	if( mpShotResult->GetMode() == true )
		mpShotResult->Update(shotno);
}

void SessionSummary::lastshotRefresh()
{
	int currow = m_ptable->currentRow();
	qDebug("%d", currow);
	if(currow <0 ) return;
	QTableWidgetItem *pItem = m_ptable->item(currow, 1);
	unsigned int shotNum = pItem->text().toInt();
	//refreshShot(shotNum);
}
void SessionSummary::refreshShot(unsigned int shotno)
{
	MDSPlusData *mdsplusdata = new MDSPlusData;
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	vector<ShotInfo> vec_shotInfo;
	ShotInfo shotinfo;
	double mxVal = 0;
	string strPara[]={"PCITFMSRD", "PCRC03", "NE_INTER01", "B_PKR251B","ECE07", "ECH_VFWD1:FOO"};
	vector<double> dtimebase;
	vector<double> dtimebase100k;
	bool bstop = false;
	double preval = 0.0;
	bool check;
	for (int i = 0;i<6;i++)
	{
		switch(i)
		{
			case 0:
			case 1:
				mdsplusdata->setMDSDataInfo("pcs_kstar",strPara[i],shotno,check);
				break;
			case 2:
				mdsplusdata->setMDSDataInfo("process",strPara[i],shotno,check);
				break;
			case 3:
				mdsplusdata->setMDSDataInfo("operation",strPara[i],shotno,check);
				break;
			case 4:
				mdsplusdata->setMDSDataInfo("electron",strPara[i],shotno,check);
				break;
			case 5:
				mdsplusdata->setMDSDataInfo("heating",strPara[i],shotno,check);
				break;
		};
		if(check==MDSPlusData::FAIL) 
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			continue;
		};
		size = mdsplusdata->getNodeSize();
		timebase = new double[size];
		shotdata = new double[size];
		if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			delete timebase;
			delete shotdata;
			continue;
		};

		if ( strPara[i].compare("PCRC03")==0)
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			//double mnVal =*min_element(&shotdata[0], &shotdata[size]);
			//if ( mxVal<fabs(mnVal) ) mxVal=mnVal;
			qDebug("Ip-Size:%d",size);
			//unsigned int count = (size<40000)?0:40001;

			unsigned int count = (size<100000+500)?0:15801; //2012

			int maxsize = 0;
			if(count == 0 ) maxsize = size;
			else maxsize = 150001;
#if 1
			// check 
			double dshotval = 0.0;
			if( mxVal < 0 )
			{
				//for(int i = count; i<size; i++)
				for(int i = count; i<(count+maxsize); i++)
				{
					dshotval = shotdata[i];
					if(dshotval<=-100.0)//0.1kA
					{
						if ( bstop == true ) break;
						//qDebug("%i  X:%f  Y:%f, Count:%d\n", i, timebase[i]+11, shotdata[i],count);
						//dtimebase.push_back(timebase[i]+11.0);
						dtimebase.push_back(timebase[i]); //2010
						preval = dshotval;
						if(dshotval<=-100000.0)//100kA
						{
							//dtimebase100k.push_back(timebase[i]+11.0);
							dtimebase100k.push_back(timebase[i]); //2010
						};
					}
					//if (preval>=100.0 && shotdata[i]<0.0 && i>46000)
					if (preval<=-100.0 && dshotval>0.0)
					{
						if(count == 0) bstop = true;
						else if (i>46000) bstop = true;
					};
				};
			}
			else
			{
				//for(int i = count; i<size; i++)
				for(int i = count; i<(count+maxsize); i++)
				{
					dshotval = shotdata[i];
					if(dshotval>=100.0)//0.1kA
					{
						if ( bstop == true ) break;
						//qDebug("%i  X:%f  Y:%f, Count:%d\n", i, timebase[i]+11, shotdata[i],count);
						//dtimebase.push_back(timebase[i]+11.0);
						dtimebase.push_back(timebase[i]); //2010
						preval = dshotval;
						if(dshotval>=100000.0)//100kA
						{
							//dtimebase100k.push_back(timebase[i]+11.0);
							dtimebase100k.push_back(timebase[i]); //2010
						};
					}
					//if (preval>=100.0 && shotdata[i]<0.0 && i>46000)
					if (preval>=100.0 && dshotval<0.0)
					{
						if(count == 0) bstop = true;
						else if (i>46000) bstop = true;
					};
				};
			};
#endif
		}
#if 0
		//2008-Gyrotron
		else if( strPara[i].compare("ECH_VFWD1:FOO")==0)
		{
			//ECH_P
			// T1_ECH ~ 60msec average
			double davg = accumulate(&shotdata[5101],&shotdata[5700],0.0)/600;
			printf("ECH_AVG of count(60ms):%f,ECH_P(kW):%f\n",davg, davg*0.113*1000);
			mxVal= davg*0.113*1000;//0.113*1000000/1000);
		}
#endif
		//2009-Gyrotron
		else if( strPara[i].compare("ECH_VFWD1:FOO")==0)
		{
			//ECH_P
			double rmax = *max_element(&shotdata[0], &shotdata[size]);
			mxVal= rmax*62.5;
			printf("RMax:%f,ECH(kW):%f\n",rmax, mxVal);
		}
		else
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
		}

		printf("Max of [%s]:%f\n",strPara[i].c_str(), mxVal);
		shotinfo.paraname = strPara[i];
		shotinfo.paramaxval = mxVal;
		vec_shotInfo.push_back(shotinfo);
		delete timebase;
		delete shotdata;
	};

	string strTime = mdsplusdata->getShotTime();
	printf("Time:%s\n", strTime.c_str());
	char strBuf[strTime.size()];
	strcpy(strBuf,strTime.c_str());
	char *pch = strtok(strBuf, " /:");
	int year = 0, mon = 0, day =0, hour=0, min=0, sec=0;
	int idx = 0;
	while(pch!=0)
	{
		switch(idx)
		{
			case 0:
				year = atoi(pch);
				break;
			case 1:
				mon = atoi(pch);
				break;
			case 2:
				day = atoi(pch);
				break;
			case 3:
				hour = atoi(pch);
				break;
			case 4:
				min = atoi(pch);
				break;
			case 5:
				sec = atoi(pch);
				break;
		};
		if(!(pch = strtok(NULL, " /:"))) continue;
		idx++;
	};

	vector<ShotInfo>::iterator veciter;
	double dMxval[vec_shotInfo.size()];
	int index =0;
	for(veciter=vec_shotInfo.begin(), index=0;veciter!=vec_shotInfo.end(); ++veciter,index++)
	{
		dMxval[index]=veciter->paramaxval;
		printf("%s,%f\n", veciter->paraname.c_str(), veciter->paramaxval);
	};
#if 0
		mysqlpp::Query query = con.query();
		query << "select * from stock where item = \"Nürnberger Brats\"";

		// Retrieve the row, throwing an exception if it fails.
		mysqlpp::Result res = query.store();
		if (res.empty()) {
			throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
					"table, run resetdb");
		}
		stock row = res.at(0);
		stock orig_row = row;
		row.item = "Nuerenberger Bratwurst";

		query.update(orig_row, row);
		cout << "Query: " << query.preview() << endl;
		query.execute();
#endif
		mysqlpp::Query query = m_con.query();
		QString qstrQuery = "select * from SHOTSUMMARY where shotnum = " + QString("%1").arg(shotno);
		query << qstrQuery.toStdString();
		cout << "Query*1: " << query.preview() << endl;

		mysqlpp::Result res = query.store();
		if (res.empty()) {
			throw mysqlpp::BadQuery("ShotNum not found in table");
		}
		SHOTSUMMARY row = res.at(0);
		SHOTSUMMARY orig_row = row;
		string strtype = row.type;
		if (strtype.compare("")==0) row.type="PS";

		mysqlpp::sql_datetime shotdate;
		shotdate.year = year;
		shotdate.month=mon;
		shotdate.day=day;
		shotdate.hour=hour;
		shotdate.minute=min;
		shotdate.second=sec;
		row.shotdate=shotdate;
		row.tfcurrent=dMxval[0];
		row.plsmacurrent=dMxval[1];
		row.edensity=dMxval[2];
		row.etemp=dMxval[4];
		row.airpressure=dMxval[3];
		row.echpow=dMxval[5];
		//add
		double mxTime = 0.0;
		double mnTime = 0.0;
		if(dtimebase.empty() == false) 
		{
			mxTime = *max_element(dtimebase.begin(), dtimebase.end());
			mnTime = *min_element(dtimebase.begin(), dtimebase.end());
		};
		double timeWidth = mxTime-mnTime;
		row.ipwidth=timeWidth;
		qDebug("Ip-W>0.1k: maxTime:%f, minTime:%f, timewidth:%f", mxTime, mnTime, timeWidth);

		double mxTime100k = 0.0;
		double mnTime100k = 0.0;
		if(dtimebase100k.empty() == false) 
		{
			mxTime100k = *max_element(dtimebase100k.begin(), dtimebase100k.end());
			mnTime100k = *min_element(dtimebase100k.begin(), dtimebase100k.end());
		};
		double timeWidth100k = mxTime100k-mnTime100k;
		row.ipwidth100k=timeWidth100k;
		qDebug("Ip-W>100k: maxTime100k:%f, minTime100k:%f, timewidth100k:%f", mxTime100k, mnTime100k, timeWidth100k);

		//qDebug("E-Temp:%f",dMxval[4]);
		query.update(orig_row, row);
		cout << "Query*2: " << query.preview() << endl;
		query.execute();
		emit Search();
}
void SessionSummary::cbChanged(int index)
{
	//qDebug("index;%d",index);
#if 1
	switch(index)
	{
		case 13:
			mdtStart->show();
			mdtEnd  ->show();
			mleStart ->hide();
			mleEnd ->hide();
			break;
		default:
			mdtStart->hide();
			mdtEnd  ->hide();
			mleStart ->show();
			mleEnd ->show();
			break;
	};
#endif
}
int SessionSummary::GetUpdateShotNum( unsigned int &ccs_shotno, unsigned int &mds_shotno)
{
	vector<string> pvnames;
	pvnames.push_back("CCS_SHOT_NUMBER");
	pvnames.push_back("MDS_SHOT_NUMBER");

	chid shot_id[pvnames.size()];
	struct dbr_time_long shotdata[pvnames.size()];
	int status=0;
	for(size_t i=0; i<pvnames.size();i++)
	{
		ca_create_channel(pvnames.at(i).c_str(),NULL, NULL,20,&shot_id[i]);
		ca_pend_io(0.5);
	};
	//int state = ca_state(shot_id);
	for(size_t i=0; i<pvnames.size();i++)
	{
		ca_get(DBR_TIME_LONG, shot_id[i], (void *)&shotdata[i]);
		status=ca_pend_io(0.5);
	}
	for(size_t i=0; i<pvnames.size();i++)
	{
		//qDebug("%d",shotdata[i].value);
		switch(i)
		{
			case 0:
				ccs_shotno =shotdata[i].value;
				break;
			case 1:
				mds_shotno =shotdata[i].value;
				break;
		};
	};
	if(status != ECA_NORMAL)
	{
		printf("CCS/MDS SHOT number: denied access\n");
		return -1;
	};
	for(size_t i=0; i<pvnames.size();i++)
	{
		ca_clear_channel(shot_id[i]);
		ca_pend_io(0.5);
	}
	return 0;
}
int SessionSummary::ResetCCSPShotN()
{
	chid shsum_reset_chid;
	//ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",NULL, NULL,20,&shsum_reset_chid);
	int resetval = 0;
	int status = ca_search("CCS_PERFORM_SHOT_SUMMARY", &shsum_reset_chid);
	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(shsum_reset_chid);
		printf("Not Found %s\n", "CCS_PERFORM_SHOT_SUMMARY");
		return -1;
	};
	ca_put(DBR_INT, shsum_reset_chid, &resetval);
	status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		printf("Reset Shot Summary failed: %s denied access\n",ca_name(shsum_reset_chid));
		return -1;
	};
	ca_clear_channel(shsum_reset_chid);
	ca_pend_io(0.5);
	return 0;
}
void SessionSummary::readSES()
{
	Query query = m_con.query();
	query << "select count(*) from SESSIONT where date(sesnum)=date('%0')";
	query.parse();
	ResUse res = query.use(mleSesNo->text().toStdString());
	Row row;
	int count = 0;
	while (row = res.fetch_row())
	{
		count = row["count(*)"];
	};
	//qDebug("count:%d",count);
	query.reset();
	if(count!=0)
	{
		try {
			query << "select sestitle, sesleader, sescmo from SESSIONT where date(sesnum)=date('%0')";
			query.parse();
			res = query.use(mleSesNo->text().toStdString());
			while (row = res.fetch_row())
			{
				mleSestitle->setText(row["sestitle"].get_string().c_str());
				mleSesleader->setText(row["sesleader"].get_string().c_str());
				mleSescmo->setText(row["sescmo"].get_string().c_str());
			};
		} catch (const BadQuery& er) {
			cerr << "Query error: " << er.what() << endl;
		} catch (const Exception& er) {
			cerr << er.what() << endl;
		};
	}
	else
	{
		mleSestitle->setText("");
		mleSesleader->setText("");
		mleSescmo->setText("");
	}
}

void SessionSummary::saveSES()
{
	Query query = m_con.query();
	query << "select count(*) from SESSIONT where date(sesnum)=date('%0')";
	query.parse();
	ResUse res = query.use(mleSesNo->text().toStdString());
	Row row;
	int count = 0;
	while (row = res.fetch_row())
	{
		count = row["count(*)"];
	};
	//qDebug("count:%d",count);
	query.reset();

	QDateTime datetime = QDateTime::fromString(mleSesNo->text(),"yyyy-MM-dd");
	QString date = datetime.date().toString("yyyy-MM-dd");
	if(count!=0)
	{
		try {
			//query2 << "update SESSIONT set sestitle='%1', sesleader='%2', sescmo='%3' where date(sesnum)=date('%0')";
			query << "update SESSIONT set sestitle='%0:title', sesleader='%1:leader', sescmo='%2:cmo' where date(sesnum)=date('%3:sesnum')";
			query.parse();
			query.def["title"]=mleSestitle->text().toStdString();
			query.def["leader"]=mleSesleader->text().toStdString();
			query.def["cmo"]=mleSescmo->text().toStdString();
			query.def["sesnum"]=date.toStdString();
			cout << "Query: " << query.preview() << endl;
			mysqlpp::ResNSel sqlres = query.execute();
		} catch (const BadQuery& er) {
			cerr << "Query error: " << er.what() << endl;
		} catch (const Exception& er) {
			cerr << er.what() << endl;
		};
		//if(sqlres.success) qDebug("Update Success");
		//else qDebug("Update Failed");
	}
	else
	{
		try {
#if 0
			query << "insert into SESSIONT values (%0q, %1q, %2q, %3q, %4q, %5q, %6q)";
			query.parse();
			cout << "Query: " << query.preview() << endl;
			mysqlpp::ResNSel sqlres = query.execute(date.toStdString(), 
					mleSestitle->text().toStdString(), mleSesleader->text().toStdString(),
					mleSescmo->text().toStdString(), "", 0, 0);

#endif
			mysqlpp::sql_datetime sesdate;
			sesdate.year = datetime.date().year();
			sesdate.month = datetime.date().month();;
			sesdate.day= datetime.date().day();
			sesdate.hour=0;
			sesdate.minute=0;
			sesdate.second=0;
			SESSIONT sessiont(0,sesdate,mleSestitle->text().toStdString(),
					mleSesleader->text().toStdString(),mleSescmo->text().toStdString(),"",0,0);
			query.insert(sessiont);
			query.execute();

		} catch (const BadQuery& er) {
			cerr << "Query error: " << er.what() << endl;
		} catch (const Exception& er) {
			cerr << er.what() << endl;
		};
	};
}
void SessionSummary::exportPDF()
{
	QString exportname = QFileDialog::getSaveFileName(this, "Export PDF",
				"", "*.pdf");

	if (exportname.isEmpty()) return;
	if (QFileInfo(exportname).suffix().isEmpty()) exportname.append(".pdf");

	mpdfprint.setOrientation(QPrinter::Landscape);
	mpdfprint.setOutputFormat(QPrinter::PdfFormat);
	mpdfprint.setOutputFileName(exportname);

	QPainter painter(&mpdfprint);
	QRect rect = painter.viewport();
	painter.setViewport(rect.x(), rect.y(), m_ptable->width()-175, m_ptable->height());
	painter.setWindow(m_ptable->rect());
	mpixmap = QPixmap::grabWidget(m_ptable);
	painter.drawPixmap(0, 0, mpixmap);

}
void SessionSummary::print()
{
#if 0
	mprint.setOrientation(QPrinter::Landscape);
	mprint.setOutputFormat(QPrinter::NativeFormat);
	QPainter painter(&mprint);
	QRect rect = painter.viewport();
	QSize size = m_ptable->size()/2;
	size.scale(rect.size(), Qt::KeepAspectRatio);
	painter.setViewport(rect.x(), rect.y(), m_ptable->width()-175, m_ptable->height());
	painter.setWindow(m_ptable->rect());
	mpixmap = QPixmap::grabWidget(m_ptable);
	//mpixmap.scaled(size);
	painter.drawPixmap(0, 0, mpixmap);
#else
	mprint.setOrientation(QPrinter::Landscape);
	mprint.setOutputFormat(QPrinter::NativeFormat);
	QPrintDialog dialog(&mprint, this);
	if (dialog.exec()) {
		QPainter painter(&mprint);
		QRect rect = painter.viewport();
		//QSize size = m_ptable->size()/2;
		//size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), m_ptable->width()-175, m_ptable->height());
		painter.setWindow(m_ptable->rect());
		mpixmap = QPixmap::grabWidget(m_ptable);
		//mpixmap.scaled(size);
		painter.drawPixmap(0, 0, mpixmap);
	};
#endif
}
int SessionSummary::messageBox(QString msg, const int type)
{
	QMessageBox msgBox;
	msgBox.setText(msg);
	if(type == APPLYCANCELBOX)
		msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Cancel);
	else
		msgBox.setStandardButtons(QMessageBox::Ok);

	switch (msgBox.exec()) {
		case QMessageBox::Apply:
		case QMessageBox::Ok:
		default:
			// yes was clicked
			return MSGOK;
		case QMessageBox::Cancel:
			// no was clicked
			return MSGNOK;
	};
}
void SessionSummary::hostHide(QString hostname)
{
	//if(hostname.compare("opi16") == 0) return;
	if(hostname.compare("opi16") == 0 || hostname.compare("mydevopi") == 0) return;

	m_pwidget->findChild<QPushButton *>("pbDelete")->hide();
	m_pwidget->findChild<QPushButton *>("pbModify")->hide();
	m_pwidget->findChild<QPushButton *>("pbMakeSes")->hide();
	m_pwidget->findChild<QPushButton *>("pbSesSave")->hide();
	mcbModify->hide();
	mpbRefresh->hide();
	msbShotNo->hide();
}
void SessionSummary::Update()
{
	emit Search();
}

void SessionSummary::updateAirPressure(MDSPlusData *mdsplusdata)
{
	bool check;
	mdsplusdata->setMDSDataInfo("operation","B_PKR251A",mshotno,check);
	if(check==MDSPlusData::FAIL) return;

	int size = mdsplusdata->getNodeSize();
	double *timebase = new double[size];
	double *shotdata = new double[size];

	if(mdsplusdata->getShotData(timebase, shotdata, size)<0)
	{
		delete timebase;
		delete shotdata;
	};

	double airPVal = *max_element(&shotdata[0], &shotdata[size]);

	Query query = m_con.query();
	query << "update SHOTSUMMARY set airpressure=%0:airpressure where shotnum=%1:shotnum"; //OK
	query.parse();
	query.def["airpressure"]=airPVal;
	query.def["shotnum"]=mshotno;

	cout << "Query: " << query.preview() << endl;
	mysqlpp::ResNSel sqlres = query.execute();

	if(sqlres.success)
	{
		qDebug("Update Success:%d", (int)mshotno);
	}
	else
	{
		qDebug("Update Failed:%d", (int)mshotno);
	}
}
void SessionSummary::smooth(double *dest, double *src, const int count, const int size)
{
	for(int i = count+10; i < size-10;i++)
	{
		dest[i-1]=(src[10]+src[i+9]+src[i+8]+src[i+7]+src[i+6]+src[i+5]+src[i+4]+src[i+3]+src[i+2]+src[i+1]+
				+src[i]+src[i-1]+src[i-2]+src[i-3]+src[i-4]+src[i-5]+src[i-6]+src[i-7]+src[i-8]+src[i-9]+src[i-10])/21;
	}
}
void SessionSummary::closeEvent(QCloseEvent *e)
{
	//mpThr -> stop();
	mpShotResult->close();
}
