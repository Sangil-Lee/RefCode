/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* Copyright (c) 2002 Berliner Elektronenspeicherringgesellschaft fuer
*     Synchrotronstrahlung.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/
/* 
 *  caarrayget.cpp,v 0.0.1.1 2009/01/19 15:17:03 
 *  Author: leesi(NFRI) base on catools(caget)
 */

#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#define epicsAlarmGLOBAL
#include <alarm.h>
#undef epicsAlarmGLOBAL
#include <epicsTime.h>
#include <cadef.h>

#include "caArrayThread.h"

/* Time stamps for program start, previous value (used for relative resp.
 * incremental times with monitors) */
int QtCAArrayDataThread::nRead = 0;

/*+**************************************************************************
 *
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Allocates the dbr structure and copies the data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/
void event_handler (evargs args)
{
    pv* ppv = (pv*)args.usr;
    ppv->status = args.status;
    if (args.status == ECA_NORMAL)
    {
        ppv->dbrType = args.type;
        ppv->nElems = args.count;
        ppv->value   = calloc(1, dbr_size_n(args.type, args.count));
        memcpy(ppv->value, args.dbr, dbr_size_n(args.type, args.count));
        ppv->onceConnected = 1;
		QtCAArrayDataThread::nRead++;
    }
}

void sprint_long (char *ret, long val)
{
    sprintf(ret, "%ld", val); /* decimal */
}

QtCAArrayDataThread::QtCAArrayDataThread(): nConn(0)
{
	init();
}
QtCAArrayDataThread::~QtCAArrayDataThread()
{
}
void
QtCAArrayDataThread::init()
{
	tsType = absolute;             /* Timestamp type flag (-riI options) */
	mbstop = false;
	//pvs[n].nElems  = ca_element_count(pvs[n].chid);
	//pvs[n].dbfType = ca_field_type(pvs[n].chid);
	//if (ca_state(pvs[n].chid) == cs_conn)
    //result = ca_pend_io(caTimeout);
	//ca_pend_event(slice);
	//ca_array_get_callback(dbrType, reqElems, pvs[n].chid, event_handler, (void*)&pvs[n]);
	//pvs[n].value = calloc(1, dbr_size_n(dbrType, reqElems));
	//result = ca_array_get(dbrType, reqElems, pvs[n].chid, pvs[n].value);
	//printf("*** CA error %s\n", ca_message(pvs[n].status));

}
/*+**************************************************************************
 *
 * Function:	val2str
 *
 * Description:	Print (convert) value to a string
 *
 * Arg(s) In:	v      -  Pointer to dbr_... structure
 *              type   -  Numeric dbr type
 *              index  -  Index of element to print (for arrays) 
 *
 * Return(s):	Pointer to static output string
 *
 **************************************************************************-*/
char*	QtCAArrayDataThread::val2str (const void *v, unsigned type, int index)
{
	static char str[500];
	char ch;
	void *val_ptr;
	unsigned base_type;

	base_type = type % (LAST_TYPE+1);
	//qDebug("basetype:%d, type:%d, listtype:%d, DBR_DOUBLE:%d", base_type, type, LAST_TYPE, DBR_DOUBLE);

	if (type == DBR_STSACK_STRING || type == DBR_CLASS_NAME)
		base_type = DBR_STRING;

	val_ptr = dbr_value_ptr(v, type);
	//qDebug("valptr:%p, v:%p", val_ptr, v);

	switch (base_type) {
		case DBR_STRING:
			sprintf(str, "%s",  ((dbr_string_t*) val_ptr)[index]);
			break;
		case DBR_FLOAT:
			sprintf(str, "%g", ((dbr_float_t*) val_ptr)[index]);
			break;
		case DBR_DOUBLE:
			sprintf(str, "%g", ((dbr_double_t*) val_ptr)[index]);
			break;
		case DBR_CHAR:
			ch = ((dbr_char_t*) val_ptr)[index];
			if(ch > 31 )
				sprintf(str, "%d \'%c\'",ch,ch);
			else
				sprintf(str, "%d",ch);
			break;
		case DBR_INT:
			sprint_long(str, ((dbr_int_t*) val_ptr)[index]);
			break;
		case DBR_LONG:
			sprint_long(str, ((dbr_long_t*) val_ptr)[index]);
			break;
		case DBR_ENUM:
			{
				dbr_enum_t *val = (dbr_enum_t *)val_ptr;
				if (dbr_type_is_GR(type))
					sprintf(str,"%s (%d)", 
							((struct dbr_gr_enum *)v)->strs[val[index]],val[index]);
				else if (dbr_type_is_CTRL(type))
					sprintf(str,"%s (%d)", 
							((struct dbr_ctrl_enum *)v)->strs[val[index]],val[index]);
				else
					sprintf(str, "%d", val[index]);
			}
	}
	return str;
}


