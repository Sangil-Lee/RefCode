#ifndef __DDS2_PV_NAME_LIST_H
#define __DDS2_PV_NAME_LIST_H

#include "epicsFunc.h"
//#include <alarmString.h>
#include <envDefs.h>


#define str_DDS2_setOpMode		"DDS2_setOpMode"

#define str_DDS2_ADCstart		"DDS2_ADCstart"
#define str_DDS2_DAQstart		"DDS2_DAQstart"
#define str_DDS2_DAQstop		"DDS2_DAQstop"
#define str_DDS2_ADCstop		"DDS2_ADCstop"
#define str_DDS2_LocalSave		"DDS2_LocalSave"
#define str_DDS2_RemoteSave		"DDS2_RemoteSave"

#define str_DDS2_master_chBitMask	"DDS2_master_chBitMask"
#define str_DDS2_Dev2_chBitMask		"DDS2_Dev2_chBitMask"
/* #define str_DDS2_Dev3_chBitMask		"DDS2_Dev3_chBitMask"   2010.10.12 Remove  */
/* #define str_DDS2_Dev4_chBitMask		"DDS2_Dev4_chBitMask"	 2009. 08. 18 add one more ADC  */



#define str_DDS2_setInternal	"DDS2_setInternal"
#define str_DDS2_setExternal	"DDS2_setExternal"
#define str_DDS2_sampleRate		"DDS2_sampleRate"
#define str_DDS2_shotNumber		"DDS2_shotNumber"

#define str_DDS2_setT0			"DDS2_setT0"
#define str_DDS2_setT1			"DDS2_setT1"
/* TG.Lee Delete 2008.12.05
#define str_DDS2_SOFT_DAQ_START_INTN	"DDS2_SOFT_DAQ_START_INTN"
#define str_DDS2_SOFT_DAQ_START_DISA	    "DDS2_SOFT_DAQ_START.DISA"
#define str_DDS2_SOFT_DAQ_START_INTN_DISA	"DDS2_SOFT_DAQ_START_INTN.DISA"
*/
/* Add OPI Edit PV - SNL Program Create Time  20081107 . Edit by TG.Lee 
#define str_DDS2_SOFT_DAQ_START 	"DDS2_SOFT_DAQ_START"
#define str_DDS2_SOFT_REMOTE_READY 	"DDS2_SOFT_REMOTE_READY"   */
#define str_DDS2_REMOTE_READY_BTN 	"DDS2_REMOTE_READY_BTN"
#define str_DDS2_LOCAL_READY_BTN 	"DDS2_LOCAL_READY_BTN"
#define str_DDS2_REMOTE_STORE_BTN	"DDS2_REMOTE_STORE_BTN"
#define str_DDS2_DAQ_ABORT_BTN  	"DDS2_DAQ_ABORT_BTN"


#define str_DDS2_getState	"DDS2_getState"  /* local Varialbe is in DDS2 class */
#define str_DDS2_DAQ_STATUS	"DDS2_DAQ_STATUS"  /* local Varialbe is in DDS2 class */

#define str_DDS2_setTrigInternal			"DDS2_setTrigInternal"
#define str_DDS2_setTrigExternal			"DDS2_setTrigExternal"


chid chid_DDS2_setOpMode;

chid chid_DDS2_ADCstart;
chid chid_DDS2_DAQstart;
chid chid_DDS2_DAQstop;
chid chid_DDS2_ADCstop;
chid chid_DDS2_LocalSave;
chid chid_DDS2_RemoteSave;

chid chid_DDS2_tag[96];			// 2009. 08. 18 add one more ADC
/* chid chid_DDS2_tag[128];			 2009. 08. 18 add one more ADC  */
chid chid_DDS2_master_chBitMask;
chid chid_DDS2_Dev2_chBitMask;
/* chid chid_DDS2_Dev3_chBitMask;      remove 2010.10.12 */
/* chid chid_DDS2_Dev4_chBitMask;		 2009. 08. 18 add one more ADC  */


chid chid_DDS2_setClkInternal;
chid chid_DDS2_setClkExternal;
// chid chid_DDS2_sampleRate;		// move into Class   2009. 8. 25
chid chid_DDS2_shotNumber;

