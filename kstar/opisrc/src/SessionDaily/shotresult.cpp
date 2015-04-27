#include "shotremark.h"
#include "shotresult.h"

using namespace mysqlpp;

ShotResult::ShotResult():m_con(false),mfilepath("../screenshot/"),getProc(),putProc(),mauto(false)
{
	init();
	getUiObject();
	registerSignal();
}
ShotResult::~ShotResult()
{
}

void
ShotResult::init()
{
	QFile file ("../ui/ShotResult.ui");
	if ( !file.exists() )
	{
		qDebug("Ui(ShotResult.ui) File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->hide();
	file.close();
}

void ShotResult::getUiObject()
{
	milbPCS	= m_pwidget->findChild<QLabel *>("ilbPCS");
	milbDAQ	= m_pwidget->findChild<QLabel *>("ilbDAQ");
	mpbMake	= m_pwidget->findChild<QPushButton *>("pbMake");
	msbShotNum = m_pwidget->findChild<QSpinBox *>("sbShotNum");
	mtw_Shotremark = m_pwidget->findChild<QTableWidget *>("tw_Shotremark");

	mpbClose = m_pwidget->findChild<QPushButton *>("pbClose");
	mpbAdd  = m_pwidget->findChild<QPushButton *>("pbAdd");
	mpbSave  = m_pwidget->findChild<QPushButton *>("pbSave");
	mpbDelete = m_pwidget->findChild<QPushButton *>("pbDelete");
	mlbTitle = m_pwidget->findChild<QLabel *>("lbTitle");

	mpbAuto= m_pwidget->findChild<QPushButton *>("pbAuto");
	mpbRSize= m_pwidget->findChild<QPushButton *>("pbRSize");
	mpbRGen = m_pwidget->findChild<QPushButton *>("pbRGen");

	mleTf= m_pwidget->findChild<QLineEdit *>("leTf");
	mleIp= m_pwidget->findChild<QLineEdit *>("leIp");
	mleIpPulse= m_pwidget->findChild<QLineEdit *>("leIpPulse");
	mleNe= m_pwidget->findChild<QLineEdit *>("leNe");
	mleTe= m_pwidget->findChild<QLineEdit *>("leTe");
	mlePr= m_pwidget->findChild<QLineEdit *>("lePr");
	mleEchP= m_pwidget->findChild<QLineEdit *>("leEchP");
	mleRegenS= m_pwidget->findChild<QLineEdit *>("leRegenS");
	mleRegenE= m_pwidget->findChild<QLineEdit *>("leRegenE");

	milbPCS->setBackgroundRole(QPalette::Base);
	milbPCS->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	milbPCS->setScaledContents(true);
	milbDAQ->setBackgroundRole(QPalette::Base);
	milbDAQ->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	milbDAQ->setScaledContents(true);

	QFont font("Helvetica", 12);
	QStringList labels;
	labels <<tr("Writer")<< tr("Comment") << tr("Date");
	mtw_Shotremark->setColumnCount(labels.size());
	mtw_Shotremark->setHorizontalHeaderLabels(labels);
	for(int i = 0; i < labels.size(); i++)
	{
		switch(i)
		{
			case 0:
				mtw_Shotremark->horizontalHeader()->resizeSection(i, 100);
				break;
			case 1:
				mtw_Shotremark->horizontalHeader()->resizeSection(i, 800);
				break;
			case 2:
				mtw_Shotremark->horizontalHeader()->resizeSection(i, 140);
				break;
		}
	}
	mtw_Shotremark->verticalHeader()->show();
	mtw_Shotremark->setShowGrid(true);
	mtw_Shotremark->setSelectionMode(QAbstractItemView::SingleSelection);
	mtw_Shotremark->setSelectionBehavior(QAbstractItemView::SelectRows);
	mtw_Shotremark->setFont(font);
	font.setPointSize(10);
	//font.setBold(true);
	mtw_Shotremark->horizontalHeader()->setFont(font);
	//mtw_Shotremark->horizontalHeader()->setFixedHeight(54);

}

void ShotResult::registerSignal()
{
	connect(mpbMake, SIGNAL(clicked()), this, SLOT(makeImage()));
	//connect(mpbClose, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(mpbClose, SIGNAL(clicked()), this, SLOT(hideWindow()));
	connect(mpbAdd, SIGNAL(clicked()), this, SLOT(addRemark()));
	connect(mpbSave, SIGNAL(clicked()), this, SLOT(saveRemark()));
	connect(mpbDelete, SIGNAL(clicked()), this, SLOT(deleteRemark()));
	connect(mpbAuto, SIGNAL(clicked()), this, SLOT(autoManual()));
	connect(mpbRSize, SIGNAL(clicked()), this, SLOT(resetWindow()));
	connect(mpbRGen, SIGNAL(clicked()), this, SLOT(reGeneration()));
	connect(this, SIGNAL(update()), this, SLOT(makeImage()));
	connect(this, SIGNAL(resetwin()), this, SLOT(resetWindow()));
	//connect(msbShotNum, SIGNAL(valueChanged(int)), this, SLOT(loadImage(int)));
}

void ShotResult::loadImage(int shotno)
{
	readImage(milbPCS, QString("pcsdata_shot%1.jpg").arg(shotno));
	readImage(milbDAQ, QString("daqdata_shot%1.jpg").arg(shotno));
}

void ShotResult::reGeneration()
{
	QPushButton *pbut = qobject_cast<QPushButton *>(sender());
	QString strName = pbut->text();
	if(strName.compare("Re-generation")==0)
	{
		if(parseShotNum() < 0) 
		{
			messageBox("Input Error!");
			return;
		};
		pbut->setText("Stop");
		mstid = startTimer(3000);
	}
	else if(strName.compare("Stop")==0)
	{
		pbut->setText("Re-generation");
		killTimer(mstid);
	};
}

void ShotResult::makeImage(unsigned int shotno)
{
	qDebug("MakeImage****");
	QString filename = QString("pcsdata_shot%1.jpg").arg(shotno); 
	QString fullpath =  mfilepath+filename;
	int status = readImage(milbPCS, filename);
	if(status < -1 )
	{
		QString strmsg = QString("Cannot find file:%1").arg(fullpath);
		qDebug("%s", strmsg.toStdString().c_str());
		milbPCS->setPixmap(0);
	};
	filename = QString("daqdata_shot%1.jpg").arg(shotno); 
	fullpath =  mfilepath+filename;
	status = readImage(milbDAQ, filename);
	if(status < -1 )
	{
		QString strmsg = QString("Cannot find file:%1").arg(fullpath);
		qDebug("%s", strmsg.toStdString().c_str());
		milbDAQ->setPixmap(0);
	};
	mshotno = shotno;
	QString stitle = QString("@Shot %1").arg(mshotno);
	mlbTitle->setText(stitle);
#if 1
	if(m_pParent->GetHostname().compare("opi16")==0) 
	{
		filename = QString("shotresult_%1.jpg").arg(shotno); 
		exportImage(filename);
	}
#endif
}
int ShotResult::isAlphabet(const char *alpha, const int size)
{
	for (int i = 0; i<size;i++)
	{
		if(isdigit(alpha[i])!=0)  return -1;
	}
	return 0;
}
int ShotResult::parseShotNum()
{
#if 0
	QString strshotrange = mleRegenS->text().trimmed();
	char strBuf[strshotrange.size()];
	strcpy(strBuf,strshotrange.toStdString().c_str());

	if(isAlphabet(strBuf, sizeof(strBuf))<0) 
	{
		messageBox("Input Error!-Alphabet character");
		return -1;
	}

	int ncoma = strshotrange.count(":", Qt::CaseInsensitive);
	int nshim = strshotrange.count(",", Qt::CaseInsensitive);
	int nspace = strshotrange.count(" ", Qt::CaseInsensitive);

	mvecShotregen.clear();
	if (ncoma == 1) 
	{
		char *pch = strtok(strBuf, ":");
		while(pch!=0)
		{
			qDebug("%s", pch);
			if(!(pch = strtok(NULL, ":"))) continue;
		};
	}
	else if ( ncoma == 0 && (nshim >= 0 || nspace >= 0 ))
	{
		char *pch = strtok(strBuf, " ,");
		while(pch!=0)
		{
			qDebug("%s", pch);
			mvecShotregen.push_back(atoi(pch));
			if(!(pch = strtok(NULL, " ,"))) continue;
		};
	}
#else
	bool chk = true, chk1 = true;
	mstartshot =  mleRegenS->text().trimmed().toInt(&chk);
	mendshot =  mleRegenE->text().trimmed().toInt(&chk1);
	if( mstartshot > mendshot ) return -1;
	if (chk == false || chk1 == false ) return -1;
	return 0;
#endif
}
void ShotResult::timerEvent(QTimerEvent *)
{
	if (mstartshot > mendshot) 
	{
		killTimer(mstid);
		return;
	};

	//makeImage(mstartshot);
	qDebug("shot:%d", mstartshot);

	mstartshot++;
}
void ShotResult::makeImage()
{
	unsigned int shotno = msbShotNum->value();
	QString filename = QString("pcsdata_shot%1.jpg").arg(shotno); 
	QString fullpath =  mfilepath+filename;
	int status = readImage(milbPCS, filename);
	if(status < -1 )
	{
		QString strmsg = QString("Cannot find file:%1").arg(fullpath);
		qDebug("%s", strmsg.toStdString().c_str());
		milbPCS->setPixmap(0);
	};
	filename = QString("daqdata_shot%1.jpg").arg(shotno); 
	fullpath =  mfilepath+filename;
	status = readImage(milbDAQ, filename);
	if(status < -1 )
	{
		QString strmsg = QString("Cannot find file:%1").arg(fullpath);
		qDebug("%s", strmsg.toStdString().c_str());
		milbDAQ->setPixmap(0);
	};
	mshotno = shotno;
	QString stitle = QString("@Shot %1").arg(mshotno);
	mlbTitle->setText(stitle);
	readSession(mshotno);
	readRemark(mshotno);

#if 1 
	if(m_pParent->GetHostname().compare("opi16")==0) 
	{
		filename = QString("shotresult_%1.jpg").arg(shotno); 
		exportImage(filename);
	};
#endif
}
void ShotResult::exportImage(QString filename)
{
	QString format = "jpg";
	QFrame *pframe			= m_pwidget->findChild<QFrame *>("frame");
	mpixmap = QPixmap::grabWidget(pframe);
	QString fullpath =QString("../shotresult/%1").arg(filename);
	mpixmap.save(fullpath, format.toAscii());
	uploadFile(filename);
}

int ShotResult::messageBox(QString msg, const int type)
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
int ShotResult::downloadFile(const QString filename)
{
#if 1
	//QString cmd="ncftpget -u kstar -p kstar2007 da ../screenshot/ /home/kstar/screenshot/" + filename;
	QString cmd="ncftpget -u kstar -p kstar2007 da ../screenshot/ /home/kstar/screenshot/" + filename;
	FILE *fp = popen(cmd.toStdString().c_str(), "r");
	pclose(fp);
#else
	QString proc = "ncftpget";
	QStringList arguments;
	//arguments <<"-u"<<"kstar"<<"-p"<<"kstar2007"<<"da"<<"../screenshot/"<<"/home/kstar/screen/shot/"<<filename;
	//arguments <<"-u"<<"kstar"<<"-p"<<"kstar2007"<<"da"<<"../screenshot/"<<QString("/home/kstar/screenshot/%1").arg(filename);
	arguments <<QString("-u kstar -p kstar2007 da ../screenshot/ /home/kstar/screenshot/%1").arg(filename);
	if( getProc.state()!=QProcess::Running )
	{
		getProc.start(proc, arguments);
		//getProc.waitForStarted();
	}
#endif
	return 0;
}
int ShotResult::uploadFile(const QString filename)
{
#if 1
	QString cmd="ncftpput -u kstar -p kstar2007 web /usr/local/tomcat5/webapps/kstarweb/intranet/shotresult/ ../shotresult/" + filename;
	FILE *fp = popen(cmd.toStdString().c_str(), "r");
	pclose(fp);
#else
	QString proc = "ncftpput";
	QStringList arguments;
	arguments <<"-u"<<"kstar"<<"-p"<<"kstar2007"<<"web"<<"/usr/local/tomcat5/webapps/kstarweb/intranet/shotresult/"
		<<"../screenshot/"<<filename;
	if( putProc.state()!=QProcess::Running )
	{
		putProc.start(proc, arguments);
	}
#endif
	return 0;
}

int ShotResult::readImage(QLabel *imageLabel, QString filename)
{
	QString fullfilepath = mfilepath+filename;
	if ( QFile::exists(fullfilepath) == false )
	{
		QString strmsg= QString("Cannot find file %1").arg(fullfilepath);
		qDebug("%s",strmsg.toStdString().c_str());
		downloadFile(filename);
		if ( QFile::exists(fullfilepath) == false ) return -2;
	};
	QImage image(fullfilepath);
	if (image.isNull()) {
		messageBox("Cannot load image");
		return -1;
	}
	QSize size(imageLabel->size());
	image.scaled(size);
	imageLabel->setPixmap(QPixmap::fromImage(image));
	return 0;
}

void ShotResult::addRemark()
{
	int row = mtw_Shotremark->rowCount();
	mtw_Shotremark->insertRow(row);
	mtw_Shotremark->setRowHeight(row,20);

}
void ShotResult::saveRemark()
{
	int index = mtw_Shotremark->currentRow();
	qDebug("Index:%d",index);
	if (index < 0) return;
	QTableWidgetItem *pItem = mtw_Shotremark->item(index, WRITER);
	string strwriter = pItem->text().toStdString();
	pItem = mtw_Shotremark->item(index, REMARK);
	string strremark = pItem->text().toStdString();
	try {
		sql_datetime wdate;
		QDateTime datetime = QDateTime::currentDateTime();
		wdate.year = datetime.date().year();
		wdate.month = datetime.date().month();
		wdate.day= datetime.date().day();
		wdate.hour=datetime.time().hour();
		wdate.minute=datetime.time().minute();
		wdate.second=datetime.time().second();

		Query query = m_con.query();
		REMARKT remark(0,mshotno,strwriter,strremark,wdate);
		query.insert(remark);
		query.execute();
	} catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
	};
}
void ShotResult::deleteRemark()
{
	int currow = mtw_Shotremark->currentRow();
	if(currow < 0 ) return;
	if(messageBox("Are you sure to delete the record ?",APPLYCANCELBOX)==MSGNOK) return;
	map<int,int>::iterator mapIndexiter;
	try {
		mapIndexiter = mmapIndex.find(currow);
		int index = mapIndexiter->second;
		Query query = m_con.query();
		query << "delete from REMARKT where idx=%0";
		query.parse();
		query.use(index);
		readRemark(mshotno);
		//mtw_Shotremark->removeRow(currow);
	} catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
	} catch (const Exception& er) {
		cerr << er.what() << endl;
	};
}
void ShotResult::readSession(unsigned int shotno)
{
	ResUse res;
	Query query = m_con.query();
	query << "select type,tfcurrent,plsmacurrent,ipwidth,edensity,etemp,airpressure,echpow from SHOTSUMMARY where shotnum=%0";
	query.parse();
	res = query.use(shotno);
	Row row;
	string strtype;
	while (row = res.fetch_row())
	{
		strtype = row["type"].get_string();

		double dtfcurrent		= row["tfcurrent"]/1000.0;
		double dplmacurrent		= row["plsmacurrent"]/1000.0;
		double dipwidth			= row["ipwidth"]*1000.0;
		double dedensity		= row["edensity"];
		double detemp			= row["etemp"];
		double dairp			= row["airpressure"];
		double dechp			= row["echpow"];

		QString qstrtfcur	= QString("%1").arg(dtfcurrent, 0, 'f', 1);
		QString qstrairp	= QString("%1").arg(dairp, 0, 'e', 1);
		mleTf ->setText(qstrtfcur);
		mlePr ->setText(qstrairp);
		QString qstrplmacur, qstreden, qstretemp, qstrechp,qstripwidth;
		if (strtype.compare("TS")==0)
		{
			qstreden	= "--";
			qstretemp	= "--";
			qstrechp	= "--";
			qstrplmacur = "--";
			qstripwidth = "--";
		}
		else
		{
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
					qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 1);
				};
			} else {
				qstrplmacur = QString("%1").arg(dplmacurrent, 0, 'f', 1);
				qstreden	= QString("%1").arg(dedensity, 0, 'f', 1);
				qstretemp	= QString("%1").arg(detemp, 0, 'f', 1);
				qstripwidth	= QString("%1").arg(dipwidth, 0, 'f', 1);
			};
			qstrechp	= QString("%1").arg(dechp, 0, 'f', 1);
		};

		mleIp ->setText(qstrplmacur);
		mleIpPulse ->setText(qstripwidth);
		mleNe ->setText(qstreden);
		mleTe ->setText(qstretemp);
		mleEchP ->setText(qstrechp);
	};
}
void ShotResult::resetWindow()
{
	m_pwidget->setGeometry(0,0,1046,800);

}
void ShotResult::autoManual()
{
	QPushButton *pbut = qobject_cast<QPushButton *>(sender());

	QString strName = pbut->text();
	if(strName.compare("Auto")==0)
	{
		pbut->setText("Manual");
		mauto = true;
	}
	else if(strName.compare("Manual")==0)
	{
		pbut->setText("Auto");
		mauto = false;
	};
}
void ShotResult::readRemark(unsigned int shotno)
{
	Query query = m_con.query();
	ResUse res;
	string strquery = "select idx, shotnum, writer, remark,wdate from REMARKT where shotnum=%0";
	query << strquery;
	query.parse();
	res = query.use(shotno);
	Row row;
	QTableWidgetItem *pItem = 0;
	QDateTime wdate;
	mtw_Shotremark->setRowCount(0);
	int index = 0;
	mmapIndex.clear();
	while (row = res.fetch_row())
	{
		index = mtw_Shotremark->rowCount();
		mtw_Shotremark->insertRow(index);
		mtw_Shotremark->setRowHeight(index,20);

		mmapIndex[index]= row["idx"];

		pItem = new QTableWidgetItem(row["writer"].get_string().c_str());
		pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
		mtw_Shotremark->setItem(index, WRITER, pItem);

		pItem = new QTableWidgetItem(row["remark"].get_string().c_str());
		pItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		mtw_Shotremark->setItem(index, REMARK, pItem);

		wdate = QDateTime::fromString(row["wdate"].get_string().c_str(),"yyyy-MM-dd hh:mm:ss");
		pItem = new QTableWidgetItem(wdate.toString("yy-MM-dd hh:mm:ss").toStdString().c_str());
		pItem->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
		mtw_Shotremark->setItem(index, WDATE, pItem);
	};
	mtw_Shotremark->update();
}

void ShotResult::Update(unsigned int shotno)
{
	emit resetwin();
	msbShotNum->setValue(shotno);
	emit update();
}
void ShotResult::setParent(QWidget *parent)
{
	m_pParent = (SessionSummary*)parent;
	m_con = m_pParent->getConnection();
}
bool ShotResult::isShow()
{
	return (m_pwidget->isVisible());
}
void ShotResult::Show()
{
	m_pwidget->show();
}
void ShotResult::Hide()
{
	m_pwidget->hide();
}
void ShotResult::hideWindow()
{
	m_pwidget->hide();
}