/*+**************************************************************************
 *
 * Function:	create_pvs
 *
 * Description:	Creates an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *              pCB   -  Connection state change callback
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs created
 *                  1  -  Some PV(s) not created
 *
 **************************************************************************-*/
 
int	QtCAArrayDataThread::create_pvs (pv* pvs, int nPvs, caCh *pCB)
{
    int n;
    int result;
    int returncode = 0;
                                 /* Issue channel connections */
    for (n = 0; n < nPvs; n++) {
        result = ca_create_channel (pvs[n].name, pCB, &pvs[n], CA_PRIORITY, &pvs[n].mychid);
		if (result != ECA_NORMAL) {
            qDebug("CA error %s occurred while trying "
                    "to create channel '%s'.", ca_message(result), pvs[n].name);
            pvs[n].status = result;
            returncode = 1;
        }
    }

    return returncode;
}


/*+**************************************************************************
 *
 * Function:	connect_pvs
 *
 * Description:	Connects an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs connected
 *                  1  -  Some PV(s) not connected
 *
 **************************************************************************-*/
 
int	QtCAArrayDataThread::connect_pvs (pv* pvs, int nPvs)
{
    int returncode = create_pvs ( pvs, nPvs, 0);
    if ( returncode == 0 ) 
	{
                            /* Wait for channels to connect */
        int result = ca_pend_io (caTimeout);
        if (result == ECA_TIMEOUT)
        {
            if (nPvs > 1)
            {
                qDebug("Channel connect timed out: some PV(s) not found.");
            } else {
                qDebug("Channel connect timed out: '%s' not found.", 
                        pvs[0].name);
            }
            returncode = 1;
        };
    };
	mpvs = pvs;
	mpvcnt = nPvs;
    return returncode;
}

/*+**************************************************************************
 *
 * Function:	caget
 *
 * Description:	Issue read requests, wait for incoming data
 * 		and print the data according to the selected format
 *
 * Arg(s) In:	pvs       -  Pointer to an array of pv structures
 *              nPvs      -  Number of elements in the pvs array
 *              request   -  Request type
 *              format    -  Output format
 *              dbrType   -  Requested dbr type
 *              reqElems  -  Requested number of (array) elements
 *
 * Return(s):	Error code: 0 = OK, 1 = Error
 *
 **************************************************************************-*/
 
int	QtCAArrayDataThread::caget (pv *pvs, int nPvs, RequestT request, OutputT format,
           chtype dbrType, unsigned long reqElems)
{
	unsigned int i;
	int n, result;

	for (n = 0; n < nPvs; n++) 
	{

		/* Set up pvs structure */
		/* -------------------- */
		/* Get natural type and array count */
		pvs[n].nElems  = ca_element_count(pvs[n].mychid);
		pvs[n].dbfType = ca_field_type(pvs[n].mychid);

		/* Set up value structures */
		if (format != specifiedDbr)
		{
			dbrType = dbf_type_to_DBR_TIME(pvs[n].dbfType); /* Use native type */
			if (dbr_type_is_ENUM(dbrType))                  /* Enums honour -n option */
			{
				if (enumAsNr) dbrType = DBR_TIME_INT;
				else          dbrType = DBR_TIME_STRING;
			};
		};
		/* Adjust array count */
		if (reqElems == 0 || pvs[n].nElems < reqElems)
			reqElems = pvs[n].nElems;
		/* Remember dbrType and reqElems */
		pvs[n].dbrType  = dbrType;
		pvs[n].reqElems = reqElems;

		/* Issue CA request */
		/* ---------------- */
		if (ca_state(pvs[n].mychid) == cs_conn)
		{
			nConn++;
			pvs[n].onceConnected = 1;
			if (request == callback)
			{                          /* Event handler will allocate value */
				result = ca_array_get_callback(dbrType, reqElems,
						pvs[n].mychid, event_handler, (void*)&pvs[n]);
			} 
			else 
			{
				/* Allocate value structure */
				pvs[n].value = calloc(1, dbr_size_n(dbrType, reqElems));
				result = ca_array_get(dbrType, reqElems,
						pvs[n].mychid, pvs[n].value);
			};
			pvs[n].status = result;
		} 
		else 
		{
			pvs[n].status = ECA_DISCONN;
		};
	};
	if (!nConn) return 1;              /* No connection? We're done. */
	/* Wait for completion */
	/* ------------------- */

	result = ca_pend_io(caTimeout);
	if (result == ECA_TIMEOUT)
		qDebug("Read operation timed out: some PV data was not read.");

	if (request == callback)    /* Also wait for callbacks */
	{
		double slice = caTimeout / PEND_EVENT_SLICES;
		for (n = 0; n < PEND_EVENT_SLICES; n++)
		{
			ca_pend_event(slice);
			if (nRead >= nConn) break;
		}
		if (nRead < nConn)
			qDebug("Read operation timed out: some PV data was not read.");
	}

	/* Print the data */
	/* -------------- */

	for (n = 0; n < nPvs; n++) 
	{
		reqElems = pvs[n].reqElems;
		switch (format) 
		{
			case plain:             /* Emulate old caget behaviour */
				if (reqElems <= 1) printf("%-30s ", pvs[n].name);
				else               printf("%s", pvs[n].name);
			case terse:
				if (pvs[n].status == ECA_DISCONN)
					printf("*** not connected\n");
				else if (pvs[n].status == ECA_NORDACCESS)
					printf("*** no read access\n");
				else if (pvs[n].status != ECA_NORMAL)
					printf("*** CA error %s\n", ca_message(pvs[n].status));
				else if (pvs[n].value == 0)
					printf("*** no data available (timeout)\n");
				else
				{
					if (reqElems > 1) printf(" %lu ", reqElems);
					/*++leesi araary data display.*/
					void * val_ptr = NULL;
					for (i=0; i<reqElems; ++i)
					{
						val_ptr = dbr_value_ptr(pvs[n].value, DBR_DOUBLE);
						//printf("%f* ", ((dbr_double_t*) val_ptr)[i]);
						printf("%s* ", val2str(pvs[n].value, pvs[n].dbrType, i));
					};
					printf("\n");
				};
				break;
			default :
				break;
		};
	};
	return 0;
}

