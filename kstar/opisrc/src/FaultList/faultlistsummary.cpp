#include <sys/types.h>
#include <signal.h>
#include <iostream>
#include "faultlistsummary.h"
#include "sqlsfaultlist.h"

using namespace mysqlpp;

FaultListSummary::FaultListSummary():m_con(false),maddline(0),mbaddmode(false)
{
	init();
	getUiObject();
	registerSignal();
	m_ptable->hideColumn(NUMBER);
	emit Search();
	m_pwidget->showMaximized();
}
FaultListSummary::~FaultListSummary()
{
}

void FaultListSummary::init()
{
	QFile file ("/usr/local/opi/ui/faultlistsummary.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(faultlistsummary.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	file.close();

	string dbname   = "kstarweb";
	//string dbserverhost = "203.230.119.122";
	string dbserverhost = "172.17.100.204";
	//string dbserverhost = "172.17.100.101";
	string user     = "root";
	string passwd   = "kstar2004";

	if ( get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
	};

	if( !mysqlkstardb.Connect_to_db(dbname, dbserverhost, user, passwd, m_con) )
	{
		cerr<< "MySQL Database Connection Failed. Check Database Server or Network." << endl;
	};
}

void FaultListSummary::getUiObject()
{
	m_ptable		= m_pwidget->findChild<QTableWidget*>("tableWidget");
	mcbSubSystem	= m_pwidget->findChild<QComboBox*>("cbSubSystem");
	mcbFaultType	= m_pwidget->findChild<QComboBox*>("cbFaultType");
	mcbLevel		= m_pwidget->findChild<QComboBox*>("cbLevel");
	mcbDescType		= m_pwidget->findChild<QComboBox*>("cbDescType");
	mcbDateType		= m_pwidget->findChild<QComboBox*>("cbDateType");
	mcbColShowHide	= m_pwidget->findChild<QComboBox*>("cbColShowHide");
	mcbColumn		= m_pwidget->findChild<QComboBox*>("cbColumn");
	mcbWriterShot	= m_pwidget->findChild<QComboBox*>("cbWriterRefShot");
	mleDesc			= m_pwidget->findChild<QLineEdit*>("leDesc");
	mdtStart		= m_pwidget->findChild<QDateTimeEdit*>("dtStart");
	mdtEnd			= m_pwidget->findChild<QDateTimeEdit*>("dtEnd");
	mleWriterSNS 	= m_pwidget->findChild<QLineEdit*>("leWriterSNS");
	mleShotNE 	= m_pwidget->findChild<QLineEdit*>("leShotNE");
	QFont font("Helvetica", 10);
	QStringList labels;
	labels <<tr("No")<<tr("Name")<<tr("Component")<<tr("Function")<<tr("F-Date")
		<<tr("F-Class")<<tr("F-Name")<<tr("F-Severity")<<tr("F-Cause")<<tr("F-Effect")
		<<tr("R-Date")<<tr("R-Description")<<tr("R-Duration")<<tr("I-Level") 
		<<tr("I-Description")<<tr("RefShot")<<tr("MTBF")<<tr("Writer")<<tr("Comment");
	m_ptable->setColumnCount(labels.size());
	m_ptable->setHorizontalHeaderLabels(labels);
	//m_ptable->sortByColumn (1, Qt::AscendingOrder);
	m_ptable->verticalHeader()->show();
	//m_ptable->setFixedHeight(20);
	m_ptable->setShowGrid(true);
	m_ptable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ptable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ptable->setFont(font);
	//font.setPointSize(12);
	font.setBold(true);
	m_ptable->horizontalHeader()->setFont(font);
	//m_ptable->horizontalHeader()->setFixedHeight(54);
}

void FaultListSummary::registerSignal()
{
	QPushButton *pbClose	= m_pwidget->findChild<QPushButton*>("pbClose");
	QPushButton *pbAdd		= m_pwidget->findChild<QPushButton*>("pbAdd");
	QPushButton *pbAddLine	= m_pwidget->findChild<QPushButton*>("pbAddLine");
	QPushButton *pbModifyLine	= m_pwidget->findChild<QPushButton*>("pbModifyLine");
	QPushButton *pbCopy		= m_pwidget->findChild<QPushButton*>("pbCopy");
	QPushButton *pbModify	= m_pwidget->findChild<QPushButton*>("pbModify");
	QPushButton *pbSearch	= m_pwidget->findChild<QPushButton*>("pbSearch");
	QPushButton *pbDelete	= m_pwidget->findChild<QPushButton*>("pbDelete");
	QPushButton *pbQueryReset	= m_pwidget->findChild<QPushButton*>("pbQueryReset");
	QPushButton *pbSaveFile	= m_pwidget->findChild<QPushButton*>("pbSaveFile");
	QPushButton *pbReport	= m_pwidget->findChild<QPushButton*>("pbReport");
	connect(pbClose, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(pbAdd, SIGNAL(clicked()), this, SLOT(addFaultlist()));
	connect(pbAddLine, SIGNAL(clicked()), this, SLOT(addLine()));
	connect(pbModifyLine, SIGNAL(clicked()), this, SLOT(modifyLine()));
	connect(pbCopy, SIGNAL(clicked()), this, SLOT(copyFaultlist()));
	connect(pbModify, SIGNAL(clicked()), this, SLOT(modifyFaultlist()));
	connect(pbSearch, SIGNAL(clicked()), this, SLOT(dbQuery()));
	connect(pbDelete, SIGNAL(clicked()), this, SLOT(deleteFaultlist()));
	connect(pbQueryReset, SIGNAL(clicked()), this, SLOT(dbQueryReset()));
	connect(this, SIGNAL(Search(bool)), this, SLOT(dbFaultlist(bool)) );
	connect(mcbColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(columnHideShow(int)) );
	connect(mcbColShowHide, SIGNAL(currentIndexChanged(int)), this, SLOT(columnHideShow(int)) );
	connect(pbSaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));
	connect(pbReport, SIGNAL(clicked()), this, SLOT(dailyReport()));
}

void FaultListSummary::dailyReport()
{
}
void FaultListSummary::addFaultlist()
{
	AddFaultList *pfaultlist = new AddFaultList;
	pfaultlist->setParent(this);
	pfaultlist->updateCurtime();
}
void FaultListSummary::modifyLine()
{
#if 1
	int selectedRow = m_ptable->currentRow();
	QTableWidgetItem *pItem = m_ptable->item(selectedRow,NUMBER);
	int number = pItem->text().toInt(); 

	pItem = m_ptable->item(selectedRow, SNAME);
	string sname = pItem->text().toStdString(); 
	int status = mcbSubSystem->findText(sname.c_str());
	if (status < 0 )
	{
		messageBox("Type Error!: Name");
		return;
	};

	pItem = m_ptable->item(selectedRow, SCOMPONENT);
	string scomponent = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, SFUNCTION);
	string sfunction = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, FDATE);
	QDateTime fdatetime = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
	bool bvalid = fdatetime.isValid();
	if(bvalid == false)
	{
		messageBox("Type Error!: F-Date");
		return;
	};
	mysqlpp::sql_datetime fdate;
	fdate.year = fdatetime.date().year(); 
	fdate.month = fdatetime.date().month();
	fdate.day= fdatetime.date().day();
	fdate.hour=fdatetime.time().hour();
	fdate.minute=fdatetime.time().minute();
	fdate.second=fdatetime.time().second();

	pItem = m_ptable->item(selectedRow, FCLASS);
	string fclass = pItem->text().toStdString(); 
	status = mcbFaultType->findText(fclass.c_str());
	if (status < 0 )
	{
		messageBox("Type Error!: F-Class");
		return;
	};
	pItem = m_ptable->item(selectedRow, FNAME);
	string fname = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, FSEVERITY);
	int fseverity = pItem->text().toInt(); 

	pItem = m_ptable->item(selectedRow, FCAUSE);
	string fcause = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, FEFFECT);
	string feffect = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, RDATE);
	QDateTime rdatetime = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
	bvalid = rdatetime.isValid();
	if(bvalid == false)
	{
		//messageBox("Type Error!: R-Date");
		//return;
	};
	mysqlpp::sql_datetime rdate;
	rdate.year = rdatetime.date().year(); 
	rdate.month = rdatetime.date().month();
	rdate.day= rdatetime.date().day();
	rdate.hour=rdatetime.time().hour();
	rdate.minute=rdatetime.time().minute();
	rdate.second=rdatetime.time().second();

	pItem = m_ptable->item(selectedRow, RDESC);
	string rdesc = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, RDURATION);
	string rduration = pItem->text().toStdString();

	pItem = m_ptable->item(selectedRow, ILEVEL);
	string ilevel = pItem->text().toStdString();
	status = mcbLevel->findText(ilevel.c_str());
	if (status < 0 )
	{
		messageBox("Type Error!: I-Level");
		return;
	};

	pItem = m_ptable->item(selectedRow, IDESC);
	string idesc = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, REFSHOTNUM);
	int refshotnum = pItem->text().toInt();

	pItem = m_ptable->item(selectedRow, MTBF);
	int mtbf = pItem->text().toInt();

	pItem = m_ptable->item(selectedRow, WRITER);
	string writer = pItem->text().toUtf8().data(); 

	pItem = m_ptable->item(selectedRow, COMMENT);
	string comment = pItem->text().toUtf8().data(); 
	QDateTime wdatetime = QDateTime::currentDateTime();
	Query query = m_con.query();

	query << "update FAULTLISTT set sname='%1:sname', scomponent='%2:scomponent', sfunction='%3:sfunction', fdatetime='%4:fdatetime', \
		fclass='%5:fclass', fname='%6:fname',fseverity=%7:fseverity,fcause='%8:fcause',feffect='%9:feffect',rdatetime='%10:rdatetime',\
		rdesc='%11:rdesc', rduration='%12:rduration', intlevel='%13:ilevel', intdesc='%14:idesc', refshotnum=%15:refshotnum, \
		mtbf=%16:mtbf, writer='%17:writer',comment='%18:comment',wdate='%19:wdate' where num=%20:number";
	query.parse();
	query.def["sname"]=sname;
	query.def["scomponent"]=scomponent;
	query.def["sfunction"]=sfunction;
	query.def["fdatetime"]=fdatetime.toString("yyyy-MM-dd hh:mm:ss").toStdString();
	query.def["fclass"]=fclass;
	query.def["fname"]=fname;
	query.def["fseverity"]=fseverity;
	query.def["fcause"]=fcause;
	query.def["feffect"]=feffect;
	query.def["rdatetime"]=rdatetime.toString("yyyy-MM-dd hh:mm:ss").toStdString();
	query.def["rdesc"]=rdesc;
	query.def["rduration"]=rduration;
	query.def["ilevel"]=ilevel;
	query.def["idesc"]=idesc;
	query.def["refshotnum"]=refshotnum;
	query.def["mtbf"]=mtbf;
	query.def["writer"]=writer;
	query.def["comment"]=comment;
	query.def["wdate"]=wdatetime.toString("yyyy-MM-dd hh:mm:ss").toStdString();
	query.def["number"]=number;
	cout << "Query: " << query.preview() << endl;
	mysqlpp::ResNSel sqlres = query.execute();
