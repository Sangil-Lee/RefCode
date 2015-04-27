#include "shotsummary.h"
#include "shotresult.h"

using namespace mysqlpp;

ShotResult::ShotResult():m_con(false),mfilepath("../screenshot/"),getProc(),putProc()
{
	init();
	getUiObject();
	registerSignal();
	//mpThr = new ShotResultThr(this);
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
	//m_pwidget->showMaximized();
	m_pwidget->show();
	file.close();

	string dbname   = "kstarweb";
	string hostname = "203.230.119.122";
	string user     = "root";
	string passwd   = "kstar2004";
	if ( get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
	};
	if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, m_con) )
	{
		cerr<< "MySQL Database Connection Failed. Check Database Server or Network." << endl;
	};
}

void ShotResult::getUiObject()
{
	milbPCS	= m_pwidget->findChild<QLabel *>("ilbPCS");
	milbDAQ	= m_pwidget->findChild<QLabel *>("ilbDAQ");
	mpbMake	= m_pwidget->findChild<QPushButton *>("pbMake");
	msbShotNum = m_pwidget->findChild<QSpinBox *>("sbShotNum");

	milbPCS->setBackgroundRole(QPalette::Base);
	milbPCS->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	milbPCS->setScaledContents(true);
	milbDAQ->setBackgroundRole(QPalette::Base);
	milbDAQ->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	milbDAQ->setScaledContents(true);

	//readImage(milbPCS, "pcsdata_shot984.jpg");
	//readImage(milbDAQ, "daqdata_shot984.jpg");
}

void ShotResult::registerSignal()
{
	connect(mpbMake, SIGNAL(clicked()), this, SLOT(makeImage()));
	//connect(msbShotNum, SIGNAL(valueChanged(int)), this, SLOT(loadImage(int)));
}

void ShotResult::loadImage(int shotno)
{
	readImage(milbPCS, QString("pcsdata_shot%1.jpg").arg(shotno));
	readImage(milbDAQ, QString("daqdata_shot%1.jpg").arg(shotno));
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
		messageBox(strmsg);
		return;
	}
	filename = QString("daqdata_shot%1.jpg").arg(shotno); 
	fullpath =  mfilepath+filename;
	status = readImage(milbDAQ, filename);
	if(status < -1 )
	{
		QString strmsg = QString("Cannot find file:%1").arg(fullpath);
		messageBox(strmsg);
		return;
	};

	filename = QString("shotresult_%1.jpg").arg(shotno); 
	exportImage(filename);
	//ncftpget fileimage or nfs mount 
	//make image 
	//send to webserver or nfs file generation.
}
void ShotResult::exportImage(QString filename)
{
	QString format = "jpg";
	QFrame *pframe			= m_pwidget->findChild<QFrame *>("frame");
	QTableWidget *ptbwidget = m_pwidget->findChild<QTableWidget *>("tw_SesInfo");
	QRect rect = m_pwidget->rect();
	rect.setHeight(pframe->height()+ptbwidget->height()+60);
	mpixmap = QPixmap::grabWidget(m_pwidget,rect);

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
	//ncftpput -u kstar -p kstar2007 203.230.119.122 /usr/local/tomcat5/webapps/kstarweb/screenshot 
	// /usr/local/opi/screenshot/tms.jpg
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
	//imageLabel->adjustSize();
	return 0;
}
