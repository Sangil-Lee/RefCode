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

/* caCheckbox.cpp
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 * Revision 1  2010-03
 * Author: Sulhee Baek
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

#include "caCheckbox.h"


class CACheckBox::PrivateData
{
public:
    PrivateData()
    {
    };

	bool m_val;
	QString m_pvname;
	QString m_label;
	bool	m_bcontrol;
	bool m_status;
	QStringList m_pvlist;
};

CACheckBox::CACheckBox(QWidget *parent):QCheckBox(parent)
{
    initCheckBox();
}

CACheckBox::~CACheckBox()
{
    delete d_data;
}

QSize CACheckBox::sizeHint() const
{
	int mw = 20;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

QSize CACheckBox::minimumSizeHint() const
{
	int mw = 10;
	int mh = 10;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CACheckBox::setStatus(const bool status)
{
	d_data->m_status = status;
}
const bool CACheckBox::getStatus()
{
	return d_data->m_status;
}

const QStringList CACheckBox::getPVList() const
{
	return d_data->m_pvlist;
}

void CACheckBox::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}

void CACheckBox::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CACheckBox::getPvname() const
{
	return d_data->m_pvname;
}

void CACheckBox::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}
const bool &CACheckBox::getControl() const
{
    return d_data->m_bcontrol;
}

void CACheckBox::setLabel(const QString &label)
{
    d_data->m_label = label;
	setText(label);
}

const QString &CACheckBox::getLabel() const
{
	return d_data->m_label;
}

void CACheckBox::initCheckBox()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

    setObjectName(QString::fromUtf8("CACheckBox"));
	setPvname("pvname");
	setControl(true);
	setCheckState(Qt::Unchecked);
	
	connect(this, SIGNAL(clicked(bool)), this, SIGNAL(valueChanged(bool)));
}

void CACheckBox::changeValue (const int &value)
{
	bool status = (bool) value;
	if (status)
	{
		setCheckState(Qt::Checked);

	} else
	{
		setCheckState(Qt::Unchecked);
	}
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value,0,10)));
}

void CACheckBox::changePvName (const QString &pvname)
{
    setPvname(pvname);
}
void CACheckBox::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
}
void CACheckBox::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st= false;
	else st = true;
	setEnabled(st);
}
