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

char channelTagName[5][15] = {"\\HR_VSS01:FOO", "\\HR_VSS02:FOO", "\\HR_VSS03:FOO", "\\HR_VSS04:FOO", "\\HR_VSS05:FOO"};
char channelNodeName1[7][22] = {"\\HR_INFO:IMAGE_NO", "\\HR_INFO:IMAGE_RIGHT", "\\HR_INFO:IMAGE_TOP", "\\HR_INFO:SUBIMAGE_NO", "\\HR_INFO:T_EXPOSURE", "\\HR_INFO:T_INTEGRATE", "\\HR_INFO:T_KINETIC"};
/* char channelNodeName2[6][22] = {"\\HR_VSS01:POS_BOTTOM", "\\HR_VSS02:POS_LEFT", "\\HR_VSS03:RIGHT", "\\HR_VSS04:TOP", "\\HR_VSS05:HOR", "\\HR_VSS05:VER"};  */
char channelNodeName2[6][22] = {":POS_BOTTOM", ":POS_LEFT", ":RIGHT", ":TOP", ":HOR", ":VER"};
char channelWaveName[5][17] = {"\\HR_WAVE:FOO", "\\HR_WAVE:X_CAL0", "\\HR_WAVE:X_CAL1", "\\HR_WAVE:X_CAL2", "\\HR_WAVE:X_CAL3"};

char proterty[91][40] = {ATSIF_PROP_TYPE,ATSIF_PROP_ACTIVE,ATSIF_PROP_VERSION,ATSIF_PROP_TIME,ATSIF_PROP_FORMATTED_TIME,ATSIF_PROP_FILENAME,
ATSIF_PROP_TEMPERATURE,ATSIF_PROP_UNSTABILIZEDTEMPERATURE,ATSIF_PROP_HEAD,ATSIF_PROP_HEADMODEL,ATSIF_PROP_STORETYPE,
ATSIF_PROP_DATATYPE,ATSIF_PROP_SIDISPLACEMENT,ATSIF_PROP_SINUMBERSUBFRAMES,ATSIF_PROP_PIXELREADOUTTIME,ATSIF_PROP_TRACKHEIGHT,
ATSIF_PROP_READPATTERN,ATSIF_PROP_READPATTERN_FULLNAME,ATSIF_PROP_SHUTTERDELAY,ATSIF_PROP_CENTREROW,ATSIF_PROP_ROWOFFSET,
ATSIF_PROP_OPERATION,ATSIF_PROP_MODE,ATSIF_PROP_MODE_FULLNAME,ATSIF_PROP_TRIGGERSOURCE,ATSIF_PROP_TRIGGERSOURCE_FULLNAME,
ATSIF_PROP_TRIGGERLEVEL,ATSIF_PROP_EXPOSURETIME,ATSIF_PROP_DELAY,ATSIF_PROP_INTEGRATIONCYCLETIME,ATSIF_PROP_NUMBERINTEGRATIONS,
ATSIF_PROP_KINETICCYCLETIME,ATSIF_PROP_FLIPX,ATSIF_PROP_FLIPY,ATSIF_PROP_CLOCK,ATSIF_PROP_ACLOCK,ATSIF_PROP_IOC,
ATSIF_PROP_FREQUENCY,ATSIF_PROP_NUMBERPULSES,ATSIF_PROP_FRAMETRANSFERACQMODE,ATSIF_PROP_BASELINECLAMP,ATSIF_PROP_PRESCAN,
ATSIF_PROP_EMREALGAIN,ATSIF_PROP_BASELINEOFFSET,ATSIF_PROP_SWVERSION,ATSIF_PROP_SWVERSIONEX,ATSIF_PROP_MCP,ATSIF_PROP_GAIN,
ATSIF_PROP_VERTICALCLOCKAMP,ATSIF_PROP_VERTICALSHIFTSPEED,ATSIF_PROP_OUTPUTAMPLIFIER,ATSIF_PROP_PREAMPLIFIERGAIN,ATSIF_PROP_SERIAL,
ATSIF_PROP_DETECTORFORMATX,ATSIF_PROP_DETECTORFORMATZ,ATSIF_PROP_NUMBERIMAGES,ATSIF_PROP_NUMBERSUBIMAGES,ATSIF_PROP_SUBIMAGE_HBIN,
ATSIF_PROP_SUBIMAGE_VBIN,ATSIF_PROP_SUBIMAGE_LEFT,ATSIF_PROP_SUBIMAGE_RIGHT,ATSIF_PROP_SUBIMAGE_TOP,ATSIF_PROP_SUBIMAGE_BOTTOM,
ATSIF_PROP_BASELINE,ATSIF_PROP_CCD_LEFT,ATSIF_PROP_CCD_RIGHT,ATSIF_PROP_CCD_TOP,ATSIF_PROP_CCD_BOTTOM,ATSIF_PROP_SENSITIVITY,ATSIF_PROP_DETECTIONWAVELENGTH,
ATSIF_PROP_COUNTCONVERTMODE,ATSIF_PROP_ISCOUNTCONVERT,ATSIF_PROP_X_AXIS_TYPE,ATSIF_PROP_X_AXIS_UNIT,ATSIF_PROP_Y_AXIS_TYPE,ATSIF_PROP_Y_AXIS_UNIT,
ATSIF_PROP_Z_AXIS_TYPE,ATSIF_PROP_Z_AXIS_UNIT,ATSIF_PROP_USERTEXT,ATSIF_PROP_ISPHOTONCOUNTINGENABLED,ATSIF_PROP_NUMBERTHRESHOLDS,ATSIF_PROP_THRESHOLD1,
ATSIF_PROP_THRESHOLD2,ATSIF_PROP_THRESHOLD3,ATSIF_PROP_THRESHOLD4,ATSIF_PROP_AVERAGINGFILTERMODE,ATSIF_PROP_AVERAGINGFACTOR,ATSIF_PROP_FRAMECOUNT,
ATSIF_PROP_NOISEFILTER,ATSIF_PROP_THRESHOLD,ATSIF_PROP_TIME_STAMP};

