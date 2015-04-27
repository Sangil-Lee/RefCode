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

#pragma hdrstop

#include "ATSIFIO.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>

//---------------------------------------------------------------------------

void retrieveErrorCode(unsigned int _ui_ret, char * sz_error);
void retrievePropertyType(ATSIF_PropertyType _propType, char * sz_propertyType);
int getUserInput();


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
#define MDSIP       "127.0.0.1:8000"
#define status_ok(status)  (((status) & 1) == 1)

char channelTagName[5][15] = {"\\HR_VSS01:FOO", "\\HR_VSS02:FOO", "\\HR_VSS03:FOO", "\\HR_VSS04:FOO", "\\HR_VSS05:FOO"};
char channelNodeName1[7][22] = {"\\HR_INFO:IMAGE_NO", "\\HR_INFO:IMAGE_RIGHT", "\\HR_INFO:IMAGE_TOP", "\\HR_INFO:SUBIMAGE_NO", "\\HR_INFO:T_EXPOSURE", "\\HR_INFO:T_INTEGRATE", "\\HR_INFO:T_KINETIC"};
char channelNodeName2[6][22] = {"\\HR_VSS01:POS_BOTTOM", "\\HR_VSS02:POS_LEFT", "\\HR_VSS03:RIGHT", "\\HR_VSS04:TOP", "\\HR_VSS05:HOR", "\\HR_VSS05:VER"};
char channelWaveName[5][17] = {"\\HR_WAVE:FOO", "\\HR_WAVE:X_CAL0", "\\HR_WAVE:X_CAL1", "\\HR_WAVE:X_CAL2", "\\HR_WAVE:X_CAL3"};


/*****************************************************************************
//Function: mdsplus_put_data
//Inputs: shutNumber
//Outputs: none
//The purpose of this function is to write the mdsplus tree node 
*****************************************************************************/
int mdsplus_data_put(float *image_buffer1, char* modeWave, FILE* out, int shotNumber, int storeMode, float blipTime, float daqTime)

