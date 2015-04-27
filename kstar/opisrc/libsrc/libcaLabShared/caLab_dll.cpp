// This software is copyrighted by the HELMHOLTZ-ZENTRUM BERLIN FUER MATERIALIEN UND ENERGIE G.M.B.H., BERLIN, GERMANY (HZB).
// The following terms apply to all files associated with the software. HZB hereby grants permission to use, copy, and modify
// this software and its documentation for non-commercial educational or research purposes, provided that existing copyright
// notices are retained in all copies. The receiver of the software provides HZB with all enhancements, including complete
// translations, made by the receiver.
// IN NO EVENT SHALL HZB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING
// OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF, EVEN IF HZB HAS BEEN ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE. HZB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS,
// AND HZB HAS NO OBLIGATION TO PROVIDE MAlNTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

//==================================================================================================
// Name        : caLab.cpp
// Author      : Carsten Winkler, Tobias Höft
// Version     : 1.2.0.0
// Copyright   : HZB
// Description : library for get, put and handle events of EPICS variables (PVs) in LabVIEW™
//==================================================================================================

#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <string.h>
#include <cadef.h>
#include <epicsStdio.h>
#include <alarmString.h>
#include <epicsMath.h>
#include <epicsString.h>
#include "extcode.h"
#include "caLab.h"

#if MSWin && (ProcessorType == kX86)
	/* Windows x86 targets use 1-byte structure packing. */
	#pragma pack(push,1)
	#pragma warning (disable : 4103)
#endif

#if  IsOpSystem64Bit
	#define uPtr uQ
#else
	#define uPtr uL
#endif

using namespace std;

#define IsLabView7 1

#if IsLabView7
typedef struct {
	long dimSize;
	LStrHandle elt[1];
	} TD2;
typedef TD2 **TD2Hdl;

typedef struct {
	long dimSize;
	double elt[1];
	} TD3;
typedef TD3 **TD3Hdl;

typedef struct {
	LVBoolean status;
	long code;
	LStrHandle source;
	} TD4;

typedef struct {
	LStrHandle PVName;
	long elements;
	TD2Hdl valueArrayString;
	TD3Hdl valueArrayNumber;
	LStrHandle statusString;
	long statusNumber;
	LStrHandle severityString;
	long severityNumber;
	LStrHandle timeStampString;
	long timeStampNumber;
	TD2Hdl fieldNames;
	TD2Hdl fieldValues;
	TD4 errorIO;
	} TD1;
#else
typedef struct {
	int32_t dimSize;
	LStrHandle elt[1];
	} TD2;
typedef TD2 **TD2Hdl;

typedef struct {
	int32_t dimSize;
	double elt[1];
	} TD3;
typedef TD3 **TD3Hdl;

typedef struct {
	int32_t dimSize;
	LStrHandle elt[1];
	} TD4;
typedef TD4 **TD4Hdl;

typedef struct {
	LVBoolean status;
	int32_t code;
	LStrHandle source;
	} TD5;

typedef struct {
	LStrHandle PVName;
	int32_t elements;
	TD2Hdl valueArrayString;
	TD3Hdl valueArrayNumber;
	LStrHandle statusString;
	int16_t statusNumber;
	LStrHandle severityString;
	int16_t severityNumber;
	LStrHandle timeStampString;
	int32_t timeStampNumber;
	TD4Hdl fieldNames;
	TD2Hdl fieldValues;
	TD5 errorIO;
	} TD1;
#endif

struct PV {
	epicsMutexId lock;
	chid sChid;
	evid sEvid;
	short nChannelStatus;
	dbr_gr_enum sEnum;
	short dbfType;
	short dbrType;
	long lError;
	char szError[MAX_ERROR_SIZE];
	char szName[MAX_STRING_SIZE];
	long lElems;
	char* szValueArray;
	double* dValueArray;
	char szStatus[MAX_STRING_SIZE];
	short nStatus;
	char szSeverity[MAX_STRING_SIZE];
	short nSeverity;
	char szTimeStamp[MAX_STRING_SIZE];
	long lTimeStamp;
	char* szFieldNames;
	char* szFieldResults;
	long lFieldNameCount;
	double dTimeout;
	unsigned long lRefNum;
	TD1 *pCaLabCluster;
};

static PV** pPvList;
static long lPvCount;
static char bStop = 1;
static epicsMutexId initLock;
static epicsMutexId getValLock;
static epicsMutexId putValLock;
static epicsMutexId fireAndForgetLock;
static epicsMutexId connectLock;
static epicsMutexId putLock;
static epicsMutexId addEventLock;
static ca_client_context *pcac;
static char szDbgMessage[255];

struct sPutMe {
	long lElems;
	double dTimeOut;
	double* dValueArray;
	char* szValueArray;
	char szName[MAX_STRING_SIZE];
};

static queue<int> sConnectQueue;
static queue<sPutMe> sPutQueue;