int mdsplus_Tree_create(int storeMode, int shotNumber)
{	
  char buf[50];
	int null = 0;
	int status;

	int dtype_float = DTYPE_FLOAT;
	int dtypeLong = DTYPE_LONG;
	
	int  tstat, len;
	int  idesc = descr(&dtypeLong, &tstat, &null); 
	
  if(storeMode == 0){
	  sprintf(buf, "TCL(\"set tree %s/shot=-1\")",TREE);
	  status = MdsValue(buf, &idesc, &null, &len);
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Set tree -1 \n");
		  return -1;
	  }
	  sprintf(buf, "TCL(\"create pulse %ld\")",shotNumber);
	  status = MdsValue(buf, &idesc, &null, &len);
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Create Pulse  \n");
		  return -1;
	  }
	  sprintf(buf, "TCL(\"close\")",TREE);
	  status = MdsValue(buf, &idesc, &null, &len);
  }
  return 1;
}
/*****************************************************************************
//Function: mdsplus_put_data
//Inputs: shutNumber
//Outputs: none
//The purpose of this function is to write the mdsplus tree node 
*****************************************************************************/

int mdsplus_data_put_sif(char* fileNamePath, int shotNumber, int storeMode, float blipTime, float daqTime)
{

  AT_U32 atu32_ret, atu32_noFrames, atu32_frameSize, atu32_noSubImages;
  float * imageBuffer;
  double calibValue;


  char *  sz_propertyValue;
  AT_U32 wave_mode;
  AT_U32 atu32_left,atu32_bottom,atu32_right,atu32_top,atu32_hBin, atu32_vBin;
  double at32_delay, at32_exposeTime, at32_integCycleTime, at32_kineticCycleTime;


  double x_cal0;
	double x_cal1;
	double x_cal2;
	double x_cal3;

	int imageNumber;                          /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
	int number_subImage;                   /* image count, subImage = 5  */
	int right_pixel;                         /* pixel number : 1024   int => unsigned long int  */
	int	image_length;                  /* 5120  = SubImage * 1024(frameSize) */
	
	int image_top;               /* Image Top ex.255  */
	int image_bottom;         /*Image Bottom ex.1  */
  AT_U32 total_length;
	  
	double cycle_time;   /* same value delay & kinetic_cycle & integration cycle time */
	double exposure_time;    /* exposure time ex.0.01700  */
	double integration_cycle;  /* Integration cycle time ex.0.21200  */
	char buf[50];
  char bufNode[50];
	int null = 0;
	int status;
	int socket;
	int j, i, m, iwave;
	unsigned long int k, jum;
	int dtype_float = DTYPE_FLOAT;
	int dtype_double = DTYPE_DOUBLE;
	int timeCount=0;   /* imageNumber * right_pixel  */
	int dtypeLong = DTYPE_LONG;

	double *timeArray;  /* number_image = imageNumber = Image.no_images  */
	float *dataArray;

	double *data;
	float *image_buffer1;
	
	int  tstat;
	int  dataDesc;
	int  timeDesc;
	int  wavedataDesc;
	int  idesc = descr(&dtypeLong, &tstat, &null); 
	int kkk;

  char tmpBuf[50];


 	
/* This function is used to select if the entire SIF file should be read or just the header section */
  atu32_ret = ATSIF_SetFileAccessMode(ATSIF_ReadAll);
  if(atu32_ret != ATSIF_SUCCESS) {
        printf("Could not set File access Mode. Error: %u\n", atu32_ret);
      }
      else {
        /* This function is used to open a SIF file where the file name and path are contained 
                                                           in the character array _sz_filename */
        atu32_ret = ATSIF_ReadFromFile(fileNamePath);
        if(atu32_ret != ATSIF_SUCCESS) {
          printf("Could not open File : %s.\nError: %u\n", fileNamePath,atu32_ret);
        }
        else {
          atu32_ret = ATSIF_GetNumberFrames(ATSIF_Signal, &atu32_noFrames);
          /* This function is used to retrieve the number of frames in the SIF file. */
          if(atu32_ret != ATSIF_SUCCESS) {
            printf("Could not Get Number Frames. Error: %u\n", atu32_ret);
          } else {
            printf("Image contains %u frames.\n", atu32_noFrames);
            atu32_ret = ATSIF_GetFrameSize(ATSIF_Signal, &atu32_frameSize);
            /* This function is used to retrieve the number of pixels in each frame in the SIF file. */
            if(atu32_ret != ATSIF_SUCCESS) {
              printf("Could not Get Frame Size. Error: %u\n", atu32_ret);
            } else {
              printf("Each frame contains %u pixels.\n", atu32_frameSize);
              atu32_ret = ATSIF_GetNumberSubImages(ATSIF_Signal, &atu32_noSubImages);
              /* This function is used to retrieve the number of sub-images in each frame in the SIF file. */
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not Get Number Sub Images. Error: %u\n", atu32_ret);
              } else {
                printf("Each frame contains %u sub images.\n", atu32_noSubImages);
                }
            }
            
            /* ################# Property Type and Value ##################*/
               sz_propertyValue = (char*)malloc((sizeof(char)*MAX_PATH));
              
/* We need This function. Because conversion and used the value from string to int value  */
              atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_EXPOSURETIME, sz_propertyValue, MAX_PATH);
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not get Property Value.\n");
              }
              else {
                at32_exposeTime = atof(sz_propertyValue);
                printf("Property Value ExposureTime : %s, %0.4f\n", sz_propertyValue,at32_exposeTime);
              }
              atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_DELAY, sz_propertyValue, MAX_PATH);
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not get Property Value.\n");
              }
              else {
                at32_delay = atof(sz_propertyValue);
                printf("Property Value Delay : %s\n", sz_propertyValue);
              }
              atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_INTEGRATIONCYCLETIME, sz_propertyValue, MAX_PATH);
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not get Property Value.\n");
              }
              else {
                at32_integCycleTime = atof(sz_propertyValue);
                printf("Property Value IntegrationCycleTime : %s, %0.4f\n", sz_propertyValue,at32_integCycleTime );
              }
              atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_KINETICCYCLETIME, sz_propertyValue, MAX_PATH);
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not get Property Value.\n");
              }
              else {
                at32_kineticCycleTime = atof(sz_propertyValue);
                printf("Property Value KineticCycleTime : %s, %0.4f \n", sz_propertyValue, at32_kineticCycleTime);
              }
              atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_MODE, sz_propertyValue, MAX_PATH);
              if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not get Property Value.\n");
              }
              else {
                 printf("Property Value Mode : %s\n", sz_propertyValue);
              }
              for(i = 0; i < 91; i++) {
                  atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, proterty[i], sz_propertyValue, MAX_PATH);
                  if(atu32_ret != ATSIF_SUCCESS) {
                      printf("Could not get Property Value : %s.\n",proterty[i]);
                  }
                  else {
                      printf("Property Value %s : %s\n",proterty[i], sz_propertyValue);
                  }
              }
           
           for(i = 0; i < (int)atu32_noFrames; i++) {
                  sprintf(tmpBuf,"TimeStamp %d",i);
                  atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, tmpBuf, sz_propertyValue, MAX_PATH);
                  if(atu32_ret != ATSIF_SUCCESS) {
                      printf("Could not get Property Value : %s.\n",tmpBuf);
                  }
                  else {
                      printf("Property Value %s : %s\n",tmpBuf, sz_propertyValue);
                  }
              }
            /* ################# Property Type and Value ##################*/
            for(i = 0; i < (int)atu32_noSubImages; i++) {
                
                printf("SubImage %u Properties:\n", (i + 1));
                /* This function is used to retrieve the information about each sub-image in the SIF file. */
                atu32_ret = ATSIF_GetSubImageInfo(ATSIF_Signal,
                                                  i,
                                                  &atu32_left,&atu32_bottom,
                                                  &atu32_right,&atu32_top,
                                                  &atu32_hBin,&atu32_vBin);
                if(atu32_ret != ATSIF_SUCCESS) {
                  printf("Could not Get Sub Image Info. Error: %u\n", atu32_ret);
                }
                else {
                  printf("\tleft\t: %u\tbottom\t: %u\n", atu32_left, atu32_bottom);
                  printf("\tright\t: %u\ttop\t: %u\n", atu32_right, atu32_top);
                  printf("\thBin\t: %u\tvBin\t: %u\n", atu32_hBin, atu32_vBin);
                }
           }
           /* I just check the data of frame in SIF file */
            imageBuffer  = (float*)malloc((sizeof(float))* atu32_frameSize);
            atu32_ret = ATSIF_GetFrame(ATSIF_Signal,0, imageBuffer, atu32_frameSize);
            if(atu32_ret != ATSIF_SUCCESS) {
              printf("Could not Get Frame. Error: %u\n", atu32_ret);
            } else {
              printf("The first 20 pixel values are : \n");
              for(i = 0; i < 20; i++) {
                  printf("%f\n", imageBuffer[i]);
               }
            }
             free(imageBuffer);
          }
        }
      }




