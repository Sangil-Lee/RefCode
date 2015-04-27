// vim: expandtab

#include "caListBox.h"


class CAListBox::PrivateData
{
public:
    PrivateData()
    {
    };
	QString m_pvname;
    ProcessMode m_procmode;
    QString m_prefix;
};

CAListBox::CAListBox(QWidget *parent):QListWidget(parent)
{
    init();
}

CAListBox::~CAListBox()
{
    delete d_data;
}

QSize CAListBox::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAListBox::minimumSizeHint() const
{
	int mw = 120;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAListBox::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAListBox::getPvname() const
{
	return d_data->m_pvname;
}

CAListBox::ProcessMode CAListBox::getProcessmode() const
{
	    return d_data->m_procmode;
}

void CAListBox::setProcessmode(ProcessMode mode)
{
	    d_data->m_procmode = mode;
}

void CAListBox::init()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

   	setObjectName(QString::fromUtf8("CAListBox"));
	setPvname("pvname");
}

int CAListBox::bitCheck(unsigned int src, unsigned int org)
{
    return (src&org) == 0 ? 0 : 1;
};

void CAListBox::setPrefix(const QString &prefix)
{
    d_data->m_prefix = prefix;
}

const QString &CAListBox::getPrefix() const
{
	return d_data->m_prefix;
}

void CAListBox::changeValue (const short &connstatus, const double &value, const short& /*severity*/)
{
	QString str;
    clear();
    QString prefix = getPrefix().toUpper();
	if (connstatus != ECA_CONN)
	{
		str = QString("Disconnected");
		addItem(str);
	}
    else
    {
        if(getProcessmode()==CAListBox::User)
        {
            unsigned int uvalue = static_cast<unsigned int> (value);
            if(bitCheck(uvalue,CAListBox::TMS)==1)
            {
                addItem("TMS [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::VMS)==1)
            {
                addItem("VMS [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::ECH)==1)
            {
                addItem("ECH [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::SCS)==1)
            {
                addItem("SCS [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::FUEL)==1)
            {
                addItem("FUEL [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::GCDS)==1)
            {
                addItem("GCDS [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::ICRH)==1)
            {
                addItem("ICRH [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::CSM)==1)
            {
                addItem("CSM [RUN]");
            }
            if(bitCheck(uvalue,CAListBox::MPS)==1)
            {
                addItem("MPS [RUN]");
            }
            if(prefix.compare("CHARCH")==0)
            {
                if(bitCheck(uvalue,CAListBox::TMSENGINE)==1)
                {
                    addItem("TMSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::VMSENGINE)==1)
                {
                    addItem("VMSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::CLSENGINE)==1)
                {
                    addItem("CLSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::HDSENGINE)==1)
                {
                    addItem("HDSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::HRSENGINE)==1)
                {
                    addItem("HRSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::ICSENGINE)==1)
                {
                    addItem("ICSENGINE [RUN]");
                }
                if(bitCheck(uvalue,CAListBox::PMSENGINE)==1)
                {
                    addItem("PMSENGINE [RUN]");
                }
            }
        }
        else if(getProcessmode()==CAListBox::System)
        {
            unsigned int uvalue = static_cast<unsigned int> (value);
            if(bitCheck(uvalue,CAListBox::CAREPEATER)==0)
            {
                QListWidgetItem *pItem = new QListWidgetItem;
                pItem->setText("caRepeater [OK]");
                addItem(pItem);
            }
            else if(bitCheck(uvalue,CAListBox::CAREPEATER)==1)
            {
                QListWidgetItem *pItem = new QListWidgetItem;
                pItem ->setForeground(QBrush(Qt::red));
                pItem->setText("caRepeater [NOK]");
                addItem(pItem);
            }
            if(bitCheck(uvalue,CAListBox::NTPD)==0)
            {
                QListWidgetItem *pItem = new QListWidgetItem;
                pItem->setText("ntpd [OK]");
                addItem(pItem);
            }
            else if(bitCheck(uvalue,CAListBox::NTPD)==1)
            {
                QListWidgetItem *pItem = new QListWidgetItem;
                pItem ->setForeground(QBrush(Qt::red));
                pItem->setText("ntpd [NOK]");
                addItem(pItem);
            };

            qDebug("prefix:%s", prefix.toStdString().c_str());
            //additional processlist each server
            if(prefix.compare("KSTARWEB")==0)
            {
                if(bitCheck(uvalue,CAListBox::HTTPD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("httpd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::HTTPD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("httpd [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::JAVA)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("java [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::JAVA)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("java [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::MYSQLD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mysqld [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MYSQLD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mysqld [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("CHARCH")==0 )
            {
                if(bitCheck(uvalue,CAListBox::MMFSD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mmfsd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MMFSD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mmfsd [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("BACKUP")==0 )
            {
                if(bitCheck(uvalue,CAListBox::MMFSD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mmfsd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MMFSD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mmfsd [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::NSRD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("nsrd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::NSRD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("nsrd [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("MDSPLUS")==0 )
            {
                if(bitCheck(uvalue,CAListBox::MMFSD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mmfsd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MMFSD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mmfsd [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::MDSIP)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mdsip [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MDSIP)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mdsip [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("DATA_ANAL")==0 )
            {
                if(bitCheck(uvalue,CAListBox::MMFSD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mmfsd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MMFSD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mmfsd [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::MDSIP)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mdsip [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MDSIP)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mdsip [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::HTTPD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("httpd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::HTTPD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("httpd [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("GAWEB")==0 )
            {
                if(bitCheck(uvalue,CAListBox::HTTPD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("httpd [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::HTTPD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("httpd [NOK]");
                    addItem(pItem);
                }
                if(bitCheck(uvalue,CAListBox::MYSQLD)==0)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("mysqld [OK]");
                    addItem(pItem);
                }
                else if(bitCheck(uvalue,CAListBox::MYSQLD)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("mysqld [NOK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("CAGATEWAY")==0 )
            {
                if(bitCheck(uvalue,CAListBox::GATEWAY)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("gateway [NOK]");
                    addItem(pItem);
                }
                else 
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("gateway [OK]");
                    addItem(pItem);
                }
            }
            else if(prefix.compare("ECH")    ==0 
                    || prefix.compare("GCDS")==0 
                    || prefix.compare("SXR") ==0 
                    || prefix.compare("ER")  ==0 
                    || prefix.compare("RBA") ==0 
                    //|| prefix.compare("TMS1")==0 
                    //|| prefix.compare("TMS2")==0 
                    )
            {
                if(bitCheck(uvalue,CAListBox::NIMXS)==1)
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem ->setForeground(QBrush(Qt::red));
                    pItem->setText("nimxs [NOK]");
                    addItem(pItem);
                }
                else 
                {
                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setText("nimxs [OK]");
                    addItem(pItem);
                }
            }
        };
    };

}