static void postEvent(PV* pv) {
	epicsMutexLock(pv->lock);
	MgErr err = noErr;
	long lCount;

	// *** write value count ***
	pv->pCaLabCluster->elements = pv->lElems;

	// *** write value arrays ***
	// resize array
	if((*pv->pCaLabCluster->valueArrayString)->dimSize != pv->lElems) {
		err = NumericArrayResize(uPtr, 1, (UHandle*)&pv->pCaLabCluster->valueArrayString, pv->lElems);

		if(!err)
			(*pv->pCaLabCluster->valueArrayString)->dimSize = pv->lElems;
	}
	if((*pv->pCaLabCluster->valueArrayNumber)->dimSize != pv->lElems) {
		err = NumericArrayResize(fD, 1, (UHandle*)&pv->pCaLabCluster->valueArrayNumber, pv->lElems);
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
		(*pv->pCaLabCluster->valueArrayNumber)->dimSize = pv->lElems;
	}

	// write array
	for(lCount = 0; !err && lCount < pv->lElems; lCount++) { 
		if(!*(*pv->pCaLabCluster->valueArrayString)->elt || !(*pv->pCaLabCluster->valueArrayString)->elt[lCount] || (*((*pv->pCaLabCluster->valueArrayString)->elt[lCount]))->cnt != strlen(pv->szValueArray+(lCount*MAX_STRING_SIZE))) {
			err = NumericArrayResize(uB, 1, (UHandle*)&((*pv->pCaLabCluster->valueArrayString)->elt[lCount]), strlen(pv->szValueArray+(lCount*MAX_STRING_SIZE)));
			(*((*pv->pCaLabCluster->valueArrayString)->elt[lCount]))->cnt = strlen(pv->szValueArray+(lCount*MAX_STRING_SIZE));
		}
		if (!err && (*((*pv->pCaLabCluster->valueArrayString)->elt[lCount]))->cnt) {
			err = LStrPrintf((*pv->pCaLabCluster->valueArrayString)->elt[lCount], (CStr)"%s", pv->szValueArray+(lCount*MAX_STRING_SIZE));
		}
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
		(*pv->pCaLabCluster->valueArrayNumber)->elt[lCount] = pv->dValueArray[lCount];
    } 
	
	// *** write PV name ***
	if((*pv->pCaLabCluster->PVName)->cnt != strlen(pv->szName)) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->PVName, strlen(pv->szName));
		(*pv->pCaLabCluster->PVName)->cnt = strlen(pv->szName);
	}
	if (!err && (*pv->pCaLabCluster->PVName)->cnt) {
		err = LStrPrintf(pv->pCaLabCluster->PVName, (CStr)"%s", pv->szName);
	}
	if(err) {
		epicsMutexUnlock(pv->lock);
		return;
	}
	// *** write time stamp ***
	if((*pv->pCaLabCluster->timeStampString)->cnt != strlen(pv->szTimeStamp)) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->timeStampString, strlen(pv->szTimeStamp));
		(*pv->pCaLabCluster->timeStampString)->cnt = strlen(pv->szTimeStamp);
	}
	if (!err && (*pv->pCaLabCluster->timeStampString)->cnt) {
		err = LStrPrintf(pv->pCaLabCluster->timeStampString, (CStr)"%s", pv->szTimeStamp);
	}
	if(err) {
		epicsMutexUnlock(pv->lock);
		return;
	}
	pv->pCaLabCluster->timeStampNumber = pv->lTimeStamp;

	// *** write channel status ***
	if((*pv->pCaLabCluster->statusString)->cnt != strlen(pv->szStatus)) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->statusString, strlen(pv->szStatus));
		(*pv->pCaLabCluster->statusString)->cnt = strlen(pv->szStatus);
	}
	if (!err && (*pv->pCaLabCluster->statusString)->cnt) {
		err = LStrPrintf(pv->pCaLabCluster->statusString, (CStr)"%s", pv->szStatus);
	}
	if(err) {
		epicsMutexUnlock(pv->lock);
		return;
	}
	pv->pCaLabCluster->statusNumber = pv->nStatus;

	// *** write channel severity ***
	if((*pv->pCaLabCluster->severityString)->cnt != strlen(pv->szSeverity)) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->severityString, strlen(pv->szSeverity));
		(*pv->pCaLabCluster->severityString)->cnt = strlen(pv->szSeverity);
	}
	if (!err && (*pv->pCaLabCluster->severityString)->cnt) {
		err = LStrPrintf(pv->pCaLabCluster->severityString, (CStr)"%s", pv->szSeverity);
	}
	if(err) {
		epicsMutexUnlock(pv->lock);
		return;
	}
	pv->pCaLabCluster->severityNumber = pv->nSeverity;

	// *** write field names and values
	// resize array
	if((*pv->pCaLabCluster->fieldNames)->dimSize != pv->lFieldNameCount) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->fieldNames, pv->lFieldNameCount);
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->fieldValues, pv->lFieldNameCount);
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
	}
	// write array
	for(lCount=0; lCount<pv->lFieldNameCount; lCount++) {
		if(!*(*pv->pCaLabCluster->fieldNames)->elt || !(*pv->pCaLabCluster->fieldNames)->elt[lCount] || (*((*pv->pCaLabCluster->fieldNames)->elt[lCount]))->cnt != strlen(pv->szFieldNames+(lCount*MAX_STRING_SIZE))) {
			err = NumericArrayResize(uB, 1, (UHandle*)&((*pv->pCaLabCluster->fieldNames)->elt[lCount]), strlen(pv->szFieldNames+(lCount*MAX_STRING_SIZE)));
			(*((*pv->pCaLabCluster->fieldNames)->elt[lCount]))->cnt = strlen(pv->szFieldNames+(lCount*MAX_STRING_SIZE));
		}
		if (!err && (*((*pv->pCaLabCluster->fieldNames)->elt[lCount]))->cnt) {
			err = LStrPrintf((*pv->pCaLabCluster->fieldNames)->elt[lCount], (CStr)"%s", pv->szFieldNames+(lCount*MAX_STRING_SIZE));
		}
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
		if(!*(*pv->pCaLabCluster->fieldValues)->elt || !(*pv->pCaLabCluster->fieldValues)->elt[lCount] || (*((*pv->pCaLabCluster->fieldValues)->elt[lCount]))->cnt != strlen(pv->szFieldResults+(lCount*MAX_STRING_SIZE))) {
			err = NumericArrayResize(uB, 1, (UHandle*)&((*pv->pCaLabCluster->fieldValues)->elt[lCount]), strlen(pv->szFieldResults+(lCount*MAX_STRING_SIZE)));
			(*((*pv->pCaLabCluster->fieldValues)->elt[lCount]))->cnt = strlen(pv->szFieldResults+(lCount*MAX_STRING_SIZE));
		}
		if (!err && (*((*pv->pCaLabCluster->fieldValues)->elt[lCount]))->cnt) {
			err = LStrPrintf((*pv->pCaLabCluster->fieldValues)->elt[lCount], (CStr)"%s", pv->szFieldResults+(lCount*MAX_STRING_SIZE));
		}
		if(err) {
			epicsMutexUnlock(pv->lock);
			return;
		}
	}
	(*pv->pCaLabCluster->fieldNames)->dimSize = lCount;
	(*pv->pCaLabCluster->fieldValues)->dimSize = lCount;

	// *** write error status ***
	if((*pv->pCaLabCluster->errorIO.source)->cnt != strlen(pv->szError)) {
		err = NumericArrayResize(uB, 1, (UHandle*)&pv->pCaLabCluster->errorIO.source, strlen(pv->szError));
		(*pv->pCaLabCluster->errorIO.source)->cnt = strlen(pv->szError);
	}
	if (!err & (*pv->pCaLabCluster->errorIO.source)->cnt) {
		err = LStrPrintf(pv->pCaLabCluster->errorIO.source, (CStr)"%s", pv->szError);
	}
	if(err) {
		epicsMutexUnlock(pv->lock);
		return;
	}
	pv->pCaLabCluster->errorIO.status = 0;
	// post it!
	try {
		//leesi++
		PostLVUserEvent((MagicCookie_t*)pv->lRefNum, pv->pCaLabCluster);
	} catch(...) {
	}
	epicsMutexUnlock(pv->lock);
}

