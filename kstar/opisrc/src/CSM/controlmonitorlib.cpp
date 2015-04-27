/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  controlmonitorlib.cpp
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 *
 * Revision 1  2006-12-29
 * Author: Sangil Lee [silee], Sulhee Baek [shbaek]
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
#include "controlmonitorlib.h"

/* ---------------------------------------------------------------------------
 *  * Imported Global Variables & Function Declarations *
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *  * Exported Global Variables & Function Declarations *
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *  * Module Variables & Function Declarations *
 * ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  update channel access value into user set value.
 * ---------------------------------------------------------------------------*/
template <class T> int caPut(const int type, const QString pvname, T &value = 0)
{
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

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
		case DBR_INT:
		{
			int intval = (int) value;
			status = ca_put(DBR_INT, chan_id, &intval);
			break;
		}
		case DBR_LONG:
		{
			long longval = (long) value;
			status = ca_put(DBR_LONG, chan_id, &longval);
			break;
		}
		case DBR_FLOAT:
		{
			float fval = (float) value;
			status = ca_put(DBR_FLOAT, chan_id, &fval);
			break;
		}
		case DBR_DOUBLE:
		{
			double dval = value;
			status = ca_put(DBR_DOUBLE, chan_id, &dval);
			break;
		}
		case DBR_CHAR:
		{
			char cval = (char)value;
			status = ca_put(DBR_CHAR, chan_id, &cval);
			break;
		}
		default:
		{
			break;
		}
	};
	status = ca_pend_io(1.0);
	ca_clear_channel(chan_id);
	return status;
};

static ControlMonitorChannel *gattach = NULL;
//vector <ControlMonitorChannel *> vecGAttach;
QList<ControlMonitorChannel *> vecGAttach;
/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorChannel::ControlMonitorChannel(const QString &uifilename, int index, QWidget *widget):
	QWidget(widget), m_filename(uifilename)
{
	m_curindex = -1;
	m_index = index;
	m_control = m_lastcontrol = true;
	init1();
};