#if 1
/* Wavelength = x_cal(0) + x_cal(1).X + x_cal(2).X^2 + x_cal(3).X^3  */
/* X is the pixel number on in left hand column.  double to float type changed 2009.05.15  */
/* AT_U32 atu32_ret, atu32_noFrames, atu32_frameSize, atu32_noSubImages;  */
/* AT_U32 atu32_left,atu32_bottom,atu32_right,atu32_top,atu32_hBin, atu32_vBin; */
/* AT_32 at32_delay, at32_exposeTime, at32_integCycleTime, at32_kineticCycleTime; */
/* time count number = total_length/image_length  */



cycle_time = at32_kineticCycleTime;   /* same value delay & kinetic_cycle & integration cycle time */
exposure_time = at32_exposeTime;    /* exposure time ex.0.01700  */
integration_cycle = at32_integCycleTime;  /* Integration cycle time ex.0.21200  */

/* This function is used to retrieve the information about each sub-image in the SIF file. */
atu32_ret = ATSIF_GetSubImageInfo(ATSIF_Signal, 0, &atu32_left,&atu32_bottom, &atu32_right,&atu32_top, &atu32_hBin,&atu32_vBin);
if(atu32_ret != ATSIF_SUCCESS) {
  printf("Could not Get Sub Image Info. Error: %u\n", atu32_ret);
}
 else {
 printf("\tleft\t: %u\tbottom\t: %u\n", atu32_left, atu32_bottom);
 printf("\tright\t: %u\ttop\t: %u\n", atu32_right, atu32_top);
 printf("\thBin\t: %u\tvBin\t: %u\n", atu32_hBin, atu32_vBin);
 }
 
image_top = atu32_top;               /* Image Top ex.255  */
image_bottom = atu32_bottom;         /*Image Bottom ex.1  */
image_length = atu32_frameSize * atu32_noSubImages;    /* 5120  = SubImage * 1024(frameSize) */
total_length = atu32_noFrames * atu32_frameSize * atu32_noSubImages;

