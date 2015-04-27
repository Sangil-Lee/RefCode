
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "alarm.h"
#include "callback.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "dbCommon.h"

#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"

#include "epicsExport.h"

#include "drvPXISCXI.h"


#define  READ_GAIN	   (0x00000001<<0)
#define  READ_CUTOFFFREQ   (0x00000001<<1)
#define  READ_VAL	   (0x00000001<<2)
#define  READ_RELAY	   (0x00000001<<3)             /* Modify 20070511 */

#define  READ_GAIN_STR         "gain"
#define  READ_CUTOFFFREQ_STR   "cut-off"
#define  READ_VAL_STR          "val"
#define  READ_RELAY_STR		"pos"			/* Modify 20070511 */

#define  CONTROL_SAMPLING          (0x00000001<<0)
#define  CONTROL_CERNOX_GAIN       (0x00000001<<1)
#define  CONTROL_CERNOX_LOWPASS    (0x00000001<<2)
#define  CONTROL_RELAY		   (0x00000001<<3)      /* Modify 20070511 */

#define  CONTROL_SAMPLING_STR        "smpl_rate"
#define  CONTROL_CERNOX_GAIN_STR     "cernox_gain"
#define  CONTROL_CERNOX_LOWPASS_STR  "cernox_lowpass"
#define  CONTROL_RELAY_STR	     "relay"		/* Modify 20070511 */



LOCAL double calc_cernox(cernoxDataConfig *pcernoxDataConfig, double data);
LOCAL void devPXISCXI_set_samplingRate(execParam *pParam);
LOCAL void devPXISCXI_set_cernoxChanGain(execParam *pParam);
LOCAL void devPXISCXI_set_lowpassFilter(execParam *pParam);
LOCAL void devPXISCXI_set_relayChanConnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig);       /*  Modify 20070511 */
LOCAL void devPXISCXI_set_relayChanDisconnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig);    /*  Modify 20070511 */


typedef struct {
	char taskName[60];
	char option1[60];
	char option2[60];

	char recordName[80];

	drvPXISCXI_taskConfig *ptaskConfig;
	drvPXISCXI_channelConfig *pchannelConfig;

	unsigned        ind;

} devAoPXISCXIControldpvt;

typedef struct {
	char taskName[60];
	char chanName[60];
	char option[60];
	char recordName[80];

	drvPXISCXI_taskConfig *ptaskConfig;
	drvPXISCXI_channelConfig *pchannelConfig;

	unsigned	ind;
} devAiPXISCXIRawReaddpvt;


typedef struct {
	char taskName[60];
	char chanName[60];
	char serial[60];
	char recordName[80];

	drvPXISCXI_taskConfig *ptaskConfig;
	drvPXISCXI_channelConfig *pchannelConfig;
	drvPXISCXI_cernoxChannelConfig *pcernoxChannelConfig;
	cernoxDataConfig *pcernoxDataConfig;
} devAiPXISCXICernoxdpvt;

/* Modify 20070511 */
typedef struct {
	char taskName[60];
	char chanName[60];
	char option[60];	
	char recordName[80];

	drvPXISCXI_taskConfig *ptaskConfig;
	drvPXISCXI_channelConfig *pchannelConfig;
	unsigned	ind;
} devBoPXISCXIControldpvt;


LOCAL long devBoPXISCXIControl_init_record(boRecord *precord);    /* Modify 20070511 */
LOCAL long devBoPXISCXIControl_write_bo(boRecord *precord);       /* Modify 20070511 */

LOCAL long devAoPXISCXIControl_init_record(aoRecord *precord);
LOCAL long devAoPXISCXIControl_write_ao(aoRecord *precord);

LOCAL long devAiPXISCXIRawRead_init_record(aiRecord *precord);
LOCAL long devAiPXISCXIRawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiPXISCXIRawRead_read_ai(aiRecord *precord);

