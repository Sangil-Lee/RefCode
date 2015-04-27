/*! qtchaccesslib.cpp
 ***************************************************************************
 * Copyright(C) 2007, National Fusion Research Institute, All rights reserved.
 * Proprietary information, NFRI
 * ***************************************************************************
 *
 * ***** HIGH LEVEL DESCRIPTION **********************************************
 *
 * \brief Module for managing process information
 *
 * ***************************************************************************
 *
 **** REVISION HISTORY *****************************************************
 *
 * Revision 1  2006-12-29
 * Author: leesi
 * Initial revision
 *
 * ***************************************************************************
 * ---------------------------------------------------------------------------
 * System Include Files
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 * User Type Definitions
 * ---------------------------------------------------------------------------
 */
#include "qtchaccesslib.h"
#include "pv.h"
// ---------------------------------------------------------------------------
// Import Global Variables & Function Declarations
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Export Global Variables & Function Declarations
// ---------------------------------------------------------------------------
#define DEBUG 1 

// ---------------------------------------------------------------------------
// Module Variables & Function Declarations
// ---------------------------------------------------------------------------

template <class T> int caPut(const int type, const QString pvname, T &value = 0)
{
	//qDebug("----------------------------------------------------- caPut() start");

	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	SEVCHK(status,NULL);
	status = ca_pend_io(2.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	switch(type)
	{
		case DBR_INT:
			{
				int intval = (int) value;
				status = ca_put(DBR_INT, chan_id, &intval);
			}
			break;
		case DBR_LONG:
			{
				long longval = (long) value;
				status = ca_put(DBR_LONG, chan_id, &longval);
			}
			break;
		case DBR_FLOAT:
			{
				float fval = (float) value;
				status = ca_put(DBR_FLOAT, chan_id, &fval);
			}
			break;
		case DBR_DOUBLE:
			{
				double dval = value;
				status = ca_put(DBR_DOUBLE, chan_id, &dval);
			}
			break;
		case DBR_CHAR:
			{
				char cval = (char) value;
				status = ca_put(DBR_CHAR, chan_id, &cval);
			};
			break;
		default:
			break;
	};

	status = ca_pend_io(2.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPut timed out: Data was not written.\n");
		ca_clear_channel(chan_id);
		return status;
	}

	if(chan_id)
		ca_clear_channel(chan_id);
	//qDebug("----------------------------------------------------- caPut() end");
	return status;
};

int caPutString(const int /*type*/, const QString pvname, QString &value)
{
	//qDebug("----------------------------------------------------- caPutString() start");

	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	SEVCHK(status,NULL);
	status = ca_pend_io(2.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	const char *sval = value.toStdString().c_str();
	//qDebug("PV: %s, value: %s, sval: %s\n",pvname.toStdString().c_str(), value.toStdString().c_str(), sval);
	status = ca_put(DBR_STRING, chan_id, sval);
	status = ca_pend_io(2.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPutString timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	//qDebug("----------------------------------------------------- caPutString() end");
	return status;
};

static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
	//qDebug("----------------------------------------------------- exceptionCallback() start");
    chid	chid = args.chid;
    //long	stat = args.stat; /* Channel access status code*/
    const char  *channel;
    static char *noname = "unknown";
    channel = (chid ? ca_name(chid) : noname);
    if(chid) printChidInfo(chid,"exceptionCallback");
	//qDebug("----------------------------------------------------- exceptionCallback() end");

};

/*
 *	parameters:
 *	uifilename: xxx.ui file
 *	widget: parent widget
 */

AttachChannelAccess::AttachChannelAccess(const QString &uifilename, int index, bool bmonitor, QWidget *widget):
	QWidget(widget), m_filename(uifilename), m_bmon(bmonitor)
{
	m_curindex = -1;
	m_index = index;
	m_control = m_lastcontrol = true;
	init();
};


AttachChannelAccess::AttachChannelAccess(QWidget *widget):
	QWidget(widget),m_pwidget(widget)
{
	//m_pvlists.append("String");
	//QList<QString> pvlists = m_pvlists;
	m_curindex = -1;
	m_index = 0;
	m_control = m_lastcontrol = true;
	m_bmon = false;
	init2();
};

AttachChannelAccess::AttachChannelAccess(QWidget *widget, const QList<QString>& pvlists):
	QWidget(widget),m_pwidget(widget)
{
	QList<QString> m_pvlists = pvlists;
	m_control = m_lastcontrol = true;
	m_bmon = false;
	init3(m_pvlists);
};

AttachChannelAccess::~AttachChannelAccess()
{
	//qDebug("AttachChannelAccess::~AttachChannelAccess called");
	register_chacc.clear();

	// shbaek changed following one line to the next to solve smartptr memory corruption problem
	vecPlotPtr.clear();
	//vecPlotPtr->clear();
	//++leesi, for exit of QThread safely.
#if 1
	if(mChThr->isRunning())
	{
		mChThr->setStop(true);
		mChThr->wait();
	};
#endif
	//qDebug("Thread Running ? : %d", mChThr->isRunning());
	delete mChThr;
}

/*********************************************************
 *	setEnabled(false) for all control widgets
 *********************************************************/
void AttachChannelAccess::setControl(const bool &controlmaster)
{
	//qDebug("----------------------------------------------------- setControl() start");
    	m_control = controlmaster;
		QHash<QString, ChAccess>::const_iterator hash_chacciter_tmp;

	if (m_control == m_lastcontrol)
	{
		//qDebug("\\\\m_control(%d) was not changed.", m_control);
		return;
	}
	else
	{
		//qDebug("%%%%m_control was changed.[%d]->[%d]", m_lastcontrol, m_control);
		ChAccess *pchacc;
		hash_chacciter_tmp = hash_chacc.constBegin();
		//qDebug("size:(%d):.", hash_chacc.size());
		while ( hash_chacciter_tmp != hash_chacc.constEnd() ) 
		{
			pchacc = (ChAccess*)&(hash_chacciter_tmp.value());
			//qDebug("pchacc(%p):pobj(%p)", pchacc, pchacc->pobj);
			QString objName = pchacc->pobj->metaObject()->className();
			QWidget *pobj = (QWidget*) pchacc->pobj;
			QVariant control = pobj->property("control");
			if(control.isValid() && control.toBool())
			{
				pobj->setEnabled(m_control);
			}
			//qDebug("index:%s, opvname:%s, pobj:%p", hash_chacciter_tmp.key().toStdString().c_str(), pchacc->pvname.c_str(), pchacc->pobj);
			++hash_chacciter_tmp;
		}

		m_lastcontrol = m_control;
	}
	//qDebug("----------------------------------------------------- setControl() end");
}

const bool &AttachChannelAccess::getControl() const
{
        return m_control;
}

void 
AttachChannelAccess::PrintData(const double pvalue)
{
	m_data = pvalue;
}

/*  description: eventFilter override function. for mouse event redefine.
 *	parameters:
 *	target: target widget.
 *	event: taget widget event
 */

bool AttachChannelAccess::eventFilter(QObject *target, QEvent *event)
{
	//qDebug("----------------------------------------------------- eventFilter() start");
	QWidget *targetwidget = (QWidget*)target;
	if(targetwidget == 0 ) return false;
	QString tobjName = targetwidget->metaObject()->className();
	if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick 
			|| event->type() == QEvent::MouseButtonRelease ) 
	{
		if(m_bmon == true) return false;
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->buttons() & Qt::RightButton) 
		{
			if ( tobjName.compare("CADisplayer") == 0 )
			{
				QWidget *child = targetwidget->childAt(mouseEvent->x(), mouseEvent->y());
				if (child==0) return false;
				CADisplayer *pDisp = (CADisplayer*)targetwidget;
				QVariant VarPeriodic = pDisp->property("periodicdata");
				int periodic = VarPeriodic.toInt();

				//qDebug("chObjName:%s",child->objectName().toStdString().c_str());
				QString objName = child->objectName();
				if ( objName.compare("lineEdit") == 0 )
				{
					QLineEdit *label = (QLineEdit*)child;
					//SinglePlotWindow *plot = new SinglePlotWindow(label->windowTitle());
					//qDebug("ObjName:%s, Text:%s",objName.toStdString().c_str(), label->text().toStdString().c_str());

					vecPlotPtr.push_back(SinglePlotPtr(new SinglePlotWindow(label->windowTitle(), periodic)));
				}
				else if ( objName.compare("pvName") == 0 )
				{
					//QLabel *pvlabel = (QLabel*)child;
					//qDebug("PvName:%s",pvlabel->text().toStdString().c_str());
				};
			}
			else if ( tobjName.compare("CAGraphic") == 0 )
			{
				CAGraphic *pcagraphic = (CAGraphic*)targetwidget;
				if (pcagraphic->getPasswd() == true) 
				{
					//int value = pcagraphic->getValue();
					Password pword(pcagraphic->getPasswdStr(), new QWidget());
					if(pword.getStatus() == false)
					{
						qDebug("Entered password is wrong");
						return false;
					};
				};

				QRect geom = pcagraphic->geometry();
				bool popup = pcagraphic->getPopup();

				if (popup == true)
				{
					QString filename = pcagraphic->getFilename();
					QList<QString> slist = pcagraphic->getPvs();
					int cntPv=slist.size();
					qDebug("ListSize:%d",cntPv);
					for (int i=0; i<cntPv; i++)
					{
						qDebug("PVnames:%s",slist.at(i).toStdString().c_str());
					}
					qDebug("Filename:%s",filename.toStdString().c_str());
					QUiLoader uiLoader;
					QFile file(filename);
					QWidget *ctlDialog;
					QList<QObject*> clist;
					QVariant control;
					QString pv = pcagraphic->getPvname();
						
					if(!file.exists())
					{
						qDebug("File(%s) is invalid!",filename.toStdString().c_str());
					} else
					{
						ctlDialog = uiLoader.load(&file);
						ctlDialog->setWindowTitle(QString("").append(QString(pv)));
						QRect ctlg = ctlDialog->geometry();
						ctlDialog->move(geom.x()+geom.width()-ctlg.width()+190,geom.y()+geom.height()+95);
						ctlDialog->setGeometry(geom.x()+geom.width()-ctlg.width()+190,geom.y()+geom.height()+95, ctlg.width(), ctlg.height());

						clist = ctlDialog->children();

						for (int j=0; j<clist.size(); j++)
						{
						//	clist.at(j)->setProperty("control", QString("false"));
						}
						if (!ctlDialog)
						{
							qDebug("Failed to create pop-up dialog!");
						}
						else
						{
							AttachChannelAccess *pattachDialog = new AttachChannelAccess(ctlDialog, slist);
						}
					}
					file.close();
				};
			}
			else if ( tobjName.compare("CAScheduler") == 0 )
			{
				CAScheduler *pscheduler = (CAScheduler*)targetwidget;

#if 0
				QVariant varTimes = pscheduler->property("times");
				QStringList times = varTimes.toStringList();
				QDateTime reservetime[times.size()];
				for(int i = 0; i < times.size();++i)
				{
					qDebug("times-1:%s", times.at(i).toStdString().c_str());
					reservetime[i] = QDateTime::fromString (times.at(i), "yyyy/MM/dd hh:mm:ss" );
					reservetime[i].setTime_t(reservetime[i].toTime_t()+60);
					//qDebug("reservetimes-1:%s", reservetime[i].toString().toStdString().c_str());
					times.replace(i, reservetime[i].toString("yyyy/MM/dd hh:mm:ss"));
				};

				QVariant varDatetime = QVariant::fromValue(times);
				pscheduler->setProperty("times",varDatetime);
#else
				pscheduler->ShowDialog();
#endif
			}
		} 
		else if (mouseEvent->buttons() & Qt::LeftButton) 
		{
#if 0
			if ( tobjName.compare("PopupPushButton") == 0 )
			{
				PopupPushButton *popup = (PopupPushButton*)targetwidget;
				QVariant var = popup->property("uifile");
				QString uifile = var.toString();
				qDebug("uifile-name:%s", uifile.toStdString().c_str());
				AttachChannelAccess *pattachDialog = new AttachChannelAccess(uifile, 1);
			};
#endif

		};
		return true;
	}
	else if ( event->type() == QEvent::Close )
	{
		//removePlots(); /*destroy plot threads*/
	}
	//qDebug("----------------------------------------------------- eventFilter() end");
    return QWidget::eventFilter(target, event);
};