// avoid annoying messages at console
static void exceptionCallback(struct exception_handler_args args) {
	if(bStop) return;
    long	stat = args.stat;
	for(long lCount=0; lCount<lPvCount; lCount++) {
		epicsMutexLock(pPvList[lCount]->lock);
		pPvList[lCount]->lError = stat;
		epicsSnprintf(pPvList[lCount]->szError, MAX_ERROR_SIZE, "%s", ca_message(stat));
		if(pPvList[lCount]->lRefNum)
			postEvent(pPvList[lCount]);
		epicsMutexUnlock(pPvList[lCount]->lock);
	}
}

// callback for changed EPICS values
static void valueChanged(evargs args) {
	if(bStop) return;
	PV* pv = (PV*)ca_puser(args.chid);
	epicsMutexLock(pv->lock);
	pv->sChid = args.chid;
	if(args.status == ECA_NORMAL) {
		epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(ECA_NORMAL));
		pv->lError = ECA_NORMAL;
		// check type of PV and convert values to double and string array
		switch(args.type) {
			case DBR_TIME_STRING:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					char * tmp;
					pv->dValueArray[lCount] = strtod(((dbr_string_t*)dbr_value_ptr(args.dbr, args.type))[lCount], &tmp);
					epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40s", ((dbr_string_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			case DBR_TIME_SHORT:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_short_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					if(pv->dValueArray[lCount] < pv->sEnum.no_str) {
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40s", pv->sEnum.strs[(int)pv->dValueArray[lCount]]);
					} else {
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40d", ((dbr_short_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
					}
				}
				break;
			case DBR_TIME_CHAR:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_char_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40c", ((dbr_char_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			case DBR_TIME_LONG:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_long_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40d", ((dbr_long_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			case DBR_TIME_FLOAT:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_float_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40g", ((dbr_float_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			case DBR_TIME_DOUBLE:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_double_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40g", ((dbr_double_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			case DBR_TIME_ENUM:
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					pv->dValueArray[lCount] = ((dbr_enum_t*)dbr_value_ptr(args.dbr, args.type))[lCount];
					if(pv->dValueArray[lCount] < pv->sEnum.no_str)
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%-40s", pv->sEnum.strs[((dbr_enum_t*)dbr_value_ptr(args.dbr, args.type))[lCount]]);
					else
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%40d", ((dbr_enum_t*)dbr_value_ptr(args.dbr, args.type))[lCount]);
				}
				break;
			default:
				long result = ECA_BADTYPE;
				epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(result));
				pv->lError = result;
				pv->nChannelStatus &= ~PV_VALUE_RECEIVED;
		}
		// return read values
		if(pv->lError == ECA_NORMAL) {
			epicsTimeToStrftime(pv->szTimeStamp, sizeof(pv->szTimeStamp),  "%Y-%m-%d %H:%M:%S.%06f", &((struct dbr_time_short*)args.dbr)->stamp);
			pv->lTimeStamp = ((struct dbr_time_short*)args.dbr)->stamp.secPastEpoch;
			epicsSnprintf(pv->szStatus, MAX_STRING_SIZE, "%s", alarmStatusString[((struct dbr_time_short*)args.dbr)->status]);
			pv->nStatus = ((struct dbr_time_short*)args.dbr)->status;
			epicsSnprintf(pv->szSeverity, MAX_STRING_SIZE, "%s", alarmSeverityString[((struct dbr_time_short*)args.dbr)->severity]);
			pv->nSeverity = ((struct dbr_time_short*)args.dbr)->severity;
			pv->nChannelStatus |= PV_VALUE_RECEIVED;
		}
	} else {
		long result = args.status;
		epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(result));
		pv->lError = result;
		pv->nChannelStatus &= ~PV_VALUE_RECEIVED;
	}
	if(pv->lRefNum)
		postEvent(pv);
	epicsMutexUnlock(pv->lock);
}

