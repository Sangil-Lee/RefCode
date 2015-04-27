#include <algorithm>
#include "qtchaccessthr.h"
#include "caDisplayer.h"
//#include "caTableList.h"

//class CATableList;


static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
    chid	chid = args.chid;
    CHNODE *pNode = (CHNODE *)ca_puser(args.chid);
	if(pNode == NULL ) return;
    if(chid) printChidInfo(chid,"exceptionCallback");
    //long	stat = args.stat; /* Channel access status code*/
    //const char  *channel;
    //static char *noname = "unknown";
    //channel = (chid ? ca_name(chid) : noname);
	//qDebug("No PV - pv(%s)", pNode->pvname.toStdString().c_str());
};

static void accessRightsCallback(struct access_rights_handler_args args)
{
    chid	chid = args.chid;
    printChidInfo(chid,"accessRightsCallback");
};

static void eventCallback(struct event_handler_args args)
{
    CHNODE *pNode = (CHNODE *)ca_puser(args.chid);
	AttachChannelAccess	*pacacc = (AttachChannelAccess *)pNode->acacc;
    pNode->dbfType = ca_field_type(pNode->ch_id);

	QString pvname = ca_name(args.chid);
	QString strvalue;
	unsigned int chindex = pNode->chindex;

    if( args.status != ECA_NORMAL ) 
	{
		printChidInfo(args.chid,"eventCallback");
    } 
	else 
	{
		QString objName = pNode->objname;
		if(objName.contains("CATableList", Qt::CaseInsensitive) == true)
		{
			struct dbr_time_string *pdata = (struct dbr_time_string*)args.dbr;
			QTableWidgetItem *pItem = pNode->pItem;
			strvalue = QString(pdata->value);
			pItem->setText(strvalue);

			//CATableList *pTable = (CATableList*)pItem->tableWidget();
			//pItem->setStatusTip(QString("PVname : ").append(pvname).append(QString(", Value: ")).append(strvalue));
			//pTable->TableUpdate();
			return;
		};

		//++leesi
		switch(pNode->dbrequest)
		{
			//case DBR_GR_CHAR:
			case DBR_TIME_STRING:
				{
					struct dbr_time_string *pdata = (struct dbr_time_string*)args.dbr;
					strvalue = QString(pdata->value);
					pacacc->UpdateObj(pNode->objname, strvalue, pdata->severity);
				}
				break;
			case DBR_GR_DOUBLE:
			default:
				{
					struct dbr_gr_double* pdata = (struct dbr_gr_double*)args.dbr;
					if(objName.contains("CAPopUp", Qt::CaseInsensitive) == true)
						qDebug("PVName: %s, Value:%f", pvname.toStdString().c_str(), pdata->value);
#if 0
					//for test,,,
					static int count = 0;
					qDebug("CAMon Value:%f, count(%d)",pdata->value, count++);
#endif
					pacacc->UpdateObj(pNode->objname, pdata->value, pdata->severity, pdata->precision);
				}
				break;
		};
    };
};

static void connectionCallback(struct connection_handler_args args)
{
    CHNODE *pNode = (CHNODE *)ca_puser(args.chid);
	AttachChannelAccess	*pacacc = (AttachChannelAccess *)pNode->acacc;
    //printf("PV(%s) state(%d)\n", ca_name(args.chid),ca_state(args.chid));
    if ( args.op == CA_OP_CONN_UP ) 
	{
        //int dbrType;
        //pNode->nElems  = ca_element_count(pNode->ch_id);
        //pNode->dbfType = ca_field_type(pNode->ch_id);
        pNode->onceConnected = 1;
        pNode->status = ECA_CONN;
		pacacc->ConnectionStatusObj(pNode->objname, ECA_CONN );
		//++leesi
		//ca_create_subscription (pNode->dbrequest, 0, pNode->ch_id, DBE_VALUE|DBE_ALARM, eventCallback, (void*)pNode, NULL);
		ca_create_subscription (pNode->dbrequest, 0, pNode->ch_id, DBE_VALUE|DBE_ALARM, eventCallback, (void*)pNode, &(pNode->ev_id));
		//ca_clear_event(pNode->ev_id);
    }
    else if ( args.op == CA_OP_CONN_DOWN ) {
        pNode->status = ECA_DISCONN;
		pacacc->ConnectionStatusObj(pNode->objname, ECA_DISCONN );
		pacacc->UpdateObj(pNode->objname, -1,0,0);
    }
	else
	{
		qDebug("NO PV - pv(%s)", pNode->pvname.toStdString().c_str());
	}
};

ChannelAccessThr::ChannelAccessThr(VecChacc &vecchacc, AttachChannelAccess *pattach, QObject *parent):QThread(parent),m_pattach(pattach)
{
	register_chacc = vecchacc;
    ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);
	start();
	mstop = false;
	mbclearev = false;
	mbaddev = false;
}

ChannelAccessThr::~ChannelAccessThr()
{
	qDebug("Thread Terminate");
	//exit();
}

