#include "logsetup.h"

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

	regsignals();
	mpLogSave = new LogSaveThr(this);
	startTimer(500);
}

void CyberLogSetup::timerEvent(QTimerEvent *event)
{
	QTime curtime = QTime::currentTime();
	qDebug("curTime:%s", curtime.toString().toStdString().c_str());
}

void CyberLogSetup::regsignals()
{
	if(!m_plogsetup) return;

	mleTestID = m_plogsetup->findChild<QLineEdit *>("leTestID");
	mleProName = m_plogsetup->findChild<QLineEdit *>("leProName");

	mrbAuto = m_plogsetup->findChild<QRadioButton *>("rbAuto");
	mrbManual = m_plogsetup->findChild<QRadioButton *>("rbManual");

	mrbSetEnd = m_plogsetup->findChild<QRadioButton *>("rbSetEnd");
	mrbSetTimer = m_plogsetup->findChild<QRadioButton *>("rbSetTimer");

	msbMin = m_plogsetup->findChild<QSpinBox *>("sbMin");
	msbSec = m_plogsetup->findChild<QSpinBox *>("sbSec");

	mdteStart = m_plogsetup->findChild<QDateTimeEdit *>("dteStart");
	mdteStop = m_plogsetup->findChild<QDateTimeEdit *>("dteEnd");

	mteSetTimer = m_plogsetup->findChild<QTimeEdit *>("teSetTimer");

	mpbSave = m_plogsetup->findChild<QPushButton *>("pbSave");
	mpbStop = m_plogsetup->findChild<QPushButton *>("pbStop");

}
