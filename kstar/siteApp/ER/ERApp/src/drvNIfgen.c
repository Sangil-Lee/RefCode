
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "drvNIfgen.h"

#include "sfwCommon.h"


/*
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
*/





LOCAL long drvNIfgen_io_report(int level);
LOCAL long drvNIfgen_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvNIfgen = {
       2,
       drvNIfgen_io_report,
       drvNIfgen_init_driver
};

epicsExportAddress(drvet, drvNIfgen);



int create_ni5412_taskConfig( ST_STD_device *pSTDdev)
{
	ST_NIFGEN* pNI5412=NULL;
	pNI5412 = (ST_NIFGEN *)malloc(sizeof(ST_NIFGEN));
	if( !pNI5412) return WR_ERROR;

	pSTDdev->pUser = pNI5412;


	strcpy(pNI5412->Resource, pSTDdev->strArg0);

	pNI5412->vi = VI_NULL;
	pNI5412->outputMode = NIFGEN_VAL_OUTPUT_FUNC;
	
	strcpy( pNI5412->sampleClockSource, "OnboardClock");
	pNI5412->sampleRate = 20e+6;
	pNI5412->clockMode = NIFGEN_VAL_DIVIDE_DOWN;
	pNI5412->trigMode = NIFGEN_VAL_CONTINUOUS;
	pNI5412->trigType = NO_TRIGGER;
	strcpy( pNI5412->triggerSource, INIT_FGEN_TRIG_SRC );
	pNI5412->exportSigIn = INIT_FGEN_EXPORT_SIG_IN;
	strcpy( pNI5412->strExportSigTO, INIT_FGEN_EXPORT_SIG_TO);

	/*********************************************************/

	
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/

	pSTDdev->ST_Func._SYS_ARMING = func_ni5412_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_ni5412_SYS_RUN;

	pSTDdev->ST_Func._Exit_Call = clear_ni5412_Tasks;
	

	return WR_OK;
}


LOCAL long drvNIfgen_init_driver(void)
{
	ViStatus error = VI_SUCCESS;

	ST_MASTER *pAdminConfig = NULL;
	ST_NIFGEN *pNI5412 = NULL;
	ST_STD_device *pSTDdev = NULL;
	ST_STD_channel *pSTDch = NULL;
	
	pAdminConfig = get_master();
	if(!pAdminConfig)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pAdminConfig->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
		{	
			if( create_ni5412_taskConfig(pSTDdev) == WR_ERROR )
				return -1;
			

			pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
			while(pSTDch) {
				pSTDch->pUser = create_ni5412_channel();
				if(!pSTDch->pUser) {
					printf("ERROR!	\"%s\" create_ni5412_channel() failed.\n", pSTDdev->taskName );
					free(pSTDdev);
					return -1;
				}
				/* Important...... just for NI device...... */
				sprintf(pSTDch->chName, "%d", pSTDch->channelId);
				
				pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
			}

			pNI5412 = (ST_NIFGEN *)pSTDdev->pUser;
			printf("call function: niFgen_init( \"%s\" ) \n", pNI5412->Resource );
			error = niFgen_init(pNI5412->Resource, VI_TRUE, VI_TRUE, &(pNI5412->vi));
			niFgen_error(pSTDdev, error);

			drvNIfgen_cfg_ouputMode(pSTDdev);

/*			epicsAtExit((VOIDFUNCPTREXIT) clear_ni5412_Tasks, (void *)pNI5412); */
		}


		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	printf("Local NI Fgen init OK!\n");


	return 0;
}

void clear_ni5412_Tasks(void  *pArg, double arg1, double arg2)
{

	ST_STD_device *pSTDdev = NULL;
	ST_NIFGEN *pNIFGEN = NULL;

	pSTDdev = (ST_STD_device *)pArg;
	
	printf("%s: call nifgen device clear!\n", pSTDdev->taskName);
	

	pNIFGEN = (ST_NIFGEN*) pSTDdev->pUser;
	if (pNIFGEN->vi) {
		niFgen_AbortGeneration(pNIFGEN->vi);
		niFgen_close(pNIFGEN->vi);
	}

	printf("%s: nifgen device clear!\n", pSTDdev->taskName);

	return;
}

ST_NIFGEN_channel* create_ni5412_channel()
{
	ST_NIFGEN_channel *pChannel = NULL;
	pChannel = (ST_NIFGEN_channel*) malloc(sizeof(ST_NIFGEN_channel));
	if(!pChannel) return pChannel;


/********* NI fgen **************/
	pChannel->wfmType = NIFGEN_VAL_WFM_SINE;
	pChannel->f64Frequency = INIT_FGEN_FREQ;
	pChannel->f64Amplitude = INIT_FGEN_AMP;
	pChannel->f64StartPhase = 0.0;
	pChannel->f64DCoffset = INIT_FGEN_DCOFFSET;
	pChannel->wfmHandle = 0;
	strcpy(pChannel->wfmFileName, "sine.bin");
	pChannel->f64gain = INIT_FGEN_GAIN;
/********* NI fgen **************/


	


	return pChannel;
}

