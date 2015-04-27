#include <cadef.h>
#include <dbDefs.h>

#include "caScheduler.h"

#if 1
//using namespace Ui;
void Ui_Dialog::Accept()
{
	//QVariant varSchedules = pScheduler->property("schedules");
	//QStringList schedulelist = varSchedules.toStringList();
	QStringList schedulelist;
	int itemcount = LW_Schedule->count();

	for(int i = 0; i < itemcount;++i)
	{
		QListWidgetItem* pItem = LW_Schedule->item(i);
		schedulelist.append(pItem->text());
	};

	QVariant newVarSchedules = QVariant::fromValue(schedulelist);
	pScheduler->setProperty("schedules",newVarSchedules);
	delete this;
}

void Ui_Dialog::Reject()
{
	delete this;
	//close();
	//reject();
}
void Ui_Dialog::Display(QListWidgetItem* pItem)
{
	QString strSched = pItem->text();
	qDebug("Schedule: %s", strSched.toStdString().c_str());
	
	char str[strSched.toStdString().size()];
	strcpy (str, strSched.toStdString().c_str());
	char *pch = strtok (str,":");
	while (pch != NULL)
	{
		//qDebug("Token:%s", pch);
		if(strcmp(pch,"Datetime") == 0)
		{
			RB_Datetime->setChecked(true);
			pch = strtok (NULL, ":");
			QDateTime datetime = QDateTime::fromString (pch, "yyyy/MM/dd hh-mm-ss" );
			DTE_Datetime->setDateTime(datetime);
			pch = strtok (NULL, ":");
			LE_PVname->setText(pch);
			pch = strtok (NULL, ":");
			LE_REValue->setText(pch);
		}
		else if(strcmp(pch,"Day") == 0)
		{
			RB_Day->setChecked(true);
			pch = strtok (NULL, " ");
			int type = 0;
			QString sday = pch;
			if(sday.compare("Everyday")==0)			type = 0;
			else if(sday.compare("Monday")==0)		type = 1;
			else if(sday.compare("Tuesday")==0)		type = 2;
			else if(sday.compare("Wednesday")==0)	type = 3;
			else if(sday.compare("Thursday")==0)	type = 4;
			else if(sday.compare("Friday")==0)		type = 5;
			else if(sday.compare("Saturday")==0)	type = 6;
			else if(sday.compare("Sunday")==0)		type = 7;
			CB_Daily->setCurrentIndex(type);
			pch = strtok (NULL, ":");
			QTime dayTime = QTime::fromString(pch,"hh-mm-ss");
			TE_Time->setTime(dayTime);
			pch = strtok (NULL, ":");
			LE_PVname->setText(pch);
			pch = strtok (NULL, ":");
			LE_REValue->setText(pch);
		}
		else if(strcmp(pch,"Every secs") == 0)
		{
			RB_Secs->setChecked(true);
			pch = strtok (NULL, ":");
			QString svalue = pch;
			SB_Secs->setValue(svalue.toInt());
			pch = strtok (NULL, ":");
			LE_PVname->setText(pch);
			pch = strtok (NULL, ":");
			LE_REValue->setText(pch);
		}
		else
		{
			pch = strtok (NULL, ":");
		};
	};
}

void Ui_Dialog::ReadSchedule()
{
	if(!pScheduler) return;
	QVariant varSchedules = pScheduler->property("schedules");
	QStringList schedulelist = varSchedules.toStringList();

	for(int i = 0; i < schedulelist.size();++i)
	{
		//schedulelist.at(i);
		LW_Schedule->addItem(schedulelist.at(i));
	};
	if(pScheduler->IsTimerRun())
	{
		PB_Start->setChecked(true);
		PB_Start->setText("Started");
	}
	else
	{
		PB_Start->setChecked(false);
		PB_Start->setText("Start");
	};
};

void Ui_Dialog::EditSchedule()
{
	QListWidgetItem *pItem = LW_Schedule->currentItem();
	if(!pItem) return;

	QString list=schedlist();
	pItem->setText(list);
}

void Ui_Dialog::StartSchedule(bool bstart)
{
	if(!pScheduler) return;

	if(bstart)
	{
		PB_Start->setText("Started");
		pScheduler->Start();
	}
	else
	{
		PB_Start->setText("Start");
		pScheduler->Stop();
	};
}

void Ui_Dialog::DeleteSchedule()
{
	QListWidgetItem *pItem = LW_Schedule->currentItem();
	if(!pItem) return;
	delete pItem;
}

