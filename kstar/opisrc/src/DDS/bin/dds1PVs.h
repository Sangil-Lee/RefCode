#ifndef __DDS1_PV_NAME_LIST_H
#define __DDS1_PV_NAME_LIST_H

//#include "epicsFunc.h"
//#include <alarmString.h>
#include <envDefs.h>
#include "global.h"




#define str_DDS1A_ARM_ENABLE  "DDS1A_SYS_ARMING"
#define str_DDS1A_RUN_START  "DDS1A_SYS_RUN"
#define str_DDS1A_STATUS  "DDS1A_STATUS"
#define str_DDS1A_ERROR  "DDS1A_ERROR"
#define str_DDS1A_RESET  "DDS1A_RESET"


#define str_DDS1B_ARM_ENABLE  "DDS1B_SYS_ARMING"
#define str_DDS1B_RUN_START  "DDS1B_SYS_RUN"
#define str_DDS1B_STATUS  "DDS1B_STATUS"
#define str_DDS1B_ERROR  "DDS1B_ERROR"
#define str_DDS1B_RESET  "DDS1B_RESET"



chid DDS1A_ARM_ENABLE;
chid DDS1A_RUN_START;
chid DDS1A_STATUS;
chid DDS1A_ERROR;
chid DDS1A_RESET;


chid DDS1B_ARM_ENABLE;
chid DDS1B_RUN_START;
chid DDS1B_STATUS;
chid DDS1B_ERROR;
chid DDS1B_RESET;




#define str_DDS1A_OpMode		"DDS1A_OP_MODE"
#define str_DDS1A_shotNumber		"DDS1A_SHOT_NUMBER"
#define str_DDS1A_CREATE_LOCAL_SHOT	"DDS1A_CREATE_LOCAL_TREE"

#define str_DDS1B_OpMode		"DDS1B_OP_MODE"
#define str_DDS1B_shotNumber		"DDS1B_SHOT_NUMBER"
#define str_DDS1B_CREATE_LOCAL_SHOT	"DDS1B_CREATE_LOCAL_TREE"


chid DDS1A_OpMode;
chid DDS1A_shotNumber;
chid DDS1A_CREATE_LOCAL_SHOT;

chid DDS1B_OpMode;
chid DDS1B_shotNumber;
chid DDS1B_CREATE_LOCAL_SHOT;






// #define str_DDS1A_TO_CCS_NOTIFY_READY	 "DDS1A_TO_CCS_NOTIFY_READY"	// 2009. 10 .20 ... modify again
#define str_CCS_SHOTSEQ_START    "CCS_SHOTSEQ_START" // 2009. 10. 13.   

//chid DDS1A_TO_CCS_NOTIFY_READY;		// 2009. 09 .04 ... woong.
//chid DDS1A_SOFT_SHOTSEQ_STARTED;		// 2009. 09 .04 ... woong.