// channel of PV has new state
static void connState(connection_handler_args args) {
	if(bStop) return;
	PV* pv = (PV*)ca_puser(args.chid);
	epicsMutexLock(pv->lock);
	pv->sChid = args.chid;
	switch (ca_state(args.chid)) {
		case cs_never_conn:
			pv->nChannelStatus &= ~PV_CONNECTED;
			break;
		case cs_prev_conn:
			// reset PV
			if(pv->sEvid) {
				ca_clear_subscription(pv->sEvid);
				pv->sEvid = 0;
			}
			if(pv->sChid) {
				ca_clear_channel(pv->sChid);
				pv->sChid = 0;
			}
			epicsSnprintf(pv->szStatus, MAX_STRING_SIZE, "%s", "INVALID");
			pv->nStatus	 = epicsSevInvalid;
			epicsSnprintf(pv->szSeverity, MAX_STRING_SIZE, "%s", "DISCONNECTED");
			pv->nSeverity = epicsAlarmComm;
			pv->nChannelStatus &= ~PV_CONNECTED;
			pv->lError = ECA_DISCONN;
			epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(pv->lError));
			break;
		case cs_conn:
			// initialize missing fields of PV
			if(pv->nChannelStatus &= PV_CONNECTED) {
				break;
			}
			pv->nChannelStatus |= PV_CONNECTED;
			pv->lElems = ca_element_count(args.chid);
			pv->dbfType = ca_field_type(args.chid);
			pv->dbrType = dbf_type_to_DBR_TIME(pv->dbfType);
			pv->sChid = args.chid;
			pv->dValueArray = (double*)realloc(pv->dValueArray, pv->lElems * sizeof(double));
			if(!pv->dValueArray) {
				pv->lError = ECA_ALLOCMEM;
				epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(ECA_ALLOCMEM));
			}
			for(long lCount=0; pv->dValueArray && lCount<pv->lElems; lCount++) {
				pv->dValueArray[lCount] = epicsNAN;
			}
			pv->szValueArray = (char*)realloc(pv->szValueArray, pv->lElems * MAX_STRING_SIZE * sizeof(char));
			if(!pv->szValueArray) {
				pv->lError = ECA_ALLOCMEM;
				epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(ECA_ALLOCMEM));
			}
			memset(pv->szValueArray, 0, pv->lElems * MAX_STRING_SIZE * sizeof(char));
			pv->lError = ca_create_subscription(pv->dbrType, pv->lElems, pv->sChid, DBE_VALUE | DBE_ALARM, valueChanged, (void*)pv, &pv->sEvid);
			epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(pv->lError));
			break;
		case cs_closed:
			pv->nChannelStatus &= ~PV_CONNECTED;
			break;
		default:
			pv->nChannelStatus &= ~PV_CONNECTED;
	}
	if(pv->lRefNum)
		postEvent(pv);
	epicsMutexUnlock(pv->lock);
}

// preinitialization of PV
static void initPV(PV* pv, char* szName) {
	pv->lock					= epicsMutexCreate();
	pv->sChid				= 0;
	pv->sEvid				= 0;
	pv->nChannelStatus		= 0;
	pv->sEnum.no_str			= 0;
	pv->dbfType				= DBF_NO_ACCESS;
	pv->dbrType				= LAST_BUFFER_TYPE;
	pv->lError				= ECA_NORMAL;
	pv->szError[0]			= 0;
	epicsSnprintf(pv->szName, MAX_STRING_SIZE, "%s", szName);
	pv->lElems				= 0;
	pv->lFieldNameCount		= 0;
	pv->szValueArray			= 0;
	pv->dValueArray			= 0;
	epicsSnprintf(pv->szStatus, MAX_STRING_SIZE, "%s", "INVALID");
	pv->nStatus				= epicsSevInvalid;
	epicsSnprintf(pv->szSeverity, MAX_STRING_SIZE, "%s", "DISCONNECTED");
	pv->nSeverity			= epicsAlarmComm;
	epicsSnprintf(pv->szTimeStamp, MAX_STRING_SIZE, "%s", "UNKNOWN");
	pv->lTimeStamp			= 0;
	pv->lRefNum				= 0;
	pv->pCaLabCluster		= 0;
}

// task for monitoring disconnected variables
static void caCheckTask(void) {
	long l,ll;
	ca_attach_context(pcac);
	while(!bStop) {
		for(l=0; !bStop && l<lPvCount; l++) {
			// if PV is already in "create queue" skip next steps
			if(pPvList[l]->nChannelStatus & PV_IN_QUEUE)
				continue;
			// put PVs without valid EPICS channel to "create queue" and skip next steps
			if(!pPvList[l]->sChid) {
				epicsMutexLock(connectLock);
				sConnectQueue.push(l);
				epicsMutexUnlock(connectLock);
				continue;
			}
			// wait for connection
			for(ll=0; !bStop && pPvList[l]->sChid && ll<pPvList[l]->dTimeout*1000; ll++) {
				try {
					if(ca_state(pPvList[l]->sChid) == cs_conn) {
						pPvList[l]->nChannelStatus &= ~PV_IN_QUEUE;
						break;
					}
				} catch(...) {
					;
				}
				epicsThreadSleep(.001);
			}
			// put unconnected PVs to "create queue"
			if(ll >= pPvList[l]->dTimeout*1000) {
				epicsMutexLock(pPvList[l]->lock);
				pPvList[l]->nChannelStatus |= PV_IN_QUEUE;
				epicsMutexUnlock(pPvList[l]->lock);
				epicsMutexLock(connectLock);
				sConnectQueue.push(l);
				epicsMutexUnlock(connectLock);
			}
		}
		for(ll=0; !bStop && ll<3000; ll++) {
			epicsThreadSleep(.001);
		}
	}
	ca_detach_context();
}

