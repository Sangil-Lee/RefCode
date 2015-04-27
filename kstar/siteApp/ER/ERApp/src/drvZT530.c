
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "drvZT530.h"

#include "sfwCommon.h"


/*
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
*/



static ZT_ERROR error;
static u16 count;
static s16 errcode[128];


LOCAL long drvZT530_io_report(int level);
LOCAL long drvZT530_init_driver();

struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvZT530 = {
       2,
       drvZT530_io_report,
       drvZT530_init_driver
};

epicsExportAddress(drvet, drvZT530);

void zt530_err_message(s16 code, char *message) {
   switch (code) {
      case -220: strcpy(message, "Parameter error");
        break;
      case -222: strcpy(message, "Data out of range");
        break;
      default: strcpy(message, "Unknown error");
   }
}


int create_zt530_taskConfig( ST_STD_device *pSTDdev)
{
	ST_ZT530* pZT530=NULL;
	pZT530 = (ST_ZT530 *)malloc(sizeof(ST_ZT530));
	if( !pZT530) return WR_ERROR;

	pSTDdev->pUser = pZT530;


	strcpy(pZT530->Resource, pSTDdev->strArg0);

//	pZT530->instr_handle = VI_NULL;
	pZT530->outputMode = ZT530_OUTPUT_ACTIVE;
	pZT530->ch0Enable = ZT530_OUTPUT_ACTIVE;
	pZT530->ch1Enable = ZT530_OUTPUT_ACTIVE;
	pZT530->sampleClockSource = ZT530_CLK_INT;
	pZT530->sampleRate = 100e+6;
	pZT530->segmentPoints = 5000;
	pZT530->clockMode = NIFGEN_VAL_DIVIDE_DOWN;
	pZT530->trigMode = NIFGEN_VAL_CONTINUOUS;
	pZT530->trigType = ZT530_SOURCE_SOFTWARE;
	strcpy(pZT530->triggerSource,"PFI0");
	pZT530->exportSigIn = ZT530_OUTPUT_TRIG_EVENT;
	pZT530->exportSigTO =ZT530_SOURCE_PXI_TTL0;
	/*********************************************************/

	
	/**********************************************************************/
	/* almost standard function.......................	*/
	/**********************************************************************/

	pSTDdev->ST_Func._SYS_ARMING = func_zt530_SYS_ARMING;
	pSTDdev->ST_Func._SYS_RUN = func_zt530_SYS_RUN;

	pSTDdev->ST_Func._Exit_Call = clear_zt530_Tasks;
	

	return WR_OK;
}


LOCAL long drvZT530_init_driver(void)
{
	ZT_ERROR error = ZT_SUCCESS;

	ST_MASTER *pAdminConfig = NULL;
	ST_ZT530 *pZT530 = NULL;
	ST_STD_device *pSTDdev = NULL;
	ST_STD_channel *pSTDch = NULL;
	ST_ZT530_channel *pZT530_ch = NULL;
	
	pAdminConfig = get_master();
	if(!pAdminConfig)	return 0;

	pSTDdev = (ST_STD_device*) ellFirst(pAdminConfig->pList_DeviceTask);
	while(pSTDdev) 
	{
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_3) )
		{	
			if( create_zt530_taskConfig(pSTDdev) == WR_ERROR )
				return -1;

			pZT530 = (ST_ZT530 *)pSTDdev->pUser;
			printf("call function: ztwaveC_initialize( \"%s\" ) \n", pZT530->Resource );
			                                  /* resource , query,  reset,  handle */
			error = zt530_initialize (pZT530->Resource, 0, 1, &(pZT530->instr_handle));
/*			zt530_error(pSTDdev, error); */

			error=zt530_trigger_and_arm (pZT530->instr_handle, pZT530->trigType, ZT530_TRIG_RISING, 0.0, 0.01, ZT530_SOURCE_IMM, ZT530_POLARITY_POS);

			

			pSTDch = (ST_STD_channel*) ellFirst(pSTDdev->pList_Channel);
			while(pSTDch) {
				pSTDch->pUser = create_zt530_channel(pSTDch);
				if(!pSTDch->pUser) {
					printf("ERROR!	\"%s\" create_zt530_channel() failed.\n", pSTDdev->taskName );
					free(pSTDdev);
					return -1;
				}
				pZT530_ch = (ST_ZT530_channel*)pSTDch->pUser;

				ztwaveC_channel_enable(pZT530->instr_handle, pZT530_ch->u16ChannelID, pZT530_ch->u16OutputState, ZT530_PATT_CONST);
				
				pSTDch = (ST_STD_channel*) ellNext(&pSTDch->node);
			}

			

/*			drvZT530_cfg_ouputMode(pSTDdev); */

/*			epicsAtExit((VOIDFUNCPTREXIT) clear_zt530_Tasks, (void *)pZT530); */
		}


		pSTDdev = (ST_STD_device*) ellNext(&pSTDdev->node);
	}
	printf("Local ZT530 init OK!\n");


	return 0;
}

