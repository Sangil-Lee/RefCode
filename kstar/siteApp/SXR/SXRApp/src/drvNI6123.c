#include "sfwCommon.h"
#include "sfwMDSplus.h"


#include "drvNI6123.h"
#include <NIDAQmx.h>
#define NOT_CREATE_Chan 0
#define ALLOCATE_CREATE_Chan 1


//ST_NI6123_dev* create_NI6123_taskConfig(char *arg);

static long drvNI6123_io_report(int level);
static long drvNI6123_init_driver();
void make_local_file(void *param);
void clear_local_file(void *param);
void OFF_ALL_POWER_SUPPLY(void *param);
void make_mds_put_data(void *param);


int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
#if 0
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);
#endif


struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvNI6123 = {
       2,
       drvNI6123_io_report,
       drvNI6123_init_driver
};

epicsExportAddress(drvet, drvNI6123);




ST_NI6123_dev* create_NI6123_taskConfig(char *arg, char *arg1, char *arg2)
{

	ST_NI6123_dev *pNI6123 = NULL;
	int i;

        pNI6123 = (ST_NI6123_dev*) malloc(sizeof(ST_NI6123_dev));
        if(!pNI6123) return pNI6123;
	

/************************************* for NI-scope */
        pNI6123->myStatusDev = 0;
	pNI6123->taskHandle = 0x0;
	memset(pNI6123->physical_channel, 0, sizeof(pNI6123->physical_channel));
	sprintf(pNI6123->physical_channel[0], "%s\n",arg);
	pNI6123->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));
	if(!pNI6123->data_buf) return WR_ERROR;
	else ellInit(pNI6123->data_buf);
	
	for(i=1;i<8;i++)
		{
			strcpy(pNI6123->physical_channel[i], pNI6123->physical_channel[0]);
			pNI6123->physical_channel[i][7] = i + 48;
		}

	memset(pNI6123->triggerSource, 0, sizeof(pNI6123->triggerSource));
	sprintf(pNI6123->triggerSource, "%s\n",arg1); 
	memset(pNI6123->Local_data_name, 0, sizeof(pNI6123->Local_data_name));
	sprintf(pNI6123->Local_data_name, "%s",arg2); 

	
        pNI6123->terminal_config = DAQmx_Val_Diff; 
		//2012.09.07 change terminal block.
//        pNI6123->terminal_config = DAQmx_Val_RSE;
        pNI6123->minVal = -10.0;
        pNI6123->maxVal = 10.0;
        pNI6123->smp_rate = 100000.0;
        pNI6123->sample_time = 5;
//        pNI6123->smp_mode = DAQmx_Val_FiniteSamps;	
		pNI6123->smp_mode = DAQmx_Val_ContSamps;
		pNI6123->units = DAQmx_Val_Volts;
        pNI6123->sampPerChanToAcquie = NULL;


/************************************* for MDSplus */
        return pNI6123;

}


static long drvNI6123_io_report(int level)
{

        return 0;
}
static long drvNI6123_init_driver(void)
{
        return 0;

}




int NI_err_message(int error);
int NI_err_message(int error)
{

	char		errBuff[2048]={'\0'};

	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);

	return(0);
	
}