char str_DDS1_chRbitMask[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_chMbitMask[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_chLbitMask[USED_ACQ196][MAX_PV_NAME_SIZE];

char str_DDS1_setInternal[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_setExternal[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_sampleRate[USED_ACQ196][MAX_PV_NAME_SIZE];


char str_DDS1_setT0[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_setT1[USED_ACQ196][MAX_PV_NAME_SIZE];

//char str_DDS1_getState[USED_ACQ196][MAX_PV_NAME_SIZE];
char str_DDS1_clearStatus[USED_ACQ196][MAX_PV_NAME_SIZE];

//char str_DDS1A_SOFT_BX_INTNAL_DAQ[USED_ACQ196][MAX_PV_NAME_SIZE];

//char str_DDS1_BX_PARSING_CNT[USED_ACQ196][MAX_PV_NAME_SIZE];
//char str_DDS1_BX_MDS_SND_CNT[USED_ACQ196][MAX_PV_NAME_SIZE];

//char str_DDS1A_BX_REMOTE_READY[USED_ACQ196][MAX_PV_NAME_SIZE];		// 2009. 08. 04
//char str_DDS1_BX_DAQING_CNT[USED_ACQ196][MAX_PV_NAME_SIZE];		// 2009. 08. 14

char str_DDS1_BX_DIVIDER[USED_ACQ196][MAX_PV_NAME_SIZE];		// 2009. 09. 01
char str_DDS1_BX_CARD_MODE[USED_ACQ196][MAX_PV_NAME_SIZE];		// 2009. 09. 01




chid DDS1A_tag[USED_ACQ196][96];

chid DDS1_chRbitMask[USED_ACQ196];
chid DDS1_chMbitMask[USED_ACQ196];
chid DDS1_chLbitMask[USED_ACQ196];

chid DDS1_setInternal[USED_ACQ196];
chid DDS1_setExternal[USED_ACQ196];
chid DDS1_sampleRate[USED_ACQ196];

chid DDS1_setT0[USED_ACQ196];
chid DDS1_setT1[USED_ACQ196];

chid DDS1_clearStatus[USED_ACQ196];


chid DDS1_BX_DIVIDER[USED_ACQ196];				// 2009. 09. 01
chid DDS1_BX_CARD_MODE[USED_ACQ196];		// 2009. 09. 01




/**********************************************/
/* LTU         DDS1A          LTU         DDS1A         LTU */
/*
#define DDS1A_LTU_ShotTerm		"DDS1A_LTU_ShotTerm"

#define DDS1A_LTU_Trig_p0		"DDS1A_LTU_Trig_p0"
#define DDS1A_LTU_Clock_p0	"DDS1A_LTU_Clock_p0"
#define DDS1A_LTU_T0_p0		"DDS1A_LTU_T0_p0"
#define DDS1A_LTU_T1_p0		"DDS1A_LTU_T1_p0"
#define DDS1A_LTU_Trig_p1		"DDS1A_LTU_Trig_p1"
#define DDS1A_LTU_Clock_p1	"DDS1A_LTU_Clock_p1"
#define DDS1A_LTU_T0_p1		"DDS1A_LTU_T0_p1"
#define DDS1A_LTU_T1_p1		"DDS1A_LTU_T1_p1"
#define DDS1A_LTU_Trig_p2		"DDS1A_LTU_Trig_p2"
#define DDS1A_LTU_Clock_p2	"DDS1A_LTU_Clock_p2"
#define DDS1A_LTU_T0_p2		"DDS1A_LTU_T0_p2"
#define DDS1A_LTU_T1_p2		"DDS1A_LTU_T1_p2"
#define DDS1A_LTU_Trig_p3		"DDS1A_LTU_Trig_p3"
#define DDS1A_LTU_Clock_p3	"DDS1A_LTU_Clock_p3"
#define DDS1A_LTU_T0_p3		"DDS1A_LTU_T0_p3"
#define DDS1A_LTU_T1_p3		"DDS1A_LTU_T1_p3"

#define DDS1A_LTU_setup_p0		"DDS1A_LTU_setup_p0"
#define DDS1A_LTU_setup_p1		"DDS1A_LTU_setup_p1"
#define DDS1A_LTU_setup_p2		"DDS1A_LTU_setup_p2"
#define DDS1A_LTU_setup_p3		"DDS1A_LTU_setup_p3"
#define DDS1A_LTU_setup_p4		"DDS1A_LTU_setup_p4"

#define DDS1A_LTU_mTrig		"DDS1A_LTU_mTrig"
#define DDS1A_LTU_mClock_sec0	"DDS1A_LTU_mClock_sec0"
#define DDS1A_LTU_mT0_sec0		"DDS1A_LTU_mT0_sec0"
#define DDS1A_LTU_mT1_sec0		"DDS1A_LTU_mT1_sec0"
#define DDS1A_LTU_mClock_sec1	"DDS1A_LTU_mClock_sec1"
#define DDS1A_LTU_mT0_sec1		"DDS1A_LTU_mT0_sec1"
#define DDS1A_LTU_mT1_sec1		"DDS1A_LTU_mT1_sec1"
#define DDS1A_LTU_mClock_sec2	"DDS1A_LTU_mClock_sec2"
#define DDS1A_LTU_mT0_sec2		"DDS1A_LTU_mT0_sec2"
#define DDS1A_LTU_mT1_sec2		"DDS1A_LTU_mT1_sec2"
*/



#endif