void clear_zt530_Tasks(void  *pArg, double arg1, double arg2)
{

	ST_STD_device *pSTDdev = NULL;
	ST_ZT530 *pZT530 = NULL;

	pSTDdev = (ST_STD_device *)pArg;
	
	printf("%s: call ZT530 device clear!\n", pSTDdev->taskName);
	

	pZT530 = (ST_ZT530*) pSTDdev->pUser;
	if (pZT530->instr_handle) {
		zt530_abort(pZT530->instr_handle);
		zt530_close(pZT530->instr_handle);
	}

	printf("%s: ZT530 device clear!\n", pSTDdev->taskName);

	return;
}

ST_ZT530_channel* create_zt530_channel(ST_STD_channel *pSTDch)
{
	ST_ZT530_channel *pChannel = NULL;
	pChannel = (ST_ZT530_channel*) malloc(sizeof(ST_ZT530_channel));
	if(!pChannel) return pChannel;

	sprintf(pSTDch->chName, "%d", pSTDch->channelId);

/********* ZT530 **************/
	pChannel->u16ChannelID = (pSTDch->channelId) ? ZT530_CH2 : ZT530_CH1 ;
	pChannel->u16OutputState = ZT530_OUTPUT_ACTIVE;

	pChannel->wfmType = ZT530_FUNC_SINE_WAVE;
	pChannel->f64Frequency = 1E+6;
	pChannel->f64Amplitude = INIT_FGEN_AMP;
	pChannel->f64StartPhase = 0.0;
	pChannel->f64DCoffset = INIT_FGEN_DCOFFSET;
	pChannel->wfmHandle = 0;	
	strcpy(pChannel->wfmFileName, "sine.bin");
	pChannel->f64Gain = INIT_FGEN_GAIN;
	pChannel->u16FilterState = ZT530_FILT_10MHZ;
	pChannel->f64Impedance  = 1000000.0;

	pChannel->u16Clock_interpolate = ZT530_INTERP_AUTO;
	pChannel->u32Segment_points = 2000000;
/********* ZT530 **************/
	


	return pChannel;
}

