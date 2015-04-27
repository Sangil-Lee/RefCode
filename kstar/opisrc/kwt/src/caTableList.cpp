/*************************************************************************
 * KSTAR Widget Toolkit (KWT) V 1.0
 * This is Qt-based graphic application development toolkit for EPICS
 * and it was designed to develop operator interfaces to control KSTAR.
 *
 * Copyright (c) 2010 The National Fusion Research Institute
 * National Fusion Research Institute (NFRI)
 * 113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 * KWT is distributed under the terms of the GNU General Public License.
*************************************************************************/

/* caTableList.cpp
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 * Revision 1  2013-06
 * Author: leesi
 * Initial revision
 *
 * ---------------------------------------------------------------------------
 *  * DESCRIPTION *
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 *  * System Include Files *
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 *  * User Type Definitions *
 * ---------------------------------------------------------------------------
 */

//#include "caTableList.h"
#include "qtchaccesslib.h"
#include "pv.h"

//extern void qt_x11_set_global_double_buffer(bool);

class CATableList::PrivateData
{
public:
    PrivateData():m_bcontrol(true)
    {
    };

	bool m_bcontrol;
	int mrow;
	QString m_pvname;
};

CATableList::CATableList(QWidget *parent):QTableWidget(1, 2, parent)
{
    init();
}

CATableList::~CATableList()
{
    delete d_data;
}
int CATableList::caPut(const QString pvname, const QString &value)
{
	chid  chan_id;
	int status = ca_search_and_connect(pvname.toStdString().c_str(), &chan_id, NULL, NULL);
	SEVCHK(status,NULL);
	status = ca_pend_io(2.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	bool ok;
	double dval = value.toDouble(&ok);
	int type = 0;

	if(ok==true) type = DBR_DOUBLE;
	else type = DBR_STRING;

	switch(type)
	{
		case DBR_INT:
			{
				int intval = value.toInt();
				status = ca_put(DBR_INT, chan_id, &intval);
			}
			break;
		case DBR_LONG:
			{
				long longval = (long) value.toLong();
				status = ca_put(DBR_LONG, chan_id, &longval);
			}
			break;
		case DBR_FLOAT:
			{
				float fval = (float) value.toFloat();
				status = ca_put(DBR_FLOAT, chan_id, &fval);
			}
			break;
		case DBR_STRING:
			{
				const char *sval = value.toStdString().c_str();
				status = ca_put(DBR_STRING, chan_id, sval);
			}
			break;
		case DBR_DOUBLE:
		default:
			{
				double dval = value.toDouble();
				status = ca_put(DBR_DOUBLE, chan_id, &dval);
			}
			break;
	};

	status = ca_pend_io(2.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPut timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	//qDebug("----------------------------------------------------- caPut() end");
	return status;
};
#if 1
QSize CATableList::sizeHint() const
{
	int mw = 20;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

QSize CATableList::minimumSizeHint() const
{
	int mw = 200;
	int mh = 120;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}
#endif

void CATableList::init()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;
    setObjectName(QString::fromUtf8("CATableList"));

	QStringList headerlabel;
	headerlabel << "On/Off" << "Tag Name";
	setHorizontalHeaderLabels(headerlabel);
	setMouseTracking(true);
	setSelectionMode(QAbstractItemView::NoSelection);
	//QTimer::singleShot(1000, this, SLOT(regSignal()));
}
void CATableList::regSignal()
{
	//qDebug("Connected Signal");
	connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(changeValue(int, int)));
	connect(this, SIGNAL(SIGTableUpdate()), this, SLOT(SLOTTableUpdate()));
}
void CATableList::SLOTTableUpdate()
{
	//update();
	//repaint();
	//Instead of update/repaint.
	hide();
	show();
}

//void CATableList::InsertHash(const QString pvname, QTableWidgetItem *pItem)
void CATableList::InsertHash(QTableWidgetItem *pItem, QString pvname)
{
	//hashWidgetTable.insert(pvname, pItem);
	hashWidgetTable.insert(pItem, pvname);
}

#if 0
QTableWidgetItem * CATableList::FindItem(QString pvname)
{
	return hashWidgetTable[pvname];
}
#else
QString CATableList::FindProcessVariable(QTableWidgetItem *pItem)
{
	return hashWidgetTable[pItem];
}
#endif

void CATableList::TableUpdate()
{
	emit SIGTableUpdate();
}

void CATableList::changeValue (const int row, const int col)
{
	//qDebug("Row(%d),Col(%d)", row, col);
	QTableWidgetItem *pItem = item(row, col);
	if(pItem == 0) return;

	QString pvname = FindProcessVariable(pItem);
	if(pvname.isEmpty() == true) return;

	QString strValue = pItem -> text();
	caPut(pvname,strValue);
}

void CATableList::changeOnOffValue(const int row, const int col)
{
#if 0
	pvSystem *sys = newPvSystem("ca");
	QString onoffpv = getOnOffPVList().at(row);

	pvVariable *var = sys->newVariable(onoffpv.toStdString().c_str());
	sys->pend(1.0);

	QTableWidgetItem *pItem = item(row, col);
	int intval = pItem->text().toInt();
	qDebug("OnOff List:%s(%d)", onoffpv.toStdString().c_str(), intval);
	
	pvValue &value = *(pvValue*)intval;
	int sts = var->put(pvTypeDOUBLE,1, &value);
#else
	//if(row > getOnOffPVList().size()) return;
	//QString onoffpv = getOnOffPVList().at(row);

	QTableWidgetItem *pItem = item(row, col);
	QString pvname = FindProcessVariable(pItem);
	if(pvname.isEmpty() == true) return;
	QString strValue = pItem -> text();
	caPut(pvname,strValue);
#endif
}
void CATableList::changeTagNameValue(const int row, const int col)
{
#if 0
	pvSystem *sys = newPvSystem("ca");
	QString tagpv = getTagPVList().at(row);
	//qDebug("Tag List:%s", tagpv.toStdString().c_str());

	pvVariable *var = sys->newVariable(tagpv.toStdString().c_str());
	sys->pend(1.0);

	QTableWidgetItem *pItem = item(row, col);
	QString strValue = pItem -> text();
	qDebug("Tag List:%s(%s)", tagpv.toStdString().c_str(), strValue.toStdString().c_str());

	pvValue *pvval = new pvValue;
	strcpy(pvval->stringVal[1], strValue.toStdString().c_str());
	//pvValue &value = *(pvValue*)strValue.toStdString().c_str();
	int sts = var->put(pvTypeSTRING,1, pvval);
#else
	//if(row > getTagPVList().size()) return;
	//QString tagpv = getTagPVList().at(row);
	QTableWidgetItem *pItem = item(row, col);
	QString pvname = FindProcessVariable(pItem);
	if(pvname.isEmpty() == true) return;
	QString strValue = pItem -> text();
	caPut(pvname,strValue);
#endif
}

#if 0
const bool &CATableList::getControl() const
{
	return d_data->m_bcontrol;
}

void CATableList::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}
#endif
const QString &CATableList::getPvname() const
{
	return d_data->m_pvname;
}

void CATableList::setPvname(const QString &name)
{
    d_data->m_pvname = name;
}