image_buffer1 = (float*)malloc(4*(size_t)total_length);

atu32_ret = ATSIF_GetAllFrames(ATSIF_Signal, image_buffer1, total_length);
   if(atu32_ret != ATSIF_SUCCESS) {
       printf("Could not Get Frame. Error: %u\n", atu32_ret);
    } else {
          printf(" Success All frames data read. : \n");
    }

imageNumber = atu32_noFrames;                          /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
number_subImage = atu32_noSubImages;                   /* image count, subImage = 5  */
right_pixel = atu32_frameSize;                         /* pixel number : 1024   int => unsigned long int  */



	printf("total_length : %d \n",total_length);
	printf("image_length : %d \n",image_length);
	printf("number_image : %d \n",imageNumber);
	printf("imageNumber : %d \n",imageNumber);
	printf("number_subImage : %d \n",number_subImage);
	printf("left_pixel : %d \n",atu32_left);
	printf("right_pixel : %d \n",right_pixel);
	printf("cycle_time : %f \n",cycle_time);
	printf("exposure_time : %f \n",exposure_time);
	printf("integration_cycle : %f \n",integration_cycle);


	printf("SIF MDSplus Data Put Part. \n");
	timeCount = imageNumber*right_pixel;
	
	timeArray = (double*)malloc((sizeof(double))*imageNumber*right_pixel);  /* 1D plot Data Time - OK  */
	dataArray = (float*)malloc(4*(size_t)timeCount);  /* 1D plot Data   */
	data = (double *)malloc(sizeof(double)*right_pixel);   /* waveLength and Pixel Number  */

  iwave = 0;
  for(i=(int)atu32_frameSize; i >= 1; i--) {
       atu32_ret = ATSIF_GetPixelCalibration(ATSIF_Signal, ATSIF_CalibX, i, &calibValue);
      /* This function is used to retrieve wavelength in each frame in the SIF file.  */
       if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not Get ATSIF_GetPixelCalibration. Error: %u\n", atu32_ret);
       }else {
/*              printf("Wavelength pixel:%d , X: %f   ", i, calibValue);   */
               data[iwave] = calibValue;
               iwave ++;
       }
       }
       printf("\n");

 x_cal0 = data[0];
 x_cal1 = data[1] - data[0];
 x_cal2 = -0.000000;
 x_cal3 = -0.000000;

 if(x_cal1 < 0) {
      iwave = 0;
      for(i=1; i <= (int)atu32_frameSize; i++) {
       atu32_ret = ATSIF_GetPixelCalibration(ATSIF_Signal, ATSIF_CalibX, i, &calibValue);
      /* This function is used to retrieve wavelength in each frame in the SIF file.  */
       if(atu32_ret != ATSIF_SUCCESS) {
                printf("Could not Get ATSIF_GetPixelCalibration. Error: %u\n", atu32_ret);
       }else {
/*              printf("Wavelength pixel:%d , X: %f   ", i, calibValue);   */
               data[iwave] = calibValue;
               iwave ++;
       }
       }
      x_cal0 = data[0];
      x_cal1 = data[1] - data[0];
      x_cal2 = -0.000000;
      x_cal3 = -0.000000;
}

	printf("x_cal0 : %f \n",x_cal0);
	printf("x_cal1 : %f \n",x_cal1);
	printf("x_cal2 : %f \n",x_cal2);
	printf("x_cal3 : %f \n",x_cal3);

	if(storeMode == 1){
	  socket = MdsConnect(MDSIP);
	  if ( socket == -1)
	  {
		fprintf(stderr,"Error connecting to Server.\n");
		free(data);
		free(dataArray);
		free(timeArray);
		return 0;
	  }
	  fprintf(stdout, "\nMdsplus >>> MdsConnect(\"%s\")...OK\n",MDSIP);
  }
#if 0
  
  if(storeMode == 0){
	  sprintf(buf, "TCL(\"set tree %s/shot=-1\")",TREE);
	  status = MdsValue(buf, &idesc, &null, &len);
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Set tree -1 \n");
		  return -1;
	  }
	  sprintf(buf, "TCL(\"create pulse %ld\")",shotNumber);
	  status = MdsValue(buf, &idesc, &null, &len);
	  if (!status_ok(status))
	  {
		  fprintf(stderr,"Error TCL Command Create Pulse  \n");
		  return -1;
	  }
	  sprintf(buf, "TCL(\"close\")",TREE);
	  status = MdsValue(buf, &idesc, &null, &len);
  }
#endif
  
  
	status = MdsOpen(TREE, &shotNumber);
	if (!status_ok(status))
	{
		fprintf(stderr,"Error openning tree for shot %l. \n",shotNumber);
		return -1;
	}
    fprintf(stdout, "\nMdsplus >>> MdsOpen(\"%s %d\")...OK\n",TREE,shotNumber);

	