LOCAL long drvNIfgen_io_report(int level)
{
	ST_STD_device *pSTDdev=NULL;
	ST_NIFGEN *pNIFGEN=NULL;
	ST_MASTER *pAdminCfg = get_master();

	if(!pAdminCfg) return 0;

	if(ellCount(pAdminCfg->pList_DeviceTask))
		pSTDdev = (ST_STD_device*) ellFirst (pAdminCfg->pList_DeviceTask);
	else {
		epicsPrintf("Task not found\n");
		return 0;
	}

	epicsPrintf("Totoal %d task(s) found\n",ellCount(pAdminCfg->pList_DeviceTask));

	if(level<1) return 0;


	while(pSTDdev) {
		pNIFGEN = (ST_NIFGEN*)pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, 
			    (unsigned int)pSTDdev, 
			    pSTDdev->StatusDev );
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", (int)pNIFGEN->sampleRate);
		}

		if(level>3 ) {
			epicsPrintf("   status of Buffer-Pool (reused-counter/number of data/buffer pointer)\n");
			epicsPrintf("   ");
			
			epicsPrintf("\n");
/*		
			epicsPrintf("   callback time: %fusec\n", pAcq196->callbackTimeUsec);
			epicsPrintf("   SmplRate adj. counter: %d, adj. time: %fusec\n", pAcq196->adjCount_smplRate,
					                                                 pAcq196->adjTime_smplRate_Usec);
			epicsPrintf("   Gain adj. counter: %d, adj. time: %fusec\n", pAcq196->adjCount_Gain,
					                                                   pAcq196->adjTime_Gain_Usec);
*/
		}


/*		if(ellCount(pAcq196->pchannelConfig)>0) print_channelConfig(pAcq196,level); */
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}

	return 0;
}


int drvNIfgen_stop_all_Generation()
{
	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pAdminCfg = get_master();

	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_1) )
		{
			if( drvNIfgen_abort_Generation(pSTDdev) != WR_OK)  {
				notify_error(1, "%s: failed abort generation!\n", pSTDdev->taskName );
				return WR_ERROR;
			}			
			pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
			pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
			pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;
			pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
			
			scanIoRequest(pSTDdev->ioScanPvt_status);
		}
		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	return WR_OK;
}


int niFgen_error(ST_STD_device *pSTDdev, ViStatus error)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;

	if(error != VI_SUCCESS) {
		ViChar errMsg[256];
		niFgen_ErrorHandler(pNI5412->vi, error, errMsg);
		epicsPrintf("\nError %x: %s\n", (unsigned int)error, errMsg);
		return WR_ERROR;
	}
	pSTDdev->ErrorDev = 0;
	return WR_OK;
}

int drvNIfgen_cfg_ouputMode(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;

	ViStatus error = VI_SUCCESS;
	error = niFgen_ConfigureOutputMode(pNI5412->vi, pNI5412->outputMode);
	return niFgen_error(pSTDdev, error);
/*	if(error != VI_SUCCESS) {
		ViChar errMsg[256];
		niFgen_ErrorHandler(ptaskConfig->vi, error, errMsg);
		epicsPrintf("\nError %x: %s\n", error, errMsg);
		return WR_ERROR;
	}
	return WR_OK;
*/
}