ControlMonitorChannel::ControlMonitorChannel(const QString &uifilename, int index, const QString &prefix, const QString &windowname):
	m_filename(uifilename),m_prefix(prefix),m_windowname(windowname)
{
	m_curindex = -1;
	m_index = index;
	m_control = m_lastcontrol = true;
	mhealthcount = 0;
	init1();
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorChannel::ControlMonitorChannel(QWidget *widget):
	QWidget(widget),m_pwidget(widget)
{
	m_curindex = -1;
	m_index = 0;
	m_control = m_lastcontrol = true;
	mhealthcount = 0;
	init2();
};

/* ---------------------------------------------------------------------------
 *	Description by shbaek: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorChannel::ControlMonitorChannel(QWidget *widget, const QList<QString>& pvlists):
	QWidget(widget),m_pwidget(widget)
{
	QList<QString> m_pvlists = pvlists;
	m_control = m_lastcontrol = true;
	mhealthcount = 0;
	init3(m_pvlists);
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorChannel::~ControlMonitorChannel()
{
	register_chacc.clear();

#if 1
	if(mChThr->isRunning())
	{
		mChThr->setStop(true);
		mChThr->wait();
	};
#endif
	delete mChThr;
}

/* ---------------------------------------------------------------------------
 *	Description by shbaek: 
 *  	It enables all control widgets by a control master pv within CAUITime widget.
 * ---------------------------------------------------------------------------*/
void ControlMonitorChannel::setControl(const bool &controlmaster)
{
    	m_control = controlmaster;
		QHash<QString, ChAccess>::const_iterator hash_chacciter_tmp;

	if (m_control == m_lastcontrol)
	{
		return;
	}
	else
	{
		ChAccess *pchacc;
		hash_chacciter_tmp = hash_chacc.constBegin();
		while ( hash_chacciter_tmp != hash_chacc.constEnd() ) 
		{
			pchacc = (ChAccess*)&(hash_chacciter_tmp.value());
			QString objName = pchacc->pobj->metaObject()->className();
			QWidget *pobj = (QWidget*) pchacc->pobj;
			QVariant control = pobj->property("control");
			if(control.isValid() && control.toBool())
			{
				pobj->setEnabled(m_control);
			}
			++hash_chacciter_tmp;
		}

		m_lastcontrol = m_control;
	}
}

/* ---------------------------------------------------------------------------
 *	Description by shbaek: 
 *	It gets control ability from all the control widgets.
 * ---------------------------------------------------------------------------*/
const bool &ControlMonitorChannel::getControl() const
{
        return m_control;
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::PrintData(const double pvalue)
{
	m_data = pvalue;
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *		eventFilter override function. for mouse event redefine.
 *  	Parameters:
 *  	Target: target widget.
 *  	Event: taget widget event
 *
 * ---------------------------------------------------------------------------*/
bool ControlMonitorChannel::eventFilter(QObject *target, QEvent *event)
{
	QWidget *targetwidget = (QWidget*)target;
	if(targetwidget == 0 ) return false;
	QString tobjName = targetwidget->metaObject()->className();

	if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick 
			|| event->type() == QEvent::MouseButtonRelease ) 
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->buttons() & Qt::RightButton) 
		{
			if ( tobjName.compare("CADisplayer") == 0 )
			{
				QWidget *child = targetwidget->childAt(mouseEvent->x(), mouseEvent->y());
				if (child==0) return false;

				QString objName = child->objectName();
				if ( objName.compare("lineEdit") == 0 )
				{
					//QLineEdit *label = (QLineEdit*)child;
				}
				else if ( objName.compare("pvName") == 0 )
				{
					//QLabel *pvlabel = (QLabel*)child;
				};
			}
		} 
		else if (mouseEvent->buttons() & Qt::LeftButton) 
		{
			if ( tobjName.compare("CABlinkLabel") == 0 )
			{
				CABlinkLabel *pBlinkLabel = (CABlinkLabel*)targetwidget;
				//QString objName = pBlinkLabel->objectName();
				string objName = pBlinkLabel->objectName().toStdString();
				//QWidget *child = pBlinkLabel->childAt(mouseEvent->x(), mouseEvent->y());
				//if (child==0) return false;
				//qDebug("ObjName:%s, ChildName:%s", objName.toStdString().c_str(), child->objectName().toStdString().c_str());
				string strPvName = pBlinkLabel->getPvname().toStdString();
				if(strPvName.compare("pvname") == 0 ) 
				{
					QMessageBox::information(this, "Warning!","Not Defined!!");
					return false;
				}

#if 1
				for(int i = 0; i < vecGAttach.size(); i++)
				{
					ControlMonitorChannel *pAttach = vecGAttach.at(i);
					QString winName = pAttach->getWindowName();
					if (winName.compare( pBlinkLabel->getPvname() ) == 0)
					{
						QWidget *pwidget = pAttach->GetWidget();
						pwidget -> activateWindow();
						pwidget -> raise();
						return false;
					};
				};
#endif
				if( objName.compare("caBlinkLabel_ICS_TMON1") == 0 )
				{
						gattach = new ControlMonitorChannel("../ui/ICS_Server_TMON_1.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
				else if( objName.compare("caBlinkLabel_ICS_TMON2") == 0 )
				{
					gattach = new ControlMonitorChannel("../ui/ICS_Server_TMON_2.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
				else if( objName.compare(0,7,"vxworks") == 0 )
				{
					gattach = new ControlMonitorChannel("../ui/Control_VxWorks_info.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
				//else if( objName.compare(0,17,"caBlinkLabel_GATE") == 0 )
				else if( objName.compare("caBlinkLabel_GATE") == 0 || 
						objName.compare("caBlinkLabel_GATE_2") == 0 ||
						objName.compare("caBlinkLabel_GATE_3") == 0 )
				{
					gattach = new ControlMonitorChannel("../ui/Gateway_Server_info.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
#if 1
				else if( objName.compare("caBlinkLabel_GATE_4") == 0 || objName.compare("caBlinkLabel_GATE_5") == 0 )
				{
					gattach = new ControlMonitorChannel("../ui/Gateway_Server_info_2.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
#endif
				else if ( objName.compare(13,6,"OPIWIN") == 0 || objName.compare(13,3,"MC1") == 0 )
				{
					gattach = new ControlMonitorChannel("../ui/Control_WinServer_info.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
				else 
				{
					gattach = new ControlMonitorChannel("../ui/Control_Server_info.ui",1, pBlinkLabel->getPrefix(), pBlinkLabel->getPvname());
				}
				vecGAttach.push_back(gattach);
				QWidget *pwidget = gattach->GetWidget();
				pwidget->installEventFilter(this);
				pwidget->window()->setWindowTitle(pBlinkLabel->getPrefix());
				pwidget->show();
				//QMessageBox::information(0, pBlinkLabel->getPrefix(), tr("Show Information in detail") );
			}
		}
		return true;
	}
	else if (event->type() == QEvent::ToolTip)
	{
		//QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		//m_prefix
		if ( tobjName.compare("CABlinkLabel") == 0 )
		{
			CABlinkLabel *pBlinkLabel = (CABlinkLabel*)targetwidget;
			QToolTip::showText(QCursor::pos(),pBlinkLabel->getPvname());
		}
		else if ( tobjName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisplayer = (CADisplayer*)targetwidget;
			QToolTip::showText(QCursor::pos(),pDisplayer->getPvname());
		}
		else if ( tobjName.compare("CALabel") == 0 )
		{
			CALabel *pLabel = (CALabel*)targetwidget;
			QToolTip::showText(QCursor::pos(),pLabel->getPvname());
		}
	}
	else if (event->type() == QEvent::Close)
	{
		qDebug("***New Delete gattach***");
		for(int i = 0; i < vecGAttach.size(); i++)
		{
			ControlMonitorChannel *pAttach = vecGAttach.at(i);
			QString winName = pAttach->getWindowName();
			if (winName.compare(m_windowname) == 0)
			{
				pAttach->mChThr->setStop(true);
				pAttach->mChThr->wait();
				pAttach->mChThr->exit();
				delete pAttach;
				//vecGAttach.erase(find(vecGAttach.begin(), vecGAttach.end(), pAttach));
				vecGAttach.removeAt(i);
			};
		};
	}
	else if (event->type() == QEvent::KeyPress )
	{
		int key = static_cast<QKeyEvent *>(event)->key();
		quint32 modifier = static_cast<QKeyEvent *>(event)->nativeModifiers();
		//qDebug("modifier:%x, %x", modifier, key);
		if ((modifier==0x14||modifier == 0x4) && key == Qt::Key_P)
		{
			QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
			QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
			QString format = "jpg";
			mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
			//mpixmap = QPixmap::grabWidget(m_pwidget);
			mpixmap.save(filename, format.toAscii());
		};
	}
    return QWidget::eventFilter(target, event);
};

/* ---------------------------------------------------------------------------
 *	Description by shbaek: 
 *
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::setpvs(const QWidget *pwidget, const QList<QString>& stringlist)
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
	for (i = 0; i < objlist.size(); ++i) 
	{
		widget = (QWidget*)objlist.at(i);
		clsName = widget->metaObject()->className();
		objName  = widget->objectName();
		QVariant var = widget->property("pvname");

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
				setpvs(tabwidget->widget(t), strlist);
			}
		}

		if(var.isValid())
		{
			objHash.insert(objName, widget);
			pvlist.append(objName);
		}
	};

	qSort(pvlist);
	for (int i=0; i<pvlist.size(); i++)
	{
		//qDebug("Sorted PVnames:%s",pvlist.at(i).toStdString().c_str());
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
			//qDebug("(1) %d. pvname of %s: %s",i, objName.toStdString().c_str(), widget->property("pvname").toString().toStdString().c_str());
			regControlObject(widget, clsName);
		}
		else if(var.isValid() && !control.isValid())
		{
			widget->setProperty("pvname", strlist.at(i));
			//qDebug("(2) %d. pvname of %s: %s",i, objName.toStdString().c_str(), widget->property("pvname").toString().toStdString().c_str());
			widget->installEventFilter(this);
		}
		else
		{
		}
		//qDebug("   %d. Var: %s, Control: %s",i, widget->property("pvname").toString().toStdString().c_str(), control.toString().toStdString().c_str());
		strpvName = widget->property("pvname").toString();
		chaccess.objname  = widget->objectName().toStdString();
		chaccess.pvname  = strpvName.toStdString();
		chaccess.pobj  = widget;

#if 1
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
		hash_chacc.insert(widget->objectName(), chaccess);
		i++;
	}
#endif
	printchannel();
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  	obtaining pvname property for channel access.
 *		if the widget is containter widget, recursively getchannel.		
 *	Parameter:
 *		pwidget: parent widget pointer reading from UILoader.
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::getchannel(const QWidget *pwidget)
{
#if 1
	QList<QObject*> objlist = pwidget->children();
	QWidget	*widget = NULL;
	QString clsName;
	QString strpvName;
	int		cablinkcount = 0;
	for (int i = 0; i < objlist.size(); ++i) 
	{
		widget = (QWidget*)objlist.at(i);
		clsName = widget->metaObject()->className();

		if( clsName.compare("QFrame") == 0 || clsName.compare("QWidget") == 0 
				|| clsName.compare("QDockWidget") == 0
				|| clsName.compare("QGroupBox") == 0)
		{
			getchannel(widget);
		};

		if( clsName.compare("QTabWidget") == 0 )
		{
			QTabWidget *tabwidget = (QTabWidget*)widget;
			for(int t=0; t<tabwidget->count(); t++)
			{
				getchannel(tabwidget->widget(t));
			}
		}
		//if(clsName.compare("QLabel") == 0) widget->lower();
		string objName = widget->objectName().toStdString();
		if(objName.compare(0,5,"group") == 0) widget->lower();

		if(widget->objectName().compare("titleLabel")==0 && m_prefix.isEmpty()==false)
		{
			QLabel *titlelabel = static_cast<QLabel *> (widget);
			titlelabel->setText(m_prefix);
		};

		QVariant ipvar = widget->property("ipaddr");
		QString sipaddr = ipvar.toString(); 
		if(sipaddr.isEmpty() == false)
		{
			ipcheck.pobj     = widget;
			ipcheck.ipAddr   = sipaddr;
			hash_ipcheck.insert(ipvar.toString(), ipcheck);
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
		}
		if(m_prefix.isEmpty()==true || m_prefix.isNull()==true)
		{
			strpvName = var.toString();
		}
		else
		{
			string strprefix = m_prefix.toStdString();
			QChar	alpha = var.toString().at(0);
			if( alpha.isLower() == true && m_prefix.compare("CAGATEWAY1") == 0 ||
				alpha.isLower() == true && m_prefix.compare("CAGATEWAY2") == 0 ||
				alpha.isLower() == true && m_prefix.compare("CAGATEWAY5") == 0 ||
				alpha.isLower() == true && m_prefix.compare("CAGATEWAY6") == 0 ||
				alpha.isLower() == true && m_prefix.compare("OFFICEGW") == 0 )
			{
				//strprefix = m_prefix.toLower().toStdString();
				strpvName = QString("ICS_")+QString(strprefix.c_str())+QString("_")+var.toString().toUpper();
			}
#if 0
			else if(alpha.isLower() == true && m_prefix.compare("OFFICEGW") == 0 )
			{
				strprefix = m_prefix.toLower().toStdString();
				strpvName = QString(strprefix.c_str())+QString(":")+var.toString();
			}
#endif
			else if( m_prefix.compare("CCS") == 0 ||  strprefix.compare(0,3,"MPS") == 0 || m_prefix.compare("IVC") == 0 )
			{
				strpvName = m_prefix+QString(":")+var.toString();
			}
			else
			{
				strpvName = m_prefix+QString("_")+var.toString();
			};
		};

		chaccess.objname  = widget->objectName().toStdString();
		chaccess.pvname  = strpvName.toStdString();
		chaccess.pobj  = widget;
		chaccess.index  = cablinkcount;
		QVariant dbrVar = widget->property("dbrequest");
		if(!dbrVar.isValid())
		{
			chaccess.dbrequest  = DBR_GR_DOUBLE;
		}
		else
		{
			chaccess.dbrequest  = dbrVar.toInt();
		};

		if(clsName.compare("CABlinkLabel") == 0 && var.toString().compare("pvname") != 0) 
		{
			qDebug("PVName:%s", var.toString().toStdString().c_str());

#if 0
			QString exPV = var.toString();
			if(exPV.compare("DDS1A_HEALTH_STATE")== 0||exPV.compare("DDS1B_HEALTH_STATE") == 0 ||
			   exPV.compare("DDS2_HEALTH_STATE") == 0||exPV.compare("ER_HEALTH_STATE")    == 0 ||
			   exPV.compare("MC_HEALTH_STATE")   == 0||exPV.compare("ECEHR_HEALTH_STATE") == 0 ||
			   exPV.compare("TS_HEALTH_STATE")   == 0||exPV.compare("RBA_HEALTH_STATE")   == 0 )
			{
				;
			}
			else
			{
				vecICSHealth.push_back(0);
				cablinkcount++;
			};
#else
			vecICSHealth.push_back(0);
			cablinkcount++;
#endif	
		};
		register_chacc.push_back(chaccess);
		hash_chacc.insert(widget->objectName(), chaccess);
	};

	fill(vecICSHealth.begin(), vecICSHealth.end(), 0);
#endif
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  		eventFilter override function. for mouse event redefine.
 *  Not used
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::mousePressEvent(QMouseEvent *event)
{
	//static int cnt = 0;
	if (event->button() == Qt::RightButton) 
	{
		QWidget *child = childAt(event->pos());
		QString objName = child->metaObject()->className();
		if ( objName.compare("CADisplayer") == 0 )
		{
			//CADisplayer *pDisp = (CADisplayer*)child;
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			//CAGraphic *pGraph = (CAGraphic*)child;
		};
	};
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  KWT widgets included each page update the value 
                             for user defining periodic timer event.
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::getValue()
{
	ChAccess *pchacc;
	hash_chacciter = hash_chacc.constBegin();
	while ( hash_chacciter != hash_chacc.constEnd() ) 
	{
		pchacc = (ChAccess*)&(hash_chacciter.value());
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
			}
#endif
		}
		else if ( objName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisp = (CADisplayer*)pchacc->pobj;
			pDisp -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity, pchacc->prec);
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			CAGraphic *pcagraphic = (CAGraphic*)pchacc->pobj;
			pcagraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
		}
		else if ( objName.compare("CALabel") == 0 )
		{
			CALabel *pLabel = (CALabel*)pchacc->pobj;
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
		++hash_chacciter;
	};
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  channel access list defined in hash table  print
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::printchannel()
{
	ChAccess *pchacc;
	hash_chacciter = hash_chacc.constBegin();
	while ( hash_chacciter != hash_chacc.constEnd() ) 
	{
		pchacc = (ChAccess*)&(hash_chacciter.value());
		// qDebug("index:%s, opvname:%s, pobj:%p", hash_chacciter.key().toStdString().c_str(), pchacc->pvname.c_str(), pchacc->pobj);
		++hash_chacciter;
	}
}
/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  public method for QT channel access thread.
 *  generating QT signal about EPICS ca monitor event.
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::UpdateObj(const QString &keyname, const double &dvalue, const short &severity, const short &precision)
{
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(keyname);
	if ( chacciter != hash_chacc.constEnd() )
	{
		chacciter.value().pvvalue = dvalue;
		chacciter.value().severity = severity;
		chacciter.value().prec = precision;
	};
	emit updatesignal(keyname, severity, precision);
}

/* ---------------------------------------------------------------------------
 *	Description by shbaek: 
 *		UpdateObj for String
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::UpdateObj(const QString &keyname, const QString &dvalue, const short &severity, const short &precision)
{
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
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  channel access connection status
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::ConnectionStatusObj(const QString &keyname, const short &connstatus)
{
	QHash<QString, ChAccess>::iterator chacciter;
	chacciter = hash_chacc.find(keyname);
	if ( chacciter != hash_chacc.constEnd() )
	{
		chacciter.value().connstatus = connstatus;
	};
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::removePlots()
{
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  ui page refresh time set
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::timeSet()
{
	 CAUITime *puitime = m_pwidget->findChild<CAUITime *>("CAUITime");
	 if (puitime == 0)
	 {
		 connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),
				 this,SLOT(updateobj(const QString&, const short&, const short &)) );
	 }
	 else
	 {
		 QVariant time = puitime->property("timeset");
		 QVariant second = puitime->property("periodicsecond");

		 if(time.toInt() == CAUITime::Periodic)
		 {
			 int periodic = second.toInt()*1000;
			 (void)startTimer(periodic);
		 }
		 else
		 {
			 connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),
					 this,SLOT(updateobj(const QString&, const short&, const short &)) );
		 };
	};

	 if(m_prefix.isNull()==true) startTimer(1000);
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  Using QUIloader, get pvname property and register EventFilter for QWidget
 *  QThread creation per 1 ui file
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::init1()
{
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

	m_pwidget->installEventFilter(this);
	mChThr = new ControlMonitorThr(register_chacc, this);

	//timeLabel
	m_ptimeLabel = m_pwidget->findChild<QLabel *>("timeLabel");

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();

	if(m_filename.compare("../ui/ControlSystemMonitoring.ui") == 0)
	{
		mipCheckthr = new IPCheckThr(this);
	}
}

void ControlMonitorChannel::closeEvent(QCloseEvent* /*event*/)
{
	delete this;
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::init3(const QList<QString>& stringlist)
{
	QList<QString> m_pvlists = stringlist;
	if ( !m_pwidget ) return;

	setpvs(m_pwidget, m_pvlists);

	m_pwidget->installEventFilter(this);
	mChThr = new ControlMonitorThr(register_chacc, this);

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();

}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
void
ControlMonitorChannel::init2()
{
	if ( !m_pwidget ) return;
	m_pwidget->hide();
	getchannel(m_pwidget);

	m_pwidget->installEventFilter(this);
	mChThr = new ControlMonitorThr(register_chacc, this);

#if 1
	QTimer::singleShot(1000, this, SLOT(getValue()));
	timeSet();
#else
	connect(this,SIGNAL(updatesignal(const QString&, const short&, const short&)),this,SLOT(updateobj(const QString&, const short&, const short &)) );
#endif
	m_pwidget->show();
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  Update current ui page <- startTimer
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::timerEvent(QTimerEvent *)
{
	int caval = 0;
	int ncnt = count(vecICSHealth.begin(), vecICSHealth.end(), 1);

	QDateTime curTime = QDateTime::currentDateTime();

	QString strTime = curTime.toString("yyyy-MM-dd hh:mm:ss");
	m_ptimeLabel->setText(strTime);

	qDebug("Fail Count: %d, ICSHealthSize:%d", ncnt, vecICSHealth.size());
	//if(ncnt > 0 && mhealthcount != ncnt )
	if(ncnt > 0 )
	{
		//CCS_LCSST_ICS -> Bad
		caval = 0;
		mhealthcount = ncnt;
		caPut(DBR_INT, "CCS_LCSST_ICS", caval);
	}
	//else if(ncnt == 0 && mhealthcount != ncnt )
	else if(ncnt == 0)
	{
		//CCS_LCSST_ICS -> Good
		caval = 2;
		mhealthcount = ncnt;
		caPut(DBR_INT, "CCS_LCSST_ICS", caval);
	};
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  set current ui page.
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::SetUiCurIndex(int curindex)
{
	m_curindex = curindex;
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  KWT widgets included each page update the value for channel access monitoring event.
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::updateobj(const QString& objname, const short &severity, const short &precision)
{
	//static int dbcount = 0;
	//qDebug("Debug Count;%d", dbcount++);

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
				// qDebug("updateobj CAUITime. value:%d", (int)pchacc->pvvalue);
			}
#endif
		}
		else if ( objName.compare("CADisplayer") == 0 )
		{
			CADisplayer *pDisp = (CADisplayer*)pchacc->pobj;
			pDisp -> changeValue(pchacc->connstatus, pchacc->pvvalue, severity, precision);
		}
		else if ( objName.compare("CAGraphic") == 0 )
		{
			CAGraphic *pGraphic = (CAGraphic*)pchacc->pobj;
			pGraphic -> changeValue(pchacc->connstatus, pchacc->pvvalue, severity);
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
		else if ( objName.compare("CABlinkLabel") == 0 )
		{
			//++leesi
			CABlinkLabel *pBlinkLabel = (CABlinkLabel*)pchacc->pobj;
#if 0
			string exPV = pchacc->pvname;
			if(exPV.compare("DDS1A_HEALTH_STATE")== 0||exPV.compare("DDS1B_HEALTH_STATE") == 0 ||
			   exPV.compare("DDS2_HEALTH_STATE") == 0||exPV.compare("ER_HEALTH_STATE")    == 0 ||
			   exPV.compare("MC_HEALTH_STATE")   == 0||exPV.compare("ECEHR_HEALTH_STATE") == 0 ||
			   exPV.compare("TS_HEALTH_STATE")   == 0||exPV.compare("RBA_HEALTH_STATE")   == 0 )
			{
				;
			}
			else
			{
				if(pchacc->severity == -1 || pchacc->pvvalue == 1)
					vecICSHealth[pchacc->index] = 1;
				else
					vecICSHealth[pchacc->index] = 0;
			};
#else
			if(pchacc->severity == -1 || pchacc->pvvalue == 1)
				vecICSHealth[pchacc->index] = 1;
			else
				vecICSHealth[pchacc->index] = 0;
#endif

			pBlinkLabel->changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
		}
		else if ( objName.compare("BlinkLine") == 0 )
		{
			//++leesi
			BlinkLine *pBlinkLine = (BlinkLine*)pchacc->pobj;
			pBlinkLine->changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
		}
		else if ( objName.compare("CAListBox") == 0 )
		{
			//++leesi
			CAListBox *pList = (CAListBox*)pchacc->pobj;
			pList -> setPrefix(m_prefix);
			pList->changeValue(pchacc->connstatus, pchacc->pvvalue, pchacc->severity);
		}
	};
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  connection signal and slot to control widgets
 * ---------------------------------------------------------------------------*/
void 
ControlMonitorChannel::regControlObject(const QWidget * /*pwidget*/, const QString& /*controlObj*/) 
{
};
void
ControlMonitorChannel::keyPressEvent(QKeyEvent *event )
{
	int key = event->key();
	quint32 modifier = event->nativeModifiers();
	qDebug("modifier:%x, %x", modifier, key);
	if ((modifier==0x14 || modifier == 0x4)&& key == Qt::Key_P)
	{
		QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
		QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
		QString format = "jpg";
		//mpixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
		mpixmap = QPixmap::grabWidget(m_pwidget);
		mpixmap.save(filename, format.toAscii());
	};
}
