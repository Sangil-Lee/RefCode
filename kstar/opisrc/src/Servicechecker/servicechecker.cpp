#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "servicechecker.h"

using namespace xmlrpc_c;

Servicechecker::Servicechecker()
{
	init();
	getUiObject();
	registerSignal();
}

void
Servicechecker::init()
{
	QFile file ("/usr/local/opi/ui/servicechecker.ui");
	if ( !file.exists() )
	{
		qDebug("Ui File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->show();
	file.close();
	strhostlist.push_back("opi01"); strhostlist.push_back("opi02"); strhostlist.push_back("opi03");
	strhostlist.push_back("opi04"); strhostlist.push_back("opi05"); strhostlist.push_back("opi06");
	strhostlist.push_back("opi07"); strhostlist.push_back("opi08"); strhostlist.push_back("opi09");
	strhostlist.push_back("opi10"); strhostlist.push_back("opi11"); strhostlist.push_back("opi12");
	strhostlist.push_back("opi13"); strhostlist.push_back("opi14"); strhostlist.push_back("opi15");
	strhostlist.push_back("opi17"); strhostlist.push_back("opi18"); strhostlist.push_back("opi19");
	strhostlist.push_back("da"); strhostlist.push_back("mdsplus");
	strhostlist.push_back("backup"); strhostlist.push_back("web");
	strhostlist.push_back("ca"); 

	bchk = true;

}

void Servicechecker::getUiObject()
{
	mallcbBox= m_pwidget->findChildren<QComboBox *>();
	QTabWidget *ptab = m_pwidget->findChild<QTabWidget*>("tabWidget");
	m_ptable = m_pwidget->findChild<QTableWidget*>("tableWidget");
	mleresult = m_pwidget->findChild<QLineEdit*>("leresult");
	mleprechktime = m_pwidget->findChild<QLineEdit*>("leprechktime");
	mlecurchktime  = m_pwidget->findChild<QLineEdit*>("lecurchktime");
	//qDebug("%d, %d", mallcbBox[0]->count(), mallcbBox[1]->count());
	//qDebug("%d, %d", getCBObject("comboBox_2")->count(), getCBObject("comboBox")->count());
}
void Servicechecker::registerSignal()
{
	QPushButton *ppush = m_pwidget->findChild<QPushButton *>("pushButton");
	if (ppush!=0) connect(ppush, SIGNAL(clicked()), this, SLOT(checkHost())); 

	QPushButton *pstart0 = m_pwidget->findChild<QPushButton *>("startChannel");
	QPushButton *pstart1 = m_pwidget->findChild<QPushButton *>("startMdsplus");
	QPushButton *pstart2 = m_pwidget->findChild<QPushButton *>("startData");
	QPushButton *pstart3 = m_pwidget->findChild<QPushButton *>("startBackup");
	QPushButton *pstart4 = m_pwidget->findChild<QPushButton *>("startWeb");
	QPushButton *pstart5 = m_pwidget->findChild<QPushButton *>("startOPI");

	connect(pstart0, SIGNAL(clicked()), this, SLOT(startService()));
	connect(pstart1, SIGNAL(clicked()), this, SLOT(startService()));
	connect(pstart2, SIGNAL(clicked()), this, SLOT(startService()));
	connect(pstart3, SIGNAL(clicked()), this, SLOT(startService()));
	connect(pstart4, SIGNAL(clicked()), this, SLOT(startService()));
	connect(pstart5, SIGNAL(clicked()), this, SLOT(startService()));
}

void Servicechecker::startService()
{
	QPushButton *ppush = qobject_cast<QPushButton *>(sender());

	QString objname = ppush->objectName();
	if ( objname.compare("startChannel") == 0 ) {
		startChArchive();
	}
	else if ( objname.compare("startMdsplus") == 0 ){
		startMdsplus();
	}
	else if ( objname.compare("startData") == 0 ){
		startData();
	}
	else if ( objname.compare("startBackup") == 0 ){
		startBackup();
	}
	else if ( objname.compare("startWeb") == 0 ){
		startWeb();
	}
	else if ( objname.compare("startOPI") == 0 ){
		startOPIs();
	}
}

void 
Servicechecker::startChArchive()
{
	QComboBox* pCombo = getCBObject("cbChannel");
	if (pCombo == 0) return;

	int index = pCombo->currentIndex();

	string svcname;
	switch(index)
	{
		case 0:
			svcname="tmsengine";
			break;
		case 1:
			svcname="vmsengine";
			break;
		case 2:
			svcname="clsengine";
			break;
		case 3:
			svcname="hrsengine";
			break;
		case 4:
			svcname="hdsengine";
			break;
		case 5:
			svcname="qdsengine";
			break;
		case 6:
			svcname="icrfengine";
			break;
		case 7:
			svcname="echengine";
			break;
		case 8:
			svcname="fuelengine";
			break;
		case 9:
			svcname="ccsengine";
			break;
		case 10:
			svcname="mpsengine";
			break;
		case 11:
			svcname="ntpd";
			break;
		case 12:
			svcname="sysMonitor";
			break;
		default:
			break;
	}

	//qDebug("svcname:%s", svcname.c_str());
	remoteStartCmd("ca", svcname, "");
}
void 
Servicechecker::startMdsplus()
{
	QComboBox* pCombo = getCBObject("cbMdsplus");
	if (pCombo == 0) return;

	QString svcname = pCombo->currentText();
	remoteStartCmd("mdsplus", svcname.toStdString(), "");

}
void 
Servicechecker::startData()
{
	QComboBox* pCombo = getCBObject("cbData");
	if (pCombo == 0) return;
	QString svcname = pCombo->currentText();
	remoteStartCmd("da", svcname.toStdString(), "");
}
void 
Servicechecker::startBackup()
{
	QComboBox* pCombo = getCBObject("cbBackup");
	if (pCombo == 0) return;
	QString svcname = pCombo->currentText();
	remoteStartCmd("backup", svcname.toStdString(), "");
}
void 
Servicechecker::startWeb()
{
	QComboBox* pCombo = getCBObject("cbWeb");
	if (pCombo == 0) return;
	QString svcname = pCombo->currentText();
	remoteStartCmd("203.230.119.122", svcname.toStdString(), "");
}
void 
Servicechecker::startOPIs()
{
	QComboBox* pcbOPI = getCBObject("cbOPI");
	QComboBox* pcbSvc = getCBObject("cbOPISvc");
	if (pcbOPI == 0||pcbSvc == 0) return;

	QString opiname = pcbOPI->currentText();
	QString svcname = pcbSvc->currentText();
	qDebug("%s", opiname.toStdString().c_str());
	qDebug("%s", svcname.toStdString().c_str());
	remoteStartCmd(opiname.toStdString(), svcname.toStdString(), "");

}
void Servicechecker::remoteStartCmd(const string &svrname, const string &svcname, const string &arglist)
{
	const string strProtocol = "http://";
	const string strport = ":8081/RPC2";
	string serverUrl;

	serverUrl = strProtocol+svrname+strport;

	clientSimple clientlist;
	value rpcresultlist;
	const string methodName = string("localCmd.") + string("remotestart");
	mleresult->setText("");

	try {
		clientlist.call(serverUrl, methodName, "ss", &rpcresultlist, svcname.c_str(), arglist.c_str());
		string msg = value_string(rpcresultlist);

		mleresult->setText(msg.c_str());
	} catch (girerr::error const error) {
		cerr << "Client threw error: " << error.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	};
}

void Servicechecker::checkHost()
{
	QPushButton *ppush = qobject_cast<QPushButton *>(sender());
	QComboBox *pCombo = getCBObject("comboBox");
	QString hostname = pCombo->currentText();
	if(pCombo == 0 || hostname.compare("NONE")==0 ) return;

	m_ptable->setRowCount(0); 

	qDebug("HostName:%s", hostname.toStdString().c_str());

	vector<string> vec_serverUrl;
	const string strProtocol = "http://";
	const string strport = ":8081/RPC2";
	string serverUrl;
	if ( hostname.compare("all" ) == 0 )
	{
		for (size_t i = 0; i < strhostlist.size();i++)
		{
			serverUrl = strProtocol+strhostlist.at(i)+strport;
			vec_serverUrl.push_back(serverUrl);
		};
	}
	else
	{
		serverUrl = strProtocol+hostname.toStdString()+strport;
		vec_serverUrl.push_back(serverUrl);
	}

	clientSimple clientlist[vec_serverUrl.size()];
	value rpcresultlist[vec_serverUrl.size()];
	const string methodName = string("localCmd.") + string("servicechecker");

	for ( size_t i = 0; i < vec_serverUrl.size();i++)
	{
		try 
		{
			clientlist[i].call(vec_serverUrl.at(i), methodName, "", &rpcresultlist[i]);
			string msg = value_string(rpcresultlist[i]);
			parserMsg(msg);
		}
		catch (girerr::error const error) 
		{
			cerr <<strhostlist.at(i)<<":Client threw error: " <<error.what() << endl;
		}
		catch (...) {
			cerr << "Client threw unexpected error." << endl;
		};
	}; 

	QString strdatetime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
	if(bchk == true )
	{
		bchk = false;
		mleprechktime->setText(strdatetime);
	}
	else
	{
		mleprechktime->setText(mlecurchktime->text());
	};
	mlecurchktime->setText(strdatetime);
}

void Servicechecker::parserMsg(string msg)
{
	int svccnt = count(msg.begin(), msg.end(),'\n');
	//qDebug("svc count:%d", svccnt);

	char strBuf[msg.size()];
	strcpy(strBuf, msg.c_str());
	int index = 0;
	int row= m_ptable->rowCount(); 
	int col=0;

	char *token;
	token = strtok(strBuf, "\n");

	for(int i = 0; i<svccnt; i++)
	{
		int j = m_ptable->rowCount(); 
		m_ptable->insertRow(j); 
	};
	QTableWidgetItem *pItem = 0;
	while(token !=NULL )
	{
		if(index == 0) {
			//qDebug("0:%s", token);
			m_ptable->setItem(row, col, new QTableWidgetItem(token));
		} else if(index != 0 && index%2 == 1 ) {
			//qDebug("Service:%s", token);
			m_ptable->setItem(row, col+1, new QTableWidgetItem(token));
		} else if(index != 0 && index%2 == 0 ) {
			//qDebug("ProcessCnt:%s", token);
			m_ptable->setItem(row, col+1, new QTableWidgetItem(token));
			int proccnt = atoi(token);
			//pItem=m_ptable->item(row, col+2);
			pItem = new QTableWidgetItem(token);
			switch(proccnt)
			{
				case 0:
					pItem->setText("Stop");
					pItem->setForeground(QBrush(QColor("red")));
					break;
				default:
					pItem->setText("Run");
					pItem->setForeground(QBrush(QColor("blue")));
					break;
			};
			m_ptable->setItem(row, col+2, pItem);
		};

		token = strtok(NULL, ";\n");
		if(index%2 == 0) {
			col=0;
			row++;
		} else {
			col++;
		};
		index++;
	};
#if 0
	int row = pvlist_tw->rowCount();
	pvlist_tw->insertRow(row);
	pvlist_tw->setItem(row, 0, new QTableWidgetItem(strpvindex.c_str()));
	pvlist_tw->setItem(row, 1, new QTableWidgetItem(strpvname.c_str()));
	pvlist_tw->setItem(row, 2, new QTableWidgetItem(""));
	count++;
#endif
}

QComboBox* Servicechecker::getCBObject(QString objname)
{
#if 0
	QList<QComboBox*>::iterator cblistiter;

	for(cblistiter=mallcbBox.begin(); cblistiter!=mallcbBox.end(); ++cblistiter)
	{
		//QString ob=(static_cast<QComboBox*>cblistiter)->objectName();
		//if( objname.compare(cblistiter->objectName()) == 0) break;
	}
#else
	QComboBox *pCombo = 0;
	for( int i = 0; i < mallcbBox.size(); i++)
	{
		pCombo = mallcbBox.value(i);
		if (objname.compare(pCombo->objectName()) == 0) break;
		//qDebug("%s",pCombo->objectName().toStdString().c_str());
	}
#endif
	return pCombo;
}
