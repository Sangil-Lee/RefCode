/*
// SIFConvert.cpp : Take all of the Andor .SIF files in a given directory and
//                  convert them to ASCII format
//
// Author: Stephen Granade                Date: 23 Mar 2001
//
// 폴더에 데이터를 모두 변경함.   - Local Mode
// Version 1.0
// Modify : TGLee							Date : 2009 09 10 Day  - Local Mode 
// If you make remote mode, you should comment out remove at mdsconnect and tree create part remove !!!!
*/
//---------------------------------------------------------------------------
/*
#pragma hdrstop
*/
#include "ATSIFIO.h"

#include "PiSPE.h"
/*
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
*/

//---------------------------------------------------------------------------

#include <io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
/* #include <dir.h> */
#include <process.h>

#include "mdslib.h"
#include "mdsdescrip.h"
#include "mdsshr.h"


#define MODEPIX				"Pixel number"
#define MODEWAVE			"Wavelength"

#define TREE				"r_spectro"
#define MDSIP       "172.17.100.202:8000"
#define status_ok(status)  (((status) & 1) == 1)


int mdsplus_data_put_sif(char* fileNamePath, int shotNumber, int storeMode, float blipTime, float daqTime)
{
  	return 1;
}
/*****************************************************************************
//Function: mdsplus_put_data
//Inputs: shutNumber
//Outputs: none
//The purpose of this function is to write the mdsplus tree node 
*****************************************************************************/