// task for connecting PV to Channel Access
static void caConnectTask(void) {
	long lResult = 0;
	long lIndex;
	ca_attach_context(pcac);
	while(!bStop) {
		// wait for job
		while(!bStop && !sConnectQueue.empty()) {
			epicsMutexLock(connectLock);
			lIndex = sConnectQueue.front();
			sConnectQueue.pop();
			epicsMutexUnlock(connectLock);
			if(lIndex > lPvCount) break;
			epicsMutexLock(pPvList[lIndex]->lock);
			// create channel and forget this job ("sConnectQueue.pop()")
			lResult = ca_create_channel(pPvList[lIndex]->szName, connState, pPvList[lIndex], 20, &pPvList[lIndex]->sChid);
			if(lResult != ECA_NORMAL) {
				epicsSnprintf(pPvList[lIndex]->szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
				pPvList[lIndex]->lError = lResult;
				if(pPvList[lIndex]->sChid) {
					ca_clear_channel(pPvList[lIndex]->sChid);
					pPvList[lIndex]->sChid = 0;
				}
				pPvList[lIndex]->nChannelStatus &= ~PV_CONNECTED;
			}
			pPvList[lIndex]->nChannelStatus &= ~PV_IN_QUEUE;
			epicsMutexUnlock(pPvList[lIndex]->lock);
		}
		epicsThreadSleep(.01);
	}
	ca_detach_context();
}

// put task for "fire and forget" function
static void caPutTask(void) {
	ca_attach_context(pcac);
	sPutMe tmp;
	sPutMe putMe;
	if(!putMe.szName)
		return;
	long lIndex;
	long lElems;
	int iResult;
	while(!bStop) {
		// wait for job
		while(!bStop && !sPutQueue.empty()) {
			epicsMutexLock(putLock);
			tmp = sPutQueue.front();
			putMe.lElems = tmp.lElems;
			putMe.dTimeOut = tmp.dTimeOut;
			epicsSnprintf(putMe.szName, MAX_STRING_SIZE, "%s", tmp.szName);
			putMe.dValueArray = 0;
			putMe.szValueArray = 0;
			putMe.lElems = tmp.lElems;
			putMe.dTimeOut = tmp.dTimeOut;
			if(tmp.szValueArray && tmp.szValueArray[0] >= 32) {
				putMe.szValueArray = (char*)calloc(putMe.lElems*MAX_STRING_SIZE, sizeof(char));
				if(putMe.szValueArray) {
					memcpy(putMe.szValueArray, tmp.szValueArray, putMe.lElems*MAX_STRING_SIZE*sizeof(char));
				}
			} else {
				if(tmp.dValueArray) {
					putMe.dValueArray = (double*)calloc(putMe.lElems, sizeof(double));
					if(putMe.dValueArray ) {
						memcpy(putMe.dValueArray, tmp.dValueArray, putMe.lElems*sizeof(double));
					}
				}
			}
			sPutQueue.pop();
			epicsMutexUnlock(putLock);
			// do the put-job and forget it ("sPutQueue.pop()")
			iResult = init(putMe.szName, 0, 0, 0, &lIndex, &lElems, putMe.dTimeOut);
			if(iResult == ECA_NORMAL) {
				iResult = put(0, lIndex, putMe.lElems, putMe.szValueArray, putMe.dValueArray);
			}
			if(iResult != ECA_NORMAL) {
				epicsSnprintf(szDbgMessage, 255, "%s\n", ca_message(iResult));
			}
			if(putMe.szValueArray) {
				free(putMe.szValueArray);
				putMe.szValueArray = 0;
			}
			if(putMe.dValueArray) {
				free(putMe.dValueArray);
				putMe.dValueArray = 0;
			}
		}
		epicsThreadSleep(.1);
	}
	ca_detach_context();
}

// initialization of PV
//    szName:                name of EPICS PV
//    szError:               result of function calls
//                           this is no indicator for valid PV values!!!
//    szFieldNames:          chain of optional field requests
//                           field names have format: "%-40s%-40s%-40s" (3 field names)
//    lFieldNameCount:       number of field names
//    lIndex:                internal PV index needed by "get" and "put"-function (return value)
//    lElems:                array size of native EPICS value (return value)
//    dTimeout:              timeout for Channel Access calls; default is 3.0
//    lBufferSize:           max. size of "(re)connect PV queue"; default is 100
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long init(char szName[], char szError[], char szFieldNames[], long lFieldNameCount, long *lIndex, long *lElems, double dTimeout, long lBufferSize) {
	long lResult = ECA_UNRESPTMO;
	// check shutdown signal
	if(bStop) return lResult;
	if(dTimeout == 0)
		dTimeout = 3.0;
	if(lBufferSize == 0)
		lBufferSize = 100;
	epicsMutexLock(initLock);
	// PV name known?
	for(long lCount=0; !bStop && lCount<lPvCount; lCount++) {
		if(strcmp(pPvList[lCount]->szName, szName) == 0) {
			epicsMutexLock(pPvList[lCount]->lock);
			if(szError)
				epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", pPvList[lCount]->szError);
			*lIndex = lCount;
			*lElems = pPvList[lCount]->lElems;
			lResult = pPvList[lCount]->lError;
			epicsMutexUnlock(pPvList[lCount]->lock);
			epicsMutexUnlock(initLock);
			return lResult;
		}
	}
	if(szError)
		*szError = 0;
	*lIndex = -1;
	*lElems = 0;
	if(bStop) {
		epicsMutexUnlock(initLock);
		return lResult;
	}
	// create a new structure for PV
	for(long lCount=0; lCount<lPvCount; lCount++)
		epicsMutexLock(pPvList[lCount]->lock);
	pPvList = (PV**)realloc(pPvList, (lPvCount+1) * sizeof(PV*));
	if(!pPvList) {
		for(long lCount=0; lCount<lPvCount; lCount++)
			epicsMutexUnlock(pPvList[lCount]->lock);
		lResult = ECA_ALLOCMEM;
		if(szError)
			epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
		epicsMutexUnlock(initLock);
		return lResult;
	}
	PV* pv = (PV*)calloc(1, sizeof(PV));
	if(!pv) {
		for(long lCount=0; lCount<lPvCount; lCount++)
			epicsMutexUnlock(pPvList[lCount]->lock);
		lResult = ECA_ALLOCMEM;
		if(szError)
			epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
		epicsMutexUnlock(initLock);
		return lResult;
	}
	for(long lCount=0; lCount<lPvCount; lCount++) {
		epicsMutexUnlock(pPvList[lCount]->lock);
	}
	// initialize PV and optional PV-fields
	initPV(pv, szName);
	pv->lFieldNameCount = lFieldNameCount;
	pv->dTimeout = dTimeout;
	if(pv->lFieldNameCount) {
		pv->szFieldNames = (char*)calloc(pv->lFieldNameCount*MAX_STRING_SIZE, sizeof(char));
		for(long l=0; l<lFieldNameCount; l++)
			epicsSnprintf(pv->szFieldNames+l*MAX_STRING_SIZE, MAX_STRING_SIZE, "%-40s", szFieldNames+l*MAX_STRING_SIZE);
		pv->szFieldResults = (char*)calloc(pv->lFieldNameCount*MAX_STRING_SIZE, sizeof(char));
		for(long l=0; l<lFieldNameCount; l++)
			*(pv->szFieldResults+l*MAX_STRING_SIZE)=0;
	}
	// finish creating of new PV structure
	pPvList[lPvCount] = pv;
	// add new "connect PV" job
	epicsMutexLock(connectLock);
	sConnectQueue.push(lPvCount);
	if(sConnectQueue.size() > (unsigned)lBufferSize) {
		printf("buffer overflow in sConnectQueue\n");
		sConnectQueue.pop();
	}
	epicsMutexUnlock(connectLock);
	lPvCount++;
	// wait for connection
	double dConnectTimeOut = 0;
	while(dConnectTimeOut < pv->dTimeout*100) {
		if(pv->sChid) {
			//if((ca_state(pv->sChid) == cs_conn)) {
			if(pv->nChannelStatus & PV_VALUE_RECEIVED) {
				break;
			}
		}
		dConnectTimeOut++;
		epicsThreadSleep(0.01);
	}
	if(dConnectTimeOut >= pv->dTimeout*100) {
		lResult = ECA_DISCONNCHID;
		epicsMutexLock(pv->lock);
		if(szError)
			epicsSnprintf(pv->szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
		pv->lError = lResult;
		if(pv->sEvid) {
			ca_clear_subscription(pv->sEvid);
			pv->sEvid = 0;
		}
		if(pv->sChid) {
			ca_clear_channel(pv->sChid);
			pv->sChid = 0;
		}
		pv->nChannelStatus &= ~PV_CONNECTED;
		if(szError)
			epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", pv->szError);
		epicsMutexUnlock(pv->lock);
		return lResult;
	}
	if(szError)
		epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", pv->szError);
	*lElems = pv->lElems;
	lResult = pv->lError;
	*lIndex = lPvCount-1;
	// read enum fields if needed
	if(!(pv->nChannelStatus & PV_INITIALIZED) && pv->dbfType == DBR_ENUM) {
		lResult = ca_get(DBR_CTRL_ENUM, pv->sChid, &pv->sEnum);
		if(lResult == ECA_NORMAL) {
			lResult = ca_pend_io(pv->dTimeout);
			if(lResult == ECA_NORMAL) {
				for(long lCount=0; lCount<pv->lElems; lCount++) {
					if(pv->dValueArray[lCount] < pv->sEnum.no_str)
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%s", pv->sEnum.strs[(int)pv->dValueArray[lCount]]);
					else
						epicsSnprintf(pv->szValueArray+(lCount*MAX_STRING_SIZE), MAX_STRING_SIZE, "%f", pv->dValueArray[lCount]);
				}
			} else {
				printf("%s has error with timeout %f\n", ca_name(pv->sChid), pv->dTimeout);
			}
		}
	}
	// read optional fields of PV
	if(!(pv->nChannelStatus & PV_INITIALIZED) && pv->lFieldNameCount) {
		long lResult;
		double dConnectTimeout;
		char szName[MAX_STRING_SIZE*2];
		char szFieldResult[MAX_STRING_SIZE];
		chid channel;
		for(long l=0; l<pv->lFieldNameCount; l++) {
			*szName = 0;
			*(pv->szFieldNames+l*MAX_STRING_SIZE+MAX_STRING_SIZE-1)=0;
			epicsSnprintf(szName, MAX_STRING_SIZE*2, "%s.%s", pv->szName, pv->szFieldNames+l*MAX_STRING_SIZE);
			lResult = ca_create_channel(szName, 0, 0, CA_PRIORITY_DEFAULT, &channel);
			if(lResult != ECA_NORMAL) {
				break;
			}
			dConnectTimeout = 0;
			while((dConnectTimeout < pv->dTimeout*100) && (ca_state(channel) != cs_conn)) {
				dConnectTimeout++;
				epicsThreadSleep(0.01);
			}
			if(dConnectTimeout >= pv->dTimeout*100)  {
				break;
			}
			lResult = ca_get(DBR_STRING, channel, szFieldResult);
			if(lResult != ECA_NORMAL)  {
				break;
			}
			lResult = ca_pend_io(pv->dTimeout);
			if(lResult != ECA_NORMAL) {
				break;
			}
			epicsSnprintf(pv->szFieldResults+l*MAX_STRING_SIZE, MAX_STRING_SIZE, "%-40s", szFieldResult);
			ca_clear_channel(channel);
		}
	}
	pv->nChannelStatus |= PV_INITIALIZED;
	epicsMutexUnlock(initLock);
	return lResult;
}

// get value of Epics PV
//    szError:               result of function calls
//    lIndex:                internal PV index read from "init"-function
//    szValueArray:          values formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values as double array
//    szStatus:              status of EPICS PV as string
//    nStatus:               status of EPICS PV as number
//    szSeverity:            severity of EPICS PV as string
//    nSeverity:             severity of EPICS PV as number
//    szTimestamp:           time stamp of EPICS PV as string
//    lTimestamp:            time stamp of EPICS PV as number
//    szFieldResults:        optional field values of PV formatet as "%-40s%-40s%-40s" (3 fields)
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long get(char szError[], long lIndex, char szValueArray[], double dValueArray[], char szStatus[], short *nStatus, char szSeverity[], short *nSeverity, char szTimeStamp[], long *lTimeStamp, char szFieldResults[]) {
	long lResult = ECA_CHIDNOTFND;
	epicsMutexLock(getValLock);
	if(szValueArray)
		*szValueArray = 0;
	epicsSnprintf(szStatus, MAX_STRING_SIZE, "%s", "INVALID");
	*nStatus	 = epicsSevInvalid;
	epicsSnprintf(szSeverity, MAX_STRING_SIZE, "%s", "DISCONNECTED");
	*nSeverity = epicsAlarmComm;
	epicsSnprintf(szTimeStamp, MAX_STRING_SIZE, "%s", "UNKNOWN");
	epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
	*lTimeStamp=0;
	// check shutdown signal
	if(bStop) {
		epicsMutexUnlock(getValLock);
		return ECA_UNRESPTMO;
	}
	if(lIndex >= lPvCount || lIndex <0) {
		epicsMutexUnlock(getValLock);
		return lResult;
	}
	// get optional field values
	epicsMutexLock(pPvList[lIndex]->lock);
	if(pPvList[lIndex]->lFieldNameCount) {
		for(long l=0; l<pPvList[lIndex]->lFieldNameCount; l++) {
			if(l==0 && szFieldResults) {
				epicsSnprintf(szFieldResults, MAX_STRING_SIZE, "%s", pPvList[lIndex]->szFieldResults);
			} else if(szFieldResults){
				epicsSnprintf(szFieldResults+(l*MAX_STRING_SIZE-1), MAX_STRING_SIZE+1, "\t%s", pPvList[lIndex]->szFieldResults+l*MAX_STRING_SIZE);
			}
		}
	}
	// get error string and error code
	if(szError)
		epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", pPvList[lIndex]->szError);
	lResult = pPvList[lIndex]->lError;
	// get PV values
	if(pPvList[lIndex]->lElems > 0 && pPvList[lIndex]->nChannelStatus & PV_VALUE_RECEIVED) {
		if(szValueArray) {
			memcpy(szValueArray, pPvList[lIndex]->szValueArray, pPvList[lIndex]->lElems*MAX_STRING_SIZE*sizeof(char));
			for(long l=1; l<pPvList[lIndex]->lElems;l++)
				*(szValueArray+(l*MAX_STRING_SIZE)-1) = '\t';
		}
		if(dValueArray)
			memcpy(dValueArray, pPvList[lIndex]->dValueArray, pPvList[lIndex]->lElems*sizeof(double));
	}
	// get context of PV
	epicsSnprintf(szStatus, MAX_STRING_SIZE, "%s", pPvList[lIndex]->szStatus);
	*nStatus = pPvList[lIndex]->nStatus;
	epicsSnprintf(szSeverity, MAX_STRING_SIZE, "%s", pPvList[lIndex]->szSeverity);
	*nSeverity = pPvList[lIndex]->nSeverity;
	epicsSnprintf(szTimeStamp, MAX_STRING_SIZE, "%s", pPvList[lIndex]->szTimeStamp);
	*lTimeStamp = pPvList[lIndex]->lTimeStamp;
	epicsMutexUnlock(pPvList[lIndex]->lock);
	epicsMutexUnlock(getValLock);
	return lResult;
}

// put value of Epics PV
//    szError:               result of function calls
//    lIndex:                internal PV index read from "init"-function
//    lElems:                number of values in value array
//    szValueArray:          values to be written formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values to be written as double array
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long put(char szError[], long lIndex, long lElems, char szValueArray[], double *dValueArray) {
	// check shutdown signal
	if(bStop || lIndex < 0) return ECA_UNRESPTMO;
	epicsMutexLock(putValLock);
	// validate and write PV
	if(lIndex < lPvCount) {
		long lResult = 0;
		if(lIndex < lPvCount && pPvList[lIndex]->sChid) {
			lResult = 0;
			if(szValueArray && szValueArray[0] >= 32) {
				for(long l=0; l<lElems; l++) {
					*(szValueArray+l*MAX_STRING_SIZE+MAX_STRING_SIZE-1)=0;
				}
				lResult = ca_array_put(DBR_STRING, lElems, pPvList[lIndex]->sChid, szValueArray);
				if(lResult == ECA_NORMAL)
					lResult = ca_flush_io();
				if(szError)
					epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
				epicsMutexUnlock(putValLock);
				return lResult;
			} else {
				if(dValueArray) {
					lResult = ca_array_put(DBR_DOUBLE, lElems, pPvList[lIndex]->sChid, dValueArray);
					ca_flush_io();
					if(szError)
						epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
					epicsMutexUnlock(putValLock);
					return lResult;
				}
				lResult = ECA_PUTFAIL;
				if(szError)
					epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lResult));
				epicsMutexUnlock(putValLock);
				return lResult;
			}
		}
	}
	if(szError)
		epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(ECA_CHIDNOTFND));
	epicsMutexUnlock(putValLock);
	return ECA_CHIDNOTFND;
}