void 
AttachChannelAccess::setpvs(const QWidget *pwidget, const QList<QString>& stringlist)
//AttachChannelAccess::setpvs(const QWidget *pwidget)
{
#if 1
	QList<QObject*> objlist = pwidget->children();
	QHash<QString, QWidget*> objHash;
	QHash<QString, QWidget*>::const_iterator j;
	QList<QString> strlist = stringlist;
	QList<QString> pvlist;
	QWidget	*widget = NULL;
	QString strpvName;
	QString clsName;
	QString objName;
	int i;
	//QWidget	*parent = NULL;
	//QString parentclsName;
	//int index=0;
	//int cnt=0;
	for (i = 0; i < objlist.size(); ++i) 
	{
		widget = (QWidget*)objlist.at(i);
		clsName = widget->metaObject()->className();
		objName  = widget->objectName();
		QVariant var = widget->property("pvname");
		//parent = widget->parentWidget();
		//parentclsName = parent->metaObject()->className();
		//qDebug("ClassName:%s", clsName.toStdString().c_str() );

		if( clsName.compare("QFrame") == 0 || clsName.compare("QWidget") == 0 
				|| clsName.compare("QDockWidget") == 0 
				|| clsName.compare("QGroupBox") == 0)
		{
			setpvs(widget, strlist);
		};

		if( clsName.compare("QTabWidget") == 0 )
		{
			QTabWidget *tabwidget = (QTabWidget*)widget;
			for(int t=0; t<tabwidget->count(); t++)
			{
				//qDebug("Current index of the tab:%d from %d", t+1, tabwidget->count());
				setpvs(tabwidget->widget(t), strlist);
			}
		}

	    	if( clsName.compare("CAMultiplot") == 0 )
       		{
            		CAMultiplot *plot = (CAMultiplot*)widget;
            		plot->Run();
            		continue;
       		};

		if( clsName.compare("CAMultiwaveplot") == 0 )
		{
			CAMultiwaveplot *plot = (CAMultiwaveplot*)widget;
			plot->Run();
			qDebug("CAMultiwaveplot Called");
			continue;
		};

		if(var.isValid())
		//if(var.isValid() && (QString)var.length() > 0) continue;
		{
		//	qDebug("Test %d", i);
			objHash.insert(objName, widget);
			pvlist.append(objName);
		}
	};

	qSort(pvlist);
	for (int i=0; i<pvlist.size(); i++)
	{
		qDebug("Sorted PVnames:%s",pvlist.at(i).toStdString().c_str());
	}

	i=0;
	while(i != pvlist.size())
	{
		j = objHash.find(pvlist.at(i));
		objName = j.key();
		widget = j.value();
		clsName = widget->metaObject()->className();
		
		QVariant var = widget->property("pvname");
		if(!var.isValid()) continue;
		QVariant control = widget->property("control");
		if(control.isValid() && control.toBool())
		{
			widget->setProperty("pvname", strlist.at(i));
			qDebug("(1) %d. pvname of %s: %s",i, objName.toStdString().c_str(), widget->property("pvname").toString().toStdString().c_str());
			//widget->installEventFilter(this);
			regControlObject(widget, clsName);
		}
		else if(var.isValid() && !control.isValid())
		{
			widget->setProperty("pvname", strlist.at(i));
			qDebug("(2) %d. pvname of %s: %s",i, objName.toStdString().c_str(), widget->property("pvname").toString().toStdString().c_str());
			widget->installEventFilter(this);
		}
		else
		{
		}
		qDebug("   %d. Var: %s, Control: %s",i, widget->property("pvname").toString().toStdString().c_str(), control.toString().toStdString().c_str());
		strpvName = widget->property("pvname").toString();
		chaccess.objname  = widget->objectName().toStdString();
		chaccess.pvname  = strpvName.toStdString();
		chaccess.pobj  = widget;

#if 1
		//++shbaek
		QVariant dbrVar = widget->property("dbrequest");
		if(!dbrVar.isValid())
		{
			chaccess.dbrequest  = DBR_GR_DOUBLE;
		}
		else
		{
			chaccess.dbrequest  = dbrVar.toInt();
		};
#endif

		register_chacc.push_back(chaccess);
		//using QMap
		hash_chacc.insert(widget->objectName(), chaccess);
		// for debug message.
		//printchannel();
		qDebug("##");
		qDebug("##");
		i++;
	}
#endif
	//qDebug("End of setpvs: %s/%s", clsName.toStdString().c_str(), widget->property("pvname").toString().toStdString().c_str());
	printchannel();
	qDebug("End of setpvs: %s", clsName.toStdString().c_str());
};