QString Ui_Dialog::schedlist()
{
	QDateTime datetime = DTE_Datetime->dateTime();
	QTime time = TE_Time->time();

	QString spvname = LE_PVname->text();
	QString svalue = LE_REValue->text();

	QString stype;
	QString sdtime;
	if(RB_Datetime->isChecked())
	{
		stype = RB_Datetime->text();
		sdtime = datetime.toString("yyyy/MM/dd hh-mm-ss");
	}
	else if(RB_Day->isChecked())
	{
		stype = RB_Day->text();
		QString sdaily = CB_Daily->currentText();
		QString stime = time.toString("hh-mm-ss");
		sdtime = sdaily+" "+stime;
	}
	else 
	{
		stype = RB_Secs->text();
		QVariant var(SB_Secs->value());
		sdtime = var.toString();
	};

	QString list = stype+":"+sdtime+":"+spvname+":"+svalue;
	return list;
}
void Ui_Dialog::Schedule()
{
	qDebug("Add List(%s)", schedlist().toStdString().c_str());
	LW_Schedule->addItem(schedlist());
};
#endif

class CAScheduler::PrivateData
{
public:
    PrivateData():m_timerid(0),m_btimerflag(false)
    {
    };

	QStringList m_pvlist;
	QStringList m_values;
	QStringList m_schedules;
	int m_timerid;
	bool m_btimerflag;
};

//
CAScheduler::CAScheduler(QWidget *parent): QLabel(parent)
{
    p_data = new PrivateData;
	//startTimer(1000);
};

CAScheduler::~CAScheduler()
{
	delete p_data;
	//U should not delete pdlg in here  
	//delete pdlg;
};

void CAScheduler::Start()
{
	killTimer(p_data->m_timerid);
	p_data->m_timerid = startTimer(1000);
};

void CAScheduler::Stop()
{
	killTimer(p_data->m_timerid);
	p_data->m_btimerflag = false;
};

bool CAScheduler::IsTimerRun()
{
	return p_data->m_btimerflag;
};

void CAScheduler::ShowDialog()
{
	pdlg = new Ui::Dialog(this);
	//Ui::Dialog dlg;
};

int CAScheduler::caPut(const QString pvname, const double &value, int type)
{
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	qDebug("PVName:%s, Value:%f", pvname.toStdString().c_str(), value);

	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	switch(type)
	{
		case DBR_LONG:
			{
				long longval = (long) value;
				status = ca_put(DBR_LONG, chan_id, &longval);
			};
			break;
		case DBR_FLOAT:
			{
				float fval = (float) value;
				status = ca_put(DBR_FLOAT, chan_id, &fval);
			};
			break;
		case DBR_DOUBLE:
			{
				double dval = value;
				status = ca_put(DBR_DOUBLE, chan_id, &dval);
			};
			break;
		case DBR_CHAR:
			{
				char cval = (char)value;
				status = ca_put(DBR_CHAR, chan_id, &cval);
			};
			break;
		case DBR_INT:
		default:	
			{
				int intval = (int)value;
				status = ca_put(DBR_INT, chan_id, &intval);
			};
			break;
	};

	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPut timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	return status;
};
int CAScheduler::caPutString(const QString pvname, const QString value)
{
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	qDebug("PVname:%s, PutValue:%s",pvname.toStdString().c_str(), value.toStdString().c_str());

	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	const char *sval = value.toStdString().c_str();
	status = ca_put(DBR_STRING, chan_id, sval);
	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPutString timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	return status;
};

const QStringList CAScheduler::getSchedules() const
{
	return p_data->m_schedules;
}
void CAScheduler::setSchedules(const QStringList schedulelist)
{
	p_data->m_schedules=schedulelist;
}

const QStringList CAScheduler::getPVs() const
{
	return p_data->m_pvlist;
}

void CAScheduler::setPVs(const QStringList pvlist)
{
	p_data->m_pvlist = pvlist;
}

const QStringList CAScheduler::getValues() const
{
	return p_data->m_values;
}

void CAScheduler::setValues(const QStringList values)
{
	p_data->m_values = values;
}