LOCAL long devAiPXISCXICernox_init_record(aiRecord *precord);
LOCAL long devAiPXISCXICernox_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiPXISCXICernox_read_ai(aiRecord *precord);



/* Modify 20070511 */
struct {
        long            number;
        DEVSUPFUN       report;
        DEVSUPFUN       init;
        DEVSUPFUN       init_record;
        DEVSUPFUN       get_ioinit_info;
        DEVSUPFUN       write_bo;
        DEVSUPFUN       special_linconv;
} devBoPXISCXIControl = {
        6,
        NULL,
        NULL,
        devBoPXISCXIControl_init_record,
        NULL,
        devBoPXISCXIControl_write_bo,
        NULL
};

epicsExportAddress(dset, devBoPXISCXIControl);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoPXISCXIControl = {
	6,
	NULL,
	NULL,
	devAoPXISCXIControl_init_record,
	NULL,
	devAoPXISCXIControl_write_ao,
	NULL
};

epicsExportAddress(dset, devAoPXISCXIControl);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiPXISCXIRawRead = {
	6,
	NULL,
	NULL,
	devAiPXISCXIRawRead_init_record,
	devAiPXISCXIRawRead_get_ioint_info,
	devAiPXISCXIRawRead_read_ai,
	NULL
};

epicsExportAddress(dset, devAiPXISCXIRawRead);


struct {
    long        number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    read_ai;
    DEVSUPFUN    special_linconv;
} devAiPXISCXICernox = {
    6,
    NULL,
    NULL,
    devAiPXISCXICernox_init_record,
    devAiPXISCXICernox_get_ioint_info,
    devAiPXISCXICernox_read_ai,
    NULL
};

epicsExportAddress(dset, devAiPXISCXICernox);



LOCAL double calc_cernox(cernoxDataConfig *pcernoxDataConfig, double data)
{
	cernoxSubData  *pcernoxSubData;
	double chev[10],val,z;
	int i;

	if(data<1.) return 0.;

	z = log10(data);

	pcernoxSubData = pcernoxDataConfig->pLowTemp;
	if(z < pcernoxSubData->zl) 
		pcernoxSubData = pcernoxDataConfig->pMidTemp;
	if(z < pcernoxSubData->zl)
		pcernoxSubData = pcernoxDataConfig->pHighTemp;

	z = (2.*z - pcernoxSubData->zl - pcernoxSubData->zu)/(pcernoxSubData->zu - pcernoxSubData->zl);

	if(z<-1.) z=-1.;
	if(z>1.)  z=1.;

	chev[0] = 1.;
	chev[1] = z;
	
	val  = pcernoxSubData->coeff[0] * chev[0];
	val += pcernoxSubData->coeff[1] * chev[1];
	for(i=2; pcernoxSubData->index[i] > 0; i++) {
		chev[i] = 2.*z*chev[i-1] - chev[i-2];
		val += pcernoxSubData->coeff[i] * chev[i];
	}

	
	return val;
}

LOCAL void devPXISCXI_set_samplingRate(execParam *pParam)
{
	drvPXISCXI_taskConfig *ptaskConfig = pParam->ptaskConfig;
	struct dbCommon *precord = pParam->precord;

	drvPXISCXI_set_samplingRate(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (smapling rate): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                            epicsThreadGetNameSelf());
}

LOCAL void devPXISCXI_set_cernoxChanGain(execParam *pParam)
{
	drvPXISCXI_taskConfig *ptaskConfig = pParam->ptaskConfig;
        struct dbCommon *precord = pParam->precord;
	drvPXISCXI_set_cernoxChanGain(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (gain): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                   epicsThreadGetNameSelf());
}

LOCAL void devPXISCXI_set_lowpassFilter(execParam *pParam)
{
	drvPXISCXI_taskConfig *ptaskConfig = pParam->ptaskConfig;
        struct dbCommon *precord = pParam->precord;
	drvPXISCXI_set_lowpassFilter(pParam->ptaskConfig, pParam->setValue);
	epicsPrintf("control thread (lowpass filter): %s %s (%s)\n", ptaskConfig->taskName, precord->name,
			                                             epicsThreadGetNameSelf());
}