int mdsplus_data_put_spe(char* fileNamePath, int shotNumber, int storeMode, float blipTime, float daqTime)
{

char speChannelTagName[5][15] = {"\\SUR_VSS01:FOO", "\\SUR_VSS02:FOO", "\\SUR_VSS03:FOO", "\\SUR_VSS04:FOO", "\\SUR_VSS05:FOO"};
char speChannelNodeName1[7][26] = {"\\SURVEY_INFO:IMAGE_NO", "\\SURVEY_INFO:IMAGE_RIGHT", "\\SURVEY_INFO:IMAGE_TOP", "\\SURVEY_INFO:SUBIMAGE_NO", "\\SURVEY_INFO:T_EXPOSURE", "\\SURVEY_INFO:T_INTEGRATE", "\\SURVEY_INFO:T_KINETIC"};
char speChannelNodeName2[6][22] = {":POS_BOTTOM", ":POS_LEFT", ":RIGHT", ":TOP", ":HOR", ":VER"};
char speChannelWaveName[5][21] = {"\\SURVEY_WAVE:FOO", "\\SURVEY_WAVE:X_CAL0", "\\SURVEY_WAVE:X_CAL1", "\\SURVEY_WAVE:X_CAL2", "\\SURVEY_WAVE:X_CAL3"};

  float x_cal0_spe;
	float x_cal1_spe;
	float x_cal2_spe;
	float x_cal3_spe;

	long imageNumber;                          /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
	int number_subImage;                   /* image count, subImage = 5  */
	int right_pixel;                         /* pixel number : 1024   int => unsigned long int  */
	unsigned long int	image_length;                  /* 5120  = SubImage * 1024(frameSize) */
	
	unsigned short image_top;               /* Image Top ex.255  */
	unsigned short image_bottom;         /*Image Bottom ex.1  */
  unsigned long int total_length;
	  
	float cycle_time_spe;   /* same value delay & kinetic_cycle & integration cycle time */
	float exposure_time;    /* exposure time ex.0.01700  */
 /*	double integration_cycle;  Integration cycle time ex.0.21200  */
	char buf_spe[1024];
  char bufNode_spe[50];
	int null = 0;
	int status;
	int socket;
	
  int j, i, m;
	unsigned long int k, jum;
	int dtype_float = DTYPE_FLOAT;
	long timeCount=0;   /* imageNumber * right_pixel  */
	int dtypeLong = DTYPE_LONG;
  char dtypeString = DTYPE_CHAR;

	float *timeArray;  /* number_image = imageNumber = Image.no_images  */
	float *dataArray;
	unsigned short *image_buffer1;
	
	float *data;

	
	int  tstat, len;
	int  dataDesc;
	int  timeDesc;
	int  wavedataDesc;
	int  idesc = descr(&dtypeLong, &tstat, &null); 
	int  cdesc = descr(&dtypeString, &tstat, &null); 
	int kkk;
	
  int statFile;
  
/* start SPE File Read and Write Function */
  FILE* speFile;

/* Local Temp variable */ 
  float delayTime, exp_Sec, clkSpd_us, shutterComp;
  unsigned long numExpRepeats, numExpAccums;
  char date[DATEMAX];
  short thrMinLive, thrMaxLive;
  float thrMinVal, thrMaxVal;
  
  unsigned short xDimDet,yDimDet, xDim, yDim;
  short chipXdim, chipYdim;
  short speAutoMode, speGlueFlag;
  float centerWavelength, startWavelength, endWavelength;
  float minOverlap, finalResol;
  float pulseWidth, pulseDelay, pulseStartWidth, pulseEndWidth, pulseStartDelay, pulseEndDelay;
  
  long lnoscan, lavgexp;
  float ReadoutTime;
  
  long numFrames;
  char ylable[LABELMAX];
  unsigned short readoutMode, clockSpeed;
  short numROI,dataType;
  
  double pixel_posWlNm[10], calib_valWlNm[10], polynom_coeff[6];
  
  int sizeFile;
  struct ROIinfo
  {                                        //            
    unsigned short  startx;                           //  left x start value.               
    unsigned short  endx;                             //  right x value.                    
    unsigned short  groupx;                           //  amount x is binned/grouped in hw. 
    unsigned short  starty;                           //  top y start value.                
    unsigned short  endy;                             //  bottom y value.                   
    unsigned short  groupy;                           //  amount y is binned/grouped in hw. 
  } ROIinfoblk[ROIMAX];                     // 

 unsigned short rawDataTest;

/* Local Temp variable */ 
  speFile = fopen(fileNamePath ,"rb");
  printf(" OK 2 %s \n", fileNamePath);
  
  /* ################### We Need########################### */  
 printf(" Check Time ################################################ \n");
  statFile =fseek(speFile, PI_DATE, SEEK_SET);              /* Move 20 - date */
  statFile = fread(&date, sizeof(char),DATEMAX,speFile);    /* Read date -- fread(&readBuff, sizeof(type), 1(항목개수),file)  */
  printf(" OK 3 %d and date %s \n",statFile, date);         

  statFile=fseek(speFile, PI_DELAYTIME, SEEK_SET);          /* Move to Read */
  statFile = fread(&delayTime, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Delay Time of SPE File : %f \n",statFile, delayTime);

  statFile=fseek(speFile, PI_ReadoutTime, SEEK_SET);          /* Move to Read */
  statFile = fread(&ReadoutTime, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Experiment readout time  : %f \n",statFile, ReadoutTime);

  statFile=fseek(speFile, PI_exp_sec, SEEK_SET);          /* Move to Read */
  statFile = fread(&exp_Sec, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Alternative exposure, in sec : %f \n",statFile, exp_Sec);

  statFile=fseek(speFile, PI_clkspd_us, SEEK_SET);          /* Move to Read */
  statFile = fread(&clkSpd_us, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d,  Vert Clock Speed in micro-sec : %f \n",statFile, clkSpd_us);
  
  statFile =fseek(speFile, PI_clkspd, SEEK_SET);            /* Clock speed for kinetics and frame transfer */
  statFile = fread(&clockSpeed, sizeof(unsigned short),DATEMAX,speFile);
  printf(" OK ? %d, Clock speed for kinetics and frame transfer : %hu \n",statFile, clockSpeed);        

  statFile=fseek(speFile, PI_shutterComp, SEEK_SET);          /* Move to Read */
  statFile = fread(&shutterComp, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d,   shutter compensation time.  : %f \n",statFile, shutterComp);

  statFile=fseek(speFile, PI_NumExpRepeats, SEEK_SET);          /* Move to Read */
  statFile = fread(&numExpRepeats, sizeof(unsigned long),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Number of Times experiment repeated : %f \n",statFile, numExpRepeats);

  statFile=fseek(speFile, PI_NumExpAccums, SEEK_SET);          /* Move to Read */
  statFile = fread(&numExpAccums, sizeof(unsigned long),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Number of Time experiment accumulated : %f \n",statFile, numExpAccums);

 printf(" Check Time until ################################################ \n");



#if 0 
  statFile =fseek(speFile, PI_VChipXdim, SEEK_SET);            /* Move 6 - xDimDet */
  statFile = fread(&chipXdim, sizeof(unsigned short),DATEMAX,speFile);    /* Read date -- fread(&readBuff, sizeof(type), 1(항목개수),file)  */
  printf(" OK ? %d, Virtual Chip X dim %hi \n",statFile, chipXdim);         
  statFile =fseek(speFile, PI_VChipYdim, SEEK_SET);            /* Move 6 - xDimDet */
  statFile = fread(&chipYdim, sizeof(unsigned short),DATEMAX,speFile);    /* Read date -- fread(&readBuff, sizeof(type), 1(항목개수),file)  */
  printf(" OK ? %d, Virtual Chip Y dim %hi \n",statFile, chipYdim);         
  
  
  statFile=fseek(speFile, PI_ThresholdMinLive, SEEK_SET);   /* Move to Read */
  statFile = fread(&thrMinLive, sizeof(short),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_THRESHOLDMINLIVE of SPE File : %hi \n",statFile, thrMinLive);
  statFile=fseek(speFile, PI_ThresholdMinVal, SEEK_SET);   /* Move to Read */
  statFile = fread(&thrMinVal, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_THRESHOLDMINVAL of SPE File : %f \n",statFile, thrMinVal);

  statFile=fseek(speFile, PI_ThresholdMaxLive, SEEK_SET);   /* Move to Read */
  statFile = fread(&thrMaxLive, sizeof(short),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_THRESHOLDMAXLIVE of SPE File : %hi \n",statFile, thrMaxLive);
  statFile=fseek(speFile, PI_ThresholdMaxVal, SEEK_SET);   /* Move to Read */
  statFile = fread(&thrMaxVal, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_THRESHOLDMAXVAL of SPE File : %f \n",statFile, thrMaxVal);

  statFile=fseek(speFile, PI_SpecGlueFlag, SEEK_SET);          /* Move to Read */
  statFile = fread(&speGlueFlag, sizeof(short),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPECGLUEFLAG of SPE File : %hi \n",statFile, speGlueFlag);

  statFile=fseek(speFile, PI_SpecGlueMinOvrlpNm, SEEK_SET);          /* Move to Read */
  statFile = fread(&minOverlap, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPEC Minimum Overlap in NM of SPE File : %f \n",statFile, minOverlap);

  statFile=fseek(speFile, PI_SpecGlueFinalResNm, SEEK_SET);          /* Move to Read */
  statFile = fread(&finalResol, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPEC Final Resolution in NM of SPE File : %f \n",statFile, finalResol);


  statFile=fseek(speFile, PI_PulseRepWidth, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseWidth, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseRepWidth in NM of SPE File : %f \n",statFile, pulseWidth);

  statFile=fseek(speFile, PI_PulseRepDelay, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseDelay, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseRepDelay in NM of SPE File : %f \n",statFile, pulseDelay);

  statFile=fseek(speFile, PI_PulseSeqStartWidth, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseStartWidth, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseSeqStartWidth in NM of SPE File : %f \n",statFile, pulseStartWidth);
  
  statFile=fseek(speFile, PI_PulseSeqEndWidth, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseEndWidth, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseSeqEndWidth in NM of SPE File : %f \n",statFile, pulseEndWidth);

  statFile=fseek(speFile, PI_PulseSeqStartDelay, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseStartDelay, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseSeqStartDelay in NM of SPE File : %f \n",statFile, pulseStartDelay);

  statFile=fseek(speFile, PI_PulseSeqEndDelay, SEEK_SET);          /* Move to Read */
  statFile = fread(&pulseEndDelay, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_PulseSeqEndDelay in NM of SPE File : %f \n",statFile, pulseEndDelay);


  statFile=fseek(speFile, PI_lnoscan, SEEK_SET);          /* Move to Read */
  statFile = fread(&lnoscan, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Number of scans (Early WinX) : %ld \n",statFile, lnoscan);
  statFile=fseek(speFile, PI_lavgexp, SEEK_SET);          /* Move to Read */
  statFile = fread(&lavgexp, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, Number of Accumulations : %ld \n",statFile, lavgexp);
  


  statFile =fseek(speFile, PI_ylabel, SEEK_SET); /* We just check the y axis label */
  statFile = fread(&ylable, sizeof(char),DATEMAX,speFile);
  printf(" OK ? %d, y axis label : %s \n",statFile, ylable);         
  statFile =fseek(speFile, PI_readoutMode, SEEK_SET); /* We just check the readout mode, full,kinetics */
  statFile = fread(&readoutMode, sizeof(unsigned short),DATEMAX,speFile);
  printf(" OK ? %d, readout mode, full,kinetics, etc : %hu \n",statFile, readoutMode);        

  statFile=fseek(speFile, PI_SpecAutoSpectroMode, SEEK_SET);          /* Move to Read */
  statFile = fread(&speAutoMode, sizeof(short),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPECAUTOMODE of SPE File : %hi \n",statFile, speAutoMode);

  statFile=fseek(speFile, PI_SpecCenterWlNm, SEEK_SET);          /* Move to Read */
  statFile = fread(&centerWavelength, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPEC CENTER WaveLength NM of SPE File : %f \n",statFile, centerWavelength);

  statFile=fseek(speFile, PI_SpecGlueStartWlNm, SEEK_SET);          /* Move to Read */
  statFile = fread(&startWavelength, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPEC Starting WaveLength NM of SPE File : %f \n",statFile, startWavelength);

  statFile=fseek(speFile, PI_SpecGlueEndWlNm, SEEK_SET);          /* Move to Read */
  statFile = fread(&endWavelength, sizeof(float),1,speFile);    /* Read data from SPE file */
  printf(" OK ? %d, PI_SPEC Ending WaveLength NM of SPE File : %f \n",statFile, endWavelength);
  
  
#endif

#if 0

  statFile=fseek(speFile, PI_pixel_position, SEEK_SET); /* We just check the calc wavelength position by user */
  statFile = fread(&pixel_posWlNm, sizeof(double),10,speFile); 
  statFile=fseek(speFile, PI_calib_value, SEEK_SET);  /* We just check the calc wavelength value by user */
  statFile = fread(&calib_valWlNm, sizeof(double),10,speFile); 
    for(i=0; i<10; i++){
  printf(" PI_SPEC Calibration pixel for WaveLength NM pixel: %f , calWlNm Val: %f \n", pixel_posWlNm[i],calib_valWlNm[i]);
 }
/* ################### We Need PI_ROI_Entery ########################### */

  statFile=fseek(speFile, PI_NumROI, SEEK_SET);  /* We just check the Y axis divide information by user */
  statFile = fread(&numROI, sizeof(short),1,speFile);
  printf(" OK ? %d, Number of ROIs used if 0 assume 1. : %hi \n",statFile, numROI);

  statFile =fseek(speFile, PI_ROI_Entery, SEEK_SET);  /* We just check the Y axis divide information by user */
  statFile = fread(&ROIinfoblk, sizeof(unsigned short),DATEMAX,speFile);
  for(i=0; i<10; i++){
  printf(" OK ? %d, Number of ROIs %d, startx : %hu \n",statFile,i, ROIinfoblk[i].startx);        
  printf(" OK ? %d, Number of ROIs %d, endx : %hu \n",statFile,i, ROIinfoblk[i].endx);
  printf(" OK ? %d, Number of ROIs %d, groupx : %hu \n",statFile,i, ROIinfoblk[i].groupx);
  printf(" OK ? %d, Number of ROIs %d, starty : %hu \n",statFile,i, ROIinfoblk[i].starty);
  printf(" OK ? %d, Number of ROIs %d, endy : %hu \n",statFile,i, ROIinfoblk[i].endy);
  printf(" OK ? %d, Number of ROIs %d, groupy : %hu \n",statFile,i, ROIinfoblk[i].groupy);
  }
  statFile=fseek(speFile, 01, SEEK_END); /* We just check the end of data file size */
  sizeFile = ftell(speFile);
  printf(" file Size of end : %d \n",sizeFile);
#endif

  
  printf(" We Need  ################################################### ? \n");
  statFile =fseek(speFile, PI_ROI_Entery, SEEK_SET);  /* ####  We Need to store about pixel information ######  */
  statFile = fread(&ROIinfoblk, sizeof(unsigned short),DATEMAX,speFile);
    for(i=0; i<10; i++){
  printf(" OK ? %d, Number of ROIs %d, startx : %hu \n",statFile,i, ROIinfoblk[i].startx);        
  printf(" OK ? %d, Number of ROIs %d, endx : %hu \n",statFile,i, ROIinfoblk[i].endx);
  printf(" OK ? %d, Number of ROIs %d, groupx : %hu \n",statFile,i, ROIinfoblk[i].groupx);
  printf(" OK ? %d, Number of ROIs %d, starty : %hu \n",statFile,i, ROIinfoblk[i].starty);
  printf(" OK ? %d, Number of ROIs %d, endy : %hu \n",statFile,i, ROIinfoblk[i].endy);
  printf(" OK ? %d, Number of ROIs %d, groupy : %hu \n",statFile,i, ROIinfoblk[i].groupy);
  }
  statFile =fseek(speFile, PI_xDimDet, SEEK_SET); /* We just check the X dimension on CCD (Pixels on X ) */
  statFile = fread(&xDimDet, sizeof(unsigned short),DATEMAX,speFile);
  printf(" OK ? %d, Detector x dimension of chip %hu \n",statFile, xDimDet);
  statFile =fseek(speFile, PI_yDimDet, SEEK_SET); /* We just check the Y dimension on CCD (Pixels on Y ) */
  statFile = fread(&yDimDet, sizeof(unsigned short),DATEMAX,speFile);  
  printf(" OK ? %d, Detector y dimension of chip %hu \n",statFile, yDimDet);

  statFile =fseek(speFile, PI_xdim, SEEK_SET);  /* ####  We Need to store about devided ccd X dimension (Pixels on X ) ###### */  
  statFile = fread(&xDim, sizeof(unsigned short),DATEMAX,speFile);    
  printf(" OK ? %d,actual # of pixels on x axis  %hu \n",statFile, xDim);

  statFile =fseek(speFile, PI_ydim, SEEK_SET); /* ####  We Need to store about devided ccd Y dimension ###### */  
  statFile = fread(&yDim, sizeof(unsigned short),DATEMAX,speFile);
  printf(" OK ? %d, y dimension of raw data  %hu \n",statFile, yDim);

  statFile=fseek(speFile, PI_NumFrames, SEEK_SET);  /* ####  We Need to store about number of frames ###### */  
  statFile = fread(&numFrames, sizeof(long),1,speFile);
  printf(" OK ? %d, number of frames in file. : %ld \n",statFile, numFrames);

  statFile=fseek(speFile, PI_polynom_coeff, SEEK_SET);  /* ####  We Need to store about wavelength ###### */  
  statFile = fread(&polynom_coeff, sizeof(double),6,speFile);   

  for(i=0; i<6; i++){
  printf(" PI_SPEC Calibration pixel for WaveLength polynom_coeff: %f \n", polynom_coeff[i]);
 }



  statFile=fseek(speFile, PI_datatype, SEEK_SET);  /* ####  We Need to store about data type for read raw data ###### */  
  statFile = fread(&dataType, sizeof(short),1,speFile);
  printf(" OK ? %d, Raw data type 0:float(4byte), 1:long(4byte), 2:short(2byte), 3:unsigned short(2byte) : %hi \n",statFile, dataType);
  printf("size of short :%d, unsigned short :%d, int : %d, float :%d, double:%d, char :%d \n",sizeof(short),sizeof(unsigned short),sizeof(int),sizeof(float),sizeof(double),sizeof(char));






  if (ReadoutTime !=0){
      cycle_time_spe =(float)ReadoutTime / 1000;   /* same value delay & kinetic_cycle & integration cycle time */
      } else if(ReadoutTime ==0) {
        cycle_time_spe =(float)0.1;                /* CES used external clock. 100ms cycle (96ms exposure and transfort 4ms) */
      } else printf("Data Type Error. You must check data type \n");

exposure_time = exp_Sec;    /* exposure time ex.0.01700  */

/* integration_cycle = at32_integCycleTime;  We do not use this function . Integration cycle time ex.0.21200  */

/* This function is used to retrieve the information about each sub-image in the SPE file. */


image_top = ROIinfoblk[0].endy;              /* Image Top ex.255  */
image_bottom = ROIinfoblk[0].starty;         /*Image Bottom ex.1  */
image_length = xDim * yDim;                  /* 5120  = SubImage(5) * 1024(frameSize) for step. */


imageNumber = numFrames;                    /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
number_subImage = yDim;                     /* image count, subImage = 5  */
right_pixel =(int)xDim;                         /* pixel number : 1024   int => unsigned long int  */

  total_length = numFrames * xDim * yDim;  /* numFrames is 76 frames per 1 ydim. xDim is X pixel 1024. yDim is Y dim divided counts  */
printf("Check This ######################## 0 \n");
	timeCount = (long)imageNumber*right_pixel;
  right_pixel =(int)xDim;
  total_length =(unsigned long int)numFrames * xDim * yDim; 
  dataArray = (float*)malloc(4*(size_t)timeCount);
  
  image_buffer1 = (unsigned short*)malloc(2*(size_t)total_length);
printf("Check This ######################## 1 \n");


  if (dataType==0){
      statFile=fseek(speFile, PI_DATAREAD, SEEK_SET);
      statFile = fread(&image_buffer1, sizeof(float),total_length,speFile);
  } else if (dataType==1){
      statFile=fseek(speFile, PI_DATAREAD, SEEK_SET);
      statFile = fread(&image_buffer1, sizeof(long),total_length,speFile);
  } else if (dataType==2){
      statFile=fseek(speFile, PI_DATAREAD, SEEK_SET);
      statFile = fread(&image_buffer1, sizeof(short),total_length,speFile);
  } else if (dataType==3){
      statFile=fseek(speFile, PI_DATAREAD, SEEK_SET);
/*      statFile = fread(&image_buffer1, sizeof(unsigned short),total_length,speFile);  */
      statFile = fread(image_buffer1, sizeof(unsigned short),total_length,speFile);
      printf("Check This ######################## 2 \n");
  } else printf("Data Type Error. You must check data type \n");
  
  printf("Check This ######################## 3 \n");

#if 0 
for (i=0; i<2048; i++){
  
  printf("%d - %hu, ", i, image_buffer1[i]);

  }
  printf("\n Check This ######################## 5 \n");
#endif

	printf("total_length : %d \n",total_length);
	printf("image_length : %d \n",image_length);
	printf("number_image : %d \n",imageNumber);
	printf("imageNumber : %d \n",imageNumber);
	printf("number_subImage : %d \n",number_subImage);

	printf("right_pixel : %d \n",right_pixel);
	printf("cycle_time_spe : %f \n",cycle_time_spe);
	printf("exposure_time : %f \n",exposure_time);
/*	printf("integration_cycle : %f \n",integration_cycle); */

  memset(buf_spe,0,sizeof(buf_spe));
  
	printf("SPE MDSplus Data Put Part. \n");




#if 1

	timeArray = (float*)malloc((sizeof(float))*imageNumber);  /* 1D plot Data Time - OK  */
/*	dataArray = (float*)malloc(4*(size_t)timeCount);  1D plot Data   */
	
 	data = (float *)malloc(sizeof(float)*right_pixel);   /* waveLength and Pixel Number  */
	printf("SPE MDSplus Data Put Part 1. storeMode :%d \n",storeMode);
	if(storeMode == 1){
	  socket = MdsConnect(MDSIP);
	  if ( socket == -1)
	  {
		fprintf(stderr,"Error connecting to Server.\n");
		free(data);
		free(image_buffer1);
		free(dataArray);
		free(timeArray);
		return 0;
	  }
	  fprintf(stdout, "\nMdsplus >>> MdsConnect(\"%s\")...OK\n",MDSIP);
  }
 	printf("SPE MDSplus Data Put Part 1.5. \n");
#if 1
  if(storeMode == 0){
  	printf("SPE MDSplus Data Put Part 1.6. \n");
	  sprintf(buf_spe, "TCL(\"set tree %s/shot=-1\")",TREE);
	  printf("SPE MDSplus Data Put Part 1.6. : %s \n",buf_spe);
	  status = MdsValue(buf_spe, &cdesc, &null, 0);

    printf("SPE MDSplus Data Put Part 1.8. \n");
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Set tree -1 \n");
		  free(data);
		  free(image_buffer1);
		  free(dataArray);
		  free(timeArray);
		  return -1;
	  }
	  sprintf(buf_spe, "TCL(\"create pulse %ld\")",shotNumber);
	  status = MdsValue(buf_spe, &cdesc, &null, &len);
  	printf("SPE MDSplus Data Put Part 1.9 \n");
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Create Pulse  \n");
		  free(data);
		  free(image_buffer1);
		  free(dataArray);
		  free(timeArray);
		  return -1;
	  }
	  sprintf(buf_spe, "TCL(\"close\")",TREE);
	  status = MdsValue(buf_spe, &cdesc, &null, &len);
  }
#endif

  	printf("SPE MDSplus Data Put Part 2. \n");
	status = MdsOpen(TREE, &shotNumber);
	if (!status_ok(status))
	{
		  fprintf(stderr,"Error openning tree for shot %l. \n",shotNumber);
		  free(data);
		  free(image_buffer1);
		  free(dataArray);
		  free(timeArray);
		  return -1;
	}
    fprintf(stdout, "\nMdsplus >>> MdsOpen(\"%s %d\")...OK\n",TREE,shotNumber);

		printf("SPE MDSplus Data Put Part 3. \n");
/*  MdsPut Visible Spectrometer Data Infomation   */

	sprintf(buf_spe, "FS_FLOAT(%d)", imageNumber);
	status  = MdsPut(speChannelNodeName1[0], buf_spe,&null); 

	sprintf(buf_spe, "FS_FLOAT(%d)", right_pixel);
	status  = MdsPut(speChannelNodeName1[1], buf_spe,&null);

  sprintf(buf_spe, "FS_FLOAT(%hu)", image_top);
	status  = MdsPut(speChannelNodeName1[2],  buf_spe,&null);

	sprintf(buf_spe, "FS_FLOAT(%d)", number_subImage);
	status  = MdsPut(speChannelNodeName1[3],  buf_spe,&null);

	sprintf(buf_spe, "FS_FLOAT(%f)", exposure_time);
	status  = MdsPut(speChannelNodeName1[4], buf_spe,&null); 

/*	sprintf(buf_spe, "FS_FLOAT(%f)", integration_cycle);
	status  = MdsPut(speChannelNodeName1[5], buf_spe,&null);
*/

	sprintf(buf_spe, "FS_FLOAT(%f)", cycle_time_spe);
	status  = MdsPut(speChannelNodeName1[6], buf_spe,&null); 
	
	wavedataDesc = descr(&dtype_float, data, &right_pixel, &null);

/* 
 x_cal0 = 653.810000;
 x_cal1 = 0.004841;
 x_cal2 = -0.000000;
 x_cal3 = -0.000000;
*/

 x_cal0_spe = (float)polynom_coeff[0];
 x_cal1_spe = (float)polynom_coeff[1];
 x_cal2_spe = (float)polynom_coeff[2];
 x_cal3_spe = (float)polynom_coeff[3];



		for (i=0; i<right_pixel-1; i++)
		{	
		data[i] = x_cal0_spe + x_cal1_spe*(i+1) + x_cal2_spe*((i+1)^2) + x_cal3_spe*((i+1)^3);
		}
		fprintf(stdout,"Wave Data Cal 0 : %f \n", data[0]);
		fprintf(stdout,"Wave Data Cal 1 : %f \n", data[1]);
		status  = MdsPut(speChannelWaveName[0], "BUILD_SIGNAL($1,,)", &wavedataDesc, &null);

		sprintf(buf_spe, "FS_FLOAT(%f)", x_cal0_spe);
		status  = MdsPut(speChannelWaveName[1], buf_spe,&null); 
		sprintf(buf_spe, "FS_FLOAT(%f)", x_cal1_spe);
		status  = MdsPut(speChannelWaveName[2], buf_spe,&null); 
		sprintf(buf_spe, "FS_FLOAT(%f)", x_cal2_spe);
		status  = MdsPut(speChannelWaveName[3], buf_spe,&null); 
		sprintf(buf_spe, "FS_FLOAT(%f)", x_cal3_spe);
		status  = MdsPut(speChannelWaveName[4], buf_spe,&null); 


	fprintf(stdout, "CA_DAQ Start Time : %f  & BLIP Time : %f \n",daqTime, blipTime);
	timeDesc = descr(&dtype_float, timeArray, &imageNumber, &null); 
	for (i=0; i<imageNumber; i++)
	{
		timeArray[i] = (float)(daqTime-blipTime)+ i*cycle_time_spe;
   /*		printf("int2: timeArray2 : %f,  i2 : %d\n",timeArray[i],i);  */
	}
	
/* 1D Data Time Plot image length = imagenumber * right_pixel  OK - 2008.02.11
	timeDesc = descr(&dtype_float, timeArray, &timeCount, &null); 

	fprintf(out,"int: timeCount %d\n",timeCount);
	for (i=0; i<timeCount; i++)
	{
			    
	//		fprintf(stdout,"int: timeArray : %d,  i : %d\n",timeArray[i],i);
	//		fprintf(out,"int1: timeArray1 : %d,  i1 : %d\n",timeArray[i],i);
		timeArray[i] = (float)i/10;
		fprintf(out,"int2: timeArray2 : %f,  i2 : %d\n",timeArray[i],i);
	}
	//Get the Data X_data => WaveLength Data Cal
	dataDesc = descr(&dtype_float, dataArray, &timeCount, &null); //descr(TYPE, Data1D, 1Dcount,0)
*/

  	kkk=1;
	  dataDesc = descr(&dtype_float, dataArray, &right_pixel, &imageNumber, &kkk, &null);  /* descr(TYPE, Data3D, nx,ny,nz,0) ?? */

    printf("number_subImage : %d, imageNumber : %d, right_pixel :%d , image_length :%d \n",number_subImage,imageNumber,right_pixel,image_length );
    
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);
		for (m=0,k=0; m<number_subImage; m++)
		{
			for (jum=0,j=0; j<imageNumber; j++)
			{
				for (i=0; i<right_pixel; i++, k++)
				{	
					dataArray[j*right_pixel + i] = image_buffer1[k];
					/* printf("dataArray Put : %f,   image_buffer Data : %hu, k = %d\n", dataArray[j*right_pixel + i],image_buffer1[k],k);  */
				}
				printf("dataArray Put : %f,   image_buffer Data : %hu, k = %uli \n", dataArray[j*right_pixel + i],image_buffer1[k],k); 
				if(imageNumber>1)
				{
					jum=(j+1)*image_length + (m*right_pixel);  /* right_pixel = 1024, image_length = 5120 */
					k= jum;
				}
			}
			k = (m+1)*right_pixel ;
    
      printf("Size of image_buffer1 : %d, and dataArray size : %d,\n",sizeof(image_buffer1), sizeof(dataArray));
      

			status  = MdsPut(speChannelTagName[m], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null);   /* Data Put 2D and Time TEST ING */
			/*   MDSput The VSS Data Infomations about Image    */
      if (!status_ok(status))
	    {
		      fprintf(stderr,"Error Data Put shot: %ld. m : %d. \n",shotNumber, m);
	    }
      if(m<10){
			  sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].starty);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[0]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
        if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
			  sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].startx);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[1]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
        if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
			  sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].endx);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[2]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
        if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
			  sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].endy);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[3]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
        if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
		  	sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].endx);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[4]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
        if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
			  sprintf(buf_spe, "FS_FLOAT(%d)", ROIinfoblk[i].endy);
			  sprintf(bufNode_spe, "\\SUR_VSS0%d%s",m+1,speChannelNodeName2[5]);
			  status  = MdsPut(bufNode_spe, buf_spe,&null);
			  if (!status_ok(status))
	      {
		      fprintf(stderr,"Error Node Data Put : %s. \n",bufNode_spe);
	      }
			  printf("check Node name for each channel VSS Name: %s\n",bufNode_spe);
			}
		}


	free(data);
	free(image_buffer1);
	free(timeArray);
	free(dataArray);

/*  MDSplus Close Tree  */
 	status = MdsClose(TREE, &shotNumber);
 	if (!status_ok(status))
	{
 		fprintf(stderr,"Error closing tree for shot %l. \n",shotNumber);
 		return -1;
	}
   fprintf(stdout, "\nMdsplus >>> MdsClose(\"%d\")...OK\n",shotNumber);


#endif
fclose(speFile);

	return 1;
}