/*  MdsPut Visible Spectrometer Data Infomation   */

	sprintf(buf, "FS_FLOAT(%d)", imageNumber);
	status  = MdsPut(channelNodeName1[0], buf,&null); 

	sprintf(buf, "FS_FLOAT(%d)", right_pixel);
	status  = MdsPut(channelNodeName1[1], buf,&null);

  sprintf(buf, "FS_FLOAT(%d)", image_top);
	status  = MdsPut(channelNodeName1[2],  buf,&null);

	sprintf(buf, "FS_FLOAT(%d)", number_subImage);
	status  = MdsPut(channelNodeName1[3],  buf,&null);

	sprintf(buf, "FS_FLOAT(%f)", exposure_time);
	status  = MdsPut(channelNodeName1[4], buf,&null); 

	sprintf(buf, "FS_FLOAT(%f)", integration_cycle);
	status  = MdsPut(channelNodeName1[5], buf,&null);

	sprintf(buf, "FS_FLOAT(%f)", cycle_time);
	status  = MdsPut(channelNodeName1[6], buf,&null); 
	wavedataDesc = descr(&dtype_double, data, &right_pixel, &null);

  atu32_ret = ATSIF_GetPropertyValue(ATSIF_Signal, ATSIF_PROP_X_AXIS_TYPE, sz_propertyValue, MAX_PATH);
  if(atu32_ret != ATSIF_SUCCESS) {
    printf("Could not get Property Value : %s.\n",ATSIF_PROP_X_AXIS_TYPE);
  } else {
          if(strcmp(sz_propertyValue,"Wavelength") != 0){
               wave_mode = 0;   /*pixel mode */
            } else {
               wave_mode = 1;  /*wavelength mode */
            }
            printf("Property Value %s : %s\n",ATSIF_PROP_X_AXIS_TYPE, sz_propertyValue);
  }

	if (wave_mode==1)
	{
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);
#if 0
		fprintf(stdout,"1 Wave Data Cal 0 : %f \n", data[0]);
		fprintf(stdout,"1 Wave Data Cal 1 : %f \n", data[1]);
		for (i=0; i<right_pixel; i++)
		{	
		data[i] = x_cal0 + x_cal1*(i) + x_cal2*((i)^2) + x_cal3*((i)^3);
		}
		fprintf(stdout,"2 Wave Data Cal 0 : %f \n", data[0]);
		fprintf(stdout,"2 Wave Data Cal 1 : %f \n", data[1]);
#endif

		status  = MdsPut(channelWaveName[0], "BUILD_SIGNAL($1,,)", &wavedataDesc, &null);

		sprintf(buf, "FS_FLOAT(%f)", x_cal0);
		status  = MdsPut(channelWaveName[1], buf,&null); 
		sprintf(buf, "FS_FLOAT(%f)", x_cal1);
		status  = MdsPut(channelWaveName[2], buf,&null); 
		sprintf(buf, "FS_FLOAT(%f)", x_cal2);
		status  = MdsPut(channelWaveName[3], buf,&null); 
		sprintf(buf, "FS_FLOAT(%f)", x_cal3);
		status  = MdsPut(channelWaveName[4], buf,&null); 
	}
	else if (wave_mode!=1)
	{
		fprintf(stdout, "DAQ PIXMode  OK : %s \n",MODEPIX);
		for (i=0; i<right_pixel; i++)
		{
		data[i] = i+1;
		}
	}
	else 
	{
		fprintf(stdout, "DAQ PIXMode  No Good  : %s \n",MODEPIX);
		for (i=0; i<right_pixel; i++)
		{
		data[i] = i+1;
		}
		fprintf(stderr, "Error mode is not waveLength and Pixel number \n");
	}



	fprintf(stdout, "CA_DAQ Start Time : %f  & BLIP Time : %f \n",daqTime, blipTime);
	timeDesc = descr(&dtype_double, timeArray, &imageNumber, &null); 
	for (i=0; i<imageNumber; i++)
	{
		timeArray[i] = (double)(daqTime-blipTime)+ i*cycle_time;
		printf("timeArray : %f,  frame nu : %d  ",timeArray[i],i);
	}
	printf("\n");
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

	if (wave_mode==1)
	{
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);

		for (m=0,k=0; m<number_subImage; m++)
		{
			for (jum=0,j=0; j<imageNumber; j++)
			{   
				for (i=0; i<right_pixel; i++, k++)
				{	
					dataArray[j*right_pixel + i] = image_buffer1[k];
/*					printf("dataArray Put : %f,   image_buffer Data : %f, k = %d\n", dataArray[j*right_pixel + i],image_buffer1[k],k);  */
				}
				
				if(imageNumber>1)
				{
					jum=(j+1)*image_length + (m*right_pixel);  /* right_pixel = 1024, image_length = 5120 */
					k= jum;
				}
				
			}
			k = (m+1)*right_pixel ;

			status  = MdsPut(channelTagName[m], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null);   /* Data Put 2D and Time TEST ING */
			/*   MDSput The VSS Data Infomations about Image    */
			atu32_ret = ATSIF_GetSubImageInfo(ATSIF_Signal,
                                                  m,
                                                  &atu32_left,&atu32_bottom,
                                                  &atu32_right,&atu32_top,
                                                  &atu32_hBin,&atu32_vBin);
      if(atu32_ret != ATSIF_SUCCESS) {
           printf("Could not Get Sub Image Info. Error: %u\n", atu32_ret);
      }
			sprintf(buf, "FS_FLOAT(%d)", atu32_bottom);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[0]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_left);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[1]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_right);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[2]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_top);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[3]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_hBin);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[4]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_vBin);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[5]);
			status  = MdsPut(bufNode, buf,&null);
			printf("check Node name for each channel VSS Name: %s\n",bufNode);
		}
	}
	else if (wave_mode!=1)
	{
		fprintf(stdout, "DAQ PIXMode  OK : %s \n",MODEPIX);

		for (m=0,k=0; m<number_subImage; m++)
		{
			for (jum=0,j=0; j<imageNumber; j++)
			{   
				for (i=0; i<right_pixel; i++, k++)
				{	
/*					if(k==0){
					fprintf(stdout, "MDSplus Put the Data image_lastBuffData : %f Num k : %d \n", image_buffer1[k], k);
					}
*/
					dataArray[j*right_pixel + i] = image_buffer1[k];

/*					printf("dataArray Put : %f,   image_buffer Data : %f, k = %d\n", dataArray[j*right_pixel + i],image_buffer1[k],k);  */
/*					if(k==1023 || k==1024){
					fprintf(stdout, "MDSplus Put the Data image_lastBuffData 1023 or 1024 : %f Num k : %d \n", image_buffer1[k], k);
					}
*/
				}
				if(imageNumber>1)
				{
/*							jum=jum + (m*right_pixel) + (image_length);  //right_pixel = 1024, image_length = 5120  */
							jum=(j+1)*image_length + (m*right_pixel);  /* right_pixel = 1024, image_length = 5120   */
							k= jum;
				}
			}
			k = (m+1)*right_pixel ;
/*
//			status  = MdsPut(channelTagName[m], "BUILD_SIGNAL($1,,)", &dataDesc, &null);  //Data Put OK 2008.01.28 
//			status  = MdsPut(channelTagName[m], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null);   //Data Put 1D and Time OK 2008.02.11
//			status  = MdsPut(channelTagName[m], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null);   //Data Put 2D and Time TEST ING
*/
			status  = MdsPut(channelTagName[m], "BUILD_SIGNAL($1,,BUILD_WITH_UNITS($2,'S')", &dataDesc, &timeDesc, &null); 
/*   MDSput The VSS Data Infomations about Image    */
			atu32_ret = ATSIF_GetSubImageInfo(ATSIF_Signal,
                                                  m,
                                                  &atu32_left,&atu32_bottom,
                                                  &atu32_right,&atu32_top,
                                                  &atu32_hBin,&atu32_vBin);
      if(atu32_ret != ATSIF_SUCCESS) {
           printf("Could not Get Sub Image Info. Error: %u\n", atu32_ret);
      }
			sprintf(buf, "FS_FLOAT(%d)", atu32_bottom);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[0]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_left);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[1]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_right);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[2]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_top);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[3]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_hBin);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[4]);
			status  = MdsPut(bufNode, buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", atu32_vBin);
			sprintf(bufNode, "\\HR_VSS0%d%s",m+1,channelNodeName2[5]);
			status  = MdsPut(bufNode, buf,&null);
			printf("check Node name for each channel VSS Name: %s\n",bufNode);
/*
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'wave'),BUILD_DIM(BUILD_WINDOW(0,2,10),BUILD_SLOPE($3)))", &dataDesc, &data, &timeDesc, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'wave'),BUILD_WITH_UNITS($3,'sec'))", &dataDesc, &data, &timeDesc, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'wave'),BUILD_WITH_UNITS($3,'sec'))", &dataDesc, &data2, &timeDesc, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'wave'),BUILD_WITH_UNITS($3,'sec'))", &dataDesc, 1024, &timeDesc, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'Sec'),BUILD_WITH_UNITS($3,'wave'))", &dataDesc, &timeDesc, &data2, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'Sec'),BUILD_WITH_UNITS($3,'wave'))", &dataDesc, &timeDesc, &right_pixel, &null);
//			status = MdsPut(channelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($1,'Counts'),BUILD_WITH_UNITS($2,'Sec'),BUILD_WITH_UNITS($3,'wave'))", &dataDesc, &timeDesc, &right_pixel, &null);
//			Image Test Put    mdsput,"\TOP.X_IMAGE:FAST_CCD1","BUILD_SIGNAL($,*,MAKE_DIM(MAKE_WINDOW(0,425,0.0),MAKE_SLOPE(0.001)),MAKE_DIM(MAKE_WINDOW(0,401,0.0),MAKE_SLOPE(0.001)),MAKE_DIM(MAKE_WINDOW(0,$,0.0),MAKE_SLOPE(MAKE_WITH_UNITS($,'s'))))",jpgimage,filecount_1,dtime

//			fprintf(stdout, "MDSplus Put the Data channelTagName2 : %s int Size : %f \n", channelTagName[m],data2);
//			free(dataArray);
*/
		}
	}
	else 
	{
		fprintf(stderr, "Error mode is not waveLength and Pixel number \n");
	}
	free(data);
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