/* Modify 20070514 */
LOCAL void devPXISCXI_set_relayChanConnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig)
{
	drvPXISCXI_set_relayChanConnect(prelayChannelConfig);
}


/* Modify 20070514 */
LOCAL void devPXISCXI_set_relayChanDisconnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig)
{
        drvPXISCXI_set_relayChanDisconnect(prelayChannelConfig);
}


LOCAL long devBoPXISCXIControl_init_record(boRecord *precord)
{
        devBoPXISCXIControldpvt *pdevBoPXISCXIControldpvt = (devBoPXISCXIControldpvt*) malloc(sizeof(devBoPXISCXIControldpvt));
        int i;

        switch(precord->out.type) {
                case INST_IO:
                        strcpy(pdevBoPXISCXIControldpvt->recordName, precord->name);
                        i = sscanf(precord->out.value.instio.string, "%s %s %s",
                                   pdevBoPXISCXIControldpvt->taskName,
                                   pdevBoPXISCXIControldpvt->chanName,
				   pdevBoPXISCXIControldpvt->option);

                        pdevBoPXISCXIControldpvt->ptaskConfig = drvPXISCXI_find_taskConfig(pdevBoPXISCXIControldpvt->taskName);
                        if(!pdevBoPXISCXIControldpvt->ptaskConfig) {
                                recGblRecordError(S_db_badField, (void*) precord,
                                                  "devBoPXISCXIControl (init_record) Illegal INP field: task_name");
                                free(pdevBoPXISCXIControldpvt);
                                precord->udf = TRUE;
                                precord->dpvt = NULL;
                                return S_db_badField;
                        }
                        break;
                default:
                        recGblRecordError(S_db_badField,(void*) precord,
                                          "devBoPXISCXIControl (init_record) Illegal OUT field");
                        free(pdevBoPXISCXIControldpvt);
                        precord->udf = TRUE;
                        precord->dpvt = NULL;
                        return S_db_badField;

        }

        if(i==3) {
                if(!strcmp(pdevBoPXISCXIControldpvt->option, CONTROL_RELAY_STR)) {
			pdevBoPXISCXIControldpvt->pchannelConfig 
				= drvPXISCXI_find_channelConfig(pdevBoPXISCXIControldpvt->taskName,
					       			pdevBoPXISCXIControldpvt->chanName);
                       if(!pdevBoPXISCXIControldpvt->pchannelConfig) {
			        recGblRecordError(S_db_badField, (void*) precord,
				                  "devBoPXISCXIControl (init_record) Illegal INP field: channel_name");
				free(pdevBoPXISCXIControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		       if(pdevBoPXISCXIControldpvt->pchannelConfig->channelType != RELAY_CHANNEL){
				recGblRecordError(S_db_badField, (void*) precord,
						 "devBoPXISCXIControl (init_record) Illegal INP field: channel type mismatch");
			        free(pdevBoPXISCXIControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			  
		       	pdevBoPXISCXIControldpvt->ind = CONTROL_RELAY;
                }
        }


        precord->udf = FALSE;
        precord->dpvt = (void*) pdevBoPXISCXIControldpvt;

        return 0;
}

LOCAL long devBoPXISCXIControl_write_bo(boRecord *precord)
{
        devBoPXISCXIControldpvt        *pdevBoPXISCXIControldpvt = (devBoPXISCXIControldpvt*) precord->dpvt;
        drvPXISCXI_taskConfig          *ptaskConfig;
	drvPXISCXI_channelConfig       *pchannelConfig;

/*        unsigned long long int start, stop;  
	unsigned 		pos;
*/
        if(!pdevBoPXISCXIControldpvt || precord->udf == TRUE) {
                precord->pact = TRUE;
                return 0;
        }

        ptaskConfig                = pdevBoPXISCXIControldpvt->ptaskConfig;
	pchannelConfig	           = pdevBoPXISCXIControldpvt->pchannelConfig;
	
	switch(pdevBoPXISCXIControldpvt->ind) {
		case CONTROL_RELAY:
			{                           
			drvPXISCXI_relayChannelConfig   *prelayChannelConfig = (drvPXISCXI_relayChannelConfig*) pchannelConfig;
			if(precord->val && prelayChannelConfig->relayPos == 10437) {
			devPXISCXI_set_relayChanConnect(prelayChannelConfig);
				if(prelayChannelConfig->relayPos == 10438) {
					epicsPrintf("Relay Channel Success : Connect relay %s, Connection Time: %f \n",
						       	prelayChannelConfig->relayName, prelayChannelConfig->adjTime_relayConnect_Usec);
				}
				else if (prelayChannelConfig->relayPos == 10437) {
					precord->val = FALSE;
					epicsPrintf("Relay Channel Connect Error : Not Connect relay %s \n",prelayChannelConfig->relayName);
				}
				else epicsPrintf("Bad Error : relayPos Value is not 10438 and 10437");
			return 0;
			}
			else if(!precord->val && prelayChannelConfig->relayPos == 10438) {
			devPXISCXI_set_relayChanDisconnect(prelayChannelConfig);
				if(prelayChannelConfig->relayPos == 10437) {
					epicsPrintf("Relay Channel Success : DisConnect relay %s, Disconnect Time: %f \n",
						       	prelayChannelConfig->relayName, prelayChannelConfig->adjTime_relayConnect_Usec);
				}
				else if (prelayChannelConfig->relayPos == 10438) {
					precord->val = TRUE;
					epicsPrintf("Relay Channel DisConnect Error : Not Disconnect relay %s \n",prelayChannelConfig->relayName);
			        }
				else epicsPrintf("Bad Error : relayPos Value is not 10438 and 10437");
			return 0;
			}
			}
		break;
	}
        return 0;
}



LOCAL long devAoPXISCXIControl_init_record(aoRecord *precord)
{
	devAoPXISCXIControldpvt *pdevAoPXISCXIControldpvt = (devAoPXISCXIControldpvt*) malloc(sizeof(devAoPXISCXIControldpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevAoPXISCXIControldpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s",
				   pdevAoPXISCXIControldpvt->taskName,
			           pdevAoPXISCXIControldpvt->option1,
			           pdevAoPXISCXIControldpvt->option2);

			pdevAoPXISCXIControldpvt->ptaskConfig = drvPXISCXI_find_taskConfig(pdevAoPXISCXIControldpvt->taskName);
			if(!pdevAoPXISCXIControldpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoPXISCXIControl (init_record) Illegal INP field: task_name");
				free(pdevAoPXISCXIControldpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoPXISCXIControl (init_record) Illegal OUT field");
			free(pdevAoPXISCXIControldpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	if(i<3) {
		if(!strcmp(pdevAoPXISCXIControldpvt->option1, CONTROL_SAMPLING_STR)) {
			pdevAoPXISCXIControldpvt->pchannelConfig = (drvPXISCXI_channelConfig*) NULL;
			pdevAoPXISCXIControldpvt->ind = CONTROL_SAMPLING;
		}
		if(!strcmp(pdevAoPXISCXIControldpvt->option1, CONTROL_CERNOX_GAIN_STR)) {
			pdevAoPXISCXIControldpvt->pchannelConfig
				= (drvPXISCXI_channelConfig*)ellFirst(pdevAoPXISCXIControldpvt->ptaskConfig->pchannelConfig);
			pdevAoPXISCXIControldpvt->ind = CONTROL_CERNOX_GAIN;
		}
		if(!strcmp(pdevAoPXISCXIControldpvt->option1, CONTROL_CERNOX_LOWPASS_STR)) {
			pdevAoPXISCXIControldpvt->pchannelConfig
				= (drvPXISCXI_channelConfig*)ellFirst(pdevAoPXISCXIControldpvt->ptaskConfig->pchannelConfig);
			pdevAoPXISCXIControldpvt->ind = CONTROL_CERNOX_LOWPASS;
		}
	}
	else if(i==3) {
		pdevAoPXISCXIControldpvt->pchannelConfig 
			= drvPXISCXI_find_channelConfig(pdevAoPXISCXIControldpvt->taskName,
					                pdevAoPXISCXIControldpvt->option2);
	}


	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAoPXISCXIControldpvt;
	
	return 2;     /* don't convert */
}

LOCAL long devAoPXISCXIControl_write_ao(aoRecord *precord)
{
	devAoPXISCXIControldpvt        *pdevAoPXISCXIControldpvt = (devAoPXISCXIControldpvt*) precord->dpvt;
	drvPXISCXI_taskConfig          *ptaskConfig;
	drvPXISCXI_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;
	

	unsigned long long int start, stop;

	if(!pdevAoPXISCXIControldpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	ptaskConfig                = pdevAoPXISCXIControldpvt->ptaskConfig;
	pcontrolThreadConfig       = ptaskConfig->pcontrolThreadConfig;
	qData.param.ptaskConfig    = ptaskConfig;
	qData.param.precord        = (struct dbCommon *)precord;
	qData.param.setValue       = precord->val;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		start = drvPXISCXI_rdtsc();

		epicsPrintf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());

		switch(pdevAoPXISCXIControldpvt->ind) {
			case CONTROL_SAMPLING:
                                qData.pFunc = devPXISCXI_set_samplingRate;
				break;
			case CONTROL_CERNOX_GAIN:
				qData.pFunc = devPXISCXI_set_cernoxChanGain;
				break;
			case CONTROL_CERNOX_LOWPASS:
				qData.pFunc = devPXISCXI_set_lowpassFilter;
				break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		epicsPrintf("db processing: phase II %s (%s)\n", precord->name,
				                                 epicsThreadGetNameSelf());
		precord->pact = FALSE;
        	precord->udf = FALSE;
		stop = drvPXISCXI_rdtsc();
		return 2;    /* don't convert */
	}

	return 0;
}



LOCAL long devAiPXISCXIRawRead_init_record(aiRecord *precord)
{
	devAiPXISCXIRawReaddpvt *pdevAiPXISCXIRawReaddpvt = (devAiPXISCXIRawReaddpvt*) malloc(sizeof(devAiPXISCXIRawReaddpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiPXISCXIRawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiPXISCXIRawReaddpvt->taskName,
				   pdevAiPXISCXIRawReaddpvt->chanName,
				   pdevAiPXISCXIRawReaddpvt->option);
			pdevAiPXISCXIRawReaddpvt->ptaskConfig = drvPXISCXI_find_taskConfig(pdevAiPXISCXIRawReaddpvt->taskName);
			if(!pdevAiPXISCXIRawReaddpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXIRawRead (init_record) Illegal INP field: task name");
				free(pdevAiPXISCXIRawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			pdevAiPXISCXIRawReaddpvt->pchannelConfig = drvPXISCXI_find_channelConfig(pdevAiPXISCXIRawReaddpvt->taskName,
											         pdevAiPXISCXIRawReaddpvt->chanName);
			if(!pdevAiPXISCXIRawReaddpvt->pchannelConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXIRawRead (init_record) Illegal INP field: channel name");
				free(pdevAiPXISCXIRawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiPXISCXIRawRead (init_record) Illegal INP field");
			free(pdevAiPXISCXIRawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(i<3) pdevAiPXISCXIRawReaddpvt->ind = READ_VAL;
	else if(!strcmp(pdevAiPXISCXIRawReaddpvt->option, READ_GAIN_STR))       
		pdevAiPXISCXIRawReaddpvt->ind = READ_GAIN;
	else if(!strcmp(pdevAiPXISCXIRawReaddpvt->option, READ_CUTOFFFREQ_STR)) 
		pdevAiPXISCXIRawReaddpvt->ind = READ_CUTOFFFREQ;
	else pdevAiPXISCXIRawReaddpvt->ind = READ_VAL;
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiPXISCXIRawReaddpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devAiPXISCXIRawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devAiPXISCXIRawReaddpvt *pdevAiPXISCXIRawReaddpvt = (devAiPXISCXIRawReaddpvt*) precord->dpvt;
	drvPXISCXI_taskConfig   *ptaskConfig;

	if(!pdevAiPXISCXIRawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevAiPXISCXIRawReaddpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devAiPXISCXIRawRead_read_ai(aiRecord *precord)
{
	devAiPXISCXIRawReaddpvt		*pdevAiPXISCXIRawReaddpvt = (devAiPXISCXIRawReaddpvt*) precord->dpvt;
	drvPXISCXI_taskConfig		*ptaskConfig;
	drvPXISCXI_channelConfig	*pchannelConfig;
	double				*pdata;

	if(!pdevAiPXISCXIRawReaddpvt) return 0;

	ptaskConfig		= pdevAiPXISCXIRawReaddpvt->ptaskConfig;
	pchannelConfig		= pdevAiPXISCXIRawReaddpvt->pchannelConfig;
	pdata			= ptaskConfig->pdata;

	

	switch(pdevAiPXISCXIRawReaddpvt->ind) {
		case READ_GAIN:
			precord->val = pchannelConfig->gain;
			break;
		case READ_CUTOFFFREQ:
			precord->val = pchannelConfig->cutOffFreq;
			break;
		case READ_VAL:
			precord->val = pdata[pchannelConfig->chanIndex];
			break;
	}

	
	return 2;
}



LOCAL long devAiPXISCXICernox_init_record(aiRecord *precord)
{
	devAiPXISCXICernoxdpvt *pdevAiPXISCXICernoxdpvt = (devAiPXISCXICernoxdpvt*) malloc(sizeof(devAiPXISCXICernoxdpvt));

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiPXISCXICernoxdpvt->recordName, precord->name);
			sscanf(precord->inp.value.instio.string, "%s %s %s",
					pdevAiPXISCXICernoxdpvt->taskName,
					pdevAiPXISCXICernoxdpvt->chanName,
					pdevAiPXISCXICernoxdpvt->serial);

			pdevAiPXISCXICernoxdpvt->ptaskConfig = drvPXISCXI_find_taskConfig(pdevAiPXISCXICernoxdpvt->taskName);
			if(!pdevAiPXISCXICernoxdpvt->ptaskConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXICernox (init_record) Illegal INP field: task name");
				epicsPrintf("record: %s, task: %s, chan: %s, cernox_serial: %s\n",
					    pdevAiPXISCXICernoxdpvt->recordName,
					    pdevAiPXISCXICernoxdpvt->taskName,
					    pdevAiPXISCXICernoxdpvt->chanName,
					    pdevAiPXISCXICernoxdpvt->serial);
				free(pdevAiPXISCXICernoxdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			pdevAiPXISCXICernoxdpvt->pchannelConfig = drvPXISCXI_find_channelConfig(pdevAiPXISCXICernoxdpvt->taskName,
												pdevAiPXISCXICernoxdpvt->chanName);
			if(!pdevAiPXISCXICernoxdpvt->pchannelConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXICernox (init_record) Illegal INP filed: channel name");
				epicsPrintf("record: %s, task: %s, chan: %s, cernox_serial: %s\n",
					    pdevAiPXISCXICernoxdpvt->recordName,
					    pdevAiPXISCXICernoxdpvt->taskName,
					    pdevAiPXISCXICernoxdpvt->chanName,
					    pdevAiPXISCXICernoxdpvt->serial);
				free(pdevAiPXISCXICernoxdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;

			}

			if(pdevAiPXISCXICernoxdpvt->pchannelConfig->channelType != CERNOX_CHANNEL) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXICernox (init_record) Illegal INP field: channel type");
				epicsPrintf("record: %s, task: %s, chan: %s, cernox_serial %s, channel type is not cernox\n",
					    pdevAiPXISCXICernoxdpvt->recordName,
					    pdevAiPXISCXICernoxdpvt->taskName,
					    pdevAiPXISCXICernoxdpvt->chanName,
					    pdevAiPXISCXICernoxdpvt->serial);
				free(pdevAiPXISCXICernoxdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			} else pdevAiPXISCXICernoxdpvt->pcernoxChannelConfig 
						= (drvPXISCXI_cernoxChannelConfig *)pdevAiPXISCXICernoxdpvt->pchannelConfig;

			pdevAiPXISCXICernoxdpvt->pcernoxDataConfig = drvPXISCXI_find_cernoxDataConfig(pdevAiPXISCXICernoxdpvt->serial);
			if(!pdevAiPXISCXICernoxdpvt->pcernoxDataConfig) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiPXISCXICernox (init_record) Illegal INP field: cernox_serial");
				epicsPrintf("record: %s, task: %s, chan: %s, cernox_serial %s, serial not fund\n",
					    pdevAiPXISCXICernoxdpvt->recordName,
					    pdevAiPXISCXICernoxdpvt->taskName,
					    pdevAiPXISCXICernoxdpvt->chanName,
					    pdevAiPXISCXICernoxdpvt->serial);
				free(pdevAiPXISCXICernoxdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
			                  "devAiPXISCXICernox (init_record) Illegal INP field");
			free(pdevAiPXISCXICernoxdpvt);
		        precord->udf = TRUE;
		        precord->dpvt = NULL;
		        return S_db_badField;
	}

    	precord->udf = FALSE;
        precord->dpvt = (void*)pdevAiPXISCXICernoxdpvt;

    	return 2;  /* don't convert */

}

LOCAL long devAiPXISCXICernox_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devAiPXISCXICernoxdpvt *pdevAiPXISCXICernoxdpvt = (devAiPXISCXICernoxdpvt*) precord->dpvt;
	drvPXISCXI_taskConfig *ptaskConfig;

	if(!pdevAiPXISCXICernoxdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	ptaskConfig = pdevAiPXISCXICernoxdpvt->ptaskConfig;
	*ioScanPvt  = ptaskConfig->ioScanPvt;
	return 0;
}

LOCAL long devAiPXISCXICernox_read_ai(aiRecord *precord)
{
	devAiPXISCXICernoxdpvt          *pdevAiPXISCXICernoxdpvt = (devAiPXISCXICernoxdpvt*) precord->dpvt;
	drvPXISCXI_taskConfig           *ptaskConfig;
	drvPXISCXI_cernoxChannelConfig  *pcernoxChannelConfig;
	cernoxDataConfig                *pcernoxDataConfig;
	double                *pdata;

	unsigned long long int start, stop;

	if(!pdevAiPXISCXICernoxdpvt) return 0;

	ptaskConfig           = pdevAiPXISCXICernoxdpvt->ptaskConfig;
	pcernoxChannelConfig  = pdevAiPXISCXICernoxdpvt->pcernoxChannelConfig;
	pcernoxDataConfig     = pdevAiPXISCXICernoxdpvt->pcernoxDataConfig;
	pdata		      = ptaskConfig->pdata;

	start = drvPXISCXI_rdtsc();
	precord->val = calc_cernox(pcernoxDataConfig, 
			           pdata[pcernoxChannelConfig->chanIndex]/pcernoxChannelConfig->excitationCurrent);
	stop = drvPXISCXI_rdtsc();

	pcernoxChannelConfig->conversionTime_usec = drvPXISCXI_intervalUSec(start,stop);


	return 2;
}