/*  Description: eventFilter override function. for mouse event redefine.
 *	Parameters:
 *	target: target widget.
 *	event: taget widget event
 */


/*  Description: recursive CADisplayer Channel obtain.
 *	Parameter:
 *	pwidget: parent widget pointer reading from UILoader.
 */
void 
AttachChannelAccess::getchannel(const QWidget *pwidget)
{
#if 1
	QList<QObject*> objlist = pwidget->children();
	QWidget	*widget = NULL;
	QString clsName;
	QString strpvName;
	//const char  SHM_KEY[]  = "0x50000000";
	//char *shmPtr = mCachedData.open (SHM_KEY, 100*sizeof(ChAccess), CachedChannelAccess::RT_SHM_CREAT | CachedChannelAccess::RT_SHM_RDWR);

	unsigned int index = 0;
	for (int i = 0; i < objlist.size(); ++i) 
	{
		widget = (QWidget*)objlist.at(i);
		clsName = widget->metaObject()->className();
		//qDebug("ClassName:%s", clsName.toStdString().c_str() );

		if( clsName.compare("QFrame") == 0 || clsName.compare("QWidget") == 0 
				|| clsName.compare("QDockWidget") == 0 || clsName.compare("QGroupBox") == 0 )
		{
			getchannel(widget);
		};

		if( clsName.compare("QTabWidget") == 0 )
		{
			QTabWidget *tabwidget = (QTabWidget*)widget;
			for(int t=0; t<tabwidget->count(); t++)
			{
				//qDebug("Current index of the tab:%d from %d", t+1, tabwidget->count());
				getchannel(tabwidget->widget(t));
			}
		};

#if 0
		if( clsName.compare("QStackedWidget") == 0 )
		{
			QStackedWidget *stackwidget = (QStackedWidget*)widget;
			for(int t=0; t<stackwidget->count(); t++)
			{
				getchannel(stackwidget->widget(t));
			}
		};
#endif

		if( clsName.compare("CAMultiplot") == 0 )
		{
			CAMultiplot *plot = (CAMultiplot*)widget;
			plot->Run();
			continue;
		};
		if( clsName.compare("CAMultiwaveplot") == 0 )
		{
			CAMultiwaveplot *plot = (CAMultiwaveplot*)widget;
			plot->Run();
			qDebug("CAMultiwaveplot Called");
			continue;
		};
		if( clsName.compare("CAWaveformPut") == 0 )
		{
			CAWaveformPut *plot = (CAWaveformPut*)widget;
			plot->MonitorOn();
			continue;
		};
		if( clsName.compare("CAScheduler") == 0 )
		{
			CAScheduler *sche = (CAScheduler*)widget;
			sche->installEventFilter(this);
			//sche->startTimer(1000);
		};

		if( clsName.compare("CATableList") == 0 )
		{
			CATableList *pTable = (CATableList*)widget;

			connect(pTable, SIGNAL(cellChanged(int, int)), pTable, SLOT(changeValue(int, int)));
			connect(pTable, SIGNAL(SIGTableUpdate()), pTable, SLOT(SLOTTableUpdate()));

			int rowcount = pTable->rowCount();
			int colcount = pTable->columnCount();

			//qDebug("RowCount:%d, ColCount:%d", rowcount, colcount);
			for(int i = 0; i < rowcount; i++)
			{
				for(int j = 0; j < colcount; j++)
				{
					QTableWidgetItem *pItem = pTable->item(i, j);
					QString pvName = pItem->text();
					//qDebug("ObjName:%s, Register PV:%s",widget->objectName().toStdString().c_str(), pvName.toStdString().c_str());
					pTable->InsertHash(pItem, pvName); 
					chaccess.objname  = widget->objectName().toStdString();
					chaccess.pvname  = pvName.toStdString();
					chaccess.pobj	= widget;
					chaccess.pItem  = pItem;
					chaccess.chindex  = index++;
					chaccess.dbrequest  = DBR_TIME_STRING;
					register_chacc.push_back(chaccess);
					//using QHash
					hash_chacc.insert(widget->objectName(), chaccess);
				};
			};

			continue;
		};

		QVariant var = widget->property("pvname");
		if(!var.isValid()) continue; 
		if(var.toString().length() == 0) continue;

		QVariant control = widget->property("control");
		if(control.isValid() && control.toBool())
		{
			regControlObject(widget, clsName);
		}
		else if(var.isValid() && !control.isValid())
		{
			widget->installEventFilter(this);
		};

		if(var.toString().compare("popup") == 0) continue;

		strpvName = var.toString();
		chaccess.objname  = widget->objectName().toStdString();
		chaccess.pvname  = strpvName.toStdString();
		chaccess.pobj  = widget;
		chaccess.chindex  = index++;
		//++leesi
		QVariant dbrVar = widget->property("dbrequest");
		if(!dbrVar.isValid())
		{
			chaccess.dbrequest  = DBR_GR_DOUBLE;
		}
		else
		{
			chaccess.dbrequest  = dbrVar.toInt();
		};

		register_chacc.push_back(chaccess);
		//using QHash
		hash_chacc.insert(widget->objectName(), chaccess);
	};
	// for debug message.
	//printchannel();
#endif
};