atu32_ret = ATSIF_CloseFile();
      if(atu32_ret != ATSIF_SUCCESS) {
           printf("Could not get Property Value.\n");
      }

#endif

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
	char speChannelNodeName2[6][22] = {":POS_BOTTOM", ":POS_LEFT", ":POS_RIGHT", ":POS_TOP", ":POS_HOR", ":POS_VER"};
	char speChannelWaveName[5][21] = {"\\SURVEY_WAVE:FOO", "\\SURVEY_WAVE:X_CAL0", "\\SURVEY_WAVE:X_CAL1", "\\SURVEY_WAVE:X_CAL2", "\\SURVEY_WAVE:X_CAL3"};

	float x_cal0_spe;
	float x_cal1_spe;
	float x_cal2_spe;
	float x_cal3_spe;

	int imageNumber;                          /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
	int number_subImage;                   /* image count, subImage = 5  */
	int right_pixel;                         /* pixel number : 1024   int => unsigned long int  */
	int	image_length;                  /* 5120  = SubImage * 1024(frameSize) */

	unsigned short image_top;               /* Image Top ex.255  */
	unsigned short image_bottom;         /*Image Bottom ex.1  */
	unsigned int total_length;

	float cycle_time_spe;   /* same value delay & kinetic_cycle & integration cycle time */
	float exposure_time;    /* exposure time ex.0.01700  */
	/*	double integration_cycle;  Integration cycle time ex.0.21200  */
	char buf_spe[100];
	char bufNode_spe[100];
	int null = 0;
	int status;
	int socket;

	int  i;
	int j, m;
	unsigned long int k, jum;
	int dtype_float = DTYPE_FLOAT;
	int dtype_ushort = DTYPE_WU;
	int dtype_double = DTYPE_DOUBLE;