// put value of Epics PV without checking of result
//    szName:                name of EPICS PV
//    lElems:                number of values in value array
//    szValueArray:          values to be written formatet as "%-40s%-40s%-40s" (array of 3 values)
//    dValueArray:           values to be written as double array
//    dTimeout:              timeout for Channel Access calls; default is 3.0
//    lBufferSize:           max. size of "(re)connect PV queue"; default is 100
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long fireAndForget(char szName[], long lElems, char szValueArray[], double *dValueArray, double dTimeout, long lBufferSize) {
	if(bStop) return ECA_UNRESPTMO;
	if(dTimeout == 0)
		dTimeout = 3.0;
	if(lBufferSize == 0)
		lBufferSize = 100;
	epicsMutexLock(fireAndForgetLock);
	sPutMe putMe;
	putMe.lElems = lElems;
	putMe.dTimeOut = dTimeout;
	putMe.dValueArray = 0;
	putMe.szValueArray = 0;
	// add write job to queue
	if(szValueArray && szValueArray[0] >= 32) {
		putMe.szValueArray = (char*)calloc(lElems*MAX_STRING_SIZE, sizeof(char));
		if(putMe.szValueArray) {
			memcpy(putMe.szValueArray, szValueArray, lElems*MAX_STRING_SIZE*sizeof(char));
		}
	} else {
		if(dValueArray) {
			putMe.dValueArray = (double*)calloc(lElems, sizeof(double));
			if(putMe.dValueArray ) {
				memcpy(putMe.dValueArray, dValueArray, lElems*sizeof(double));
			}
		}
	}
	epicsSnprintf(putMe.szName, MAX_STRING_SIZE, "%s", szName);
	epicsMutexLock(putLock);
	sPutQueue.push(putMe);
	if(sPutQueue.size() > (unsigned)lBufferSize)
		sPutQueue.pop();
	epicsMutexUnlock(putLock);
	epicsMutexUnlock(fireAndForgetLock);
	return ECA_NORMAL;
}

