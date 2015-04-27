/****************************************************************************
 * devMarteShm.c
 * --------------------------------------------------------------------------
 * Device support routine for shared memory interface with MARTe
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.04.28  yunsw        Initial revision
 ****************************************************************************/

#include <alarm.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>
#include <cantProceed.h>	// callocMustSucceed()
#include <aiRecord.h>
#include <aoRecord.h>

#include "drvMarteShm.h"

// Private structure to save IO arguments
typedef struct {
	MarteShmHandle *pHandle;	// handle for driver
	char 			name[64];	// name of record

	// GPIB
	short			link;		// link number
	short			addr;		// GPIB address = idx
	char 			parm[32];	// optional string

	// Others
	unsigned short	offs;		// Offset (in bytes) within memory block
	unsigned short	bit; 		// Bit number for bi/bo
	unsigned short	dtype;		// Data type
	unsigned short	dlen;		// Data length (in bytes)
} MarteShmPvt;

typedef struct {
	long      number;
	DEVSUPFUN report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN read_write;
	DEVSUPFUN special_linconv;
} MarteShmDset;

int MARTE_SHM_TYPE_SIZE[]	= {
	sizeof(int),
	sizeof(float),
	sizeof(long long),
	sizeof(double),
	MARTE_SHM_TYPE_STRING_SIZE,
};

#define	MARTE_SHM_TYPE_CNT	(int)(sizeof(MARTE_SHM_TYPE_SIZE) / sizeof(MARTE_SHM_TYPE_SIZE[0]))

char MARTE_SHM_TYPE_NAME[][20]	= {
	"int32",
	"float",
	"int64",
	"double",
	"string",
};

int MarteShmGetTypeIdx (char *pszTypeName)
{
	int	i;

	if (NULL != pszTypeName) {
		for (i = 0; i < MARTE_SHM_TYPE_CNT; i++) {
			if (0 == strcmp (pszTypeName, MARTE_SHM_TYPE_NAME[i])) {
				return (i);
			}
		}
	}

	return (-1);
}

// support for "I/O Intr" input records
static long MarteShmGetInputIntrInfo (int cmd, dbCommon *pRecord, IOSCANPVT *ppvt)
{
	MarteShmPvt* p = pRecord->dpvt;

	if (p == NULL) {
		recGblRecordError(S_db_badField, pRecord, "MarteShmGetInputIntrInfo: uninitialized record");
		return -1;
	}

	*ppvt = MarteShmGetInScanPvt (p->pHandle);

	return 0;
}

// support for "I/O Intr" output records
static long MarteShmGetOutputIntrInfo (int cmd, dbCommon *pRecord, IOSCANPVT *ppvt)
{
	MarteShmPvt* p = pRecord->dpvt;

	if (NULL == p) {
		recGblRecordError(S_db_badField, pRecord, "MarteShmGetOutputIntrInfo: uninitialized record");
		return -1;
	}

	*ppvt = MarteShmGetOutScanPvt (p->pHandle);

	return 0;
}

/* ai ***************************************************************/

static long MarteShmInitRecordAi (aiRecord *pRecord)
{
	MarteShmPvt *priv	= NULL;

	if (pRecord->inp.type != GPIB_IO) {
		recGblRecordError(S_db_badField, pRecord, "[MarteShmInitRecordAi] illegal INP field type");
		return S_db_badField;
	}

	// creates device private
	priv = (MarteShmPvt *)callocMustSucceed (1, sizeof(MarteShmPvt), "MarteShmInitRecordAi");

	// name
	strcpy (priv->name, pRecord->name);

	// H/W address for GPIB
	priv->link	= pRecord->inp.value.gpibio.link;
	priv->addr	= pRecord->inp.value.gpibio.addr;
	priv->dtype	= MarteShmGetTypeIdx (pRecord->inp.value.gpibio.parm);
	strcpy (priv->parm, pRecord->inp.value.gpibio.parm);	// name of MARTe signal

	msuDebug (msuINFO, "[MarteShmInitRecordAi] %s : link(%d) addr(%d) parm(%s) type(%d)\n", 
			priv->name, priv->link, priv->addr, priv->parm, priv->dtype);

	priv->pHandle	= MarteShmGetHandle ();

	pRecord->udf	= FALSE;
	pRecord->dpvt	= priv;		// stores device private to record

	return 0;
}