/*float timeCount=0.;   imageNumber * right_pixel  */
	unsigned int timeCount;   /* imageNumber * right_pixel  */
	int dtypeLong = DTYPE_LONG;

	float *timeArray;  /* number_image = imageNumber = Image.no_images  */
	/*	float *dataArraySpe;  I just test - int *dataArraySpe; */
	unsigned short *dataArraySpe;
	unsigned short *image_buffer1;

	double *data;

	int  tstat;
	int  dataSpeDesc;
	int  timeSpeDesc;
	int  wavedataDesc;
	int  idesc = descr(&dtypeLong, &tstat, &null); 
	int kkk;

	int statFile;

	/* start SPE File Read and Write Function */
	FILE* speFile;

	/* Local Temp variable */ 
	float delayTime, exp_Sec, clkSpd_us, shutterComp;
	unsigned long numExpRepeats, numExpAccums;
	char date[DATEMAX];

	unsigned short xDimDet,yDimDet, xDim, yDim;

	float ReadoutTime;
	long numFrames;
	unsigned short clockSpeed;
	short dataType;
	double polynom_coeff[6];

#if 0
	short chipYdim;
	short chipXdim;
	long lnoscan;
	long lavgexp;
	short thrMinLive, thrMaxLive;
	float thrMinVal, thrMaxVal;
	char ylable[LABELMAX];
	float centerWavelength, startWavelength, endWavelength;
	float pulseWidth, pulseDelay, pulseStartWidth, pulseEndWidth, pulseStartDelay, pulseEndDelay;
	float minOverlap, finalResol;
	unsigned short readoutMode;
	short speAutoMode, speGlueFlag;
	double pixel_posWlNm[10], calib_valWlNm[10];
	short numROI;
	unsigned short rawDataTest;
	int sizeFile;
#endif 

	struct ROIinfo
	{                                                    
		unsigned short  startx;                                         
		unsigned short  endx;                                           
		unsigned short  groupx;                           
		unsigned short  starty;                           
		unsigned short  endy;                             
		unsigned short  groupy;                           
	} ROIinfoblk[ROIMAX];                     



	/* Local Temp variable */ 
	speFile = fopen(fileNamePath ,"rb");
	printf(" OK 2 %s \n", fileNamePath);
	if( speFile == NULL) {
		fprintf(stderr, "can't oepn data file : %s\n", fileNamePath);
		return(0);
	}


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
	printf(" OK ? %d, Number of Times experiment repeated : %lu \n",statFile, numExpRepeats);

	statFile=fseek(speFile, PI_NumExpAccums, SEEK_SET);          /* Move to Read */
	statFile = fread(&numExpAccums, sizeof(unsigned long),1,speFile);    /* Read data from SPE file */
	printf(" OK ? %d, Number of Time experiment accumulated : %lu \n",statFile, numExpAccums);

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

	right_pixel = xDim;                         /* pixel number : 1024   int => unsigned long int  */

	statFile =fseek(speFile, PI_ydim, SEEK_SET); /* ####  We Need to store about devided ccd Y dimension ###### */  
	statFile = fread(&yDim, sizeof(unsigned short),DATEMAX,speFile);
	printf(" OK ? %d, y dimension of raw data  %hu \n",statFile, yDim);

	number_subImage = yDim;                     /* image count, subImage = 5  */

	statFile=fseek(speFile, PI_NumFrames, SEEK_SET);  /* ####  We Need to store about number of frames ###### */  
	statFile = fread(&numFrames, sizeof(long),1,speFile);
	printf(" OK ? %d, number of frames in file. : %ld \n",statFile, numFrames);
	imageNumber = (int)numFrames;                    /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */

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

	if (cycle_time_spe ==0) {
		cycle_time_spe =(float)0.1; 
	}
	exposure_time = exp_Sec;    /* exposure time ex.0.01700  */

	/* integration_cycle = at32_integCycleTime;  We do not use this function . Integration cycle time ex.0.21200  */


	image_top = ROIinfoblk[0].endy;              /* Image Top ex.255  */
	image_bottom = ROIinfoblk[0].starty;         /*Image Bottom ex.1  */
	image_length = number_subImage * right_pixel;   /*    Input Hard write.           5120  = SubImage(5) * 1024(frameSize) for step. */

	total_length = imageNumber * number_subImage * right_pixel;  /* numFrames is 76 frames per 1 ydim. xDim is X pixel 1024. yDim is Y dim divided counts  */
	timeCount = (unsigned int)imageNumber*right_pixel;
	total_length =(unsigned int)imageNumber * right_pixel * number_subImage; 
	/*  dataArraySpe = (float*)malloc(4*(size_t)timeCount); I just Test int point type */
	dataArraySpe = (unsigned short*)malloc(2*(size_t)timeCount);
	image_buffer1 = (unsigned short*)malloc(2*(size_t)total_length);
	memset(image_buffer1,0,2*(size_t)total_length);

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
	} else printf("Data Type Error. You must check data type \n");

#if 0 
	for (i=0; i<total_length; i++){
		if(image_buffer1[i]<=100){
			printf("%d - %hu, ", i, image_buffer1[i]);
		}
	}