#if 0
void ChannelAccessThr::run()
{
	//printchannel();	//for Debug
    CHNODE	mynode[register_chacc.size()];
	unsigned int i = 0;
	//const char  SHM_KEY[]  = "0x50000000";
	//const key_t  SHM_KEY  = 0x50000000;
	//char *shmPtr = m_pattach->mCachedData.open (SHM_KEY, register_chacc.size()*sizeof(CachedData), CachedChannelAccess::RT_SHM_CREAT | CachedChannelAccess::RT_SHM_RDWR);
	mutex.lock();
	for ( reg_chacciter = register_chacc.begin(); reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		mynode[i].objname	= QString(reg_chacciter->objname.c_str());
		mynode[i].acacc		= m_pattach;
		mynode[i].pvname	= QString(reg_chacciter->pvname.c_str());
		mynode[i].dbrequest	= reg_chacciter->dbrequest;
		mynode[i].chindex	= reg_chacciter->chindex;
		ca_create_channel(reg_chacciter->pvname.c_str(), connectionCallback, &mynode[i], 10, (oldChannelNotify**)&mynode[i].ch_id);
	};
	ca_pend_event(1.0);
	for ( reg_chacciter = register_chacc.begin(), i=0; reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		if (mynode[i].onceConnected)
		{
			//qDebug("Conn PV:%s", mynode[i].pvname.toStdString().c_str() );
		}
		else if (mynode[i].onceConnected == 0)
		{
			//qDebug("NotConn PV:%s", mynode[i].pvname.toStdString().c_str() );
			m_pattach->ConnectionStatusObj(mynode[i].objname, -1);
		};
	}
	mutex.unlock();
#if 1
	//in vm image, ca_pend_event(0.0001) stop processing on running;
	while(true)
	{
		if(getStop() == true) break;
		ca_pend_event(0.0001);
	};
    ca_context_destroy();
	exit();
#else
	//in vm image, this code no problem why?? U know?
	ca_pend_event(0.0);
#endif
}
#else
void ChannelAccessThr::run()
{
	unsigned int i = 0;
	vecnode.reserve(register_chacc.size());
	mutex.lock();
	for ( reg_chacciter = register_chacc.begin(); reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		CHNODE node;
		node.objname   = QString(reg_chacciter->objname.c_str());
		node.acacc     = m_pattach;
		node.pvname    = QString(reg_chacciter->pvname.c_str());
		node.dbrequest = reg_chacciter->dbrequest;
		node.chindex   = reg_chacciter->chindex;
		node.pItem   = reg_chacciter->pItem;
		vecnode.push_back(node);
		ca_create_channel(reg_chacciter->pvname.c_str(), connectionCallback, (void*)&(vecnode.at(i)), 10, (oldChannelNotify**)&((vecnode.at(i)).ch_id));
	};
	ca_pend_event(1.0);
	for ( reg_chacciter = register_chacc.begin(), i=0; reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		CHNODE *pNode  = &vecnode.at(i); 
		if (pNode->onceConnected)
		{
		}
		else if (pNode->onceConnected == 0)
		{
			m_pattach->ConnectionStatusObj(pNode->objname, -1);
		};
	}
	mutex.unlock();

	while(true)
	{
		if(getStop() == true) break;
		ca_pend_event(0.0001);
	};
    ca_context_destroy();
	exit();
}
#endif

void ChannelAccessThr::AddEventsOnPage(const int page)
{
	//qDebug("AddEvents-Page[%d], VecNodeSize:%d",page, vecnode.size());
	for(size_t i = 0; i < vecnode.size(); i++)
	{
		CHNODE *pNode = &vecnode.at(i);
		//qDebug("Node(%p)", pNode);
		if(pNode == NULL) continue;
		ca_add_event(pNode->dbrequest,pNode->ch_id,eventCallback,(void*)pNode,&(pNode->ev_id));
	};
	setClearEvents(false);
}

void ChannelAccessThr::ClearEventsOnPage(const int page)
{
	if(getClearEvents()==true) return;
	//qDebug("ClearEvents-Page[%d]",page);
	for(size_t i = 0; i < vecnode.size(); i++)
	{
		CHNODE *pNode = &vecnode.at(i);
		if(pNode == NULL) continue;

		int state = ca_state(pNode->ch_id);
		ca_pend_event(0.001);
		if(state == 0) 
		{
			qDebug("Not Conntected - pv(%s)", pNode->pvname.toStdString().c_str());
			continue;
		};
		SEVCHK(ca_clear_event(pNode->ev_id), NULL);
	};
	setClearEvents(true);
}
bool ChannelAccessThr::getClearEvents()
{
	return mbclearev;
}
bool ChannelAccessThr::getAddEvents()
{
	return mbaddev;
}
void ChannelAccessThr::setClearEvents(const bool bevent)
{
	 mbclearev = bevent;
}
void ChannelAccessThr::setAddEvents(const bool bevent)
{
	mbaddev = bevent;
}

void ChannelAccessThr::printchannel()
{
	//qDebug("ObjSize:%d",register_chacc.size());
	int i = 0;
	for ( reg_chacciter = register_chacc.begin(); reg_chacciter != register_chacc.end(); ++reg_chacciter,i++ )
	{
		qDebug("ChannelAccessThr::pvname[%d]:%s, chid[%d]:%p", i, reg_chacciter->pvname.c_str(), i, reg_chacciter->ch_id);
	};
}