void make_local_file(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	int i;

	sprintf(pNI6123->file_name[0],"%s%d/%s", STR_LOCAL_DATA_DIR,pSTDdev->ST_mds_fetch.shotNumber, pNI6123->Local_data_name);
	
	for(i=0;i<8;i++)
		{
	
		if(i>0) {
			if((int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-1] == 57)
				{
					if(((int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-2] < 58) && ((int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-2] > 47))
						{
							pNI6123->file_name[i][strlen(pNI6123->file_name[i])-2] = (int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-2] + 1;
							pNI6123->file_name[i][strlen(pNI6123->file_name[i])-1] = '0';
						}
					else if((int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-2] == 48)
						{
							epicsPrintf("%s", pNI6123->file_name[i]);
							pNI6123->file_name[i][strlen(pNI6123->file_name[i])-1] = '1';
							pNI6123->file_name[i][strlen(pNI6123->file_name[i])] = '0';
							pNI6123->file_name[i][strlen(pNI6123->file_name[i])+1] = 0;
						}
	
				}
			else
				{ 
					pNI6123->file_name[i][strlen(pNI6123->file_name[i])-1] = (int)pNI6123->file_name[i][strlen(pNI6123->file_name[i])-1] + 1;	
				}
			}
			sprintf(pNI6123->file_name[i+1], "%s", pNI6123->file_name[i]);
			sprintf(pNI6123->strTagName[i], "%s", "\\SXR0");
			strcat(pNI6123->strTagName[i], &pNI6123->file_name[i][strlen(pNI6123->file_name)-2]);
			strcat(pNI6123->strTagName[i], ":FOO");
		}

	for(i=0;i<8;i++)
		{
			pNI6123->write_fp[i]=fopen(pNI6123->file_name[i], "w");
		}

}

void make_mds_put_data(void *param)
{

	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	int32 i, j;
	char file_name[255], tmp[255];
	short temp = 0;
	float64 temp_f = 0.0;
	float64 *databuf=NULL;
	FILE *fp = 0;
	int nTrueSampleCnt = 0, step;
	memset(file_name, 0, sizeof(file_name));
	sprintf(file_name,"%s%s", STR_MDS_PUT_DATA_DIR, pNI6123->Local_data_name);
	nTrueSampleCnt = pSTDdev->ST_mds_fetch.nSamplingRate * (int)pSTDdev->ST_mds_fetch.dT1[0];

	for(i = 0; i<8; i++) 
	{

		if(i>0) {
			if((int)file_name[strlen(file_name)-1] == 57)
			{
				if(((int)file_name[strlen(file_name)-2] < 58) && ((int)file_name[strlen(file_name)-2] > 47))
				{
					file_name[strlen(file_name)-2] = (int)file_name[strlen(file_name)-2] + 1;
					file_name[strlen(file_name)-1] = '0';
				}
				else if((int)file_name[strlen(file_name)-2] == 48)
				{

					file_name[strlen(file_name)-1] = '1';
					file_name[strlen(file_name)] = '0';
					file_name[strlen(file_name)+1] = 0;
				}

			}
			else
			{ 
				file_name[strlen(file_name)-1] = (int)file_name[strlen(file_name)-1] + 1;	
			}
		}

		pNI6123->write_fp[i]=fopen(pNI6123->file_name[i], "rt+");
		fp = fopen(file_name, "w");

		j = 0;
		step = 0;
		if(pSTDdev->ST_mds_fetch.nSamplingRate > 10000)
		{
			step = pSTDdev->ST_mds_fetch.nSamplingRate / 10000;
			j = 0;

			while(j<nTrueSampleCnt)
			{
				memset(tmp, 0, sizeof(tmp));

				fread(&temp, 2, 1, pNI6123->write_fp[i]);

				if((j % step) == 0)
				{
					temp_f = ((float64)temp * 10) / 32768.0;
					fprintf(fp, "%f\n", temp_f);
					fflush(fp);
				}
				fflush(pNI6123->write_fp[i]);			
				j=j+1;

			}
			fflush(fp);
			fclose(fp);
			fp = NULL;


		} else {

			j = 0;
			while(j<nTrueSampleCnt)
			{
				memset(tmp, 0, sizeof(tmp));
				fread(&temp, 2, 1, pNI6123->write_fp[i]);
				fscanf(pNI6123->write_fp[i%8], "%s", tmp);
				fflush(pNI6123->write_fp[i]);			

				temp_f = ((float64)temp * 10) / 32768.0;

				fprintf(fp, "%f\n", temp_f);
				fflush(fp);
				j=j+1;
			}

			fflush(fp);
			fclose(fp);
			fp = NULL;

		}

		fflush(pNI6123->write_fp[i]);			
		fclose(pNI6123->write_fp[i]);
	}
	free(databuf);
}