static long MarteShmReadAi (aiRecord *pRecord)
{
	MarteShmPvt			*priv 	= (MarteShmPvt *)pRecord->dpvt;
	epicsInt32			iStatus;
	MarteShmTypeUnion	uType;

	if (!priv) {
		recGblSetSevr(pRecord, UDF_ALARM, INVALID_ALARM);
		msuDebug (msuFATAL, "[MarteShmReadAi ] %s : not initialized\n", pRecord->name);
		return -1;
	}

	iStatus = MarteShmRead (priv->pHandle, priv->addr, MARTE_SHM_TYPE_SIZE[priv->dtype], (char *)&uType);

	if (msuOK != iStatus) {
		msuDebug (msuFATAL, "[MarteShmReadAi ] %s : read error\n", pRecord->name);
		recGblSetSevr(pRecord, READ_ALARM, INVALID_ALARM);
		return -1;
	}

	switch (priv->dtype) {
		case MARTE_SHM_TYPE_INT32:
			pRecord->val = uType.ival;
			break;

		case MARTE_SHM_TYPE_FLOAT:
			pRecord->val = uType.fval;
			break;

		case MARTE_SHM_TYPE_INT64:
			pRecord->val = uType.lval;
			break;

		case MARTE_SHM_TYPE_DOUBLE:
			pRecord->val = uType.dval;
			break;

		default:
			recGblSetSevr(pRecord, COMM_ALARM, INVALID_ALARM);
			msuDebug (msuFATAL, "[MarteShmReadAi ] %s : unexpected data type requested\n", pRecord->name);
			return -1;
	}

	msuDebug (msuINFO, "[MarteShmReadAi ] %s : %f\n", pRecord->name, pRecord->val);

	//return 0;
	return 2; /* preserve whatever is in the VAL field */
}

static long MarteShmInitRecordAo (aoRecord *pRecord)
{
	MarteShmPvt *priv	= NULL;

	if (pRecord->out.type != GPIB_IO) {
		recGblRecordError(S_db_badField, pRecord, "[MarteShmInitRecordAo] illegal OUT field");
		return S_db_badField;
	}

	// creates device private
	priv = (MarteShmPvt *)callocMustSucceed (1, sizeof(MarteShmPvt), "MarteShmInitRecordAo");

	// name
	strcpy (priv->name, pRecord->name);

	// H/W address for GPIB
	priv->link	= pRecord->out.value.gpibio.link;
	priv->addr	= pRecord->out.value.gpibio.addr;
	priv->dtype	= MarteShmGetTypeIdx (pRecord->out.value.gpibio.parm);
	strcpy (priv->parm, pRecord->out.value.gpibio.parm);	// name of MARTe signal

	msuDebug (msuINFO, "[MarteShmInitRecordAo] %s : link(%d) addr(%d) parm(%s) type(%d)\n", 
			priv->name, priv->link, priv->addr, priv->parm, priv->dtype);

	priv->pHandle	= MarteShmGetHandle ();

	pRecord->udf	= FALSE;
	pRecord->dpvt	= priv;		// stores device private to record

	return 2; /* preserve whatever is in the VAL field */
}

static long MarteShmWriteAo (aoRecord *pRecord)
{
	MarteShmPvt			*priv 	= (MarteShmPvt *)pRecord->dpvt;
	epicsInt32			iStatus;
	MarteShmTypeUnion	uType;

	if (!priv) {
		recGblSetSevr(pRecord, UDF_ALARM, INVALID_ALARM);
		msuDebug (msuFATAL, "[MarteShmWriteAo] %s : not initialized\n", pRecord->name);
		return -1;
	}

	msuDebug (msuDEBUG, "[MarteShmWriteAo] %s : %f\n", pRecord->name, pRecord->val);

	switch (priv->dtype) {
		case MARTE_SHM_TYPE_INT32:
			uType.ival	= (int)pRecord->val;
			break;

		case MARTE_SHM_TYPE_FLOAT:
			uType.fval	= (float)pRecord->val;
			break;

		case MARTE_SHM_TYPE_INT64:
			uType.lval	= (long long)pRecord->val;
			break;

		case MARTE_SHM_TYPE_DOUBLE:
			uType.dval	= pRecord->val;
			break;

		default:
			recGblSetSevr(pRecord, COMM_ALARM, INVALID_ALARM);
			msuDebug (msuFATAL, "[MarteShmWriteAo] %s : unexpected data type requested\n", pRecord->name);
			return -1;
	}

	iStatus = MarteShmWrite (priv->pHandle, priv->addr, MARTE_SHM_TYPE_SIZE[priv->dtype], (char *)&uType);

	if (msuOK != iStatus) {
		msuDebug (msuFATAL, "[MarteShmWriteAo] %s : write error\n", pRecord->name);
		recGblSetSevr(pRecord, READ_ALARM, INVALID_ALARM);
		return -1;
	}

	return 0;
}

// Export address for device support entry table
MarteShmDset MarteShmAi = { 6, NULL, NULL, MarteShmInitRecordAi, MarteShmGetInputIntrInfo , MarteShmReadAi , NULL };
MarteShmDset MarteShmAo = { 6, NULL, NULL, MarteShmInitRecordAo, MarteShmGetOutputIntrInfo, MarteShmWriteAo, NULL };

epicsExportAddress(dset, MarteShmAi);
epicsExportAddress(dset, MarteShmAo);

