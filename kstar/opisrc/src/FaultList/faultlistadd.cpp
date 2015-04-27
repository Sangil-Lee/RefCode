#include "faultlistadd.h"
//#include "sqlsfaultlist.h"

using namespace mysqlpp;

AddFaultList::AddFaultList(bool modify):mmodify(modify)
{
	init();
	getUiObject();
	registerSignal();
}

AddFaultList::~AddFaultList()
{
}

void AddFaultList::init()
{
	QFile file ("/usr/local/opi/ui/addfaultlist.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(addfaultlist.ui) File not exists!");
		return;
	};
	file.open(QFile::ReadOnly);
	m_pFaultList = loader.load(&file);
	m_pFaultList->setGeometry(315,170,674,775);
	m_pFaultList->show();
	file.close();
}
void AddFaultList::setParent(QWidget *parent)
{
	m_pParent = (FaultListSummary*)parent;
	m_con = m_pParent->getConnection();
}
void AddFaultList::setItemString(vector<string> &itemlist)
{
	mitemlist = itemlist;
	QString strsubsystem = mitemlist.at(SNAME).c_str();
	for(int i = 0; i < mcbSubsystem->count();i++) 
	{
		if(strsubsystem.compare(mcbSubsystem->itemText(i)) == 0)
		{
			mcbSubsystem->setCurrentIndex(i);
			break;
		};
	};

	QString strftype = mitemlist.at(FCLASS).c_str();
	for(int i = 0; i < mcbFType->count();i++) 
	{
		if(strftype.compare(mcbFType->itemText(i)) == 0)
		{
			mcbFType->setCurrentIndex(i);
			break;
		};
	};

	QString strilevel = mitemlist.at(ILEVEL).c_str();
	for(int i = 0; i < mcbILevel->count();i++) 
	{
		if(strilevel.compare(mcbILevel->itemText(i)) == 0)
		{
			mcbILevel->setCurrentIndex(i);
			break;
		};
	};

	QDateTime datetime = QDateTime::fromString(mitemlist.at(FDATE).c_str(),"yyyy-MM-dd hh:mm:ss");
	mdtFDate->setDateTime(datetime);
	datetime = QDateTime::fromString(mitemlist.at(RDATE).c_str(),"yyyy-MM-dd hh:mm:ss");
	mdtRDate->setDateTime(datetime);

	mteRDesc->setPlainText(QString::fromUtf8(mitemlist.at(RDESC).c_str()));
	mteIDesc->setPlainText(QString::fromUtf8(mitemlist.at(IDESC).c_str()));
	mleWriter->setText(QString::fromUtf8(mitemlist.at(WRITER).c_str()));

	mleCompo->setText(QString::fromUtf8(mitemlist.at(SCOMPONENT).c_str()));
	mleFunc->setText(QString::fromUtf8(mitemlist.at(SFUNCTION).c_str()));
	mleFName->setText(QString::fromUtf8(mitemlist.at(FNAME).c_str()));
	mleFCause->setText(QString::fromUtf8(mitemlist.at(FCAUSE).c_str()));
	mleFEffect->setText(QString::fromUtf8(mitemlist.at(FEFFECT).c_str()));

	mteComment->setPlainText(QString::fromUtf8(mitemlist.at(COMMENT).c_str()));

	unsigned int shotno = QString(mitemlist.at(REFSHOTNUM).c_str()).toInt();
	msbRefShotNo->setValue(shotno);

	unsigned int fsever = QString(mitemlist.at(FSEVERITY).c_str()).toInt();
	msbFSeverity->setValue(fsever);

	mleRDuration->setText(QString::fromUtf8(mitemlist.at(RDURATION).c_str()));

	unsigned int mtbf = QString(mitemlist.at(MTBF).c_str()).toInt();
	msbMTBF->setValue(mtbf);

	mpbAdd->setText("Update");
	mlbTitle->setText("Update Fault List");
}
void AddFaultList::getUiObject()
{
	mcbSubsystem = m_pFaultList->findChild<QComboBox*>("cbSubsystem");
	mcbFType = m_pFaultList->findChild<QComboBox*>("cbFType");
	mcbILevel = m_pFaultList->findChild<QComboBox*>("cbILevel");
	mdtFDate = m_pFaultList->findChild<QDateTimeEdit*>("dtFDate");
	mdtRDate = m_pFaultList->findChild<QDateTimeEdit*>("dtRDate");
	mteRDesc = m_pFaultList->findChild<QTextEdit*>("teRDesc");
	mteIDesc = m_pFaultList->findChild<QTextEdit*>("teIDesc");
	mteComment = m_pFaultList->findChild<QTextEdit*>("teComment");
	mpbAdd = m_pFaultList->findChild<QPushButton*>("pbAdd");
	mlbTitle	= m_pFaultList->findChild<QLabel*>("lbTitle");
	mleWriter	= m_pFaultList->findChild<QLineEdit*>("leWriter");
	mleCompo	= m_pFaultList->findChild<QLineEdit*>("leCompo");
	mleFunc		= m_pFaultList->findChild<QLineEdit*>("leFunc");
	mleFName	= m_pFaultList->findChild<QLineEdit*>("leFName");
	mleFCause	= m_pFaultList->findChild<QLineEdit*>("leFCause");
	mleFEffect	= m_pFaultList->findChild<QLineEdit*>("leFEffect");
	mleRDuration= m_pFaultList->findChild<QLineEdit*>("leRDuration");

	msbRefShotNo= m_pFaultList->findChild<QSpinBox*>("sbRefShotNo");
	msbFSeverity= m_pFaultList->findChild<QSpinBox*>("sbFSeverity");
	msbMTBF= m_pFaultList->findChild<QSpinBox*>("sbMTBF");
}
void AddFaultList::updateCurtime()
{
	QDateTime datetime = QDateTime::currentDateTime();
	mdtFDate->setDateTime(datetime);
	mdtRDate->setDateTime(datetime);
}
void AddFaultList::Reset()
{
	mcbSubsystem->setCurrentIndex(0);
	mcbFType->setCurrentIndex(0);
	mcbILevel->setCurrentIndex(0);
	QDateTime datetime = QDateTime::currentDateTime();
	mdtFDate->setDateTime(datetime);
	mdtRDate->setDateTime(datetime);
	mteRDesc->setText("");
	mteIDesc->setText("");
	mteComment->setText("");
	mleWriter->setText("");
	mleCompo->setText("");
	mleFunc	->setText("");
	mleFName->setText("");
	mleFCause->setText("");
	mleFEffect->setText("");
	msbRefShotNo->setValue(0);
	msbFSeverity->setValue(0);
	mleRDuration->setText("");
}
void AddFaultList::registerSignal()
{
	QPushButton *pbClose = m_pFaultList->findChild<QPushButton*>("pbClose");
	QPushButton *pbReset = m_pFaultList->findChild<QPushButton*>("pbReset");

	connect(pbClose, SIGNAL(clicked()), this, SLOT(Close()));
	connect(pbReset, SIGNAL(clicked()), this, SLOT(Reset()));

	connect(mpbAdd, SIGNAL(clicked()), this, SLOT(InsertUpdate()));
}
void AddFaultList::Close()
{
	m_pFaultList->close();
	close();
}
void AddFaultList::dbSearch()
{
}
void AddFaultList::InsertUpdate()
{
	QPushButton *pbut = qobject_cast<QPushButton*>(sender());
	Query query = m_con.query();

	if (mcbSubsystem->currentIndex() <= 0 ||
		mcbFType->currentIndex() <= 0 || mcbILevel->currentIndex() <= 0 ) 
	{
		messageBox("Input Error");
		return;
	};

	string	sname		= mcbSubsystem->currentText().toStdString();
	string	scomponent	= mleCompo->text().toUtf8().data();
	string	sfunction	= mleFunc->text().toUtf8().data();
	QDateTime fdatetime = mdtFDate->dateTime();
	mysqlpp::sql_datetime fdate;
	fdate.year	= fdatetime.date().year(); 
	fdate.month = fdatetime.date().month();
	fdate.day	= fdatetime.date().day();
	fdate.hour	= fdatetime.time().hour();
	fdate.minute= fdatetime.time().minute();
	fdate.second= fdatetime.time().second();
	string	fclass		= mcbFType->currentText().toStdString();
	string	fname		= mleFName->text().toUtf8().data();
	int	fseverity		= msbFSeverity->value();
	string	fcause		= mleFCause->text().toUtf8().data();
	string	feffect		= mleFEffect->text().toUtf8().data();
	mysqlpp::sql_datetime rdate;
	QDateTime rdatetime = mdtRDate->dateTime();
	rdate.year	= rdatetime.date().year(); 
	rdate.month = rdatetime.date().month();
	rdate.day	= rdatetime.date().day();
	rdate.hour	=rdatetime.time().hour();
	rdate.minute=rdatetime.time().minute();
	rdate.second=rdatetime.time().second();
	string rdesc	= mteRDesc->toPlainText().toUtf8().data();
	string rduration= mleRDuration->text().toUtf8().data();
	string ilevel	= mcbILevel->currentText().toStdString();
	string idesc	= mteIDesc->toPlainText().toUtf8().data();
	int	refshotnum	= msbRefShotNo->value();
	int mtbf		= msbMTBF->value();
	string writer	= mleWriter->text().toUtf8().data();
	string comment	= mteComment->toPlainText().toUtf8().data();

	mysqlpp::sql_datetime wdate;
	QDateTime wdatetime = QDateTime::currentDateTime();
	wdate.year = wdatetime.date().year(); 
	wdate.month = wdatetime.date().month();
	wdate.day= wdatetime.date().day();
	wdate.hour=wdatetime.time().hour();
	wdate.minute=wdatetime.time().minute();
	wdate.second=wdatetime.time().second();

	try {
		QString strName = pbut->text();
		if(strName.compare("Add")==0)
		{
			m_pParent->InsertFaultList(sname,scomponent,sfunction,fdate,fclass,fname,fseverity,fcause,feffect,rdate,
					rdesc,rduration,ilevel,idesc,refshotnum,mtbf,writer,comment,wdate);
		}
		else if(strName.compare("Update")==0)
		{
			query << "update FAULTLISTT set sname='%1:sname', scomponent='%2:scomponent', sfunction='%3:sfunction', fdatetime='%4:fdatetime', \
				fclass='%5:fclass', fname='%6:fname',fseverity=%7:fseverity,fcause='%8:fcause',feffect='%9:feffect',rdatetime='%10:rdatetime',\
				rdesc='%11:rdesc', rduration=%12:rduration, intlevel='%13:ilevel', intdesc='%14:idesc', refshotnum=%15:refshotnum, \
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
			query.def["number"]=mitemlist.at(NUMBER);
			cout << "Query: " << query.preview() << endl;
			mysqlpp::ResNSel sqlres = query.execute();
		}
		query.execute();
	}catch (const BadQuery &er){
		cerr << "Query Error: " << er.what() << endl;
	}catch(const Exception &er) {
		cerr << "General Error: " << er.what() << endl;
	};
}
int AddFaultList::messageBox(QString msg, const int type)
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