LOCAL long drvZT530_io_report(int level)
{
	ST_STD_device *pSTDdev=NULL;
	ST_ZT530 *pZT530=NULL;
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
		pZT530 = (ST_ZT530*)pSTDdev->pUser;
		epicsPrintf("  Task name: %s, vme_addr: 0x%X, status: 0x%x\n",
			    pSTDdev->taskName, 
			    (unsigned int)pSTDdev, 
			    pSTDdev->StatusDev );
		if(level>2) {
			epicsPrintf("   Sampling Rate: %d/sec\n", (int)pZT530->sampleRate);
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


int drvZT530_stop_all_Generation()
{
	ST_STD_device *pSTDdev = NULL;
	ST_MASTER *pAdminCfg = get_master();

	pSTDdev = (ST_STD_device*) ellFirst(pAdminCfg->pList_DeviceTask);
	while(pSTDdev) {
		if( !strcmp(pSTDdev->devType, STR_DEVICE_TYPE_3) )
		{
			if( drvZT530_abort_Generation(pSTDdev) != WR_OK)  {
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


int zt530_error(ST_STD_device *pSTDdev, u16 count, s16 *errcode, char *funcname)
{
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	char errMsg[256];
	u16 i;

	pSTDdev->ErrorDev = 0;
	if (!count) return WR_OK;
	for (i=0; i<count; i++) {
		switch (errcode[i]) {
			case -220: strcpy(errMsg, "Parameter error");
				break;
			case -222:strcpy(errMsg, "Data out of range");
                                break;
			default: strcpy(errMsg, "Unknown error");
		}
		printf("Error in %s: %i, %s\n", funcname, errcode[i], errMsg);
	}
	epicsPrintf("Error in %s: %i, %s\n", funcname, errcode[i-1], errMsg);
	return WR_ERROR;
}

#if 0
int drvZT530_cfg_ouputMode(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

	ZT_ERROR error = ZT_SUCCESS;
/*	error = niFgen_ConfigureOutputMode(pZT530->instr_handle, pZT530->outputMode);*/
/*
 * ZT530_FUNC_DC_WAVE                                      0x0000
 * ZT530_FUNC_SINE_WAVE                                    0x0001
 * ZT530_FUNC_SQUARE_WAVE                                  0x0002
 * ZT530_FUNC_TRIANGLE_WAVE                                0x0003
 * ZT530_FUNC_SINC_WAVE                                    0x0004
 * ZT530_FUNC_PULSE_WAVE                                   0x0005
 * ZT530_FUNC_RAMP_WAVE                                    0x0006
 * ZT530_FUNC_NOISE_WAVE                                   0x0007
 * ZT530_FUNC_MULTI_TONE_WAVE                              0x0008
 * ZT530_FUNC_PATT_WAVE                                    0x0009
 * ZT530_FUNC_DBL_PULSE_WAVE                               0x000A
 * ZT530_FUNC_AM_WAVE                                      0x000B
 * ZT530_FUNC_FM_WAVE					   0x000C
 * ZT530_FUNC_USER_DEFINED_WAVE				   0x0100
 */
	error = zt530_function_generator(pZT530->instr_handle, ZT530_CH1, ZT530_FUNC_USER_DEFINED_WAVE, 10.0, 0.0);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	return zt530_error(pSTDdev, count, errcode,"generator in cfg_ouputMode");
}
#endif

int drvZT530_set_clockSource(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ZT_ERROR error = ZT_SUCCESS;
/*
 * ZT530_CLK_INT                                                   0x0000
 * ZT530_CLK_EXT                                                   0x0004
 */
	error = zt530_single_clock_source(pZT530->instr_handle, pZT530->sampleClockSource);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"single_clock_source");
}

/* this function is strange */
int drvZT530_arb_sampleRate(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	error = zt530_single_clock_frequency(pZT530->instr_handle, pZT530->sampleRate);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"single_clock_frequency");
}

int drvZT530_set_clockMode(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ZT_ERROR error = ZT_SUCCESS;
//	error = niFgen_ConfigureClockMode(pZT530->instr_handle, pZT530->clockMode);
//	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	//return zt530_error(pSTDdev, count, errcode);
	return error;
}

int drvZT530_set_outputEnable(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
//	ST_ZT530_channel *pZT530_ch = (ST_ZT530_channel*)pSTDch->pUser;
//	ZT_ERROR error = zt530_channel_enable(pZT530->instr_handle, pZT530->ch0Enable, pZT530->ch1Enable, ZT530_PATT_CONST, ZT530_PATT_CONST);

	if( pSTDch->channelId == 0)
	{
		error = zt530_single_channel_enable(pZT530->instr_handle, ZT530_CH1, pZT530->ch0Enable);
		error = zt530_single_channel_pattern_enable(pZT530->instr_handle, ZT530_CH1, ZT530_PATT_CONST);
		printf("call ... zt530_single_channel enable  #1\n");
	}
	else if ( pSTDch->channelId == 1)
	{
		error = zt530_single_channel_enable(pZT530->instr_handle, ZT530_CH2, pZT530->ch1Enable);
		error = zt530_single_channel_pattern_enable(pZT530->instr_handle, ZT530_CH2, ZT530_PATT_CONST);
		printf("call ... zt530_single_channel enable  #2\n");
	}

	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"channel_enable");
}

int drvZT530_init_Generation(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ZT_ERROR error = zt530_initiate(pZT530->instr_handle);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"initiate");
}

int drvZT530_abort_Generation(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ZT_ERROR error = zt530_abort(pZT530->instr_handle);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"abort");
}

int drvZT530_cfg_StandardWF(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ST_ZT530_channel *pZT530_ch = (ST_ZT530_channel*)pSTDch->pUser;
/*	epicsPrintf("\nch name: %s, type:%d, amp:%f, offset:%f, freq:%f, ph:%f\n", pSTDch->chName,
						  pchannelConfig->wfmType,
						  pchannelConfig->f64Amplitude,
						  pchannelConfig->f64DCoffset,
						  pchannelConfig->f64Frequency,
						  pchannelConfig->f64StartPhase);
*/
/*	niFgen_ConfigureOutputMode(ptaskConfig->vi, NIFGEN_VAL_OUTPUT_FUNC); */
/*	drvZT530_set_ouputMode(pSTDdev); */
#if 0
	ZT_ERROR error = zt530_vertical (pZT530->instr_handle, pSTDch->channelId, ZTWAVEC_DIFF_MODE_SINGLE_ENDED, pZT530_ch->f64Amplitude,
					pZT530_ch->f64DCoffset, 0.0, ZT530_FILT_10MHZ, 1000000.0);
	printf("\namp: %f, offset:%f, filt:%i", pZT530_ch->f64Amplitude, pZT530_ch->f64DCoffset, ZT530_FILT_10MHZ);

	error = zt530_horizontal (pZT530->instr_handle, 1E+8, ZT530_INTERP_AUTO, ZT530_OSC_LOCAL_REF, pZT530_ch->u32Segment_points, ZT530_CLK_INT);
	
	error = zt530_function_generator(pZT530->instr_handle, pSTDch->channelId, pZT530_ch->wfmType, pZT530_ch->f64Frequency, pZT530_ch->f64StartPhase);
#endif

#if 1

	error = zt530_horizontal (pZT530->instr_handle, 1E+8, ZT530_INTERP_AUTO, ZT530_OSC_LOCAL_REF, 2000000, ZT530_CLK_INT);					

	error = zt530_vertical (pZT530->instr_handle, pSTDch->channelId, ZT530_DIFF_SINGLE_ENDED,	pZT530_ch->f64Amplitude,
			pZT530_ch->f64DCoffset, 0.0, ZT530_FILT_10MHZ, 1000000.0); 

//	error = zt530_function_generator(pZT530->instr_handle, pSTDch->channelId, pZT530_ch->wfmType, pZT530_ch->f64Frequency, pZT530_ch->f64StartPhase);
/*   use belows, instead of above function.  */
	error = zt530_single_function_shape(pZT530->instr_handle,	   pSTDch->channelId, pZT530_ch->wfmType);
	error = zt530_single_function_frequency(pZT530->instr_handle, pSTDch->channelId, pZT530_ch->f64Frequency);
	error = zt530_single_function_phase(pZT530->instr_handle,	  pSTDch->channelId, pZT530_ch->f64StartPhase);

	
#endif	
	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	return zt530_error(pSTDdev, count, errcode,"generator in cfg_StandardWF");
}

int drvZT530_create_WFdownload(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ST_ZT530_channel *pZT530_ch = (ST_ZT530_channel*)pSTDch->pUser;
	FILE *in;
	u32 narr=2500;
	u16 i,count=0;
	s16 *arr,errcode[128];
	char message[128];


	in  = fopen(	pZT530_ch->wfmFileName, "rb");
	if (!in) {
		epicsPrintf("no file: %s\n", pZT530_ch->wfmFileName);
		return -1;
	}
	fseek(in, 0, SEEK_END);
	narr = ftell(in);
	arr = malloc(narr);
	narr /= 2;
	fseek(in, 0, SEEK_SET);
	fread(arr, 16, narr, in);
	fclose(in);
	error = zt530_load_waveform_segment(pZT530->instr_handle, pSTDch->channelId, 0, narr, arr);
	free(arr);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	for (i=0; i<count; i++) {
                zt530_err_message(errcode[i], message);
                printf("horizontal %i,%s\n",errcode[i],message);
        }
	pZT530->segmentPoints = narr;

	epicsPrintf("\nch name: %s, wfmFileName:%s, wfmHandle:%lu, #data:%lu\n", pSTDch->chName,
					  pZT530_ch->wfmFileName,
					  pZT530_ch->wfmHandle, pZT530->segmentPoints);

/*	error = zt530_horizontal (pZT530->instr_handle, 1E+8, ZT530_INTERP_AUTO, ZT530_OSC_LOCAL_REF, narr, ZT530_CLK_INT); */
/*	error = zt530_horizontal (pZT530->instr_handle, pZT530_ch->f64Frequency, pZT530_ch->u16Clock_interpolate, ZT530_OSC_LOCAL_REF, narr, ZT530_CLK_INT);
	error = zt530_errors(pZT530->instr_handle, &count, errcode); */

	return zt530_error(pSTDdev, count, errcode,"load_waveform_segment");
}

int drvZT530_cfg_arbitraryWF(ST_STD_device *pSTDdev, ST_STD_channel* pSTDch)
{
	u16 i, count=0;
	s16 errcode[128];
	char message[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ST_ZT530_channel *pZT530_ch = (ST_ZT530_channel*)pSTDch->pUser;
/*
	ZT_ERROR error = zt530_vertical (pZT530->instr_handle, ZT530_CH1, ZT530_DIFF_SINGLE_ENDED,  pZT530_ch->f64Gain*2.0,
				pZT530_ch->f64DCoffset, 0.0, ZT530_FILT_10MHZ, 1000000.0);

	ZT_ERROR error = zt530_vertical (pZT530->instr_handle, pSTDch->channelId, ZT530_DIFF_SINGLE_ENDED, pZT530_ch->f64Amplitude,
					pZT530_ch->f64DCoffset, 0.0, pZT530_ch->u16FilterState, pZT530_ch->f64Impedance);
*/

	error = zt530_horizontal (pZT530->instr_handle, 1E+8, ZT530_INTERP_AUTO, ZT530_OSC_LOCAL_REF, pZT530->segmentPoints, ZT530_CLK_INT);					

	error = zt530_vertical (pZT530->instr_handle, pSTDch->channelId, ZT530_DIFF_SINGLE_ENDED,	 pZT530_ch->f64Amplitude,
			pZT530_ch->f64DCoffset, 0.0, ZT530_FILT_10MHZ, 1000000.0); 


/*	
	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	for (i=0; i<count; i++) {
                zt530_err_message(errcode[i], message);
                printf("vertical %i,%s\n",errcode[i],message);
        }
*/
//	error = zt530_function_generator(pZT530->instr_handle, pSTDch->channelId, ZT530_FUNC_USER_DEFINED_WAVE, pZT530_ch->f64Frequency, pZT530_ch->f64StartPhase);
	/*	 use belows, instead of above function.  */
	error = zt530_single_function_shape(pZT530->instr_handle,	   pSTDch->channelId, ZT530_FUNC_USER_DEFINED_WAVE);
	error = zt530_single_function_frequency(pZT530->instr_handle, pSTDch->channelId, pZT530_ch->f64Frequency);
	error = zt530_single_function_phase(pZT530->instr_handle,	  pSTDch->channelId, pZT530_ch->f64StartPhase);


	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	epicsPrintf("\nch name: %s, freq:%f Hz, gain:%f\n, OFFset:%f", pSTDch->chName,pZT530_ch->f64Frequency,pZT530_ch->f64Gain,pZT530_ch->f64DCoffset);
	return zt530_error(pSTDdev, count, errcode,"generator in cfg_arbitraryWf");

}

int drvZT530_set_function_generator(ST_STD_device *pSTDdev)
{
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

	error = zt530_single_function_generate(pZT530->instr_handle);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);
	return zt530_error(pSTDdev, count, errcode,"generator in zt530_single_function_generate");
}

int drvZT530_cfg_trigMode(ST_STD_device *pSTDdev)
{
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
//	ZT_ERROR error = niFgen_ConfigureTriggerMode( pZT530->instr_handle, "0", pZT530->trigMode );
//	error = zt530_errors(pZT530->instr_handle, &count, errcode);

//	return zt530_error(pSTDdev, count, errcode);
	return 0;
}

int drvZT530_cfg_trigType(ST_STD_device *pSTDdev)
{
	u16 count=0;
	s16 errcode[128];
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	ZT_ERROR error=ZT_SUCCESS;
/*
 * ZT530_SOURCE_SOFTWARE                                   0x0000
 * ZT530_SOURCE_PXI_TTL0                                   0x0001
 * ZT530_SOURCE_PXI_TTL1                                   0x0002                                                
 * ZT530_SOURCE_PXI_TTL2                                   0x0003                                                
 * ZT530_SOURCE_PXI_TTL3                                   0x0004                                                
 * ZT530_SOURCE_PXI_TTL4                                   0x0005                                                
 * ZT530_SOURCE_PXI_TTL5                                   0x0006                                                
 * ZT530_SOURCE_PXI_TTL6                                   0x0007                                                
 * ZT530_SOURCE_PXI_TTL7                                   0x0008                                                
 * ZT530_SOURCE_PXI_STAR_TRIG                              0x0009
 * ZT530_SOURCE_EXT_TRIG                                   0x000A
 * ZT530_SOURCE_IMM                                        0x000B
 */
	printf("enter in cfg_trigType %i\n",pZT530->trigType);
#if 0
	ZT_ERROR _ZT530_FUNC zt530_trigger_and_arm (ZT_HANDLE instr_handle, 		/* instrument handle */
											 u16 trigger_source,		/* selects the trigger source */
											 u16 trigger_slope, 	/* Selects the slop of the trigger */
											 f64 trigger_delay, 			/* Selects the trigger delay */
											 f64 trigger_holdoff,				/* Selects the trigger holdoff */
											 u16 arm_source,			/* Selects the arm source */
											 u16 arm_polarity)		/* Selects the arm polarity */
#endif
	error=zt530_trigger_and_arm (pZT530->instr_handle, pZT530->trigType, ZT530_TRIG_RISING, 0.0, 0.01, ZT530_SOURCE_IMM, ZT530_POLARITY_POS);
	error = zt530_errors(pZT530->instr_handle, &count, errcode);

	return zt530_error(pSTDdev, count, errcode,"trigger_and_arm");
}

int drvZT530_ExportSignal(ST_STD_device *pSTDdev)
{
	unsigned short i;
	u16 count=0;
	s16 errcode[128];
	ZT_ERROR error;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	
/*	2013.
	if (!pZT530->exportSigTO) {
		for (i=0;i<8;i++) {
			//error = zt530_output_trigger(pZT530->instr_handle, i, ZT530_OUTPUT_INACTIVE, pZT530->exportSigIn,  ZT530_POLARITY_POS);
			error = zt530_output_trigger(pZT530->instr_handle, i, ZT530_OUTPUT_INACTIVE, 1,  ZT530_POLARITY_POS);
		        error = zt530_errors(pZT530->instr_handle, &count, errcode);
			zt530_error(pSTDdev, count, errcode,"output_trigger");
		}
	}
	else {
		//error = zt530_output_trigger(pZT530->instr_handle, pZT530->exportSigTO-1, ZT530_OUTPUT_ACTIVE, pZT530->exportSigIn,  ZT530_POLARITY_POS);
		error = zt530_output_trigger(pZT530->instr_handle, pZT530->exportSigTO-1, ZT530_OUTPUT_ACTIVE, 1,  ZT530_POLARITY_POS);
		error = zt530_errors(pZT530->instr_handle, &count, errcode);
		zt530_error(pSTDdev, count, errcode,"output_trigger");
	}
*/	
	return 0;
}

int drvZT530_reset_init(ST_STD_device  *pSTDdev)
{
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;

	if (pZT530->instr_handle) {
		zt530_abort(pZT530->instr_handle);
		zt530_close(pZT530->instr_handle);
		printf("%s:   board control closed!\n", pSTDdev->taskName);
	}

	error = zt530_initialize (pZT530->Resource, 0, 1, &(pZT530->instr_handle));
	printf("%s:   board initialized!\n", pSTDdev->taskName);

	return zt530_error(pSTDdev, count, errcode,"zt530_initialize");
}


void func_zt530_SYS_ARMING(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;
	ST_ZT530 *pZT530 = (ST_ZT530*)pSTDdev->pUser;
	
	if( (int)arg1 == 1 ) /* in case of arming  */
	{
		if( check_dev_arming_condition(pSTDdev)  == WR_ERROR) 
			return;

		/* direct call to sub-device.. for status notify immediately */
/*
		error =  zt530_trigger_and_arm(pZT530->instr_handle, ZT530_SOURCE_SOFTWARE, ZT530_POLARITY_POS, 0.0, 0.01, ZT530_SOURCE_IMM, ZT530_POLARITY_POS);

		error = zt530_vertical(pZT530->instr_handle, ZT530_CH1, ZT530_DIFF_SINGLE_ENDED, 1.0, 0.0, 0.0, ZT530_FILT_BYPASS, 1000000.);



//		error = zt530_function_generator(pZT530->instr_handle, ZT530_CH1, ZT530_FUNC_USER_DEFINED_WAVE, 10.0, 0.0);
*/		
		/* important this. at this time */
/*		zt530_single_function_generate(pZT530->instr_handle); */
			
		if( drvZT530_init_Generation(pSTDdev) == WR_ERROR ) {
			pSTDdev->ErrorDev = NI5412_ERROR_SIG_GEN;
			scanIoRequest(pSTDdev->ioScanPvt_status);
			return;				
		}

//		error = zt530_soft_arm (pZT530->instr_handle, ZT530_TRIG_RISING);
		error = zt530_soft_trigger (pZT530->instr_handle);
		
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


		if( drvZT530_abort_Generation(pSTDdev) == WR_ERROR ) {
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

void func_zt530_SYS_RUN(void *pArg, double arg1, double arg2)
{
	ST_STD_device* pSTDdev = (ST_STD_device *)pArg;

	if( (int)arg1 == 1 ) /* command to run  */
	{
		if( check_dev_run_condition(pSTDdev)== WR_ERROR) 
			return;

		/* nothing to do */
		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev |= TASK_IN_PROGRESS;
/*		epicsThreadSleep(0.5); */
	}
	else 
	{
		if( check_dev_stop_condition(pSTDdev) == WR_ERROR) 
			return;

		pSTDdev->StatusDev &= ~TASK_WAIT_FOR_TRIGGER;
		pSTDdev->StatusDev &= ~TASK_IN_PROGRESS;

	}
	
//	scanIoRequest(pSTDdev->ioScanPvt_status);
}