// create Epics context and start needed tasks
// parameter void* is needed for LabVIEW™ compatibility
// call first!
extern "C" EXPORT void create(void*) {
	if(bStop == 1) {
		ca_context_create(ca_enable_preemptive_callback);
		pcac = ca_current_context();
		initLock = epicsMutexCreate();
		getValLock = epicsMutexCreate();
		putValLock = epicsMutexCreate();
		fireAndForgetLock = epicsMutexCreate();
		connectLock = epicsMutexCreate();
		putLock = epicsMutexCreate();
		addEventLock = epicsMutexCreate();
		bStop = 0;
		pPvList = 0;
		lPvCount = 0;

		ca_add_exception_event(exceptionCallback,NULL);
		epicsThreadCreate("caCheckTask",
				epicsThreadPriorityBaseMax,
				epicsThreadGetStackSize(epicsThreadStackBig),
				(EPICSTHREADFUNC)caCheckTask,0);
		epicsThreadCreate("caConnectTask",
				epicsThreadPriorityBaseMax,
				epicsThreadGetStackSize(epicsThreadStackBig),
				(EPICSTHREADFUNC)caConnectTask,0);
		epicsThreadCreate("caPuTask",
				epicsThreadPriorityBaseMax,
				epicsThreadGetStackSize(epicsThreadStackBig),
				(EPICSTHREADFUNC)caPutTask,0);
	}
}