#if 0
void QtCAArrayDataThread::run()
{
	//leesi++ for test 
	chid val_chid[2];
	std::string m_pvname[2]={"WF:WAVE1","WF:WAVE2"};
	struct dbr_double *data[2];
	for(int i = 0; i < 2; i++)
	{
		data[i] = (dbr_double*)calloc(1, dbr_size_n(DBR_DOUBLE, 30));
		ca_create_channel(m_pvname[i].c_str(), 0, 0, 0, &val_chid[i]);
	};
	ca_pend_io(0.2);

	void *val_ptr = NULL;
	static int count = 0;
	while(true)
	{
		qDebug("cnt:%d, stop:%d", count++, getStop());
		if(getStop()==true)
		{
			qDebug("QThread quit-1");
			break;
		};
		for(int i = 0; i<2; i++)
		{
			ca_array_get(DBR_DOUBLE,30,val_chid[i], (void*)data[i]);
			ca_pend_io(0.5);
			//qDebug("%s : %f\n",ca_name(val_chid), data.value);
			val_ptr = dbr_value_ptr(data[i], DBR_DOUBLE);
			for (int j=0; j<30; ++j)
			{
				printf("%f* ", ((dbr_double_t*) val_ptr)[j]);
				//printf("%s* ", val2str(pvs[n].value, pvs[n].dbrType, j));
			};
			printf("\n");
		};
		QThread::sleep(1);
	};
	free(data[0]);
	free(data[1]);
	quit();
}
#else
void QtCAArrayDataThread::run()
{
	size_t pvcnt = mvecpvnames.size();
	//struct dbr_double *data[pvcnt];
	struct dbr_double **data = (dbr_double**)calloc(pvcnt, sizeof(dbr_double*));
	chid val_chid[pvcnt];
	for(size_t i = 0; i < pvcnt; i++)
	{
		//qDebug("In Thread***1 - PV:%s", mvecpvnames.at(i).c_str());
		data[i]= (dbr_double*)calloc(1, dbr_size_n(DBR_DOUBLE, mreqelement));
		ca_create_channel(mvecpvnames.at(i).c_str(), 0, 0, 0, &val_chid[i]);
	};
	ca_pend_io(0.2);
	unsigned long reqElems[pvcnt];
	for(size_t i = 0; i < pvcnt; i++)
	{
		reqElems[i] = ca_element_count(val_chid[i]);
	};
	unsigned long minreqElem = *std::min_element(&reqElems[0], &reqElems[pvcnt-1]);

	qDebug("MinReqEle:%ld", minreqElem);
	if ( mreqelement > minreqElem ) mreqelement = minreqElem;
	//pvs[n].dbfType = ca_field_type(pvs[n].chid);
	//if (ca_state(pvs[n].chid) == cs_conn)
	void *val_ptr = NULL;
	static int count = 0;
	int thrstarted = 0;
	while(true)
	{
		if(getStop()==true)
		{
			qDebug("QThread quit-1");
			break;
		};
		if( isUpdatable() == false) goto SLEEP;
		for(size_t i = 0; i < pvcnt; i++)
		{
			ca_array_get(DBR_DOUBLE,mreqelement,val_chid[i], (void*)data[i]);
			ca_pend_io(0.5);
			val_ptr = dbr_value_ptr(data[i], DBR_DOUBLE);
			mparent->SetDataPtr(i,val_ptr);
		};
		if(thrstarted == 0)
		{
			thrstarted = 1;
			mparent->SetData();
		};
SLEEP:
		QThread::msleep(mmsec);
	};
	for(size_t i = 0; i < pvcnt; i++)
	{
		free(data[i]);
	};
	free(data);
	quit();
}
#endif
