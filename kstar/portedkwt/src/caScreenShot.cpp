#include "caScreenShot.h"

class CAScreenShot::PrivateData
{
public:
    PrivateData()	//: initializ3
    {
    };
	QString m_prefix;
	QString m_pvname;
    int m_number;
};

//
CAScreenShot::CAScreenShot(QWidget *parent): QWidget(parent)
{
    p_data = new PrivateData;
};

CAScreenShot::~CAScreenShot()
{
};

void CAScreenShot::fileNameDateTime()
{
	QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
	QString filename = QString("/home/kstar/shotresult/")+curtime+".jpg";
	QString format = "jpg";
	//QPixmap mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	//QPixmap mpixmap = QPixmap::grabWidget(parent());
	QPixmap mpixmap = QPixmap::grabWidget((QWidget*)parent());
	//mpixmap.save(filename, format.toAscii());
	mpixmap.save(filename, format.toStdString().c_str());
};

void CAScreenShot::fileName(const QString &name)
{
	QString filename = QString("/home/kstar/shotresult/")+name+".jpg";
	QString format = "jpg";
	//QPixmap mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	QPixmap mpixmap = QPixmap::grabWidget((QWidget*)parent());
	mpixmap.save(filename, format.toStdString().c_str());
};

void CAScreenShot::changeValue (const short & /*connstatus*/, const double &value)
{
	if(value == 1.0) screenShot(p_data->m_number);
	//screenShot(p_data->m_number);
};

void CAScreenShot::setPrefix(const QString &prefix)
{
	    p_data->m_prefix = prefix;
}

const QString &CAScreenShot::getPrefix() const
{
	    return p_data->m_prefix;
}

const QString &CAScreenShot::getPvname() const
{
	return p_data->m_pvname;
}

void CAScreenShot::setPvname(const QString &name)
{
    p_data->m_pvname = name;
}

void CAScreenShot::fileNamePostfix(const int number)
{
	p_data->m_number = number;
}

void CAScreenShot::fileNamePostfix(const double number)
{
	p_data->m_number = static_cast<int> (number);
}

void 
CAScreenShot::screenShot(const int shotno)
{
	QString strShotno = QString("%1").arg(shotno);
	//QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
	//QString filename = QString("/home/kstar/shotresult/")+getPrefix()+QString("_")+strShotno+QString("_")+curtime+".jpg";
	QString filename = QString("/home/kstar/shotresult/")+getPrefix()+QString("_")+strShotno+".jpg";
	qDebug("screeShot Filename:%s", filename.toStdString().c_str());
	QString format = "jpg";
	//QPixmap mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	QPixmap mpixmap = QPixmap::grabWidget((QWidget*)parent());
	mpixmap.save(filename, format.toStdString().c_str());
}