int drvNIfgen_set_clockSource(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = VI_SUCCESS;
	error = niFgen_ConfigureSampleClockSource(pNI5412->vi, pNI5412->sampleClockSource);
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_arb_sampleRate(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_ConfigureSampleRate(pNI5412->vi, pNI5412->sampleRate);
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_set_clockMode(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = VI_SUCCESS;
	error = niFgen_ConfigureClockMode(pNI5412->vi, pNI5412->clockMode);
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_set_outputEnable(ST_STD_device *pSTDdev, char *chanName, ViBoolean onoff)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_ConfigureOutputEnabled(pNI5412->vi, chanName, onoff);
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_init_Generation(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_InitiateGeneration(pNI5412->vi);
	return niFgen_error(pSTDdev, error);
}
int drvNIfgen_abort_Generation(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_AbortGeneration(pNI5412->vi);
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_cfg_StandardWF(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ST_NIFGEN_channel *pNI5412_ch = (ST_NIFGEN_channel*)pSTDch->pUser;
/*	epicsPrintf("\nch name: %s, type:%d, amp:%f, offset:%f, freq:%f, ph:%f\n", pSTDch->chName,
						  pchannelConfig->wfmType,
						  pchannelConfig->f64Amplitude,
						  pchannelConfig->f64DCoffset,
						  pchannelConfig->f64Frequency,
						  pchannelConfig->f64StartPhase);
*/
/*	niFgen_ConfigureOutputMode(ptaskConfig->vi, NIFGEN_VAL_OUTPUT_FUNC); */
/*	drvNIfgen_set_ouputMode(pSTDdev); */

	ViStatus error = niFgen_ConfigureStandardWaveform(pNI5412->vi, 
					  pSTDch->chName,
					  pNI5412_ch->wfmType,
					  pNI5412_ch->f64Amplitude,
					  pNI5412_ch->f64DCoffset,
					  pNI5412_ch->f64Frequency,
					  pNI5412_ch->f64StartPhase);

	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_create_WFdownload(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ST_NIFGEN_channel *pNI5412_ch = (ST_NIFGEN_channel*)pSTDch->pUser;
/*	checkErr(niFgen_CreateArbWaveform(vi, WFM_SIZE, sine, &wfmHandle)); */
	ViStatus error = niFgen_CreateWaveformFromFileI16 (pNI5412->vi,
					  pSTDch->chName,
					  pNI5412_ch->wfmFileName,
					  NIFGEN_VAL_BIG_ENDIAN,
					  &(pNI5412_ch->wfmHandle) );

	epicsPrintf("\nch name: %s, wfmFileName:%s, wfmHandle:%lu\n", pSTDch->chName,
					  pNI5412_ch->wfmFileName,
					  pNI5412_ch->wfmHandle );

	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_cfg_arbitraryWF(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ST_NIFGEN_channel *pNI5412_ch = (ST_NIFGEN_channel*)pSTDch->pUser;
/*	epicsPrintf("\nch name: %s, wfmHandle:%lu, Gain:%f, DCoffset:%f\n", pSTDch->chName,
		pchannelConfig->wfmHandle, 
		pchannelConfig->f64gain,
		pchannelConfig->f64DCoffset);
*/
	ViStatus error = niFgen_ConfigureArbWaveform(pNI5412->vi, 
		pSTDch->chName, 
		pNI5412_ch->wfmHandle, 
		pNI5412_ch->f64gain,
		pNI5412_ch->f64DCoffset);
	
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_cfg_trigMode(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_ConfigureTriggerMode( pNI5412->vi, "0", pNI5412->trigMode );
	return niFgen_error(pSTDdev, error);
}
int drvNIfgen_cfg_trigType(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error=0;
	switch(pNI5412->trigType) {
	case NO_TRIGGER:
		error = niFgen_DisableStartTrigger(pNI5412->vi);
		break;
	case SOFTWARE_TRIGGER:
/*		error = niFgen_ConfigureSoftwareEdgeStartTrigger( ptaskConfig->vi ); */
		break;
	case DIGITAL_TRIGGER:
		error = niFgen_ConfigureDigitalEdgeStartTrigger(pNI5412->vi, pNI5412->triggerSource, NIFGEN_VAL_RISING_EDGE);
		//error = niFgen_ConfigureDigitalEdgeStartTrigger(pNI5412->vi, "PXI_Trig0", NIFGEN_VAL_RISING_EDGE);
		break;
	default: return WR_ERROR;

	}
	return niFgen_error(pSTDdev, error);
}

int drvNIfgen_ExportSignal(ST_STD_device *pSTDdev)
{
	ST_NIFGEN *pNI5412 = (ST_NIFGEN*)pSTDdev->pUser;
	ViStatus error = niFgen_ExportSignal( pNI5412->vi, pNI5412->exportSigIn, VI_NULL, pNI5412->strExportSigTO );
printf("to:%s from:%i error %i\n", pNI5412->strExportSigTO, pNI5412->exportSigIn, error);
	return niFgen_error(pSTDdev, error);
}


void func_ni5412_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	
	if( (int)arg1 == 1 ) /* in case of arming  */
	{
//		if( admin_check_Arming_condition()  == WR_ERROR) 
//			return;
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */
			
		if( drvNIfgen_init_Generation(pSTDdev) == WR_ERROR ) {
			pSTDdev->ErrorDev = NI5412_ERROR_SIG_GEN;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;				
		}
		epicsPrintf(">>> %s: signal generated!\n", pSTDdev->taskName );			
	
		pSTDdev->StatusDev &= ~TASK_SYSTEM_IDLE;
		pSTDdev->StatusDev |= TASK_ARM_ENABLED;
	}
	else /* release arming condition */
	{
/* do this for abort fgen signal */
/*		if( admin_check_Release_condition()  == WR_ERROR) 
			return;
*/
		/* direct call to sub-device.. for status notify immediately */


		if( drvNIfgen_abort_Generation(pSTDdev) == WR_ERROR ) {
			pSTDdev->ErrorDev = NI5412_ERROR_SIG_GEN;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;				
		}
		epicsPrintf(">>> %s: signal aborted!\n", pSTDdev->taskName );

			
		pSTDdev->StatusDev &= ~TASK_ARM_ENABLED;
		pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;			
	}
	
//	scanIoRequest(pSTDdev->ioScanPvt_status);

}

void func_ni5412_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if( (int)arg1 == 1 ) /* command to run  */
	{
//		if( admin_check_Run_condition()  == WR_ERROR) 
//			return;
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* nothing to do */
//		pSTDdev->StatusDev |= TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;

/*		epicsThreadSleep(0.5); */
	}
	else 
	{
//		if( admin_check_Stop_condition()  == WR_ERROR) 
//			return;
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;

		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;

	}
	
//	scanIoRequest(pSTDdev->ioScanPvt_status);
}




