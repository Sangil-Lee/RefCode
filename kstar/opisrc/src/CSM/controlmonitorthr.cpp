/*************************************************************************\
* Copyright (c) 2010 The National Fusion Research Institute
\*************************************************************************/
/*  
 *
 *         NFRI (National Fusion Research Institute)
 *    113 Gwahangno, Yusung-gu, Daejeon, 305-333, South Korea
 *
 */

/*  qtchaccessthr.cpp
 * ---------------------------------------------------------------------------
 *  * REVISION HISTORY *
 * ---------------------------------------------------------------------------
 *
 * Revision 1  2006-12-29
 * Author: Sangil Lee [silee]
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
#include <algorithm>
#include "controlmonitorthr.h"
#include "caDisplayer.h"
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
 *  define ChannelNode class for EPICS channel access
 * ---------------------------------------------------------------------------*/
class ChannelNode
{
public:
	ChannelNode():objname(QString("")),pvname(QString("")),
			 status(0), onceConnected(0), acacc(NULL)
	{
		memset(&ch_id, 0, sizeof(chid));
	};
	QString			objname;
	QString			pvname;
    chid			ch_id;
	chtype			dbfType;
	int				status;
	char			onceConnected;
	ControlMonitorChannel	*acacc;
	//++leesi
	int             dbrequest;
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  EPICS channel access information print
 * ---------------------------------------------------------------------------*/
static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  exception callback function for EPICS channel access
 * ---------------------------------------------------------------------------*/
static void exceptionCallback(struct exception_handler_args args)
{
#if 0
    ChannelNode *pNode = (ChannelNode *)ca_puser(args.chid);
	//ControlMonitorChannel	*pacacc = (ControlMonitorChannel *)pNode->acacc;

    pNode->dbfType = ca_field_type(pNode->ch_id);

	QString pvname = (args.chid?ca_name(args.chid):noname);
    if(args.chid) printChidInfo(args.chid,"exceptionCallback");
#endif

#if 1
    chid	chid = args.chid;
    //long	stat = args.stat; /* Channel access status code*/
    static char *noname = "unknown";
    const char  *channel;
    channel = (chid ? ca_name(chid) : noname);
    if(chid) printChidInfo(chid,"exceptionCallback");
#endif
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  channel access monitoring event callback method 
 * ---------------------------------------------------------------------------*/
static void eventCallback(struct event_handler_args args)
{
    ChannelNode *pNode = (ChannelNode *)ca_puser(args.chid);
	ControlMonitorChannel	*pacacc = (ControlMonitorChannel *)pNode->acacc;
    pNode->dbfType = ca_field_type(pNode->ch_id);

	QString pvname = ca_name(args.chid);
	QString strvalue;

    if( args.status != ECA_NORMAL ) 
	{
		printChidInfo(args.chid,"eventCallback");
    } 
	else 
	{
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
					pacacc->UpdateObj(pNode->objname, pdata->value, pdata->severity, pdata->precision);
				}
				break;
		};
    };
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  register eventCallback and connection status
 * ---------------------------------------------------------------------------*/
static void connectionCallback(struct connection_handler_args args)
{
    ChannelNode *pNode = (ChannelNode *)ca_puser(args.chid);
	ControlMonitorChannel	*pacacc = (ControlMonitorChannel *)pNode->acacc;
    if ( args.op == CA_OP_CONN_UP ) 
	{
        //int dbrType;
        //pNode->nElems  = ca_element_count(pNode->ch_id);
        //pNode->dbfType = ca_field_type(pNode->ch_id);
        pNode->onceConnected = 1;
        pNode->status = ECA_CONN;
		pacacc->ConnectionStatusObj(pNode->objname, ECA_CONN );
		//++leesi
		ca_create_subscription (pNode->dbrequest, 0, pNode->ch_id, DBE_VALUE|DBE_ALARM, eventCallback, (void*)pNode, NULL);
    }
    else if ( args.op == CA_OP_CONN_DOWN ) {
        pNode->status = ECA_DISCONN;
		pacacc->ConnectionStatusObj(pNode->objname, ECA_DISCONN );
		pacacc->UpdateObj(pNode->objname, -1,-1,0);
    }
};

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorThr::ControlMonitorThr(VecChacc &vecchacc, ControlMonitorChannel *pattach, QObject *parent):QThread(parent),m_pattach(pattach)
{
	register_chacc = vecchacc;
    ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);
	start();
	mstop=false;
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
ControlMonitorThr::~ControlMonitorThr()
{
	qDebug("QThread Terminate");
	//exit();
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *  QThread run
 * ---------------------------------------------------------------------------*/
void ControlMonitorThr::run()
{
	//printchannel();	//for Debug
    ChannelNode	chnode[register_chacc.size()];
	unsigned int i = 0;
	for ( reg_chacciter = register_chacc.begin(); reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		chnode[i].objname	= QString(reg_chacciter->objname.c_str());
		chnode[i].acacc		= m_pattach;
		chnode[i].pvname	= QString(reg_chacciter->pvname.c_str());
		chnode[i].dbrequest	= reg_chacciter->dbrequest;
		ca_create_channel(reg_chacciter->pvname.c_str(), connectionCallback, &chnode[i], 10, (oldChannelNotify**)&chnode[i].ch_id);
	};
	ca_pend_io(2.0);
	mutex.lock();
	for ( reg_chacciter = register_chacc.begin(), i=0; reg_chacciter != register_chacc.end(); ++reg_chacciter, i++)
	{
		if (chnode[i].onceConnected == 0)
		{
			string spvname = chnode[i].pvname.toStdString();
			chnode[i].status = ECA_DISCONN;
			if(spvname.compare("pvname") == 0) 
				m_pattach->UpdateObj(chnode[i].objname,-1,-2,0);
			else
				m_pattach->UpdateObj(chnode[i].objname,-1,-1,0);
		};
	};
	mutex.unlock();
#if 1
	//in vm image, ca_pend_event(0.0001) stop processing on running;
	while(true)
	{
		if(getStop() == true) break;
		ca_pend_event(0.0001);
	};
	ca_context_destroy();
#else
	//in vm image, this code no problem why?? U know?
	ca_pend_event(0.0);
#endif
}

/* ---------------------------------------------------------------------------
 *	Description by silee: 
 *
 * ---------------------------------------------------------------------------*/
void
ControlMonitorThr::printchannel()
{
	int i = 0;
	for ( reg_chacciter = register_chacc.begin(); reg_chacciter != register_chacc.end(); ++reg_chacciter,i++ )
	{
		qDebug("ControlMonitorThr::pvname[%d]:%s, chid[%d]:%p", i, reg_chacciter->pvname.c_str(), i, reg_chacciter->ch_id);
	};
}