#endif
}
void FaultListSummary::addLine()
{
	QPushButton *pbut = qobject_cast<QPushButton*>(sender());
	QString butName = pbut->text();
	if(butName.compare("Add Line") == 0 )
	{
		maddline = m_ptable->rowCount();
		m_ptable->insertRow(maddline);
		mbaddmode=true;
		pbut->setText("Insert DB");
	}
	else if(butName.compare("Insert DB") == 0 )
	{
		int selectedRow = m_ptable->currentRow();

		QTableWidgetItem *pItem = m_ptable->item(selectedRow, SNAME);
		string sname = pItem->text().toStdString(); 
		int status = mcbSubSystem->findText(sname.c_str());
		if (status < 0 )
		{
			messageBox("Type Error!: Name");
			return;
		};

		pItem = m_ptable->item(selectedRow, SCOMPONENT);
		string scomponent = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, SFUNCTION);
		string sfunction = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, FDATE);
		QDateTime fdate = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
		bool bvalid = fdate.isValid();
		if(bvalid == false)
		{
			messageBox("Type Error!: F-Date");
			return;
		};
		mysqlpp::sql_datetime fdatetime;
		fdatetime.year = fdate.date().year(); 
		fdatetime.month = fdate.date().month();
		fdatetime.day= fdate.date().day();
		fdatetime.hour=fdate.time().hour();
		fdatetime.minute=fdate.time().minute();
		fdatetime.second=fdate.time().second();

		pItem = m_ptable->item(selectedRow, FCLASS);
		string fclass = pItem->text().toStdString(); 
		status = mcbFaultType->findText(fclass.c_str());
		if (status < 0 )
		{
			messageBox("Type Error!: Name");
			return;
		};
		pItem = m_ptable->item(selectedRow, FNAME);
		string fname = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, FSEVERITY);
		int fseverity = pItem->text().toInt(); 

		pItem = m_ptable->item(selectedRow, FCAUSE);
		string fcause = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, FEFFECT);
		string feffect = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, RDATE);
		QDateTime rdate = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
		bvalid = rdate.isValid();
		if(bvalid == false)
		{
			messageBox("Type Error!: F-Date");
			return;
		};
		mysqlpp::sql_datetime rdatetime;
		rdatetime.year = rdate.date().year(); 
		rdatetime.month = rdate.date().month();
		rdatetime.day= rdate.date().day();
		rdatetime.hour=rdate.time().hour();
		rdatetime.minute=rdate.time().minute();
		rdatetime.second=rdate.time().second();

		pItem = m_ptable->item(selectedRow, RDESC);
		string rdesc = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, RDURATION);
		string rduration = pItem->text().toStdString();

		pItem = m_ptable->item(selectedRow, ILEVEL);
		string ilevel = pItem->text().toStdString();
		status = mcbLevel->findText(ilevel.c_str());
		if (status < 0 )
		{
			messageBox("Type Error!: I-Level");
			return;
		};

		pItem = m_ptable->item(selectedRow, IDESC);
		string idesc = pItem->text().toUtf8().data(); 

		pItem = m_ptable->item(selectedRow, REFSHOTNUM);
		int refshotnum = pItem->text().toInt();

		pItem = m_ptable->item(selectedRow, MTBF);
		int mtbf = pItem->text().toInt();
		pItem = m_ptable->item(selectedRow, WRITER);
		string writer = pItem->text().toUtf8().data(); 
		pItem = m_ptable->item(selectedRow, COMMENT);
		string comment = pItem->text().toUtf8().data(); 
		QDateTime wdate = QDateTime::currentDateTime();
		mysqlpp::sql_datetime wdatetime;
		wdatetime.year = wdate.date().year(); 
		wdatetime.month = wdate.date().month();
		wdatetime.day= wdate.date().day();
		wdatetime.hour=wdate.time().hour();
		wdatetime.minute=wdate.time().minute();
		wdatetime.second=wdate.time().second();

		Query query = m_con.query();

		maddline = 0;
		mbaddmode = false;

		if(InsertFaultList(sname, scomponent, sfunction, fdatetime, fclass, fname, fseverity, fcause, feffect, 
			rdatetime, rdesc, rduration, ilevel, idesc, refshotnum, mtbf, writer, comment, wdatetime)<0)
		{
			messageBox("Insert DB Error!");
			return;
		};
		pbut->setText("Add Line");
	};
}
void FaultListSummary::copyFaultlist()
{
	int selectedRow = m_ptable->currentRow();
	if(selectedRow < 0)
	{
		messageBox("Select item into fault list table for copy!");
		return;
	};
	Query query = m_con.query();
	QTableWidgetItem *pItem = m_ptable->item(selectedRow,SNAME);
	string sname = pItem->text().toStdString(); 

	pItem = m_ptable->item(selectedRow,SCOMPONENT);
	string scomponent = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow,SFUNCTION);
	string sfunction = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow,FDATE);
	QDateTime fdate = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
	mysqlpp::sql_datetime fdatetime;
	fdatetime.year = fdate.date().year(); 
	fdatetime.month = fdate.date().month();
	fdatetime.day= fdate.date().day();
	fdatetime.hour=fdate.time().hour();
	fdatetime.minute=fdate.time().minute();
	fdatetime.second=fdate.time().second();

	pItem = m_ptable->item(selectedRow,FCLASS);
	string fclass = pItem->text().toStdString();

	pItem = m_ptable->item(selectedRow,FNAME);
	string fname = pItem->text().toStdString();

	pItem = m_ptable->item(selectedRow,FSEVERITY);
	int fseverity = pItem->text().toInt();

	pItem = m_ptable->item(selectedRow, FCAUSE);
	string fcause = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow, FEFFECT);
	string feffect = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow, RDATE);
	QDateTime rdate = QDateTime::fromString(pItem->text(),"yyyy-MM-dd hh:mm:ss");
	mysqlpp::sql_datetime rdatetime;
	rdatetime.year = rdate.date().year(); 
	rdatetime.month = rdate.date().month();
	rdatetime.day= rdate.date().day();
	rdatetime.hour=rdate.time().hour();
	rdatetime.minute=rdate.time().minute();
	rdatetime.second=rdate.time().second();

	pItem = m_ptable->item(selectedRow, RDESC);
	string rdesc = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow, RDURATION);
	string rduration = pItem->text().toStdString();

	pItem = m_ptable->item(selectedRow, ILEVEL);
	string intlevel = pItem->text().toStdString();

	pItem = m_ptable->item(selectedRow, IDESC);
	string intdesc = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow, REFSHOTNUM);
	int refshotnum = pItem->text().toInt();

	pItem = m_ptable->item(selectedRow, MTBF);
	int mtbf = pItem->text().toInt();

	pItem = m_ptable->item(selectedRow, WRITER);
	string writer = pItem->text().toUtf8().data();

	pItem = m_ptable->item(selectedRow, COMMENT);
	string comment = pItem->text().toUtf8().data();

	mysqlpp::sql_datetime wdatetime;
	QDateTime wdate = QDateTime::currentDateTime();
	wdatetime.year = wdate.date().year(); 
	wdatetime.month = wdate.date().month();
	wdatetime.day= wdate.date().day();
	wdatetime.hour=wdate.time().hour();
	wdatetime.minute=wdate.time().minute();
	wdatetime.second=wdate.time().second();

	try {
		FAULTLISTT faultlist(0, sname,scomponent,sfunction,fdatetime,fclass,fname,fseverity,fcause,feffect, rdatetime,rdesc,rduration,intlevel,intdesc,refshotnum,mtbf,writer,comment,wdatetime); 
		query.insert(faultlist);
		query.execute();
		emit Search(true);
		
	}catch (const BadQuery &er){
		cerr << "Query Error: " << er.what() << endl;
	}catch(const Exception &er) {
		cerr << "General Error: " << er.what() << endl;
	};
}
void FaultListSummary::modifyFaultlist()
{
	int selectedRow = m_ptable->currentRow();
	if(selectedRow < 0)
	{
		messageBox("Select fault list table for modification!");
		return;
	};
	qDebug("SelectedRow:%d",selectedRow);

	QTableWidgetItem *pItem = 0;
	vector<string> vecItemString;
	for(int i = 0; i < m_ptable->columnCount();i++)
	{
		pItem = m_ptable->item(selectedRow,i);
		string itemText = pItem->text().toUtf8().data();
		vecItemString.push_back(itemText);
	};

	AddFaultList *pfaultlist = new AddFaultList(true);
	pfaultlist->setParent(this);
	pfaultlist->setItemString(vecItemString);
}

