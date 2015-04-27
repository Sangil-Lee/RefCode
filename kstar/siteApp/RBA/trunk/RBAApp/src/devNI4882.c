#include <sys/stat.h>
#include <sys/types.h>

#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "waveformRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "drvNI4882.h" 

static long devBoNI4882_init_record(boRecord *precord)
{
	kLog (K_INFO, "[devBoNI4882_init_record] %s : type(%d) \n", precord->name, precord->out.type);

	return 0;
}

static long devBoNI4882_write(boRecord *precord)
{
	struct link *plink = (struct link *)&precord->out;

	if (1 != precord->val) {
		return 0;
	}

	char	szName[64];

	if (INST_IO == precord->out.type) {
		strcpy (szName, plink->value.instio.string);

		kLog (K_INFO, "[devBoNI4882_write] %s : %s %d\n", precord->name, szName, precord->val);

		if (0 == strcmp(szName, "amp_reset")) {
			processAmpReset ();
		}
		else if (0 == strcmp(szName, "amp_start")) {
			processAmpStart ();
		}
		else if (0 == strcmp(szName, "amp_t_get")) {
			processGetAmpTemp ();
		}
		else if (0 == strcmp(szName, "amp_gain_get")) {
			processGetAmpGain ();
		}
		else if (0 == strcmp(szName, "amp_gain_set")) {
			processSetAmpGain ();
		}
		else if (0 == strcmp(szName, "amp_flt_get")) {
			processGetAmpFilter ();
		}
		else if (0 == strcmp(szName, "amp_flt_set")) {
			processSetAmpFilter ();
		}
	}

	// reset
	precord->val	= 0;

	return 0;
}

static long devMbboNI4882_init_record(mbboRecord *precord)
{
	kLog (K_INFO, "[devMbboNI4882_init_record] %s : type(%d) \n", precord->name, precord->out.type);

	return 0;
}

static long devMbboNI4882_write(mbboRecord *precord)
{
	kLog (K_MON, "[devMbboNI4882_write] %s : %d\n", precord->name, precord->val);

	char	szName[64];
	int		nUnit;
	int		nUnitCh;
	int		nParmCnt;

	if (INST_IO == precord->out.type) {
		nParmCnt = sscanf (precord->out.value.instio.string, "%s %d %d", szName, &nUnit, &nUnitCh);

		kLog (K_INFO, "[devMbboNI4882_write] %s : type(%s) unit(%d/%d) val(%d)\n",
				precord->name, szName, nUnit, nUnitCh, precord->val);

		if (0 == strcmp(szName, "gain")) {
			pNI4882->ST_MCU[nUnit].ST_CH[nUnitCh].gain		= precord->val;
		}
		else if (0 == strcmp(szName, "filter")) {
			pNI4882->ST_MCU[nUnit].ST_CH[nUnitCh].filter	= precord->val;
		}
	}

	return 0;
}

static long devAoNI4882_init_record(aoRecord *precord)
{
	kLog (K_INFO, "[devAoNI4882_init_record] %s : type(%d) \n", precord->name, precord->out.type);

	return 0;
}

static long devAoNI4882_write(aoRecord *precord)
{
	struct link *plink = (struct link *)&precord->out;

	kLog (K_MON, "[devAoNI4882_write] %s : %f\n", precord->name, precord->val);

	char	szName[64];
	int		nUnit;
	int		nUnitCh;

	if (INST_IO == precord->out.type) {
		sscanf (plink->value.instio.string, "%s %d %d", szName, &nUnit, &nUnitCh);

		kLog (K_INFO, "[devAoNI4882_write] %s : type(%s) unit(%d/%d) val(%f)\n",
				precord->name, szName, nUnit, nUnitCh, precord->val);

		if (0 == strcmp(szName, "temp")) {
			pNI4882->ST_MCU[nUnit].ST_CH[nUnitCh].temp	= precord->val;
		}
	}

	return 0;
}

BINARYDSET	devBoNI4882Control   = { 5, NULL, NULL, devBoNI4882_init_record  , NULL, devBoNI4882_write };
BINARYDSET	devAoNI4882Control   = { 6, NULL, NULL, devAoNI4882_init_record  , NULL, devAoNI4882_write, NULL };
BINARYDSET	devMbboNI4882Control = { 5, NULL, NULL, devMbboNI4882_init_record, NULL, devMbboNI4882_write };

epicsExportAddress(dset, devBoNI4882Control);
epicsExportAddress(dset, devAoNI4882Control);
epicsExportAddress(dset, devMbboNI4882Control);