void clear_local_file(void *param)
{
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	int i;
	for(i=0;i<8;i++)
		{
			fflush(pNI6123->write_fp[i]);
			fclose(pNI6123->write_fp[i]);
		}


}

void OFF_ALL_POWER_SUPPLY(void *param)
{
	char get[2];
	memset(get, 0, sizeof(get));
	DBproc_get("SXR_DC_SUPPLY1_CH1_ON", &get);
	if(!strcmp("1", get))
		{
			DBproc_put("SXR_DC_SUPPLY1_CH1_ON", "0");
			epicsThreadSleep(2);
		}
	memset(get, 0, sizeof(get));
	DBproc_get("SXR_DC_SUPPLY1_CH2_ON", &get);
	if(!strcmp("1", get))
		{
			DBproc_put("SXR_DC_SUPPLY1_CH2_ON", "0");
			epicsThreadSleep(2);
		}
	memset(get, 0, sizeof(get));
	DBproc_get("SXR_DC_SUPPLY2_CH1_ON", &get);
	if(!strcmp("1", get))
		{
			DBproc_put("SXR_DC_SUPPLY2_CH1_ON", "0");
			epicsThreadSleep(2);									
		}
	memset(get, 0, sizeof(get));
	DBproc_get("SXR_DC_SUPPLY2_CH2_ON", &get);
	if(!strcmp("1", get))
		{
			DBproc_put("SXR_DC_SUPPLY2_CH2_ON", "0");
			epicsThreadSleep(2);									
		}

}
void threadFunc_DAQ_NI6123(void *param)
{

	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	ST_ADMIN *pAdminCfg = drvAdmin_get_AdminPtr();
	char file_name[8][255], system_buf[255];
	memset(file_name, 0, sizeof(file_name));
	memset(system_buf, 0, sizeof(system_buf));


	/*	ST_threadCfg *pDAQST_threadConfig ; */

	if( !pSTDdev ) {
		epicsPrintf("ERROR! threadFunc_DAQ_trigger() has null pointer of STD device.\n");
		return;
	}

	pSTDdev->ST_Base.nSamplingRate = (int)pNI6123->smp_rate;
	pAdminCfg->n8After_Processing_Flag = 0;

	while(1) {
		epicsThreadSleep(0.2);
		if(pSTDdev->StatusDev & TASK_SYSTEM_READY)
		{
			if((pSTDdev->StatusDev & TASK_SYSTEM_IDLE) && (pNI6123->taskHandle == NULL))
			{

			}
			else if((pSTDdev->StatusDev & TASK_ARM_ENABLED) && (pNI6123->taskHandle == NULL))
			{

			}	
			else if(pSTDdev->StatusDev & TASK_DAQ_STARTED)
			{

				make_local_file(pSTDdev);
// remove TG 20121010				NI_err_message(DAQmxResetDevice(pSTDdev->taskName));

				NI_err_message(DAQmxCreateTask("",&pNI6123->taskHandle));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[0],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[1],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[2],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[3],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[4],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[5],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[6],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCreateAIVoltageChan(pNI6123->taskHandle,pNI6123->physical_channel[7],"",pNI6123->terminal_config,pNI6123->minVal,pNI6123->maxVal,pNI6123->units,NULL));
				NI_err_message(DAQmxCfgSampClkTiming(pNI6123->taskHandle,"",pNI6123->smp_rate,DAQmx_Val_Rising,pNI6123->smp_mode,100000));
				pAdminCfg->n8DC_OnOff_Flag = 0;
				if(pSTDdev->ST_Base.opMode == OPMODE_REMOTE)
				{
					NI_err_message(DAQmxCfgDigEdgeStartTrig(pNI6123->taskHandle,pNI6123->triggerSource,DAQmx_Val_Rising));
					epicsPrintf("waiting Start Trigger %f %d\n", pNI6123->smp_rate, pNI6123->sample_time);
				}
				else
				{
					epicsPrintf("DAQ Start(Not waiting Start Trigger.(%s) \n", pSTDdev->taskName);
				}
				NI_err_message(DAQmxRegisterEveryNSamplesEvent(pNI6123->taskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,(void *)pSTDdev));

				pNI6123->totalRead = 0;
				NI_err_message(DAQmxStartTask(pNI6123->taskHandle));

				epicsPrintf("DAQ Start and  waitinn eventCallback TG.(%s) \n", pSTDdev->taskName);
				pNI6123->auto_run_flag = 1;
				epicsEventWait(pSTDdev->epicsEvent_DAQthread);
				pNI6123->auto_run_flag = 0;


				pSTDdev->StatusDev &= ~TASK_DAQ_STARTED;
				pSTDdev->StatusDev |= TASK_AFTER_SHOT_PROCESS;

				NI_err_message(DAQmxClearTask(pNI6123->taskHandle));
				pNI6123->taskHandle = NULL;

				fflush(stderr);
				notify_refresh_admin_status();


			}

			else if((pSTDdev->StatusDev & TASK_AFTER_SHOT_PROCESS))
			{


				if(pAdminCfg->n8After_Processing_Flag == 0)
				{
					pAdminCfg->n8After_Processing_Flag = 1;
					pSTDdev->StatusDev = 0;
					pSTDdev->StatusDev |= TASK_SYSTEM_READY;
					pSTDdev->StatusDev |= TASK_DATA_PUT_STORAGE;
					clear_local_file(pSTDdev);								
					make_mds_put_data(pSTDdev);
					epicsThreadSleep(0.3);
					pAdminCfg->n8After_Processing_Flag = 0;								

				}
				else if(pAdminCfg->n8After_Processing_Flag == 1)
				{
					epicsPrintf("Waiting for After_Shot_Processing %s)\n", pSTDdev->taskName);
					epicsThreadSleep(3);
				}

				notify_refresh_admin_status();

			}

			else if((pSTDdev->StatusDev & TASK_DATA_PUT_STORAGE))
			{

				if(!pAdminCfg->n8DC_OnOff_Flag)
				{
					pAdminCfg->n8DC_OnOff_Flag = 1;
					OFF_ALL_POWER_SUPPLY(pSTDdev);
				}

				if(pAdminCfg->n8MdsPutFlag == 0)
				{
					pAdminCfg->n8MdsPutFlag = 1;
					pSTDdev->StatusDev = 0;
					pSTDdev->StatusDev |= TASK_SYSTEM_READY;
					pSTDdev->StatusDev |= TASK_SYSTEM_IDLE;
					while(1)
					{
						if(proc_sendData2Tree(pSTDdev) == WR_OK)
						{
							pAdminCfg->n8MdsPutFlag = 0;
							break;
						} else epicsPrintf("Error proc_sendData2Tree for MdsPlus Put Data %s)\n", pSTDdev->taskName);
						
						epicsThreadSleep(0.3);
					}

				}
				else if(pAdminCfg->n8MdsPutFlag == 1)
				{
					epicsPrintf("Waiting for MdsPlus Put Data %s)\n", pSTDdev->taskName);
					epicsThreadSleep(3);
				}
				notify_refresh_admin_status();
			}

		} else if(!(pSTDdev->StatusDev & TASK_SYSTEM_READY))
		{

		}

	}
		
}