chid chid_DDS2_setT0;
chid chid_DDS2_setT1;

chid chid_DDS2_SOFT_DAQ_START_INTN;
chid chid_DDS2_SOFT_DAQ_START_DISA;
chid chid_DDS2_SOFT_DAQ_START_INTN_DISA;

/* Add OPI Edit PV - SNL Program Create Time  20081107 . Edit by TG.Lee */
chid chid_DDS2_SOFT_DAQ_START;
chid chid_DDS2_SOFT_REMOTE_READY;
chid chid_DDS2_REMOTE_READY_BTN;
chid chid_DDS2_LOCAL_READY_BTN;
chid chid_DDS2_REMOTE_STORE_BTN;
chid chid_DDS2_DAQ_ABORT_BTN;

chid chid_DDS2_DAQ_STATUS;

chid chid_DDS2_setTrigInternal;
chid chid_DDS2_setTrigExternal;


/**********************************************/
/* LTU         DDS2          LTU         DDS2          LTU */

#define str_DDS2_LTU_ShotTerm		"DDS2_LTU_ShotTerm"

#define str_DDS2_LTU_Trig_p0		"DDS2_LTU_Trig_p0"
#define str_DDS2_LTU_Clock_p0	"DDS2_LTU_Clock_p0"
#define str_DDS2_LTU_T0_p0		"DDS2_LTU_T0_p0"
#define str_DDS2_LTU_T1_p0		"DDS2_LTU_T1_p0"
#define str_DDS2_LTU_Trig_p1		"DDS2_LTU_Trig_p1"
#define str_DDS2_LTU_Clock_p1	"DDS2_LTU_Clock_p1"
#define str_DDS2_LTU_T0_p1		"DDS2_LTU_T0_p1"
#define str_DDS2_LTU_T1_p1		"DDS2_LTU_T1_p1"
#define str_DDS2_LTU_Trig_p2		"DDS2_LTU_Trig_p2"
#define str_DDS2_LTU_Clock_p2	"DDS2_LTU_Clock_p2"
#define str_DDS2_LTU_T0_p2		"DDS2_LTU_T0_p2"
#define str_DDS2_LTU_T1_p2		"DDS2_LTU_T1_p2"
#define str_DDS2_LTU_Trig_p3		"DDS2_LTU_Trig_p3"
#define str_DDS2_LTU_Clock_p3	"DDS2_LTU_Clock_p3"
#define str_DDS2_LTU_T0_p3		"DDS2_LTU_T0_p3"
#define str_DDS2_LTU_T1_p3		"DDS2_LTU_T1_p3"

#define str_DDS2_LTU_setup_p0		"DDS2_LTU_setup_p0"
#define str_DDS2_LTU_setup_p1		"DDS2_LTU_setup_p1"
#define str_DDS2_LTU_setup_p2		"DDS2_LTU_setup_p2"
#define str_DDS2_LTU_setup_p3		"DDS2_LTU_setup_p3"
/* #define str_DDS2_LTU_setup_p4		"DDS2_LTU_setup_p4"  */

#define str_DDS2_LTU_mTrig		"DDS2_LTU_mTrig"
#define str_DDS2_LTU_mClock_sec0	"DDS2_LTU_mClock_sec0"
#define str_DDS2_LTU_mT0_sec0		"DDS2_LTU_mT0_sec0"
#define str_DDS2_LTU_mT1_sec0		"DDS2_LTU_mT1_sec0"
#define str_DDS2_LTU_mClock_sec1	"DDS2_LTU_mClock_sec1"
#define str_DDS2_LTU_mT0_sec1		"DDS2_LTU_mT0_sec1"
#define str_DDS2_LTU_mT1_sec1		"DDS2_LTU_mT1_sec1"
#define str_DDS2_LTU_mClock_sec2	"DDS2_LTU_mClock_sec2"
#define str_DDS2_LTU_mT0_sec2		"DDS2_LTU_mT0_sec2"
#define str_DDS2_LTU_mT1_sec2		"DDS2_LTU_mT1_sec2"







#endif
