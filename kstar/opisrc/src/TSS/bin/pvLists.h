#ifndef __PV_LISTS_H
#define __PV_LISTS_H

#include "epicsFunc.h"
#include <envDefs.h>
//#include <alarmString.h>
#include "global.h"






typedef struct {
	ST_PV_NODE SHOT;
	
	ST_PV_NODE POL[4];
	ST_PV_NODE CLOCK[4];
	ST_PV_NODE T0[4];
	ST_PV_NODE T1[4];
	ST_PV_NODE SETUP_Pt[R2_PORT_CNT];
	ST_PV_NODE mTRIG;
	ST_PV_NODE mCLOCK[3];
	ST_PV_NODE mT0[3];
	ST_PV_NODE mT1[3];
/*****************************************/
	ST_PV_NODE SHOT_END;
	ST_PV_NODE SET_FREE_RUN;
//	ST_PV_NODE IRIG_B;
	ST_PV_NODE GET_COMP_NS;
	ST_PV_NODE SET_CAL_VALUE;
//	ST_PV_NODE SHOW_INFO;
	ST_PV_NODE SHOW_STATUS;
	ST_PV_NODE SHOW_TIME;
	ST_PV_NODE SHOW_TLK;
	ST_PV_NODE LOG_GSHOT;
	
	
	ST_PV_NODE R2_ActiveLow[R2_PORT_CNT];
	ST_PV_NODE R2_Enable[R2_PORT_CNT];
	ST_PV_NODE R2_Mode[R2_PORT_CNT];
	
	ST_PV_NODE R2_CLOCK[R2_PORT_CNT][R2_SECT_CNT_EXP];
	ST_PV_NODE R2_T0[R2_PORT_CNT][R2_SECT_CNT_EXP];
	ST_PV_NODE R2_T1[R2_PORT_CNT][R2_SECT_CNT_EXP];

}
ST_CLTU_PV;



/****************************/
/*  DDS system for DAQ stop */
/*
#define DDS1_DAQstop	"DDS1_DAQstop"
#define DDS1_ADCstop	"DDS1_ADCstop"
#define DDS1_RemoteSave	"DDS1_RemoteSave"

#define DDS1_sampleRate	"DDS1_sampleRate"
#define DDS1_setT0			"DDS1_B1_setT0"
#define DDS1_setT1			"DDS1_B1_setT1"
#define DDS1_REMOTE_READY		"DDS1_REMOTE_READY"
*/
typedef struct {
/*
	ST_PV_NODE DAQstop;
	ST_PV_NODE ADCstop;
	ST_PV_NODE RemoteSave;
*/
	ST_PV_NODE sampleRate;
	ST_PV_NODE setT0;
	ST_PV_NODE setT1;
	ST_PV_NODE divider;
/*
	ST_PV_NODE getReady;
*/
} ST_DDS_PV;
/*
typedef struct {
//	char DAQstop[MAX_PV_NAME_LEN];
//	char ADCstop[MAX_PV_NAME_LEN];
//	char RemoteSave[MAX_PV_NAME_LEN];
	char sampleRate[MAX_PV_NAME_LEN];
	char setT0[MAX_PV_NAME_LEN];
	char setT1[MAX_PV_NAME_LEN];
//	char getReady[MAX_PV_NAME_LEN];
} ST_DDS_PV_NAME;
*/





#endif