/*  Description: eventFilter override function. for mouse event redefine.
 *	Parameters:
 *	target: target widget.
 *	event: taget widget event
 */
void
AttachChannelAccess::mousePressEvent(QMouseEvent * /*event*/)
{
#if 0
	qDebug("----------------------------------------------------- mousePressEvent() start");
	if (event->button() == Qt::RightButton) 
	{
		QWidget *child = childAt(event->pos());
		QString objName = child->metaObject()->className();
		if ( objName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisp = (CADisplayer*)child;
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			CAGraphic *pGraph = (CAGraphic*)child;
		};
	}
	qDebug("----------------------------------------------------- mousePressEvent() end");
#endif
};

void
AttachChannelAccess::getValue()
{
	//Hash table read
	//qDebug("----------------------------------------------------- getValue() start");
	ChAccess *pchacc;
	hash_chacciter = hash_chacc.constBegin();
	//CachedData readData;
	while ( hash_chacciter != hash_chacc.constEnd() ) 
	{
		//cout << hash_chacciter.key() << ": " << hash_chacciter.value() << endl;
		pchacc = (ChAccess*)&(hash_chacciter.value());
		QString objName = pchacc->pobj->metaObject()->className();

		//for Test. Cached Data Read.
		//unsigned int chindex = pchacc->chindex;
		//mCachedData.read(chindex, &readData, sizeof(CachedData));

		if ( objName.compare("CAUITime") == 0 )
		{
			CAUITime *pUitime = (CAUITime*)pchacc->pobj;
#if 1
			setControl((bool)pchacc->pvvalue);
			pUitime -> changeValue((int)pchacc->pvvalue);
#else
			if(pUitime->getMasterPV().length() > 0)
			{
				setControl((bool)pchacc->pvvalue);
				pUitime -> changeValue((int)pchacc->pvvalue);
				//qDebug("setControl was finished.");
			}
#endif
		}
		else if ( objName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisp = (CADisplayer*)pchacc->pobj;
//			qDebug("CADisplayer Val:%f", pchacc->pvvalue);
			pDisp -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAPushButton") == 0 )
		{
			CAPushButton *pcapush = (CAPushButton*)pchacc->pobj;
//			qDebug("CAPushButton Val:%f", pchacc->pvvalue);
			pcapush->changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CABoButton") == 0 )
		{
			CABoButton *pcabo = (CABoButton*)pchacc->pobj;
//			qDebug("CABoButton Val:%f", pchacc->pvvalue);
			pcabo->changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CACheckBox") == 0 )
		{
			CACheckBox *pcacbox = (CACheckBox*)pchacc->pobj;
			pcacbox->changeValue((int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAMbboButton") == 0 )
		{
			CAMbboButton *pcambbo = (CAMbboButton*)pchacc->pobj;
			pcambbo->changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAComboBox") == 0 )
		{
			CAComboBox *pcacombo = (CAComboBox*)pchacc->pobj;
			pcacombo -> changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CALineEdit") == 0 )
		{
			CALineEdit *pcale = (CALineEdit*)pchacc->pobj;
//			qDebug("CALineEdit Val:%f", pchacc->pvvalue);

			switch(pchacc->dbrequest)
			{
				case DBR_TIME_STRING:
					pcale -> changeValue(pchacc->connstatus, pchacc->strvalue, pchacc->severity, pchacc->prec);
					break;
				case DBR_GR_DOUBLE:
				default:
					pcale -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
					break;
			};
		}
		else if ( objName.compare("CALabel") == 0 )
		{
			CALabel *pLabel = (CALabel*)pchacc->pobj;
//			qDebug("CALabel Val:%f", pchacc->pvvalue);
			//++leesi
			switch(pchacc->dbrequest)
			{
				case DBR_TIME_STRING:
					pLabel->changeValue(pchacc->connstatus, pchacc->strvalue, pchacc->severity);
					break;
				case DBR_GR_DOUBLE:
				default:
					pLabel->changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
					break;
			};
		}
		else if ( objName.compare("CAImageMbbi") == 0 )
		{
			CAImageMbbi *pcambbi = (CAImageMbbi*)pchacc->pobj;
			pcambbi->changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAWclock") == 0 )
		{
			CAWclock *pcawclock = (CAWclock*)pchacc->pobj;
			pcawclock->changeValue(pchacc->pvvalue);
#if 0
			pcawclock->changeValue(pchacc->connstatus, pchacc->strvalue, pchacc->severity);
#endif
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			CAGraphic *pcagraphic = (CAGraphic*)pchacc->pobj;
//			qDebug("CAGraphic Val:%f", pchacc->pvvalue);
			pcagraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
		}
		else if ( objName.compare("CAQtSignal") == 0 )
		{
			CAQtSignal *pcagraphic = (CAQtSignal*)pchacc->pobj;
			pcagraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CAScreenShot") == 0 )
		{
			CAScreenShot *pcagraphic = (CAScreenShot*)pchacc->pobj;
			pcagraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CASlider") == 0 )
		{
			CASlider *pSlider = (CASlider*)pchacc->pobj;
			pSlider -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAProgressBar") == 0 )
		{
			CAProgressBar *pProgress = (CAProgressBar*)pchacc->pobj;
			pProgress -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAColorTileButton") == 0 )
		{
			CAColorTileButton *pTile = (CAColorTileButton*)pchacc->pobj;
			pTile -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CAPopUp") == 0 )
		{
			CAPopUp *pPopup = (CAPopUp*)pchacc->pobj;
			pPopup -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
//qDebug("index:%s, opvname:%s, pobj:%p", hash_chacciter.key().toStdString().c_str(), pchacc->pvname.c_str(), pchacc->pobj);
		++hash_chacciter;
	};
	//qDebug("----------------------------------------------------- getValue() end");
}

void
AttachChannelAccess::printchannel()
{
	// for QHash Interface
	ChAccess *pchacc;
	hash_chacciter = hash_chacc.constBegin();
	while ( hash_chacciter != hash_chacc.constEnd() ) 
	{
		//cout << i.key() << ": " << i.value() << endl;
		pchacc = (ChAccess*)&(hash_chacciter.value());
		qDebug("index:%s, opvname:%s, pobj:%p", hash_chacciter.key().toStdString().c_str(), pchacc->pvname.c_str(), pchacc->pobj);
		++hash_chacciter;
	}
}
void
AttachChannelAccess::UpdateObj(const QString &keyname, const double &dvalue, const short &severity, const short &precision)
{
	//qDebug("----------------------------------------------------- UpdateObj1() start");
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(keyname);
	if ( chacciter != hash_chacc.constEnd() )
	{
		chacciter.value().pvvalue = dvalue;
		chacciter.value().severity = severity;
		chacciter.value().prec = precision;
	};
	emit updatesignal(keyname, severity, precision);
	//qDebug("----------------------------------------------------- UpdateObj1() end");
}
/*shbaek added next UpdateObj for String */
void
AttachChannelAccess::UpdateObj(const QString &keyname, const QString &dvalue, const short &severity, const short &precision)
{
	//qDebug("----------------------------------------------------- UpdateObj2() start");
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(keyname);
	if ( chacciter != hash_chacc.constEnd() )
	{
		chacciter.value().pvvalue = dvalue.toDouble();
		chacciter.value().strvalue = dvalue;
		chacciter.value().severity = severity;
		chacciter.value().prec = precision;
	};
	emit updatesignal(keyname, severity, precision);
	//qDebug("----------------------------------------------------- UpdateObj2() end");
}
void
AttachChannelAccess::ConnectionStatusObj(const QString &keyname, const short &connstatus)
{
	//qDebug("----------------------------------------------------- ConnectionStatusObj() start");
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(keyname);
	if ( chacciter != hash_chacc.constEnd() )
	{
		chacciter.value().connstatus = connstatus;
	};
	//qDebug("----------------------------------------------------- ConnectionStatusObj() end");
}
void
AttachChannelAccess::removePlots()
{
	// shbaek changed following one line to the next to solve smartptr memory corruption problem
	vecPlotPtr.clear();
	//vecPlotPtr->clear();
}
void
AttachChannelAccess::timeSet()
{
	 CAUITime *puitime = m_pwidget->findChild<CAUITime *>("CAUITime");
	 if (puitime == 0)
	 {
		 //qDebug("Not Found CAUITime");
		 connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),
				 this,SLOT(updateobj(const QString&, const short&, const short &)) );
	 }
	 else
	 {
		 QVariant time = puitime->property("timeset");
		 QVariant second = puitime->property("periodicsecond");
		 //qDebug("m_pwidget:%p,%d, %d",puitime, time.toInt(), second.toInt() );

		 if(time.toInt() == CAUITime::Periodic)
		 {
			 int periodic = second.toInt()*1000;
			 (void)startTimer(periodic);
		 }
		 else
		 {
			 //Event Display Update
			 connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),
					 this,SLOT(updateobj(const QString&, const short&, const short &)) );
		 };
	};
}
void
AttachChannelAccess::init()
{
	//qDebug("----------------------------------------------------- init");
	QFile file (m_filename);
	if ( !file.exists() )
	{
		qDebug("Ui File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->hide();
	getchannel(m_pwidget);
	file.close();


    //ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);

	m_pwidget->installEventFilter(this);
	mChThr = new ChannelAccessThr(register_chacc, this);

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	// update for every epics event.
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();
}

void
AttachChannelAccess::init3(const QList<QString>& stringlist)
{
	QList<QString> m_pvlists = stringlist;
	//qDebug("----------------------------------------------------- init3 with stringlist");
	if ( !m_pwidget ) return;

	setpvs(m_pwidget, m_pvlists);

	m_pwidget->installEventFilter(this);
	mChThr = new ChannelAccessThr(register_chacc, this);

	//ca_context_create(ca_disable_preemptive_callback);
	ca_add_exception_event(exceptionCallback,NULL);

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	// update for every epics event.
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();

}

#if 0
//leesi version
void
AttachChannelAccess::init3(const QList<QString>& stringlist)
{
	//qDebug("----------------------------------------------------- init3");
	QFile file (m_filename);
	if ( !file.exists() )
	{
		qDebug("Ui File not exists!");
		m_pwidget = 0;
		return;
	};
	file.open(QFile::ReadOnly);
	m_pwidget = m_loader.load(&file);
	m_pwidget->hide();
	file.close();

	QList<QString> m_pvlists = stringlist;

    ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);

	setpvs(m_pwidget, m_pvlists);

	m_pwidget->installEventFilter(this);
	mChThr = new ChannelAccessThr(register_chacc, this);

	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
	m_pwidget->show();
}
#endif

void
AttachChannelAccess::init2()
{
	//qDebug("----------------------------------------------------- init2");
	if ( !m_pwidget ) return;
	m_pwidget->hide();

	getchannel(m_pwidget);

    //ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);

	m_pwidget->installEventFilter(this);
	mChThr = new ChannelAccessThr(register_chacc, this);

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	// update for every epics event.
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();
}

void 
AttachChannelAccess::timerEvent(QTimerEvent *)
{
	if (m_curindex != m_index || m_curindex == -1) return;
	//qDebug("timerEvent, CurIndex:%d is working", m_curindex);
	getValue();
}

void 
AttachChannelAccess::SetUiCurIndex(const int curindex)
{
	if(mChThr == NULL) return;
	m_curindex = curindex;

#if 1
	if(curindex < 0 )
		mChThr->ClearEventsOnPage(curindex);
	else
		mChThr->AddEventsOnPage(curindex);
#endif
}

void 
AttachChannelAccess::updateobj(const QString& objname, const short &severity, const short &precision)
{
	//Debug("----------------------------------------------------- updateobj() start");
	ChAccess *pchacc = 0;
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(objname);
	if ( chacciter != hash_chacc.constEnd() ) 
	{
		pchacc = (ChAccess*)&(chacciter.value());
		QString objName = pchacc->pobj->metaObject()->className();
		if ( objName.compare("CAUITime") == 0 )
		{
			CAUITime *pUitime = (CAUITime*)pchacc->pobj;
#if 1
			setControl((bool)pchacc->pvvalue);
			pUitime -> changeValue((int)pchacc->pvvalue);
#else
			if(pUitime->getMasterPV().length() > 0)
			{
				setControl((bool)pchacc->pvvalue);
				pUitime -> changeValue((int)pchacc->pvvalue);
				qDebug("updateobj CAUITime. value:%d", (int)pchacc->pvvalue);
			}
#endif
		}
		else if ( objName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisp = (CADisplayer*)pchacc->pobj;
			pDisp -> changeValue(pchacc->connstatus, pchacc->pvvalue, severity, precision);
		}
		else if ( objName.compare("CAPushButton") == 0 )
		{
			CAPushButton *pPbut = (CAPushButton*)pchacc->pobj;
			pPbut -> changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CABoButton") == 0 )
		{
			CABoButton *pBbut = (CABoButton*)pchacc->pobj;
			pBbut -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CACheckBox") == 0 )
		{
			CACheckBox *pCbox = (CACheckBox*)pchacc->pobj;
			pCbox->changeValue((int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAMbboButton") == 0 )
		{
			CAMbboButton *pmbut = (CAMbboButton*)pchacc->pobj;
			pmbut -> changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAComboBox") == 0 )
		{
			CAComboBox *pcacombo = (CAComboBox*)pchacc->pobj;
			pcacombo -> changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CALineEdit") == 0 )
		{
			CALineEdit *pLedit = (CALineEdit*)pchacc->pobj;

			switch(pchacc->dbrequest)
			{
				case DBR_TIME_STRING:
					pLedit -> changeValue(pchacc->connstatus, pchacc->strvalue, severity, precision);
					break;
				case DBR_GR_DOUBLE:
				default:
					pLedit -> changeValue(pchacc->connstatus, pchacc->pvvalue, severity, precision);
					break;
			};
		}
		else if ( objName.compare("CALabel") == 0 )
		{
			CALabel *pLabel = (CALabel*)pchacc->pobj;
			//++leesi
			switch(pchacc->dbrequest)
			{
				case DBR_TIME_STRING:
					pLabel->changeValue(pchacc->connstatus, pchacc->strvalue, pchacc->severity);
					break;
				case DBR_GR_DOUBLE:
				default:
					pLabel->changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
					break;
			};
		}
		else if ( objName.compare("CAImageMbbi") == 0 )
		{
			CAImageMbbi *pMbbi = (CAImageMbbi*)pchacc->pobj;
			pMbbi -> changeValue(pchacc->connstatus, (int)pchacc->pvvalue);
		}
		else if ( objName.compare("CAWclock") == 0 )
		{
			CAWclock *pWclock = (CAWclock*)pchacc->pobj;
			pWclock -> changeValue(pchacc->pvvalue);
#if 0
			pWclock -> changeValue(pchacc->connstatus, pchacc->strvalue, severity);
#endif
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			CAGraphic *pGraphic = (CAGraphic*)pchacc->pobj;
			pGraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue, severity);
		}
		else if ( objName.compare("CAQtSignal") == 0 )
		{
			CAQtSignal *pGraphic = (CAQtSignal*)pchacc->pobj;
			pGraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CAScreenShot") == 0 )
		{
			CAScreenShot *pGraphic = (CAScreenShot*)pchacc->pobj;
			pGraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CASlider") == 0 )
		{
			CASlider *pSlider = (CASlider*)pchacc->pobj;
			pSlider -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAProgressBar") == 0 )
		{
			CAProgressBar *pProgress = (CAProgressBar*)pchacc->pobj;
			pProgress -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAColorTileButton") == 0 )
		{
			CAColorTileButton *pTile = (CAColorTileButton*)pchacc->pobj;
			pTile -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
		else if ( objName.compare("CAPopUp") == 0 )
		{
			CAPopUp *pPopup = (CAPopUp*)pchacc->pobj;
			pPopup -> changeValue(pchacc->connstatus, pchacc->pvvalue);
		}
	};
	//qDebug("----------------------------------------------------- updateobj() end");
}

void 
AttachChannelAccess::regControlObject(const QWidget *pwidget, const QString& controlObj) 
{
	if(m_bmon == true) return;

	if(controlObj.compare("CAPushButton")== 0)
	{
		CAPushButton *pcapush = (CAPushButton*)pwidget;
		connect(pcapush, SIGNAL(pressed()), this, SLOT(capushbuttonpressSlot()));
		connect(pcapush, SIGNAL(released()), this, SLOT(capushbuttonreleaseSlot()));
	}
	else if(controlObj.compare("CABoButton")== 0)
	{
		CABoButton *pcabo = (CABoButton*)pwidget;
		connect(pcabo, SIGNAL(valueChanged(bool)), this, SLOT(cabobuttonSlot(bool)));
	}
	else if(controlObj.compare("CACheckBox")== 0)
	{
		CACheckBox *pcacbox = (CACheckBox*)pwidget;
		connect(pcacbox, SIGNAL(valueChanged(bool)), this, SLOT(cacheckboxSlot(bool)));
	}
	else if(controlObj.compare("CAMbboButton")== 0)
	{
		CAMbboButton *pcambbo = (CAMbboButton*)pwidget;
		connect(pcambbo, SIGNAL(valueChanged(int)), this, SLOT(cambbobuttonSlot(int)));
	}
	else if(controlObj.compare("CAComboBox")== 0)
	{
		CAComboBox *pcacombo = (CAComboBox*)pwidget;
		//connect(pcacombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cacomboboxSlot(int)));
		connect(pcacombo, SIGNAL(activated(int)), this, SLOT(cacomboboxSlot(int)));
	}
	else if(controlObj.compare("CALineEdit")== 0)
	{
		CALineEdit *pcale = (CALineEdit*)pwidget;

		switch(pcale->getDBRequest())
		{
			case DBR_TIME_STRING:
				connect(pcale, SIGNAL(enteredValue(QString)), this, SLOT(calineeditSlot(QString)));
				break;
			case DBR_GR_DOUBLE:
				connect(pcale, SIGNAL(enteredValue(double)), this, SLOT(calineeditSlot(double)));
				break;
			default:
				break;
		}
	}
	else if(controlObj.compare("CASlider")== 0)
	{
		CASlider *pcaslider = (CASlider*)pwidget;
		//pcaslider->setTracking(true);
		//connect(pcaslider, SIGNAL(valueChanged(int)), this, SLOT(casliderSlot(int)));
		connect(pcaslider, SIGNAL(sliderReleased()), this, SLOT(casliderSlot()));
		connect(pcaslider, SIGNAL(actionTriggered(int)), this, SLOT(actionSlot(int)));
	}
};


void 
AttachChannelAccess::capushbuttonpressSlot()
{
	//Here caput
	CAPushButton *controlbutton = qobject_cast<CAPushButton *>(sender());
	QString pvname = controlbutton->getPvname();
	bool check = true;
	//qDebug("[Pressed] PVName:%s, State:%d, Value:%d", controlbutton->getPvname().toStdString().c_str(), check, (int)check);

	if (controlbutton->getPasswd() == true) 
	{
		if(controlbutton->getEStop() == false)
		{
			Password pword(controlbutton->getPasswdStr(), new QWidget());
			if(pword.getStatus() == false) {
				qDebug("Entered password is wrong");
				return;
			};
		}
		else if(controlbutton->getEStop() == true && controlbutton->getValue() == true)
		{
			Password pword(controlbutton->getPasswdStr(), new QWidget());
			if(pword.getStatus() == false) {
				qDebug("Entered password is wrong");
				return;
			};
		};
	} 

	// TrueButton/FalseButton function for VMS RGA
	switch(controlbutton->buttonType())
	{
		case CAPushButton::ToggleButton:
			check = controlbutton->getValue();
			if(check)
				check = false;
			else
				check = true;
			break;
		case CAPushButton::TrueButton:
			check = true;
			break;
		case CAPushButton::FalseButton:
			check = false;
			break;
		case CAPushButton::PulseButton:
			check = true;
			break;
		default:
			break;
	}
#if 1
	if (controlbutton->getConfirm() == true) 
	{
		if(controlbutton->buttonType() == CAPushButton::PulseButton)
		{
			return;
		} else 
		{
			qDebug("Confirm:%d", controlbutton->getConfirm());
			QDialog *dia = new QDialog();
			QVBoxLayout *vbox = new QVBoxLayout(dia);
			QLabel *label = new QLabel("Do you want to apply it?");
			vbox->addWidget(label);

			QPushButton *yesButton = new QPushButton("Yes");
			QPushButton *noButton = new QPushButton("No");
			dia->resize(150,80);

			QDialogButtonBox *bbox = new QDialogButtonBox (Qt::Horizontal, dia);
			bbox->addButton(yesButton, QDialogButtonBox::AcceptRole);
			bbox->addButton(noButton, QDialogButtonBox::RejectRole);
			vbox->addWidget(bbox);
			dia->setLayout(vbox);
			connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
			connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));
			dia->exec();

			if(dia->result() == QDialog::Accepted)
			{
				qDebug("Accepted");
				caPut(DBR_INT, pvname, check);
				//controlbutton -> changeValue(0, (int) check);

				QStringList pvlist = controlbutton->getPVList();
				if(pvlist.isEmpty() == false)
				{    
					QVariant qvarval = QVariant(check);
					caPutList(pvlist, qvarval);
				};   
			}
			else if(dia->result() == QDialog::Rejected)
			{
				qDebug("Rejected");
				//return;
			}
		}

	} else
	{
		caPut(DBR_INT, pvname, check);
		QStringList pvlist = controlbutton->getPVList();
		if(pvlist.isEmpty() == false)
		{    
			QVariant qvarval = QVariant(check);
			caPutList(pvlist, qvarval);
		};   
#if 0
		if(controlbutton->buttonType() == CAPushButton::PulseButton)
		{
			controlbutton->PulseTime(!check);
		};
#endif
	}
#endif
}

void 
AttachChannelAccess::capushbuttonreleaseSlot()
{
	//Here caput
	CAPushButton *controlbutton = qobject_cast<CAPushButton *>(sender());
	QString pvname = controlbutton->getPvname();
	bool check = true;


	switch(controlbutton->buttonType())
	{
		case CAPushButton::ToggleButton:
			break;
		case CAPushButton::TrueButton:
			break;
		case CAPushButton::FalseButton:
			break;
		case CAPushButton::PulseButton:
			{
				check = false;
				caPut(DBR_INT, pvname, check);
				QStringList pvlist = controlbutton->getPVList();
				if(pvlist.isEmpty() == false)
				{    
					QVariant qvarval = QVariant(check);
					caPutList(pvlist, qvarval);
				};   
			};
			break;
		default:
			break;
	}

	//qDebug("[Released] PVName:%s, State:%d, Value:%d", controlbutton->getPvname().toStdString().c_str(), check, (int)check);
}

void 
AttachChannelAccess::cabobuttonSlot(bool check)
{
	//Here caput
	CABoButton *bobutton = qobject_cast<CABoButton *>(sender());
	//qDebug("cabobuttonSlot Control Signal Send PVName:%s, State:%d", bobutton->getPvname().toStdString().c_str(), check);
	QString pvname = bobutton->getPvname();
#if 1
	if (bobutton->getPasswd() == true) 
	{
		if(bobutton->getEStop() == false)
		{
			Password pword(bobutton->getPasswdStr(), new QWidget());
			if(pword.getStatus() == false) {
				qDebug("Entered password is wrong");
				return;
			};
		}
		else if(bobutton->getEStop() == true && bobutton->getValue() == true)
		{
			Password pword(bobutton->getPasswdStr(), new QWidget());
			if(pword.getStatus() == false) {
				qDebug("Entered password is wrong");
				return;
			};
		};
	} 
#endif

#if 1
	if (bobutton->getConfirm() == true) 
	{
		qDebug("Confirm:%d", bobutton->getConfirm());
		QDialog *dia = new QDialog();
		QVBoxLayout *vbox = new QVBoxLayout(dia);
		QLabel *label = new QLabel("Do you want to apply it?");
		vbox->addWidget(label);

		QPushButton *yesButton = new QPushButton("Yes");
		QPushButton *noButton = new QPushButton("No");
		dia->resize(150,80);

		QDialogButtonBox *bbox = new QDialogButtonBox (Qt::Horizontal, dia);
		bbox->addButton(yesButton, QDialogButtonBox::AcceptRole);
		bbox->addButton(noButton, QDialogButtonBox::RejectRole);
		vbox->addWidget(bbox);
		dia->setLayout(vbox);
		connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
		connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));
		dia->exec();

#if 1
		if(dia->result() == QDialog::Accepted)
		{
			qDebug("Accepted");
			caPut(DBR_INT, pvname, check);
			bobutton -> changeValue(0, check);
			//pcabo->changeValue(pchacc->connstatus, pchacc->pvvalue);
			QStringList pvlist = bobutton->getPVList();
			if(pvlist.isEmpty() == false)
			{    
				QVariant qvarval;
				if(check == true)
					qvarval = QVariant("1");
				else
					qvarval = QVariant("0");

				caPutList(pvlist, qvarval);
			};   
		}
		else if(dia->result() == QDialog::Rejected)
		{
			qDebug("Rejected");
		}
#endif
#if 0
		Password pword(bobutton->getPasswdStr(), new QWidget());
		if(pword.getStatus() == true)
		{
			//caPut(DBR_INT, pvname, check);
			//bobutton -> changeValue(check);
		}
		else 
		{
			qDebug("Entered password is wrong");
			return;
		}
#endif
	} else
	{
		caPut(DBR_INT, pvname, check);
		QStringList pvlist = bobutton->getPVList();
		if(pvlist.isEmpty() == false)
		{    
			QVariant qvarval;
			if(check == true)
				qvarval = QVariant("1");
			else
				qvarval = QVariant("0");
			caPutList(pvlist, qvarval);
		};   
	}
#endif
}

void 
AttachChannelAccess::cambbobuttonSlot(int id)
{
	//Here caput
	CAMbboButton *mbbobutton = qobject_cast<CAMbboButton *>(sender());
	QString pvname = mbbobutton->getPvname();
	caPut(DBR_INT, pvname, id);
	QStringList pvlist = mbbobutton->getPVList();
	if(pvlist.isEmpty() == false)
	{    
		QVariant qvarval = QVariant(id);
		caPutList(pvlist, qvarval);
	};   
}

void 
AttachChannelAccess::cacomboboxSlot(int value)
{
	//Here caput
	CAComboBox *cacombo = qobject_cast<CAComboBox *>(sender());
	qDebug("cacomboboxSlot Control Signal Send PVName:%s, State:%d", cacombo->getPvname().toStdString().c_str(), value);
	QString pvname = cacombo->getPvname();
	caPut(DBR_INT, pvname, value);
	QStringList pvlist = cacombo->getPVList();
	if(pvlist.isEmpty() == false)
	{    
		QVariant qvarval = QVariant(value);
		caPutList(pvlist, qvarval);
	};   
}

#if 0
void 
AttachChannelAccess::casliderSlot(int value)
{
	//for test Colortilebutton
	CASlider *pcaslider = qobject_cast<CASlider *>(sender());
	QString pvname = pcaslider->getPvname();
	caPut(DBR_INT, pvname, value);
};
#endif

void 
AttachChannelAccess::casliderSlot()
{
	CASlider *pcaslider = qobject_cast<CASlider *>(sender());
	QString pvname = pcaslider->getPvname();
	int val = pcaslider->value();
	caPut(DBR_INT, pvname, val);
};

void 
AttachChannelAccess::actionSlot(int type)
{
	if(type == QAbstractSlider::SliderPageStepAdd ||
	type == QAbstractSlider::SliderPageStepSub )
	{
		CASlider *pcaslider = qobject_cast<CASlider *>(sender());
		QString pvname = pcaslider->getPvname();
		int val = 0;
		if (type == QAbstractSlider::SliderPageStepSub )
			val = pcaslider->value()-pcaslider->pageStep();
		else
			val = pcaslider->value()+pcaslider->pageStep();

		pcaslider->setSliderPosition(val);
		pcaslider->setStatusTip(QString("%1").arg(val));
		caPut(DBR_INT, pvname, val);
	};
};

void 
AttachChannelAccess::cacheckboxSlot(bool status)
{
	//Here caput
	CACheckBox *cbox = qobject_cast<CACheckBox *>(sender());
	qDebug("cacheckboxSlot Control Signal Send PVName:%s, State:%d", cbox->getPvname().toStdString().c_str(),(int) status);
	QString pvname = cbox->getPvname();
	caPut(DBR_INT, pvname, status);
	QStringList pvlist = cbox->getPVList();
	if(pvlist.isEmpty() == false)
	{    
		QVariant qvarval = QVariant(status);
		caPutList(pvlist, qvarval);
	};   
}

void 
AttachChannelAccess::calineeditSlot(double value)
{
	//Here caput
	CALineEdit *lineedit = qobject_cast<CALineEdit *>(sender());
#if 1
	qDebug("calineeditSlot Control Signal Send PVName:%s, Value:%f", lineedit->getPvname().toStdString().c_str(), value);
	QString pvname = lineedit->getPvname();
	caPut(DBR_DOUBLE, pvname, value);

	QStringList pvlist = lineedit->getPVList();
	if(pvlist.isEmpty() == false)
	{    
		QVariant qvarval = QVariant(value);
		caPutList(pvlist, qvarval);
	};   

#endif
}

void 
AttachChannelAccess::calineeditSlot(QString value)
{
	//Here caput
	CALineEdit *lineedit = qobject_cast<CALineEdit *>(sender());
	qDebug("calineeditSlot Control Signal Send PVName:%s, Value:%s", lineedit->getPvname().toStdString().c_str(), value.toStdString().c_str());
	QString pvname = lineedit->getPvname();
	caPutString(DBR_STRING, pvname, value);

	QStringList pvlist = lineedit->getPVList();
	if(pvlist.isEmpty() == false)
	{    
		QVariant qvarval = QVariant(value);
		caPutList(pvlist, qvarval);
	};   

}
//void AttachChannelAccess::caPutList(const QStringList &pvlist, const QVariant &value, const QStringList &exlist)
void AttachChannelAccess::caPutList(const QStringList &pvlist, const QVariant &value)
{
	if(pvlist.isEmpty() == false)
	{    
		for(int i = 0; i<pvlist.count();i++)
		{    
			QString pvitem = pvlist.at(i);
			QString svalue = value.toString();
			if(svalue.compare("false")==0) svalue="0";
			else if(svalue.compare("true")==0) svalue="1";
			QString command = QString("caput %1 %2 &").arg(pvitem).arg(svalue);
			//qDebug("Command:%s", command.toStdString().c_str());
			system(command.toStdString().c_str());
		};   
	};   
}