int drvNI6123_notify_InitCondition(ST_STD_device *pSTDdev)
{
/*	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE )
		{
			pSTDdev->StatusDev |= TASK_SYSTEM_READY;
		} else {
			pSTDdev->StatusDev &= ~TASK_SYSTEM_READY;
		}
	scanIoRequest(pSTDdev->ioScanPvt_status);

notify_sys_ready(); */
	notify_refresh_admin_status();
	
	return WR_OK;

}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32     read=0;
	int32     numBytesPerSamp = 0;
	int i;
	short data[8000];
	ST_STD_device *pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	NI6123_data_buf *pDataBuf = (NI6123_data_buf*) malloc(sizeof(NI6123_data_buf*));
	NI6123_data_buf *next_pDataBuf = NULL;		

	// TODO : commected to avoid SEGV fault by SWYUN
	ellAdd(pNI6123->data_buf, &pDataBuf->node);


	/*********************************************/
	// DAQmx Read Code
	/*********************************************/



	NI_err_message(DAQmxReadRaw(pNI6123->taskHandle,1000,600.0, data, 16000,(void *)&read, &numBytesPerSamp, NULL));
	//		for(i=0;i>8000;i++)
	//			{
	//			pDataBuf->data[i] = data[i];

	//			}

	pNI6123->totalRead = pNI6123->totalRead + read;

