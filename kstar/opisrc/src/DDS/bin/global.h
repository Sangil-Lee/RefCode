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


#include "epicsFunc.h"
#include <envDefs.h>



/*  operation mode */
#define SYS_MODE_INIT           0
#define SYS_MODE_REMOTE     1
#define SYS_MODE_LOCAL        2
#define SYS_MODE_CALIB          3

#define SYS_CLOCK_INTERNAL    1
#define SYS_CLOCK_EXTERNAL    2



#define STR_PASSWD_MD		""
#define STR_PASSWD_MMW		""
#define STR_PASSWD_ELECTRON	""
#define STR_PASSWD_ION	""
#define STR_PASSWD_IMAGE	""
#define STR_PASSWD_SPECTRO	""
#define STR_PASSWD_SHUTTER		""
#define STR_PASSWD_SU		"woong"
#define STR_PASSWD_POWER		""
#define STR_PASSWD_IRTV		""


#define MAX_EVENT_PV		500


#define DDS1_LTU_TAIL_UNDER_2K		(100.0)  // (65.0)
#define DDS1_LTU_TAIL_UNDER_5K		(60.0)  // (35.0)
#define DDS1_LTU_TAIL_UNDER_10K		(50.0)  // (15.0)
#define DDS1_LTU_TAIL_DEFAULT		(20.0)  // (10.0)


#define DDS2_DAQ_TIME_TAIL (4.0)


/////////////////////////////////   DDS1
#define USED_ACQ196	6
#define USE_MASTER_SLAVE     0
#define USE_SYNC_SLAVE_TO_MASTER   0   /* all board has same parameter with master acq196 board */
#define DDS1A_USED_CNT   4
#define B1			0
#define B2			1
#define B3			2
#define B4			3
#define B5			4
#define B6			5
#define B7			6
#define B8			7






#define ACQ196_RIGHT		0
#define ACQ196_MIDDLE		1
#define ACQ196_LEFT			2

#define CARD_MODE_SINGLE		1
#define CARD_MODE_MASTER		2
#define CARD_MODE_SLAVE		3
/////////////////////////////////   ACQ196


#define MAX_PV_NAME_SIZE	40

#define STR_SELECT			"Selected"
#define STR_NOT_SELECT		"not selected"



#define TOOLBOX_ID_MD			0
#define TOOLBOX_ID_MMW		1
#define TOOLBOX_ID_ELECTRON	2
#define TOOLBOX_ID_ION			3
#define TOOLBOX_ID_IMAGE		4
#define TOOLBOX_ID_SPECTRO		5
#define TOOLBOX_ID_SHUTTER		6



#define STACKED_MD_DDS1A			0
#define STACKED_MD_MIRNOV			1

#define STACKED_H_ALPHA			2
#define STACKED_VBS			3
#define STACKED_ELECTRON_MMWI       4
#define STACKED_ECE_HR			5


#define STACKED_ELECTRON_REFLECTO		6
#define STACKED_ELECTRON_PROBE		7
#define STACKED_ELECTRON_THOMSON		8

#define STACKED_ION_XIC_SPECTRO		9
#define STACKED_ION_CE_SPECTRO		10

#define STACKED_IMAGE_BOLOMETER		11
#define STACKED_IMAGE_SOFT_XR			12
#define STACKED_IMAGE_IRVB			13



#define STACKED_SPECTRO_VS_SPECT		14
#define STACKED_SPECTRO_FILTER		15
#define STACKED_SPECTRO_HARD_XR		16

#define STACKED_SHUTTER_SHUTTER		17
#define STACKED_POWER		18
#define STACKED_IRTV_S1		19
#define STACKED_MD_MIRNOV2			20
#define STACKED_FILD		21
#define STACKED_IRTV_D1		22


















#define CURR_M6802		1
#define CURR_ACQ196		2

#define MAX_ACQ196		8
/*
#define QUEUE_THREAD_DDS2		100
*/
/*
#define NUM_OF_SENSOR			8

#define SENSOR_ROGOWSKICOIL		0
#define SENSOR_FLUXLOOPS			1
#define SENSOR_MAGNETICPROBE		2
#define SENSOR_DIAMAGNETICLOOP		3
#define SENSOR_VESSELCURRENT		4

#define SENSOR_INTERFEROMETER		5
#define SENSOR_ECERADIOMETER		6
#define SENSOR_HAMONITOR			7
*/


//#define EPICS_TIMEOUT		(1.0)


//#define DDS1A_LTU	0
//#define DDS2_LTU	1

//#define DDS1A_DAQ_TIME_TAIL (4.0)
//#define DDS1A_DAQ_TIME_TAIL (10.0)


typedef struct {
	int frame_rate;
	int channel_mask;
	double t0;
	double t1;
	int nClockDivider;
	int channelBitMask[MAX_ACQ196][3];

} ST_BOARD_CONFIG;

typedef struct {
	int id;
	unsigned int status;
} ST_QUEUE_STATUS;


typedef struct {
	chid SHOT_TERM;
	chid TRIG[4];
	chid CLOCK[4];
	chid T0[4];
	chid T1[4];
	chid SETUP_Pt[5];
	chid mTRIG;
	chid mCLOCK[3];
	chid mT0[3];
	chid mT1[3];

}ST_ca_ctu;


/*
typedef struct {
	int frame_rate;
	int channel_mask;
	
} ST_ACQ196_CONFIG;
*/


#endif
