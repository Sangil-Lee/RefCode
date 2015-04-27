#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "sfwCommon.h"
#include "sfwAdminHead.h"
#include "sfwMDSplus.h"
#include "sfwGlobalDef.h"

#include "myMDSplus.h"
#include <NIDAQmx.h>
#include "drvNI6123.h"
/*
#include <dbCommon.h>	 for epicsPrintf 
#include <epicsThread.h>  for epicsThreadSleep() 
*/
/*
- DATA is an expression defining the Y values. (sin(6.28 * [0..999]/1000.) in our example)
- RAW is an expression indicating raw data. Often an acquired signal is made of raw data converted then by taking into account parameters such as gain and offset. Now we are not interested in it, so we omit its definition (the two commas in the TCL example are not a typing error).
- DIMENSION is an expression returning the array of X axis, usually (but not always) time values. (In our example the expression is [0..999]/1000.) 
*/


int channelMapping[96];




int proc_sendData2Tree(ST_STD_device *pSTDdev)
{
		int _null = 0; /* Used to mark the end of the argument list */
		int status; /* Will contain the status of the data access operation */
		int dtype_Float64 = DTYPE_DOUBLE; /* We are going to read a signal made of float values */
/*TG		int dtype_Float = DTYPE_FLOAT;  We are going to read a signal made of float values */
/*TG		int dypte_Short = DTYPE_SHORT ;  */
		
	/*	float *data=NULL; */ /* Array of intiger used for signal */ 
		float64 *databuf=NULL;	/* 2009. 7. 24 by woong.. for read bulk data */
	/*	int dataDesc; */
		int rawDataDesc; /* Signal descriptor */
		int rateDesc, startDesc, endDesc, voltageTDesc;
	
		int32 i, j; /*, ntemp2=0; */
		int shotNumber;	
		FILE *fpRaw=NULL;	
		char buf[128];
		int32 nTrueSampleCnt = 0;
		float64 fVpp10_16bit;
		char file_name[100];
		char tmp[255];
	
	
	
	/* Add TG.Lee MDSplus Time Array Infomation Variable 20081006 */
		float64 fStartTime;   /* -1 sec */
		float64 fEndTime;	  /* -1 sec */
		float64 fRealTimeGap;
	
		ST_NI6123_dev *pNI6123 = (ST_NI6123_dev *)pSTDdev->pUser;
		
		if(pSTDdev->ST_mds_fetch.nSamplingRate >= 10000)
			{
				nTrueSampleCnt = 10000 * (int)pSTDdev->ST_mds_fetch.dT1[0];
				fRealTimeGap = 1.0 / 10000;			
			}
		else
			{
				nTrueSampleCnt = pSTDdev->ST_mds_fetch.nSamplingRate * (int)pSTDdev->ST_mds_fetch.dT1[0];
				fRealTimeGap = 1.0 / (float64)pSTDdev->ST_mds_fetch.nSamplingRate;			
			}
		epicsPrintf("\n\n%d\n",(int) nTrueSampleCnt);
		databuf = (float64 *)malloc(sizeof(float64)* nTrueSampleCnt);
	
		if( databuf == NULL){		
			epicsPrintf("\n%s: >>> malloc(sizeof(short)* %d)... fail\n", pSTDdev->taskName,(int)nTrueSampleCnt);
	/*		free(data); */
			return WR_ERROR;
		}
	
		/* test insert */ 
		if ( pSTDdev->ST_Base.opMode != OPMODE_REMOTE ) {
	
			pSTDdev->ST_mds_fetch.shotNumber = pSTDdev->ST_Base.shotNumber;
			fStartTime = 0.0;  /* Data Zero Start Time */
			fEndTime = (float64)nTrueSampleCnt;

			epicsPrintf("\n\n\nfEndTime %f\n\n\n", fEndTime);
		} else {
			fStartTime = (pSTDdev->ST_mds_fetch.dT0[0] - pSTDdev->ST_mds_fetch.fBlipTime);/* * pSTDdev->ST_mds_fetch.nSamplingRate;*/
			fEndTime = (float64)nTrueSampleCnt;
		}
	
	//	fRealTimeGap = 1.0 / 1000.0;
		epicsPrintf("BlipTime : %f %f", pSTDdev->ST_mds_fetch.dT0[0], pSTDdev->ST_mds_fetch.fBlipTime);

		epicsPrintf("\nfrealTimeGap %f %d %f\n", fRealTimeGap, pSTDdev->ST_mds_fetch.nSamplingRate, pNI6123->smp_rate);
					
	
	/*************************************************************************/
		if( mds_Connect(pSTDdev) == WR_ERROR ) {
	/*		free(data); */
			free(databuf);
			epicsPrintf("Error mds_Connect function \n");
			return WR_ERROR;
		}
		if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
	/*		free(data); */
			free(databuf);
			epicsPrintf("Error mds_Open_withFetch function \n");
			return WR_ERROR;
		}
	
	/*************************************************************************/
	
#if 0
	
	
		fVpp10_16bit = 10.0 / 32768.0;
	
		fprintf(stdout,"True Cnt:%d, Freq: %d, T0: %f, T1: %f, gap: %f \n", 
				nTrueSampleCnt, 
				pSTDdev->mds_nSamplingRate, 
				pSTDdev->mds_dT0[0], 
				pSTDdev->mds_dT1[0], 
				fRealTimeGap);