#if 1
	for(i=0;i<8000;i++)
	{
		fwrite(&data[i], 2, 1, pNI6123->write_fp[i%8]);
//		fflush(pNI6123->write_fp[i%8]);
	}

	for(i=0;i<8;i++)
	{
		fflush(pNI6123->write_fp[i]);
	}


#endif

	if(pNI6123->totalRead >= ((int32)pNI6123->smp_rate*pNI6123->sample_time))
	{
		epicsPrintf("%s Acquired %d samples.\n", pSTDdev->taskName, pNI6123->totalRead);

#if 0
		pDataBuf = (NI6123_data_buf*) ellFirst(pNI6123->data_buf);

		while(pDataBuf)
		{

			for(i=0;i<8000;i++)
			{
				fwrite(&pDataBuf->data[i], 2, 1, pNI6123->write_fp[i%8]);
				fflush(pNI6123->write_fp[i%8]);
			}


			pDataBuf = (NI6123_data_buf*) ellNext(&pDataBuf->node);
		}

		pDataBuf = (NI6123_data_buf*) ellFirst(pNI6123->data_buf);

		while(pDataBuf)
		{

			next_pDataBuf = (NI6123_data_buf*) ellNext(&pDataBuf->node);

			ellDelete(pNI6123->data_buf, &pDataBuf->node);
			free((void *)pDataBuf);
			pDataBuf = (NI6123_data_buf*) next_pDataBuf;
		}

		free((void *)pNI6123->data_buf);
		pNI6123->data_buf = (ELLLIST*) malloc(sizeof(ELLLIST));
		if(!pNI6123->data_buf) return WR_ERROR;
		else ellInit(pNI6123->data_buf);
#endif
		epicsEventSignal( pSTDdev->epicsEvent_DAQthread);
		NI_err_message(DAQmxStopTask(pNI6123->taskHandle));
		pNI6123->totalRead = 0;

	}
	return 0;
}
#if 0
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{

	ST_STD_device *pSTDdev = (ST_STD_device*) callbackData;
	ST_NI6123_dev *pNI6123 = pSTDdev->pUser;
	NI6123_data_buf *pDataBuf = NULL;
	NI6123_data_buf *next_pDataBuf = NULL;
	int i;

	pDataBuf = (NI6123_data_buf*) ellFirst(pNI6123->data_buf);

	while(pDataBuf)
	{


		for(i=0;i<8000;i++)
		{
			fwrite(&pDataBuf->data[i], 2, 1, pNI6123->write_fp[i%8]);
			fflush(pNI6123->write_fp[i%8]);
		}


		pDataBuf = (NI6123_data_buf*) ellNext(&pDataBuf->node);
	}

	pDataBuf = (NI6123_data_buf*) ellFirst(pNI6123->data_buf);

	while(pDataBuf)
	{

		next_pDataBuf = (NI6123_data_buf*) ellNext(&pDataBuf->node);

		ellDelete(pNI6123->data_buf, &pDataBuf->node);
		free((void *)pDataBuf);
		pDataBuf = (NI6123_data_buf*) next_pDataBuf;
	}


	// Check to see if an error stopped the task.
	NI_err_message(status);


}
#endif