void CAScheduler::timerEvent (QTimerEvent * /*event*/)
{
	QDateTime datetime = QDateTime::currentDateTime();
	QVariant varSchedules = property("schedules");
	QStringList schedulelist = varSchedules.toStringList();
	QDateTime reserveddatetime;
	p_data->m_btimerflag = true;

	QTime time;
	char * pch = NULL;
	//qDebug("Datetime: %s, Time:%d(sec)",datetime.toString("yyyy/MM/dd hh:mm:ss").toStdString().c_str(), time.secsTo(datetime.time()) );
	for(int i = 0; i < schedulelist.size();++i)
	{
		//qDebug("schedule:%s", schedulelist.at(i).toStdString().c_str());
		char str[schedulelist.at(i).toStdString().size()];
		strcpy (str, schedulelist.at(i).toStdString().c_str());
		//Time type
		pch = strtok (str,":");
		while (pch != NULL)
		{
			//qDebug("Token:%s", pch);
			if(strcmp(pch,"Datetime") == 0)
			{
				//DateTime
				pch = strtok (NULL, ":");
				reserveddatetime = QDateTime::fromString (pch, "yyyy/MM/dd hh-mm-ss" );
				//qDebug("Cur:%d, Res:%d",datetime.toTime_t(),reserveddatetime.toTime_t());
				if(datetime.toTime_t() == reserveddatetime.toTime_t())
				{
					pch = strtok (NULL, ":");
					QString pvname = pch;
					pch = strtok (NULL, ":");
					QString pvvalue = pch;
					//qDebug("PVName:%s, Value:%s", pvname.toStdString().c_str(), pvvalue.toStdString().c_str());
					if(pvname.contains("RCOMMANDER", Qt::CaseInsensitive)==true)
					{
						caPutString(pvname, pvvalue);
					}
					else
					{
						double dvalue = pvvalue.toDouble();
						caPut(pvname, dvalue, DBR_DOUBLE);
					};
				}
				else
				{
					pch = strtok (NULL, ":");
				};
			}
			else if(strcmp(pch,"Day") == 0)
			{
				//Day
				pch = strtok (NULL, " ");
				int type = 0;
				QString sday = pch;
				if(sday.compare("Everyday")==0)			type = 0;
				else if(sday.compare("Monday")==0)		type = 1;
				else if(sday.compare("Tuesday")==0)		type = 2;
				else if(sday.compare("Wednesday")==0)	type = 3;
				else if(sday.compare("Thursday")==0)	type = 4;
				else if(sday.compare("Friday")==0)		type = 5;
				else if(sday.compare("Saturday")==0)	type = 6;
				else if(sday.compare("Sunday")==0)		type = 7;
				int weektype = datetime.date().dayOfWeek();
				if(type==0) weektype=0;

				//Time
				pch = strtok (NULL, ":");
				QTime dayTime = QTime::fromString(pch,"hh-mm-ss");
				QTime curdayTime = datetime.time();
				QTime conTime;

				int setTime = conTime.secsTo(dayTime);
				int curTime = conTime.secsTo(curdayTime);

				if((type == weektype) && (setTime==curTime)) 
				{
					pch = strtok (NULL, ":");
					QString pvname = pch;
					pch = strtok (NULL, ":");
					QString pvvalue = pch;
					if(pvname.contains("RCOMMANDER", Qt::CaseInsensitive)==true)
					{
						caPutString(pvname, pvvalue);
					}
					else
					{
						double dvalue = pvvalue.toDouble();
						caPut(pvname, dvalue, DBR_DOUBLE);
					};
				}
				else
				{
					pch=strtok(NULL,":");
				};
			}
			else if(strcmp(pch,"Every secs") == 0)
			{
				pch = strtok (NULL, ":");
				int dailysec = time.secsTo(datetime.time());
				int secs = atoi(pch);
				if((dailysec % secs) == 0 ) 
				{
					pch = strtok (NULL, ":");
					QString pvname = pch;
					pch = strtok (NULL, ":");
					QString pvvalue = pch;
					if(pvname.contains("RCOMMANDER", Qt::CaseInsensitive)==true)
					{
						caPutString(pvname, pvvalue);
					}
					else
					{
						double dvalue = pvvalue.toDouble();
						caPut(pvname, dvalue, DBR_DOUBLE);
#if 0
						//for test
						static int counttest = 0;
						double dvalue = pvvalue.toDouble();
						dvalue += counttest++;
						caPut(pvname, dvalue, DBR_DOUBLE);
#endif
					};
				}
				else
				{
					pch = strtok (NULL, ":");
				};
			}
			else
			{
				pch = strtok (NULL, ":");
			};
		};
	};
}