int mdsplus_data_put(char* fileNamePath, int shotNumber, int storeMode, float blipTime, float daqTime)
{
	/* Wavelength = x_cal(0) + x_cal(1).X + x_cal(2).X^2 + x_cal(3).X^3  */
	/* X is the pixel number on in left hand column.  double to float type changed 2009.05.15  */
/* I can not find function fot waveleangth cal parameter.
	double x_cal0 = CalibImage.x_cal[0]; 
	double x_cal1 = CalibImage.x_cal[1];
	double x_cal2 = CalibImage.x_cal[2];
	double x_cal3 = CalibImage.x_cal[3];
	*/

	/* time count number = total_length/image_length  */
	int total_length = Image.total_length;  /* 30720 */
	int	image_length = Image.image_length;  /* 5120  */
	int imageNumber = Image.no_images;  /* imageNumber,    image count at time 30720/5120 = 6  int => unsigned long int  */
	int number_subImage = Image.no_subimages; /* image count, subImage = 5  */
	int left_pixel = Image.image_format.left;  /* pixel number : 1  */
	int right_pixel = Image.image_format.right; /* pixel number : 1024   int => unsigned long int  */
	int image_top = Image.image_format.top;     /* Image Top ex.255  */
	int image_bottom = Image.image_format.bottom; /*Image Bottom ex.1  */
	float cycle_time = InstaImage.kinetic_cycle_time;   /* frame to frame time is 4.902 */
	double exposure_time = InstaImage.exposure_time;    /* exposure time ex.0.01700  */
	double integration_cycle = InstaImage.integration_cycle_time;  /* Integration cycle time ex.0.21200  */

	/*
	int shotnumber = ShotInfo.shotNumber;
	float startVSS = ShotInfo.daqStartTime;
	float blipTime = ShotInfo.blipTime;
  */
	
	
	char buf[50];
	int null = 0;
	int status;
	int socket;
	int j, i, m;
	unsigned long int k, jum;
	int dtype_float = DTYPE_FLOAT;
	int timeCount=0;   /* imageNumber * right_pixel  */
	int dtypeLong = DTYPE_LONG;
	
	float *timeArray;  /* number_image = imageNumber = Image.no_images  */
	float *dataArray;

	float *data;
	int  tstat, len;
	int  dataDesc;
	int  timeDesc;
	int  wavedataDesc;
	int  idesc = descr(&dtypeLong, &tstat, &null); 
	int kkk;

	fprintf(stdout, "x_cal0 : %f \n",x_cal0);
	fprintf(stdout, "x_cal1 : %f \n",x_cal1);
	fprintf(stdout, "x_cal2 : %f \n",x_cal2);
	fprintf(stdout, "x_cal3 : %f \n",x_cal3);
	fprintf(stdout, "total_length : %d \n",total_length);
	fprintf(stdout, "image_length : %d \n",image_length);
	fprintf(stdout, "number_image : %d \n",imageNumber);
	fprintf(stdout, "imageNumber : %d \n",imageNumber);
	fprintf(stdout, "number_subImage : %d \n",number_subImage);
	fprintf(stdout, "left_pixel : %d \n",left_pixel);
	fprintf(stdout, "right_pixel : %d \n",right_pixel);
	fprintf(stdout, "cycle_time : %f \n",cycle_time);
	fprintf(stdout, "exposure_time : %f \n",exposure_time);
	fprintf(stdout, "integration_cycle : %f \n",integration_cycle);




	
	printf("SIF MDSplus Data Put Part. \n");
	timeCount = imageNumber*right_pixel;
	
	timeArray = (float*)malloc((sizeof(float))*imageNumber*right_pixel);  /* 1D plot Data Time - OK  */
	dataArray = (float*)malloc(4*(size_t)timeCount);  /* 1D plot Data   */
	
 	data = (float *)malloc(sizeof(float)*right_pixel);   /* waveLength and Pixel Number  */
	
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
	wavedataDesc = descr(&dtype_float, data, &right_pixel, &null);

	if (strcmp(modeWave, MODEWAVE)==0)
	{
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);
		for (i=0; i<right_pixel-1; i++)
		{	
		data[i] = x_cal0 + x_cal1*(i+1) + x_cal2*((i+1)^2) + x_cal3*((i+1)^3);
		}
		fprintf(stdout,"Wave Data Cal 0 : %f \n", data[0]);
		fprintf(stdout,"Wave Data Cal 1 : %f \n", data[1]);
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
	else if (strcmp(modeWave,MODEPIX)==0)
	{
		fprintf(stdout, "DAQ PIXMode  OK : %s \n",MODEPIX);
		for (i=0; i<right_pixel-1; i++)
		{
		data[i] = i+1;
		}
	}
	else 
	{
		fprintf(stdout, "DAQ PIXMode  No Good  : %s \n",MODEPIX);
		for (i=0; i<right_pixel-1; i++)
		{
		data[i] = i+1;
		}
		fprintf(stderr, "Error mode is not waveLength and Pixel number \n");
	}



	fprintf(stdout, "CA_DAQ Start Time : %f  & BLIP Time : %f \n",daqTime, blipTime);
	timeDesc = descr(&dtype_float, timeArray, &imageNumber, &null); 
	for (i=0; i<imageNumber; i++)
	{
		timeArray[i] = (float)(daqTime-blipTime)+ i*cycle_time;
		fprintf(out,"int2: timeArray2 : %f,  i2 : %d\n",timeArray[i],i);
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

	if (strcmp(modeWave, MODEWAVE)==0)
	{
		fprintf(stdout, "DAQ WAVEMode OK : %s \n",MODEWAVE);

		for (m=0,k=0; m<number_subImage; m++)
		{
			for (jum=0,j=0; j<imageNumber; j++)
			{   
				for (i=0; i<right_pixel; i++, k++)
				{	
					dataArray[j*right_pixel + i] = image_buffer1[k];
					fprintf(out,"dataArray Put : %f,   image_buffer Data : %f, k = %d\n", dataArray[j*right_pixel + i],image_buffer1[k],k);
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
			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].bottom);
			status  = MdsPut(channelNodeName2[0], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].left);
			status  = MdsPut(channelNodeName2[1], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].right);
			status  = MdsPut(channelNodeName2[2], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].top);
			status  = MdsPut(channelNodeName2[3], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].horizontal_bin);
			status  = MdsPut(channelNodeName2[4], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].vertical_bin);
			status  = MdsPut(channelNodeName2[5], buf,&null);
		}
	}
	else if (strcmp(modeWave,MODEPIX)==0)
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

					fprintf(out,"dataArray Put : %f,   image_buffer Data : %f, k = %d\n", dataArray[j*right_pixel + i],image_buffer1[k],k);
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
			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].bottom);
			status  = MdsPut(channelNodeName2[0], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].left);
			status  = MdsPut(channelNodeName2[1], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].right);
			status  = MdsPut(channelNodeName2[2], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].top);
			status  = MdsPut(channelNodeName2[3], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].horizontal_bin);
			status  = MdsPut(channelNodeName2[4], buf,&null);

			sprintf(buf, "FS_FLOAT(%d)", Image.position[m].vertical_bin);
			status  = MdsPut(channelNodeName2[5], buf,&null);
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
 
	return -1;
}


