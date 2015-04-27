#ifndef GLOBAL_H
#define GLOBAL_H


/*
... glibc detected *** : corrupted double-linked list: *** 

export MALLOC_CHECK_=1
0 : Do not generate an error message, and do not kill the program
1 : Generate an error message, but do not kill the program
2 : Do not generate an error message, but kill the program
3 : Generate an error message and kill the program

*/


//#include "cadef.h"

/*  WARNING!!!!	WARNING!!!!	WARNING!!!!	WARNING!!!! don't forget it......^.^
you must check below functions when you add extra LTU 
name and order is very important.!!!!
order used in return ack.

1. void MainWindow::assign_system_name()" 


 
*/
/*#define ATCA_LTU	0  *//* HPC */
#define BOX_LTU		0	
#define CES_LTU			1
#define CTUV1		2
#define DDS1A_LTU	3
#define DDS1B_LTU	4
#define ECCD1_LTU	5
#define ECCD1B_LTU	6
#define ECEHR_LTU	7
#define ECEI_LTU	8
#define ECH_LTU		9
#define ER_LTU		10
#define FILD_LTU	11
#define FUEL_LTU	12
#define FUEL2_LTU	13
#define HALPHA_LTU	14
#define HXR_LTU 	15
#define HXR2_LTU 	16
#define ICRH_LTU 	17
#define IRVB_LTU 	18
/*#define ITER_FUEL_LTU 	18 */
#define ITER_PCS_LTU	19
#define LHCD_LTU 	20
/*#define MC_LTU		21 */
#define MMWI_LTU	21
#define MPS1_LTU	22
#define MPS2_LTU	23
#define MPS3_LTU	24
#define NBI1A_LTU	25
#define NBI1B_LTU	26
#define NBI1C_LTU	27
#define NBI1D_LTU	28
//#define NBI1E_LTU	30   /* not yet installed, reserved */
#define NMS_LTU		29
#define PCS_LTU 	30
#define QDS_LTU 	31
#define RBA_LTU		32
#define RMP_LTU		33
#define SMBI_LTU	34
#define SXR_LTU		35
#define TS_LTU		36
#define TSS_CTU		37
#define VBS_LTU		38
#define VMS_LTU		39
#define VSC_LTU		40
#define XCS_LTU		41

#define MAX_LTU_CARD   42



#define CNT_R2_CARD   3 // not used.









/*  WARNING!!!!	WARNING!!!!	WARNING!!!!	WARNING!!!! */

#define str_CCS_BLIP_TIME	"CCS_BLIP_TIME"
#define str_CCS_SHOT_NUMBER  "CCS_SHOT_NUMBER"
#define str_CCS_SHOTSEQ_START  "CCS_SHOTSEQ_START"
#define str_CCS_TSS_CONFIRM	"CCS_TSS_CONFIRM"
#define str_TSS_SET_CAL_MODE	"TSS_CTU_RUN_CALIBRATION"



#define STR_PASSWD		"tss2010"
#define STR_PASSWD_CALIB		"tssmaster"

#define STR_FILEOPEN_DIR		"/home/kstar/tss_configure"


#define USE_LIMITED_OVER_1KHZ		0
#define USE_IMMEDIATE_LTU_SETUP		0

#define STACKED_ID_MAIN     0
//#define STACKED_ID_DB_MAN     1
#define STACKED_ID_TSS     1
#define STACKED_ID_R1     2
#define STACKED_ID_R2     3
#define STACKED_ID_MTRIG     4
#define STACKED_ID_SQL     5




#define DEFAULT_BLIP_TIME		16

#define LEVEL_NONE           0
#define LEVEL_ERROR         1
#define LEVEL_WARNING     2
#define LEVEL_NOTICE     3

#define LTU_SETUP_FLUSH_GAP		100  // (millisecond)

#define PV_STATUS_FIRST_EVENT		(0x1)
#define PV_STATUS_TOUCH			(0x1<<1)
#define PV_STATUS_DELETE			(0x1<<2)


#define R2_PORT_CNT    8
#define R2_SECT_CNT    8
#define R2_SECT_CNT_EXP    50


//////////////////////////////
#define FEAT_VER 	0
#define FEAT_CLASS 	1

#define VER1    0  // old CTU, LTU 
#define VER2    1  // new XTU 

#define C_MCH		0
#define C_DIAG		1