void FaultListSummary::deleteFaultlist()
{
	int currow = m_ptable->currentRow();
	QTableWidgetItem *pItem = m_ptable->item(currow, 0);
	QString itemText = pItem -> text();
	if(messageBox("Are you sure to delete the record ?",APPLYCANCELBOX)==MSGNOK) return;

	try {
		qDebug("%s", itemText.toStdString().c_str());
		m_ptable->removeRow(currow);
		Query query = m_con.query();
		query << "delete from FAULTLISTT where num=%0";
		query.parse();
		int index =itemText.toInt();
		query.use(index);
	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}

void FaultListSummary::dbFaultlist(bool init)
{
	QHash<QString, QString> hashQuery;
	m_ptable->setSortingEnabled (false);

	if (mcbSubSystem->currentIndex()>0)
	{
		hashQuery.insert("sname", mcbSubSystem->currentText());
	};

	if (mcbFaultType->currentIndex()>0)
	{
		hashQuery.insert("fclass", mcbFaultType->currentText());
	};

	if(mcbLevel->currentIndex()>0)
	{
		hashQuery.insert("intlevel",mcbLevel->currentText());
	};

	QString writerShot;
	switch(mcbWriterShot->currentIndex())
	{
		case 1:
			writerShot = QString("refshotnum >= %1 and refshotnum <= %2").arg(mleWriterSNS->text(), mleShotNE->text());
			hashQuery.insert("refshotnum", writerShot);
			break;
		case 2:
			writerShot = QString("fseverity >= %1 and fseverity <= %2").arg(mleWriterSNS->text(), mleShotNE->text());
			hashQuery.insert("fseverity", writerShot);
			break;
		case 3:
			writerShot = QString("rduration >= %1 and rduration <= %2").arg(mleWriterSNS->text(), mleShotNE->text());
			hashQuery.insert("rduration", writerShot);
			break;
		case 4:
			writerShot = QString("mtbf >= %1 and mtbf <= %2").arg(mleWriterSNS->text(), mleShotNE->text());
			hashQuery.insert("mtbf", writerShot);
			break;
		case 5:
			hashQuery.insert("writer", mleWriterSNS->text().toUtf8().data());
			break;
		default:
			break;
	};

	QString descQuery;
	switch(mcbDescType->currentIndex())
	{
		case 1:
			descQuery=QString("scomponent like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("scomponent", descQuery);
			break;
		case 2:
			descQuery=QString("sfunction like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("sfunction", descQuery);
			break;
		case 3:
			descQuery=QString("fname like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("fname", descQuery);
			break;
		case 4:
			descQuery=QString("fcause like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("fcause", descQuery);
			break;
		case 5:
			descQuery=QString("feffect like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("feffect", descQuery);
			break;
		case 6:
			descQuery=QString("rdesc like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("rdesc", descQuery);
			break;
		case 7:
			descQuery=QString("intdesc like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("intdesc", descQuery);
			break;
		case 8:
			descQuery=QString("comment like '%%1%'").arg(mleDesc->text().toUtf8().data());
			hashQuery.insert("comment", descQuery);
			break;
		default:
			break;
	};


	QString timerange;
	switch(mcbDateType->currentIndex())
	{
		case 1:
			timerange = QString("UNIX_TIMESTAMP(fdatetime) > UNIX_TIMESTAMP('%1') and UNIX_TIMESTAMP(fdatetime) <= UNIX_TIMESTAMP('%2')").arg(mdtStart->text(), mdtEnd->text());
			hashQuery.insert("fdatetime", timerange);
			break;
		case 2:
			timerange = QString("UNIX_TIMESTAMP(rdatetime) > UNIX_TIMESTAMP('%1') and UNIX_TIMESTAMP(rdatetime) <= UNIX_TIMESTAMP('%2')").arg(mdtStart->text(), mdtEnd->text());
			hashQuery.insert("rdatetime", timerange);
			break;
		default:
			break;
	};

	QHash<QString, QString>::const_iterator iterHashQuery;
	iterHashQuery = hashQuery.constBegin();
	QString strWhere;
	if(init==false ||hashQuery.empty()==true) 
	{
		strWhere="";
	}
	else
	{
		strWhere = "where ";
		qDebug("HashSize:%d",hashQuery.size());
		int count = 1;
		while ( iterHashQuery != hashQuery.constEnd() ) 
		{
			//cout << iterHashQuery.key().toStdString() << ": " << iterHashQuery.value().toStdString() << endl;
			qDebug("keyname:%s, value:%s",iterHashQuery.key().toStdString().c_str(), iterHashQuery.value().toStdString().c_str());
			QString keyval;
			if (count == hashQuery.size())
			{
				if(iterHashQuery.key().compare("rdatetime") == 0 ||
					iterHashQuery.key().compare("fdatetime") == 0 ||
					iterHashQuery.key().compare("scomponent") == 0 ||
					iterHashQuery.key().compare("sfunction") == 0 ||
					iterHashQuery.key().compare("fname") == 0 ||
					iterHashQuery.key().compare("fcause") == 0 ||
					iterHashQuery.key().compare("feffect") == 0 ||
					iterHashQuery.key().compare("rdesc") == 0 ||
					iterHashQuery.key().compare("intdesc") == 0 ||
					iterHashQuery.key().compare("comment") == 0 ||
					iterHashQuery.key().compare("refshotnum") == 0 ||
					iterHashQuery.key().compare("rduration") == 0 ||
					iterHashQuery.key().compare("fseverity") == 0 ||
					iterHashQuery.key().compare("mtbf") == 0 )
				{
					keyval = QString("%1").arg(iterHashQuery.value());
				}
				else
				{
					keyval = QString("%1='%2'").arg(iterHashQuery.key(), iterHashQuery.value());
				};
			}
			else
			{
				if(iterHashQuery.key().compare("rdatetime") == 0 ||
					iterHashQuery.key().compare("fdatetime") == 0 ||
					iterHashQuery.key().compare("scomponent") == 0 ||
					iterHashQuery.key().compare("sfunction") == 0 ||
					iterHashQuery.key().compare("fname") == 0 ||
					iterHashQuery.key().compare("fcause") == 0 ||
					iterHashQuery.key().compare("feffect") == 0 ||
					iterHashQuery.key().compare("rdesc") == 0 ||
					iterHashQuery.key().compare("intdesc") == 0 ||
					iterHashQuery.key().compare("comment") == 0 ||
					iterHashQuery.key().compare("refshotnum") == 0 ||
					iterHashQuery.key().compare("mtbf") == 0 ||
					iterHashQuery.key().compare("fseverity") == 0 ||
					iterHashQuery.key().compare("rduration") == 0 )
				{
					keyval = QString("%1 and ").arg(iterHashQuery.value());
				}
				else
				{
					keyval = QString("%1='%2' and ").arg(iterHashQuery.key(), iterHashQuery.value());
				};
			};
			strWhere += keyval;
			++iterHashQuery;
			count++;
		};
	};

	Query query = m_con.query();
	ResUse res;
	string strselect = "select num, sname,scomponent,sfunction,fdatetime,fclass,fname,fseverity,fcause,feffect,rdatetime,rdesc,rduration,intlevel,intdesc,refshotnum,mtbf,writer,comment,wdate from FAULTLISTT ";
	string strQuery = strselect + strWhere.toStdString();
	qDebug("Query:%s",strQuery.c_str());
	query <<strQuery;
	query.parse();
	res = query.use();
	Row row;
	QTableWidgetItem *pItem = 0;
	m_ptable->setRowCount(0);
	sql_datetime sqldate;
	string strfdatetime;
	string strrdatetime;
	try {
		while (row = res.fetch_row())
		{
			//0:num, 1:sname,2:scomponent,3:sfunction,4:fdatetime,5:fclass,6:fname,7:fseverity,8:fcause,9:feffect,
			//10:rdatetime,11:rdesc,12:rduration,13:intlevel,14:intdesc,15:refshotnum,16:mtbf,17:writer,18:comment,19:wdate
			//Only English
			string strnumber		= row["num"].get_string(); //0.
			string strssname		= row["sname"].get_string(); //1.
			QString strscomp		= QString::fromUtf8(row["scomponent"]);//2.
			QString strsfunc		= QString::fromUtf8(row["sfunction"]); //3.
			sqldate = row["fdatetime"];
			if((time_t)sqldate == -1 ) strfdatetime = "";
			else strfdatetime = row["fdatetime"].get_string();//4.

			string strfclass		= row["fclass"].get_string();//5.
			QString strfname		= QString::fromUtf8(row["fname"]);//6.
			string strfseverity		= row["fseverity"].get_string();//7.
			QString strfcause		= QString::fromUtf8(row["fcause"]);//8.
			QString strfeffect		= QString::fromUtf8(row["feffect"]);//9.
			sqldate = row["rdatetime"];
			if((time_t)sqldate == -1 ) strrdatetime = "";
			else strrdatetime = row["rdatetime"].get_string();//10.

			QString strrdesc		= QString::fromUtf8(row["rdesc"]);//11.
			string strrduration		= row["rduration"].get_string();//12.
			string strilevel		= row["intlevel"].get_string();//13.
			QString stridesc		= QString::fromUtf8(row["intdesc"]);//14.
			string strrefshotnum	= row["refshotnum"].get_string();//15.
			string strmtbf			= row["mtbf"].get_string();//16.
			QString strwriter		= QString::fromUtf8(row["writer"]);//17.
			QString strcomment		= QString::fromUtf8(row["comment"]);//18.
			//string strwdate			= row["wdate"].get_string(); //19.

			int row = m_ptable->rowCount();
			m_ptable->insertRow(row);

			pItem = new QTableWidgetItem(strnumber.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, NUMBER, pItem);

			pItem = new QTableWidgetItem(strssname.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, SNAME, pItem);

			pItem = new QTableWidgetItem(strscomp);
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, SCOMPONENT, pItem);

			pItem = new QTableWidgetItem(strsfunc);
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, SFUNCTION, pItem);

			pItem = new QTableWidgetItem(strfdatetime.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, FDATE, pItem);

			pItem = new QTableWidgetItem(strfclass.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, FCLASS, pItem);

			pItem = new QTableWidgetItem(strfname);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, FNAME, pItem);

			pItem = new QTableWidgetItem(strfseverity.c_str());
			pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_ptable->setItem(row, FSEVERITY, pItem);

			pItem = new QTableWidgetItem(strfcause);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, FCAUSE, pItem);

			pItem = new QTableWidgetItem(strfeffect);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, FEFFECT,pItem); 

			pItem = new QTableWidgetItem(strrdatetime.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, RDATE,pItem); 

			pItem = new QTableWidgetItem(strrdesc);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, RDESC,pItem); 

			pItem = new QTableWidgetItem(strrduration.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, RDURATION,pItem); 

			pItem = new QTableWidgetItem(strilevel.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, ILEVEL,pItem); 

			pItem = new QTableWidgetItem(stridesc);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, IDESC,pItem); 

			pItem = new QTableWidgetItem(strrefshotnum.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, REFSHOTNUM,pItem); 

			pItem = new QTableWidgetItem(strmtbf.c_str());
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, MTBF,pItem); 

			pItem = new QTableWidgetItem(strwriter);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row, WRITER,pItem); 

			pItem = new QTableWidgetItem(strcomment);
			pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_ptable->setItem(row,COMMENT,pItem); 
		}
	}catch (const BadQuery &er){
		cerr << "Query Error: " << er.what() << endl;
	}catch(const Exception &er) {
		cerr << "General Error: " << er.what() << endl;
	};

	m_ptable->setSortingEnabled (true);
	m_ptable->sortByColumn (1, Qt::DescendingOrder);
}