#endif

	printf("total_length : %u \n",total_length);
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
	/*	dataArraySpe = (float*)malloc(4*(size_t)timeCount);  1D plot Data   */

	data = (double *)malloc(sizeof(double)*right_pixel);   /* waveLength and Pixel Number  */
	printf("SPE MDSplus Data Put Part 1. storeMode :%d \n",storeMode);
	if(storeMode == 1){
		socket = MdsConnect(MDSIP);
		if ( socket == -1)
		{
			fprintf(stderr,"Error connecting to Server.\n");
			free(data);
			free(image_buffer1);
			free(dataArraySpe);
			free(timeArray);
			return 0;
		}
		fprintf(stdout, "\nMdsplus >>> MdsConnect(\"%s\")...OK\n",MDSIP);
	}

	status = MdsOpen(TREE, &shotNumber);
	if (!status_ok(status))
	{
		fprintf(stderr,"Error openning tree for shot %d \n",shotNumber);
		free(data);
		free(image_buffer1);
		free(dataArraySpe);
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

	wavedataDesc = descr(&dtype_double, data, &right_pixel, &null);

	/* 
	   x_cal0 = 653.810000;
	   x_cal1 = 0.004841;
	   x_cal2 = -0.000000;
	   x_cal3 = -0.000000;
	 */

	x_cal0_spe = (double)polynom_coeff[0];
	x_cal1_spe = (double)polynom_coeff[1];
	x_cal2_spe = (double)polynom_coeff[2];
	x_cal3_spe = (double)polynom_coeff[3];


	for (i=0; i<right_pixel; i++)
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
	timeSpeDesc = descr(&dtype_float, timeArray, &imageNumber, &null); 
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
	dataDesc = descr(&dtype_float, dataArraySpe, &timeCount, &null); //descr(TYPE, Data1D, 1Dcount,0)
	 */

	kkk=1;
#if 0
	dataSpeDesc = descr(&dtype_float, dataArraySpe, &right_pixel, &imageNumber, &kkk, &null);  /* Error data type defference we should do ushort type - descr(TYPE, Data3D, nx,ny,nz,0) ?? */
	dataSpeDesc = descr(&dtype_float, dataArraySpe, &timeCount, &null);  /* Just Test - descr(TYPE, Data3D, nx,ny,nz,0) ?? Put OK. But, I dont need. */
	dataSpeDesc = descr(&dtype_float, dataArraySpe, &timeCount, &null);  /* just Test. 1D data Put success... But I don't read data.*/
	dataSpeDesc = descr(&dtype_float, dataArraySpe, &right_pixel, &imageNumber, &kkk, &null);  /* just Test. 2D -  */
	dataSpeDesc = descr(&dtype_float, dataArraySpe, &kkk, &null);  /* Just Test - descr(TYPE, Data3D, nx,ny,nz,0) ?? Put OK. But, I dont need. */
	dataSpeDesc = descr(&dtype_ushort, image_buffer1, &right_pixel, &imageNumber, &kkk, &null);  /* I will try store just raw read data array */
	dataSpeDesc = descr(&dtype_ushort, dataArraySpe, &right_pixel, &imageNumber, &kkk, &null);  /* OK but data stored reverse. We used this desc - descr(TYPE, Data3D, nx,ny,nz,0) ?? */
	dataSpeDesc = descr(&dtype_ushort, dataArraySpe, &right_pixel, &imageNumber, &kkk, &null);  /* OK but data stored reverse. We used this desc - descr(TYPE, Data3D, nx,ny,nz,0) ?? */
#endif
	dataSpeDesc = descr(&dtype_ushort, dataArraySpe, &right_pixel, &imageNumber, &kkk, &null);  /* OK but data stored reverse. We used this desc - descr(TYPE, Data3D, nx,ny,nz,0) ?? */


	printf("number_subImage : %d, imageNumber : %d, right_pixel :%d , image_length :%d \n",number_subImage,imageNumber,right_pixel,image_length );
	for (m=0,k=0; m<number_subImage; m++) 
	{
#if 1 
		for (jum=0,j=0; j<imageNumber; j++)
		{
			for (i=0; i<right_pixel; i++, k++)
			{	
				dataArraySpe[j*right_pixel + i] = image_buffer1[k];
			}
			/* printf("dataArraySpe Put : %f,   image_buffer Data : %hu, k = %d\n", dataArraySpe[j*right_pixel + i],image_buffer1[k],k);  */
			/*	printf("dataArraySpe Put : %f,   image_buffer Data : %hu, k = %uli \n", dataArraySpe[j*right_pixel + i],image_buffer1[k],k);  */
			if(imageNumber>1)
			{
				jum=(j+1)*image_length + (m*right_pixel);  /* right_pixel = 1024, image_length = 5120 */
				k= jum;
			}
		}
		k = (m+1)*right_pixel ;
#endif
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);

#if 1
			printf("##################################################### 6.5  %s, %d, %d \n",speChannelTagName[m], right_pixel, imageNumber);
			printf("##################################################### 6.9  timeCount : %f, image_length : %d, number_subImage:%d\n",timeCount,image_length,number_subImage);
			/*		status  = MdsPut(speChannelTagName[m], "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"Wavelength\"),,)", &dataSpeDesc, &null);    Data Put 2D and Time TEST ING */
			/*      status  = MdsPut(speChannelTagName[m], "BUILD_SIGNAL($,,)", &dataSpeDesc, &null);   */
			status  = MdsPut(speChannelTagName[m], "BUILD_SIGNAL($1,,$2)", &dataSpeDesc, &timeSpeDesc, &null); /*   Data Put 2D and Time TEST ING */


#endif
			/*   MDSput The VSS Data Infomations about Image    */
			if (!status_ok(status))
			{
				fprintf(stderr,"Error Data Put shot: %d. m : %d. \n",shotNumber, m);
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
				printf("Complate the Survey information write \n");
			}
		}


		free(data);
		free(image_buffer1);
		free(timeArray);
		free(dataArraySpe);

		/*  MDSplus Close Tree  */
		status = MdsClose(TREE, &shotNumber);
		if (!status_ok(status))
		{
			fprintf(stderr,"Error closing tree for shot %d \n",shotNumber);
			return -1;
		}
		fprintf(stdout, "\nMdsplus >>> MdsClose(\"%d\")...OK\n",shotNumber);


#endif
		fclose(speFile);

		return 1;
}