/////////////////////////////////   ACQ196
#define ONLY_TARGET_ACQ196_PORT   0	/*start with zero:  0, 1, 2, 3, 4 */
#define MAX_ACQ196		6
#define CNT_DDS1A_NO     4
#define CNT_DDS1B_NO     2
#define B1			0
#define B2			1
#define B3			2
#define B4			3


/////////////////////////////////   Model6802
#define ONLY_TARGET_M6802_PORT   0	/*start with zero:  0, 1, 2, 3, 4 */




#define CLTU_ASSERT()	printf("file:%s line:%d ASSERT!!\n",__FILE__,__LINE__)



#define ACTIVE_HIGH_VAL    0
#define ACTIVE_LOW_VAL     1



#define MODE_SINGLE		0
#define MODE_MULTI		1
#define MODE_NEW_R2		2


#define TYPE_SINGLE_TRIG	1
#define TYPE_SINGLE_CLOCK	2
#define TYPE_SINGLE_T0		3
#define TYPE_SINGLE_T1		4
#define TYPE_SINGLE_SETUP		5


#define TYPE_MULTI_TRIG		6
#define TYPE_MULTI_CLOCK	7
#define TYPE_MULTI_T0		8
#define TYPE_MULTI_T1		9
#define TYPE_MULTI_SETUP		10

#define TYPE_R2_ActiveLow	11
#define TYPE_R2_Enable	12
#define TYPE_R2_Mode	13
#define TYPE_R2_CLOCK	14
#define TYPE_R2_T0	15
#define TYPE_R2_T1	16
#define TYPE_R2_SETUP		17
#define TYPE_R2_GET_COMP_NS		18
#define TYPE_R2_SET_FREE_RUN		19
#define TYPE_SHOTSEQ_START		20






/*	ST_PV_NODE R2_ActiveLow[R2_PORT_CNT];
	ST_PV_NODE R2_Enable[R2_PORT_CNT];
	ST_PV_NODE R2_Mode[R2_PORT_CNT];
	
	ST_PV_NODE R2_CLOCK[R2_PORT_CNT][R2_SECT_CNT];
	ST_PV_NODE R2_T0[R2_PORT_CNT][R2_SECT_CNT];
	ST_PV_NODE R2_T1[R2_PORT_CNT][R2_SECT_CNT];
	*/
	




#define QUEUE_ID_MAINWND		1
#define QUEUE_ID_TIMESET		2
#define QUEUE_ID_SET_ZERO		3
#define QUEUE_ID_SETUP_ALL_LIST    4
#define QUEUE_ID_CALIBRATION		5





#define PORT_NUM_MULTI		4

#define USER_INSERT_ONE_LINE	1
#define USER_REMOVE_ONE_LINE	2
#define USER_FILE_OPEN		3
#define USER_NONE			4
#define USER_ITEM_CHANGED		5
#define USER_RESET_ALL_SYS    6
#define USER_FILE_NEW        7





#define WIDTH_V1_SYS  70
#define WIDTH_V1_DESC  110
#define WIDTH_V1_CH  35
#define WIDTH_V1_POL  65
#define WIDTH_V1_T0  80
#define WIDTH_V1_T1  80
#define WIDTH_V1_CLK  75


#define WIDTH_V2_SYS  70
#define WIDTH_V2_DESC  110
#define WIDTH_V2_CH  30
#define WIDTH_V2_ONOFF  30
#define WIDTH_V2_MODE  35
#define WIDTH_V2_POL  35
#define WIDTH_V2_T0  80
#define WIDTH_V2_T1  80
#define WIDTH_V2_CLK  75




typedef struct {
	void *pMain;
	unsigned int system;
	int mode;
	int type;
	int port;
	char pvname[60];
	int arg;
} ST_ARG_EVENT_CB;

typedef struct {
	int caller;
	double dval;	
	ST_ARG_EVENT_CB *pEventArg;
} ST_QUEUE_STATUS;


typedef struct {

//	int nMode;	// 0: single, 1: multi 
	char OnOff;
	char Mode;   // Clock: 0 , Trigger 1
	
	int nTrig[4];	// 0: rising, 1: falling

	double dbT0[R2_SECT_CNT];
	double dbT1[R2_SECT_CNT];
	int nClock[R2_SECT_CNT];	// unit: Hz
	

	int nmTrig;		// 0: rising, 1: falling
	double dbmT0[3];
	double dbmT1[3];
	int nmClock[3];		// unit: Hz

} ST_CLTU_config;


#endif