void FaultListSummary::saveFile()
{
	QString exportname = QFileDialog::getSaveFileName(this, "Export File",
				QDir::currentPath(),tr("Save Files (*.txt *.pdf)"));
	

	if (exportname.isEmpty()) return;

    QFile file(exportname);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Export File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(exportname)
                             .arg(file.errorString()));
        return;
    };

	if (QFileInfo(exportname).suffix().compare("pdf")==0)
	{
		exportPDF(exportname);
	}
	else
	{
		exportText(file);
	};

	file.close();
    //if (xbelTree->write(&file))
}
void FaultListSummary::exportText(QFile &file)
{
	int rowcnt = m_ptable->rowCount();
	int colcnt = m_ptable->columnCount();

	QTableWidgetItem *pitem = 0;
	int len = 0;
	for (int i = 1; i<colcnt;i++)
	{
		pitem = m_ptable->horizontalHeaderItem(i);
		if ( pitem == NULL) continue;
		len = qstrlen(pitem->text().toUtf8().data());
		if ( len <= 1 ) continue;
		file.write(pitem->text().toUtf8().data(), len);
		file.write("\t", 1);
	};
	file.write("\n", 1);
	int row = 0, col =1;
	for(row=0; row < rowcnt;row++)
	{
		for (col=1; col < colcnt;col++)
		{
			pitem = m_ptable->item(row, col);
			if ( pitem == NULL ) continue;
			len = qstrlen(pitem->text().toUtf8().data());
			file.write(pitem->text().toUtf8().data(), len);
			file.write("\t", 1);
		};
		pitem = m_ptable->item(row, col);
		if ( pitem == NULL) continue;
		file.write("\n", 1);
	};
}
void FaultListSummary::exportPDF(QString exportname)
{
	QPrinter pdfprint;
	pdfprint.setOrientation(QPrinter::Landscape);
	pdfprint.setOutputFormat(QPrinter::PdfFormat);
	pdfprint.setOutputFileName(exportname);

	QPainter painter(&pdfprint);
	QRect rect = painter.viewport();
	painter.setViewport(rect.x(), rect.y(), m_ptable->width()-175, m_ptable->height());
	painter.setWindow(m_ptable->rect());
	QPixmap pixmap = QPixmap::grabWidget(m_ptable);
	painter.drawPixmap(0, 0, pixmap);

}
void FaultListSummary::print()
{
	QPrinter print;
	print.setOrientation(QPrinter::Landscape);
	print.setOutputFormat(QPrinter::NativeFormat);
	QPrintDialog dialog(&print, this);
	if (dialog.exec()) {
		QPainter painter(&print);
		QRect rect = painter.viewport();
		painter.setViewport(rect.x(), rect.y(), m_ptable->width()-175, m_ptable->height());
		painter.setWindow(m_ptable->rect());
		QPixmap pixmap = QPixmap::grabWidget(m_ptable);
		painter.drawPixmap(0, 0, pixmap);
	};
}
int FaultListSummary::messageBox(QString msg, const int type)
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
void FaultListSummary::dbQuery()
{
	emit Search(true);
}
void FaultListSummary::dbQueryReset()
{
	mcbSubSystem->setCurrentIndex(0);
	mcbFaultType->setCurrentIndex(0);
	mcbLevel->setCurrentIndex(0);
	mcbDescType->setCurrentIndex(0);
	mcbDateType->setCurrentIndex(0);
	mcbWriterShot->setCurrentIndex(0);
	mleDesc->setText("");
	mleWriterSNS->setText("");
	mleShotNE->setText("");
	
	QDateTime datetime = QDateTime::currentDateTime();
	mdtStart->setDateTime(datetime);
	mdtEnd->setDateTime(datetime);
}

