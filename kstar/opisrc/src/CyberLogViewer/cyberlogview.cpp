#include <sys/types.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <stdexcept>
#include <bitset>

#include "cyberlog.h"
#include "cyberlogview.h"

#define NBI_CHOICE 4

using namespace mysqlpp;

template <class T> class MultipleValue
{
	public:
		MultipleValue(const T& v):value(v){};
		void operator() (T& elem) const {elem*=value;};
	private:
		T value;
};

static ostream&
separator(ostream& os)
{
	os << endl << "---------------------------" << endl << endl;
	return os;
}

CyberLogView::CyberLogView():m_con(false),mshotno(0),mbAuto(false)
{
	init();
#if 1
	// Data reading
	getUiObject();
	registerSignal();
	//emit Search();
#else
	// Data generation
	makeSession();
#endif
	return;
	qDebug("hostname:%s", mhostname.toStdString().c_str());
}
CyberLogView::~CyberLogView()
{
}

void
CyberLogView::init()
{
	QFile file ("../ui/CyberLogViewer.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(CyberLogViewer.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->show();
	file.close();

	mbstart = true;
	char hostname[32];
	gethostname(hostname, sizeof(hostname));
	mhostname = hostname;

	string dbname   = "cyberlog";
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

void CyberLogView::getUiObject()
{
	m_ptable	= m_pwidget->findChild<QTableWidget*>("tableWidget");
	mleTestID  	= m_pwidget->findChild<QLineEdit *>("leTestID");
	mleTestTime = m_pwidget->findChild<QLineEdit *>("leTestTime");
	mleDesc  	= m_pwidget->findChild<QLineEdit *>("leDesc");
	m_psummary 	= m_pwidget->findChild<QTableWidget*>("summaryTable");

	QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	QFont font("Helvetica", 10);
	QStringList labels;

	QString sdndata;
	
//	for(int i = 1; i <= 110; i++)
		//sdndata=QString("data\n(%1)").arg(i*2,3,10,QChar('0'));
	for(int i = 1; i <= 220; i++)
	{
		sdndata=QString("data\n(%1)").arg(i,3,10,QChar('0'));
		//qDebug("%s",sdndata.toStdString().c_str());
		labels << sdndata;
	};
	m_ptable->setColumnCount(labels.size());
	m_ptable->setHorizontalHeaderLabels(labels);

#if 1
	for(int i = 0; i < labels.size(); i++)
	{
		m_ptable->horizontalHeader()->resizeSection(i, 45);
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
	m_ptable->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_ptable->setFont(font);
	font.setPointSize(10);
	//font.setBold(true);
	m_ptable->horizontalHeader()->setFont(font);
	m_ptable->horizontalHeader()->setFixedHeight(54);

	labels.clear();
	labels << "Network ID" << "Source" << "SDN Index" << "Expect R-Count" <<"Real R-Count" <<"Abnormal Saving Count" << "Periodic Saving Count";
	m_psummary->setColumnCount(labels.size());
	m_psummary->setHorizontalHeaderLabels(labels);

	for(int i = 0; i < labels.size(); i++)
	{
		m_psummary->horizontalHeader()->resizeSection(i, 150);
	}
}

void CyberLogView::showLoglist()
{
	QFile file ("../ui/Loglist.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(Loglist.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_ploglist = m_loader.load(&file);
	m_ploglist->show();
	file.close();
	
	QListWidget *pList = m_ploglist->findChild<QListWidget *>("lwLoglist");
	QPushButton	*pbSelect	= m_ploglist->findChild<QPushButton *>("pbSelect");

	connect(pbSelect, SIGNAL(clicked()), this, SLOT(LogSelected()));

	//DB Query
	Query query = m_con.query();
	Result res;

	//query << "show tables like 'CYBER%'";
	//query << "select TEST_ID,PROJECT_NAME,TEST_START_TIME,TEST_END_TIME,TEST_DESC,SDN_INDEX,NETWORK_ID,SOURCE,PRE_COMM_CNT,REAL_COMM_CNT,MISS_COMM_CNT,PERIODIC_SAVE_CNT from SDN_DATA_DB";
	// deleted filed NETWORK_ID,SOURCE,PRE_COMM_CNT,REAL_COMM_CNT,MISS_COMM_CNT,PERIODIC_SAVE_CNT;
	query << "select TEST_ID,PROJECT_NAME,TEST_START_TIME,TEST_END_TIME,TEST_DESC from SDN_DATA_DB";
	cout << "Query: " << query.preview() << endl;
	res = query.store();
	cout << "Tables found: " << res.size();

	QStringList tablelist;
	Row row;
	cout.setf(ios::left);
	Result::iterator i;
	string tab = "\t";
	string source;
	for (i = res.begin(); i != res.end(); ++i) 
	{
		row = *i;
		string strlist = string(row.at(0).c_str())+tab+string(row.at(1).c_str())+tab+string(row.at(2).c_str())+tab+string(row.at(4).c_str());
#if 0
		//deleted fields
		networkid = row.at(6);
		source = string(row.at(7).c_str());
		precommcnt = row.at(8);
		realcommcnt = row.at(9);
		misscommcnt = row.at(10);
		periodicsavecnt = row.at(11);
#endif
		tablelist << strlist.c_str();
		cout << endl << '\t' << setw(17) << row.at(0);
	};
	cout << separator;

	pList -> addItems(tablelist);
};

void CyberLogView::LogSelected()
{
	QListWidget *pList = m_ploglist->findChild<QListWidget *>("lwLoglist");
	QListWidgetItem *pItem = pList -> item(pList->currentRow());

	if(!pItem) return;

	string tablename,projectname,testtime,testdesc;
	string tablelist = pItem->text().toStdString();
	string delimiter = "\t";
	size_t pos = 0;
	string token;
	int index = 0;
	while((pos = tablelist.find(delimiter)) != string::npos)
	{
		token = tablelist.substr(0,pos);
		tablelist.erase(0, pos+delimiter.length());
		switch(index)
		{
			case 0: tablename = token;
				break;
			case 1: projectname = token;
				break;
			case 2: testtime = token;
				break;
		};
		qDebug("index:%d", index);
		index++;
	};
	//Last token;
	testdesc = tablelist;
	qDebug("Tablename:%s, prname:%s, testtime:%s, testdesc:%s", tablename.c_str(),projectname.c_str(),testtime.c_str(),testdesc.c_str());

	try {
		Query query = m_con.query();
		ResUse res;
		string strquery = "select SAVE_TIME,SDN_DATA from " + tablename;
		query << strquery;
		query.parse();
		res = query.use();
		Row row;
		string sesStartDate;
		QTableWidgetItem *pItem = 0;
		Qt::GlobalColor qtgColor = Qt::black;
		QBrush brush;
		string bytestr;
		char tempstr[3] = {'\0'};
		QTableWidgetItem *pVeritem = 0;
		while (row = res.fetch_row())
		{
			string savetime = row["SAVE_TIME"].get_string();
			string sdndata = row["SDN_DATA"].get_string();
			size_t size = sdndata.size();
			int row = m_ptable->rowCount();
			m_ptable->insertRow(row);
			m_ptable->setRowHeight(row,20);
			qDebug("\nsavetime(%s):sdndata(%d)-%s\n",savetime.c_str(),size, sdndata.c_str());

			pVeritem = new QTableWidgetItem(savetime.c_str());
			m_ptable->setVerticalHeaderItem(row,pVeritem);
			
			bytestr = string2hex(sdndata,uppercase);
			qDebug("%s", bytestr.c_str());

			for(size_t i = 0; i < size; i++)
			{
				tempstr[0] = bytestr.at(i*2);
				tempstr[1] = bytestr.at(i*2+1);
				tempstr[2] = '\0';
				pItem = new QTableWidgetItem(tempstr);
				pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				m_ptable->setItem(row, i, pItem);
			};
		};
		m_ptable->update();

	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};

	QLineEdit	*leTestID	= m_pwidget->findChild<QLineEdit *>("leTestID");
	QLineEdit	*lePrname	= m_pwidget->findChild<QLineEdit *>("leProjectName");
	QTextEdit	*teDesc	= m_pwidget->findChild<QTextEdit *>("teDesc");
	QLabel	*lbTesttime	= m_pwidget->findChild<QLabel *>("lbTestTime");

	leTestID->setText(tablename.c_str());
	lePrname->setText(projectname.c_str());
	teDesc->setText(testdesc.c_str());
	lbTesttime->setText(testtime.c_str());

	try {
		Query query = m_con.query();
		query << "select NETWORK_ID,SOURCE,SDN_INDEX,PRE_COMM_CNT,REAL_COMM_CNT,MISS_MATCH_CNT,PERIODIC_SAVE_CNT from SUMMARY where TEST_ID='"+tablename+"'";
		cout << "Query: " << query.preview() << endl;
		Result res = query.store();
		QBrush brush;
		Result::iterator i;
		int sdnindex = 0, networkid=0, precommcnt=0, realcommcnt = 0, misscommcnt=0, periodicsavecnt=0;
		string source;
		Row row;
		QTableWidgetItem *pItem = 0;
		for (i = res.begin(); i != res.end(); ++i) 
		{
			row = *i;
			networkid = row.at(0);
			source = row.at(1).c_str();
			sdnindex = row.at(2);
			precommcnt = row.at(3);
			realcommcnt = row.at(4);
			misscommcnt = row.at(5);
			periodicsavecnt = row.at(6);
			qDebug("%d,%s,%d,%d,%d,%d,%d", networkid,source.c_str(),sdnindex,precommcnt,realcommcnt,misscommcnt,periodicsavecnt);

			int rowcount = m_psummary->rowCount();
			m_psummary->insertRow(rowcount);
			m_psummary->setRowHeight(rowcount,20);

			//for(int i = 0; i < row.size();i++);
			QString strval;
			for(int i = 0; i < 7;i++)
			{
				switch(i)
				{
					case 0:
						strval = QString("%1").arg(networkid);
						break;
					case 1:
						strval = QString("%1").arg(source.c_str());
						break;
					case 2:
						strval = QString("%1").arg(sdnindex);
						break;
					case 3:
						strval = QString("%1").arg(precommcnt);
						break;
					case 4:
						strval = QString("%1").arg(realcommcnt);
						break;
					case 5:
						strval = QString("%1").arg(misscommcnt);
						break;
					case 6:
						strval = QString("%1").arg(periodicsavecnt);
						break;
				}
				pItem = new QTableWidgetItem(strval);
				pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				m_psummary->setItem(rowcount, i, pItem);
			}
		};
		m_psummary->update();
	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};

}

void CyberLogView::showDetail()
{
	qDebug("Show Detail Window");
	QFile file ("../ui/Logdetail.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(Logdetail.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_plogdetail = m_loader.load(&file);
	QwtPlot *mpPlot = m_plogdetail->findChild<QwtPlot *>("qwtPlot");
	QTableWidget *pDTable	= m_plogdetail->findChild<QTableWidget*>("detailTable");
	QStringList labels;
	labels << "Bit" << "Name" << "Value";
	pDTable->setColumnCount(labels.size());
	pDTable->setHorizontalHeaderLabels(labels);

	for(int i = 0; i < labels.size(); i++)
	{
		pDTable->horizontalHeader()->resizeSection(i, 50);
	};


	QwtArray<double> xData; //data count;
	QwtArray<double> yData; //data
	QTableWidgetItem *pItem = 0;
	string bitstr = bitstring(0xffabffff);

	for(size_t i = 0; i < bitstr.size(); ++i)
	{
		int row = pDTable -> rowCount();
		pDTable->insertRow(row);
		QString strval;
		xData.push_back(double(i));
		for(int j = 0; j < 3; j++)
		{
			switch(j)
			{
				case 0:
					strval = QString("Bit%1").arg(i,2,10,QChar('0'));
					break;
				case 1:
					strval = QString("CH_%1").arg(i,2,10,QChar('0'));
					break;
				case 2:
					strval = bitstr.at(i);
					yData.push_back(strval.toDouble());
					break;
			};
			pItem = new QTableWidgetItem(strval);
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			pDTable->setItem(row, j, pItem);
		};
	};
	pDTable->update();

	qDebug("QwtPlot(%p)", mpPlot);
	mpPlot->setAxisScale(QwtPlot::xBottom, 0, 33);
	mpPlot->setAxisScale(QwtPlot::yLeft, -0.1, 1.5);
	QwtPlotCurve *pCurve = new QwtPlotCurve();
	pCurve->setStyle(QwtPlotCurve::Steps);
	pCurve->attach(mpPlot);

	QwtPlotGrid *pGrid = new QwtPlotGrid;
	pGrid->enableXMin(true);
	pGrid->enableYMin(true);
	pGrid->setMajPen(QPen(Qt::gray, 0, Qt::DashLine));
	pGrid->setMinPen(QPen(Qt::gray , 0, Qt::DotLine));
	pGrid->attach(mpPlot);


	QPen pen;
	pen.setWidth(1);
	pen.setColor(QColor("blue"));
	pCurve->setPen(pen);

	QwtSymbol symbol;
	symbol.setStyle(QwtSymbol::Ellipse);
	symbol.setPen(QColor(Qt::black));
	symbol.setSize(5);
	symbol.setBrush(QColor(Qt::blue));

	pCurve->setSymbol(symbol);
	pCurve->setData(xData, yData);

	mpPlot->setCanvasBackground(QColor(Qt::white));
	mpPlot->replot();


	m_plogdetail->show();
	file.close();
};
void CyberLogView::dbSearch()
{
	try {
		printf("DbSearch---->\n");
		Query query = m_con.query();
		ResUse res;
		string strquery = "select sdndata from cyberlogview";
		query << strquery;
		query.parse();
		res = query.use();
		Row row;
		string sesStartDate;
		QTableWidgetItem *pItem = 0;
		Qt::GlobalColor qtgColor = Qt::black;
		QBrush brush;
#if 0
		char bytestr[3] = {'\0'};
#else
		string bytestr;
		char tempstr[3] = {'\0'};
#endif
		while (row = res.fetch_row())
		{
			string sdndata = row["sdndata"].get_string();
			size_t size = sdndata.size();
			int row = m_ptable->rowCount();
			m_ptable->insertRow(row);
			m_ptable->setRowHeight(row,20);
			qDebug("Row(%d),sdndata(%d)-%s\n",row,size, sdndata.c_str());
			
#if 0
			for(size_t i = 0; i < size/2; i++)
			{
				qDebug("%c%c", sdndata.at(i*2), sdndata.at(i*2+1));
				bytestr[0] = sdndata.at(i*2);
				bytestr[1] = sdndata.at(i*2+1);
				bytestr[2] = '\0';
				//bytestr = sdndata.at(i*2) + sdndata.at(i*2+1);
				pItem = new QTableWidgetItem(bytestr);
				//brush  = pItem->foreground();
				//brush.setColor(qtgColor);
				//pItem->setForeground(brush); 
				pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				m_ptable->setItem(row, i, pItem);
			};
#else
			bytestr = string2hex(sdndata,uppercase);
			qDebug("%s", bytestr.c_str());

			for(size_t i = 0; i < size; i++)
			{
				tempstr[0] = bytestr.at(i*2);
				tempstr[1] = bytestr.at(i*2+1);
				tempstr[2] = '\0';
				pItem = new QTableWidgetItem(tempstr);
				pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				m_ptable->setItem(row, i, pItem);
			};
#endif
		};
		m_ptable->update();
		qDebug("DbSearch<----");

	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}

void CyberLogView::registerSignal()
{
#if 1
	QPushButton	*pbClose	= m_pwidget->findChild<QPushButton *>("pbClose");
	connect(pbClose, SIGNAL(clicked()), qApp, SLOT(quit()));

	QPushButton	*pbDetail	= m_pwidget->findChild<QPushButton *>("pbDetail");
	connect(pbDetail, SIGNAL(clicked()), this, SLOT(showDetail()));

	QPushButton	*pbLoglist	= m_pwidget->findChild<QPushButton *>("pbLoglist");
	connect(pbLoglist, SIGNAL(clicked()), this, SLOT(showLoglist()));

	connect(this, SIGNAL(Search()), this, SLOT(dbSearch()));
	return;

	//Table changed save to mysql
	connect(m_ptable, SIGNAL(cellPressed(int, int)), this, SLOT(saveToMySQL(int,int)));
	m_ptable	= m_pwidget->findChild<QTableWidget*>("tableWidget");
#endif
}

// Not Complete.
void CyberLogView::saveToMySQL(int, int)
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

void CyberLogView::timerEvent(QTimerEvent *event)
{
	//if(update == 0) killTimer(event->timerId());
}


void CyberLogView::ReadCyberLogData()
{
#if 1
	//DB Insert
	try
	{
		//Data re-calculation.
		mysqlpp::Query query = m_con.query();

#if 0
		for(int i = 3; i < 40; i++)
		{
			cyberlogview cyberlog(i,i,"2014-02-2-26 00:00:00",
				"FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA11871187FF00AABBDDFF00AA11DD2200AABBCCFF35AADD2200AABBCCFF35AAFFDD2200AABBCCFF35AAFF");
			query.insert(cyberlog);
			query.execute();
		};
		mysqlpp::sql_timestamp, SAVE_TIME,
		mysqlpp::sql_int, NETWORK_ID,
		mysqlpp::sql_char, SOURCE,
		mysqlpp::sql_char, DESTINATION,
		mysqlpp::sql_int, SDN_INDEX,
		mysqlpp::sql_char, SDN_DATA)

		mysqlpp::sql_int, sdnindex,
		mysqlpp::sql_timestamp, stime,
		mysqlpp::sql_int, netid,
		mysqlpp::sql_char, src,
		mysqlpp::sql_char, dest,
		mysqlpp::sql_char, sdndata)

#endif

		CYBER_TEST_0001 cybertest;
		cybertest.SDN_INDEX = 1;
		cybertest.SAVE_TIME = mysqlpp::sql_timestamp();
		cybertest.NETWORK_ID = 1;
		cybertest.SOURCE = "SRC";
		cybertest.DESTINATION = "DESTINATION";
		cybertest.SDN_DATA = "FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA11871187FF00AABBDDFF00AA11DD2200AABBCCFF35AADD2200AABBCCFF35AAFFDD2200AABBCCFF35AAFF";
		query.insert(cybertest);
		query.execute();

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
#endif
}

void CyberLogView::makeSession()
{
	ReadCyberLogData();
}

int CyberLogView::messageBox(QString msg, const int type)
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

void CyberLogView::Update()
{
	emit Search();
}

void CyberLogView::closeEvent(QCloseEvent * e)
{
	qApp->quit();
}

string CyberLogView::string2hex(const string& inputstr, bool upper_case)
{
	ostringstream ret;
	for(size_t i = 0; i < inputstr.length();++i)
	{
		ret << hex << setfill('0') << setw(2) << (upper_case ? uppercase : nouppercase) << (int)inputstr[i];
	}
	return ret.str();
}

string CyberLogView::string2hex(const string& inputstr)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = inputstr.length();
	string outputstr;
	outputstr.reserve(2*len);

	for(size_t i = 0; i < len; i++)
	{
		const unsigned char c = inputstr[i];
		outputstr.push_back(lut[c>>4]);
		outputstr.push_back(lut[c&15]);
	};

	return outputstr;
}

string CyberLogView::hex_to_string(const string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	if (len & 1) throw invalid_argument("odd length");

	string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = input[i];
		const char* p = lower_bound(lut, lut + 16, a);
		if (*p != a) throw invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = lower_bound(lut, lut + 16, b);
		if (*q != b) throw invalid_argument("not a hex digit");

		output.push_back(((p - lut) << 4) | (q - lut));
	}
	return output;
}
string CyberLogView::bitstring(unsigned int bytedata)
{
	bitset<32> bit4byte(bytedata);

	return bit4byte.to_string();
}