#endif
		rateDesc = descr(&dtype_Float64, &fRealTimeGap, &_null);
		startDesc = descr(&dtype_Float64, &fStartTime, &_null);
		endDesc = descr(&dtype_Float64, &fEndTime, &_null);
		voltageTDesc = descr(&dtype_Float64, &fVpp10_16bit, &_null);
		
		memset(file_name, 0, sizeof(file_name));
		sprintf(file_name,"%s%s", STR_MDS_PUT_DATA_DIR, pNI6123->Local_data_name);
	
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
	
	
				
				fpRaw = fopen(file_name, "rt+");
				if( fpRaw == NULL) {
					shotNumber = (int)pSTDdev->ST_mds_fetch.shotNumber;
					/*TG status = MdsClose(pSTDdev->treeName, (int*)(pSTDdev->ST_mds_fetch.shotNumber));   */
					status = MdsClose(pSTDdev->treeName, &shotNumber);
					
					epicsPrintf("can't oepn data file : %s\n", buf);
					/* TG MdsDisconnect(pSTDdev->treeIPport);   */
					MdsDisconnect();
					free(databuf);
					return(0);
				}
	
				for(j=0;j<nTrueSampleCnt;j++)
					{
						memset(tmp, 0, sizeof(tmp));
						fscanf(fpRaw, "%s", tmp);
						databuf[j] = atof(tmp);
					}
	
				fclose(fpRaw);
				fflush(fpRaw);
#if 0			
	/*			for(j = 0; j < pAcq196->sampleDataCnt; j++) */
				for(j = 0; j < nTrueSampleCnt; j++) 
				{
	/*				fread( &int16TempBuf, 2, 1, fpRaw );
					fTemp = ((float)(int16TempBuf) * 10.0 ) / 32768.0;	*/
					data[j] = ((float)(databuf[j]) * 10.0 ) / 32768.0; 
	/*				
					if ( j > nTrueSampleCnt ) {
						epicsPrintf(" got to end count:%lu\n", j); 
						break;
					}
	*/
				}
#endif
	
	
	/*			dataDesc = descr(&dtype_Float, data, &nTrueSampleCnt, &_null); */
				rawDataDesc = descr(&dtype_Float64, databuf, &nTrueSampleCnt, &_null);
	
				status = MdsPut(pNI6123->strTagName[i] , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);
	
	
				if ( !((status) & 1) )
				{
					epicsPrintf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, pNI6123->strTagName[i],(int)i, MdsGetMsg(status));
					printlog("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, pNI6123->strTagName[i],(int)i, MdsGetMsg(status));
#if 0
					mds_Close_Disconnect(pAcq196);
					clearAllchFiles((int)pAcq196->slot);
		
					free(databuf);
					free(data);
					data=NULL;
					databuf=NULL;
					return 0;
#endif
				} 
				else 
	
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
	
	
		} /* for(i = 0; i<96; i++)	 */
	
	/*	printf("%s, Tree Put Done! <cnt:%d>\n", pSTDdev->taskName, pAcq196->mdsplus_snd_cnt ); 
	
		printlog("%d: %s:(%.2f~%.2f) %dKHz, (%.2f/ %.2f MB) (%d ch), Cnt:%d\n", pSTDdev->ST_mds_fetch.shotNumber, pSTDdev->taskName, pSTDdev->ST_mds_fetch.dT0[0], pSTDdev->ST_mds_fetch.dT1[0], 
					pSTDdev->ST_mds_fetch.nSamplingRate/1000, 
					pAcq196->needDataSize/1024.0/1024.0,
					pAcq196->mdsplus_snd_cnt *2*nTrueSampleCnt/1024.0/1024.0, 
					pAcq196->mdsplus_snd_cnt,
						nTrueSampleCnt	);

		printf("Put %s: raw:%.2fMB, snd:%.2fMB(%d), Cnt:%d ... OK!\n", pSTDdev->taskName, 
							pAcq196->needDataSize/1024.0/1024.0,
							pAcq196->mdsplus_snd_cnt *2*nTrueSampleCnt/1024.0/1024.0, 
							pAcq196->mdsplus_snd_cnt,
							nTrueSampleCnt );
	*/
	/*	fprintf(stdout,"\nMDSplus >>> Data size: %luKB/ch\n", (4*pAcq196->sampleDataCnt)/1024 ); */
	
	/*	free(data); */
		free(databuf);
	/*	data=NULL; */
		databuf = NULL;
	
	
		mds_Close_withFetch(pSTDdev);
		mds_Disconnect(pSTDdev);
		
	/*
		clearAllchFiles((int)pAcq196->slot);
	*/
	/*	epicsThreadSleep(.3);	 2009. 10. 15 */
		
		
		return WR_OK;
}



/******************************************
  use file pointer   by woong 2009. 3. 25 
  use ptaskconfig pointer   2009. 05. 13
  
******************************************/

int swap32(int *n)
{
	unsigned char *cptr, tmp0, tmp1;

	cptr = (unsigned char*)n;
	
	tmp0 = cptr[0];
	tmp1 = cptr[1];

	cptr[0] = cptr[3];
	cptr[1] = cptr[2];
	cptr[2] = tmp1;
	cptr[3] = tmp0;

	return 1;
}

void clearAllchFiles(int slot)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];

	sprintf(strCmd, "rm -rf /media/data2/b%d.*", slot);
/*	epicsPrintf("sys cmd: %s\n", strCmd); */
	pfp=popen( strCmd, "r");
	if(!pfp) {
		epicsPrintf("\n>>> devACQ196_set_ADCstart : pipe command ... failed\n");
		return ;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		epicsPrintf("\n>>> clearAllfiles : %s ", buff);
	}
	pclose(pfp);
}