// cleanup function
// call last!
extern "C" EXPORT void disconnect(void*) {
	if(bStop == 0) {
		bStop = 1;
		epicsThreadSleep(.5);
		for(long l=0; l<lPvCount; l++) {
			if(pPvList[l]->sEvid)
				ca_clear_subscription(pPvList[l]->sEvid);
			if(pPvList[l]->sChid)
				ca_clear_channel(pPvList[l]->sChid);
			if(pPvList[l]->szValueArray) {
				free(pPvList[l]->szValueArray);
				pPvList[l]->szValueArray = 0;
			}
			if(pPvList[l]->dValueArray) {
				free(pPvList[l]->dValueArray);
				pPvList[l]->dValueArray = 0;
			}
			if(pPvList[l]->lFieldNameCount) {
				free(pPvList[l]->szFieldNames);
				pPvList[l]->szFieldNames = 0;
				free(pPvList[l]->szFieldResults);
				pPvList[l]->szFieldResults = 0;
			}
			//epicsMutexDestroy(pPvList[l]->lock);
			free(pPvList[l]);
			pPvList[l] = 0;
		}
		if(pPvList)
			free(pPvList);
		pPvList = 0;
		lPvCount = 0;

		//epicsMutexDestroy(putLock);
		//epicsMutexDestroy(connectLock);
		//epicsMutexDestroy(fireAndForgetLock);
		//epicsMutexDestroy(putValLock);
		//epicsMutexDestroy(getValLock);
		//epicsMutexDestroy(initLock);
		//epicsMutexDestroy(addEventLock);

		pcac = 0;
		ca_context_destroy();
	}
}

// get value of Epics PV as string
//    szName:                name of EPICS PV
//    szValueArray:          values formatet as "%-40s%-40s%-40s" (array of 3 values)
//    szTimestamp:           time stamp of EPICS PV as string
//    nStatus:               status of EPICS PV as number
//    nSeverity:             severity of EPICS PV as number
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long getValueAsString(char szName[MAX_STRING_SIZE], char szResult[MAX_STRING_SIZE], char szTimestamp[MAX_STRING_SIZE], short *nStatus, short *nSeverity) {
	if(bStop) {
		if(szResult)
			*szResult = 0;
		if(szTimestamp)
			*szTimestamp = 0;
		*nStatus = epicsSevInvalid;
		*nSeverity = epicsAlarmComm;
		return ECA_UNRESPTMO;
	}
	long lError, lIndex, lElems, lTimestamp;
	char szError[MAX_ERROR_SIZE], *szValueArray, szStatus[MAX_STRING_SIZE], szSeverity[MAX_STRING_SIZE], szTimestamp1[MAX_STRING_SIZE];
	lError = init(szName, szError, 0, 0, &lIndex, &lElems, 6.0);
	if(lError != ECA_NORMAL) {
		if(szResult)
			*szResult = 0;
		if(szTimestamp)
			*szTimestamp = 0;
		*nStatus = epicsSevInvalid;
		*nSeverity = epicsAlarmComm;
		return lError;
	}
	szValueArray = (char*)calloc(lElems*MAX_STRING_SIZE, sizeof(char));
	if(lElems)
		*szValueArray = 0;
	lError = get(szError, lIndex, szValueArray, 0, szStatus, nStatus, szSeverity, nSeverity, szTimestamp1, &lTimestamp, 0);
	if(szResult)
		epicsSnprintf(szResult, MAX_STRING_SIZE, "%s", szValueArray);
	if(szTimestamp)
		epicsSnprintf(szTimestamp, MAX_STRING_SIZE, "%s", szTimestamp1);
	return lError;
}

// put value of Epics PV as string
//    szName:                name of EPICS PV
//    szValue:               value to be written formatet as "%-40s"
//    return value:          error code of function
//                           this is no indicator for valid PV values!!!
extern "C" EXPORT long putValueAsString(char szName[MAX_STRING_SIZE], char szValue[MAX_STRING_SIZE]) {
	if(bStop) return ECA_UNRESPTMO;
	long lError, lIndex, lElems;
	char szError[MAX_ERROR_SIZE];
	lError = init(szName, szError, 0, 1, &lIndex, &lElems);
	if(lError != ECA_NORMAL)
		return lError;
	lError = put(szError, lIndex, 1, szValue, 0);
	return lError;
}

// converts error code to error string
//    lError:                error code of function
//    szError:               error string (return value)
extern "C" EXPORT void getErrorString(long lError, char szError[MAX_ERROR_SIZE]) {
	epicsSnprintf(szError, MAX_ERROR_SIZE, "%s", ca_message(lError));
}

// registers PV from pCaLabCluster as LabVIEW™-event
//    pRefNum:               event registration reference number of LabVIEW™
//    pCaLabCluster:         structure of pointer table for event data
extern "C" EXPORT void addEvent(unsigned long *pRefNum, TD1 *pCaLabCluster) {
	if(bStop || lPvCount < 1) return;
	epicsMutexLock(addEventLock);
	char szPVName[MAX_STRING_SIZE];
	memcpy(szPVName, (*pCaLabCluster->PVName)->str, (*pCaLabCluster->PVName)->cnt);
	*(szPVName+(*pCaLabCluster->PVName)->cnt) = 0;
	for(long l=0; !bStop && l<lPvCount; l++) {
		if(!strcmp(pPvList[l]->szName, szPVName)) {
			pPvList[l]->lRefNum = *pRefNum;
			pPvList[l]->pCaLabCluster = pCaLabCluster;
			postEvent(pPvList[l]);
			break;
		}
	}
	epicsMutexUnlock(addEventLock);
}
