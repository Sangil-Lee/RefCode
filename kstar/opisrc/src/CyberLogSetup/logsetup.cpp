#include "cyberlog.h"
#include "logsetup.h"

using namespace mysqlpp;

CyberLogSetup::CyberLogSetup()
{
	init();
};
CyberLogSetup::~CyberLogSetup()
{
};

void CyberLogSetup::init()
{
	QFile file ("../ui/LogSetup.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(LogSetup.ui) File not exists!");
		m_plogsetup = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_plogsetup = m_loader.load(&file);
	m_plogsetup->show();
	file.close();

	string dbname   = "cyberlog";
	string dbserverhost = "localhost";
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
	regsignals();
	startTimer(500);

	mpLogSave = new LogSaveThr(this);
	//createtable("CYBER_TEST_0040");
}

void CyberLogSetup::timerEvent(QTimerEvent *event)
{
	QTime curtime = QTime::currentTime();
	qDebug("curTime:%s", curtime.toString().toStdString().c_str());
	QDateTime curdatetime = QDateTime::currentDateTime();
	mlbCurDateTime->setText(curdatetime.toString("yyyy-MM-dd hh:mm:ss"));

	//mpLogSave->start();
}

void CyberLogSetup::regsignals()
{
	if(!m_plogsetup) return;

	QDateTime curdtime = QDateTime::currentDateTime();

	mleTestID = m_plogsetup->findChild<QLineEdit *>("leTestID");
	mleProName = m_plogsetup->findChild<QLineEdit *>("leProName");
	mteDesc = m_plogsetup->findChild<QTextEdit *>("teDesc");;

	mrbAuto = m_plogsetup->findChild<QRadioButton *>("rbAuto");
	mrbManual = m_plogsetup->findChild<QRadioButton *>("rbManual");

	mrbSetEnd = m_plogsetup->findChild<QRadioButton *>("rbSetEnd");
	mrbSetTimer = m_plogsetup->findChild<QRadioButton *>("rbSetTimer");

	msbMin = m_plogsetup->findChild<QSpinBox *>("sbMin");
	msbSec = m_plogsetup->findChild<QSpinBox *>("sbSec");

	mdteStart = m_plogsetup->findChild<QDateTimeEdit *>("dteStart");
	mdteStart -> setDateTime(curdtime);
	mdteStop = m_plogsetup->findChild<QDateTimeEdit *>("dteEnd");
	mdteStop -> setDateTime(curdtime.addDays(1));

	mteSetTimer = m_plogsetup->findChild<QTimeEdit *>("teSetTimer");

	mpbSave = m_plogsetup->findChild<QPushButton *>("pbSave");
	mpbStop = m_plogsetup->findChild<QPushButton *>("pbStop");

	mpbCreate = m_plogsetup->findChild<QPushButton *>("pbCreate");
	connect(mpbCreate, SIGNAL(clicked()), this, SLOT(sdnDataInsert()));
	mpbModify = m_plogsetup->findChild<QPushButton *>("pbModify");
	mpbDelete = m_plogsetup->findChild<QPushButton *>("pbDelete");

	mlbCurDateTime = m_plogsetup->findChild<QLabel *>("lbCurDateTime");
	mlbRunStop = m_plogsetup->findChild<QLabel *>("lbRunStop");
}

int CyberLogSetup::sdnDataInsert()
{
	qDebug("Start-Time(%s), End-Time(%s)", mdteStart->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str(),
			mdteStart->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
	QString tablename = mleTestID->text();
	if(createtable(tablename.toStdString()) < 0)
	{
		return -1;
	};
#if 1
	try{
		Query query = m_con.query();
		SDN_DATA_DB sdndata;
		sdndata.TEST_ID = tablename.toStdString();
		sdndata.PROJECT_NAME = mleProName->text().toStdString();
		sdndata.TEST_DESC = mteDesc->toPlainText().toStdString();
		sdndata.TEST_START_TIME = mdteStart->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
		sdndata.TEST_END_TIME = mdteStop->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
		sdndata.PERIODIC_SAVE_TIME = msbMin->value()*60 + msbSec->value();
		sdndata.MODE = (mrbAuto->isChecked()==true)?true:false;
		sdndata.END_TYPE = (mrbSetEnd->isChecked()==true)?true:false;
		query.insert(sdndata);
		query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -2;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -3;
	}
#endif
	return 0;
}
int CyberLogSetup::createtable(string tablename)
{
	try{
		Query query = m_con.query();
		query << 
			"CREATE TABLE " << tablename.c_str() << " (" <<
			"  SDN_INDEX int, " <<
			"  SAVE_TIME datetime, " <<
			"  NETWORK_ID int, " <<
			"  SOURCE varchar(10), " <<
			"  DESTINATION varchar(20), " <<
			"  SDN_DATA varchar(220)) " <<
			"ENGINE = InnoDB " <<
			"CHARACTER SET utf8 COLLATE utf8_general_ci";
		cout << "Query: " << query.preview() << endl;
		query.execute();

		// Set up the template query to insert the data.  The parse()
		// call tells the query object that this is a template and
		// not a literal query string.
		query << "insert into %7:table values (%0q, %1q, %2q, %3q, %4q, %5q)";
		query.parse();

		// Set the template query parameter "table" to "stock".
		query.def["table"] = tablename.c_str();

		// Notice that we don't give a sixth parameter in these calls,
		// so the default value of "stock" is used.  Also notice that
		// the first row is a UTF-8 encoded Unicode string!  All you
		// have to do to store Unicode data in recent versions of MySQL
		// is use UTF-8 encoding.
		//cout << "Populating stock table..." << endl;
		query.execute(10,"2014-03-10 00:00:01",3,"ER", "DESTINATION40", "FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA1187FF00AABBDDFF00AA11DD2200AABBCCFF35AA11871187FF00AABBDDFF00AA11DD2200AABBCCFF35AADD2200AABBCCFF35AAFFDD2200AABBCCFF35AAFF");

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -2;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -3;
	}
	return 0;
};