int FaultListSummary::InsertFaultList(string& sname, string& scomponent, string& sfunction, sql_datetime& fdatetime, string& fclass,
	string& fname, int& fseverity, string& fcause, string& feffect, sql_datetime& rdatetime, string& rdesc, string& rduration, string& intlevel,
	string& intdesc, int& refshotnum, int& mtbf, string& writer, string& comment, sql_datetime& wdatetime)
{
	Query query = m_con.query();
	try {
		FAULTLISTT faultlist(0, sname,scomponent,sfunction,fdatetime,fclass,fname,fseverity,fcause,feffect, rdatetime,rdesc,rduration,intlevel,intdesc,refshotnum,mtbf,writer,comment,wdatetime); 
		query.insert(faultlist);
		query.execute();
	}catch (const BadQuery &er){
		cerr << "Query Error: " << er.what() << endl;
		return -1;
	}catch(const Exception &er) {
		cerr << "General Error: " << er.what() << endl;
		return -1;
	};
	return 0;
}

void FaultListSummary::columnHideShow(int index)
{
	QComboBox *pcombo = qobject_cast<QComboBox*>(sender());

	QString objName = pcombo->objectName();
	if(objName.compare("cbColumn") == 0 )
	{
		QString hideshow = mcbColShowHide->currentText();
		if(hideshow.compare("Hide")== 0)
		{
			m_ptable->hideColumn(index-1);
		}
		else if(hideshow.compare("Show")== 0)
		{
			m_ptable->showColumn(index-1);
		};
	}
	else if(objName.compare("cbColShowHide") == 0 )
	{
		QString hideshow = mcbColShowHide->currentText();
		int col = mcbColumn->currentIndex();
		if(hideshow.compare("Hide")== 0)
		{
			m_ptable->hideColumn(col-1);
		}
		else if(hideshow.compare("Show")== 0)
		{
			m_ptable->showColumn(col-1);
		};
	};
}
