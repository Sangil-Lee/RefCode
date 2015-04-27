//#include "caPopup.h"
#include "qtchaccesslib.h"

class AttachChannelAccess;

class CAPopUp::PrivateData
{
public:
    PrivateData():m_val(0),m_singleton(0)
    {
    };
	QString m_pvname;
	QString m_filename;
	int m_val;
	int m_singleton;
};

//
CAPopUp::CAPopUp(QWidget *parent): QWidget(parent), bshow(0)
{
    p_data = new PrivateData;
};

CAPopUp::~CAPopUp()
{
};

void CAPopUp::showSingleton ()
{
#if 1
	QString uifile = p_data->m_filename;
	//qDebug("uifile-name:%s", uifile.toStdString().c_str());

	if(uifile.isEmpty() == true || bshow == 0 ) return;
	//qDebug("uifile-name-1:%s", uifile.toStdString().c_str());

	if(p_data->m_singleton != 0) return;

	QUiLoader loader;
	QString filename = QString("/usr/local/opi/ui/") + uifile;
	QFile file;
	file.setFileName(filename);
	file.open(QFile::ReadOnly);
	QDialog *pDia = (QDialog*)loader.load(&file);

	if(!pDia) return;

	connect(pDia, SIGNAL(rejected()), this, SLOT(buttonCancel()));
	QDialogButtonBox *pBBox = pDia->findChild<QDialogButtonBox *> ("buttonBox");

	if(pBBox!=0)
	{
		connect(pBBox, SIGNAL(rejected()), this, SLOT(buttonCancel()));
	}
	
	pDia -> show();
	p_data->m_singleton = 1;
#endif
}
void CAPopUp::buttonCancel()
{
	QDialogButtonBox *pBBox = qobject_cast<QDialogButtonBox *>(sender());
	p_data->m_singleton = 0;
	//qDebug("CAPopUp - singleTon Window closed");
}

void CAPopUp::changeValue (const short &connstatus, const double &value)
{
	p_data->m_val = (int)value;
	if ( p_data->m_val != 0 )
	{
		//AttachChannelAccess *pattachDialog = new AttachChannelAccess(uifile, 1);
		showSingleton();
	};
};

const QString &CAPopUp::getPvname() const
{
	return p_data->m_pvname;
}

void CAPopUp::setPvname(const QString &name)
{
    p_data->m_pvname = name;
}

const QString & CAPopUp::getFilename() const
{
	return p_data->m_filename;
}
void  CAPopUp::setFilename(const QString &filename)
{
	p_data->m_filename = filename;
}
int  CAPopUp::Show()
{
	setShow(1);
	changeValue(0, p_data->m_val);
}
int  CAPopUp::getValue()
{
	return p_data->m_val;
}
