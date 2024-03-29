#ifndef _NI_DEV_DEF_H_
#define _NI_DEV_DEF_H_



/*#define CFG_STD_FGEN_IMMEDIATELY	1
#define CFG_ARB_FGEN_IMMEDIATELY	1
*/

#define USE_AUTO_FETCH       1
#define USE_AUTO_TREE_PUT    0


#define USE_BINARY_WFM    0


#define SHOW_PHASE_INFO         0
#define SHOW_PHASE_NISCOPE   0
#define SHOW_PHASE_NIFGEN     0



#define CFG_NI_FGEN_IMMEDIATELY   1
#define CFG_NI_SCOPE_IMMEDIATELY  1

#define INIT_FGEN_TRIG_SRC    "PFI0"
#define INIT_SCOPE_TRIG_SRC   "TRIG"

/* something wrong.....Is route PFI signal only for 5404 ???  2009. 10.7 */
#define INIT_FGEN_EXPORT_SIG_IN	NIFGEN_VAL_START_TRIGGER /* NIFGEN_VAL_PFI_0 */
#define INIT_FGEN_EXPORT_SIG_TO	""

#define WF_ROOT_DIR    "/usr/local/epics/siteApp/wfDb/"



#define INIT_BLIP_TIME    (16.0)
#define INIT_T0_TIME    (0.0)






#define NI_FGEN_EXPORT_NOTHING		0

#define NI_FGEN_SIGNAL_START_TRIGGER	1
#define NI_FGEN_SIGNAL_PFI_0			2
#define NI_FGEN_SIGNAL_PFI_1			3

#define NI_FGEN_EXPORT_SIG_TO_RTSI0	1
#define NI_FGEN_EXPORT_SIG_TO_RTSI1	2
#define NI_FGEN_EXPORT_SIG_TO_RTSI2	3
#define NI_FGEN_EXPORT_SIG_TO_RTSI3	4
#define NI_FGEN_EXPORT_SIG_TO_RTSI4	5
#define NI_FGEN_EXPORT_SIG_TO_RTSI5	6
#define NI_FGEN_EXPORT_SIG_TO_RTSI6	7
#define NI_FGEN_EXPORT_SIG_TO_RTSI7	8
#define NI_FGEN_EXPORT_SIG_TO_RefOut	9
#define NI_FGEN_EXPORT_SIG_TO_PFI0	10
#define NI_FGEN_EXPORT_SIG_TO_PFI1	11
#define NI_FGEN_EXPORT_SIG_TO_PXIStar		12
/*	[RTSI0|RTSI1|RTSI2|RTSI3|RTSI4|RTSI5|RTSI6|RTSI7|RefOut|PFI0|PFI1|PXIStar] */



#define TASK_NOT_INIT			0x0
#define TASK_INITIALIZED		(0x0001<<0)
#define SCOPE_BEGIN_ACQUSITION 		(0x0001<<1) /* 2 */
#define SCOPE_ABORT_ACQUSITION 	(0x0001<<2) /* 4 */
#define SCOPE_START_FETCH 			(0x0001<<3) /* 8 */
#define SCOPE_FETCH_SUCCESS 		(0x0001<<4) /* 16 */
#define SCOPE_STORAGE_FINISH		(0x0001<<5) /* 32 */
#define SCOPE_GET_ACTUAL_CONFIRM     (0x0001<<6) 
#define SCOPE_FINISH_ACQUISITION         (0x0001<<7) 




#define EREF_NOT_INIT                    0x0
#define EREF_INITIALIZED                (0x0001<<0)
#define EREF_SET_SHOT_NUMBER     (0x0001<<1) /* 2 */
#define EREF_SET_TREE_NAME          (0x0001<<2) /* 4 */
#define EREF_SET_CREATE_TREE       (0x0001<<3) /* 8 */

/*
#define MDS_INIT                      0x0
#define MDS_CONNECTED          ( 0x01<<0)
#define MDS_OPEN_OK              ( 0x01<<1)


#define OPMODE_INIT				0
#define OPMODE_REMOTE				1
#define OPMODE_CALIBRATION		2
#define OPMODE_LOCAL				3
*/



#define NI5412_ERROR_OUT_MODE		1
#define NI5412_ERROR_TYPE			2
#define NI5412_ERROR_AMPLITUDE	3
#define NI5412_ERROR_FREQ			4
#define NI5412_ERROR_OFFSET		5
#define NI5412_ERROR_PHASE			6
#define NI5412_ERROR_GAIN			7
#define NI5412_ERROR_CFG_ARB		8
#define NI5412_ERROR_CFG_STD		9
#define NI5412_ERROR_TRIG_TYPE		10
#define NI5412_ERROR_ROUTE		11
#define NI5412_ERROR_OUTPUT_ENABLE		12
#define NI5412_ERROR_SIG_GEN		13
#define NI5412_ERROR_CLOCK_MODE		14
#define NI5412_ERROR_WF_DOWNLOAD	15


#define NISCOPE_OK_CONFIRM		1
#define NISCOPE_ERROR_VPP_RANGE		2
#define NISCOPE_ERROR_OFFSET			3
#define NISCOPE_ERROR_PROBE_ATTEN		4
#define NISCOPE_ERROR_IMPEDANCE		5
#define NISCOPE_ERROR_TAG_NAME		6
#define NISCOPE_ERROR_SMP_RATE			7
#define NISCOPE_ERROR_REC_LENGTH			8
#define NISCOPE_ERROR_TRIGER		9
#define NISCOPE_ERROR_INIT		10
#define NISCOPE_ERROR_ABORT		11
#define NISCOPE_ERROR_TREE_PUT		12
#define NISCOPE_ERROR_AUTO_SETUP		13
#define NISCOPE_ERROR_FETCH_DATA		14
#define NISCOPE_ERROR_FETCH_BINARY		15
#define NISCOPE_ERROR_READ_DATA		16
#define NISCOPE_ERROR_FILTER_TYPE		17
#define NISCOPE_ERROR_FILTER_CUTOFF_FREQ		18
#define NISCOPE_ERROR_FILTER_CENTER_FREQ		19
#define NISCOPE_ERROR_FILTER_BND_WIDTH		20
#define NISCOPE_ERROR_GET_NUM_WFM			21
#define NISCOPE_ERROR_MALLOC			22
#define NISCOPE_ERROR_MAX_FREQ		23
#define NISCOPE_ERROR_REC_NUMBER		24




















/*
#define CLOCK_INTERNAL		0
#define CLOCK_EXTERNAL		1

#define ACQ196_SET_CLOCK_RATE	(0x01<<0)
#define ACQ196_SET_R_BITMASK	(0x01<<1)
#define ACQ196_SET_M_BITMASK	(0x01<<2)
#define ACQ196_SET_L_BITMASK	(0x01<<3)
#define ACQ196_SET_T0			(0x01<<4)
#define ACQ196_SET_T1			(0x01<<5)
#define ACQ196_SET_TAG			(0x01<<6)
#define ACQ196_SET_SHOT_NUM		(0x01<<7)
*/

#define INIT_FGEN_FREQ			100000
#define INIT_FGEN_AMP			1
#define INIT_FGEN_DCOFFSET		0
#define INIT_FGEN_GAIN			1

#define INIT_SCOPE_SAMPLERATE    10000
#define INIT_SCOPE_RECD_LEN        30000
#define INIT_SCOPE_V_RANGE		2
#define INIT_SCOPE_OFFSET		0
#define INIT_SCOPE_PROBE_ATTENU		1
#define INIT_SCOPE_IMPEDANCE_50		(50.0)
#define INIT_SCOPE_IMPEDANCE_1M		(1000000.0)
#define INIT_SCOPE_MAX_FREQ		(0.0)


#define INIT_SCOPE_TRIG_LEVEL		4
#define INIT_SCOPE_TRIG_HOLDOFF		0
#define INIT_SCOPE_TRIG_DELAY		0


#define NISCOPE_FILTER_LOWPASS	0
#define NISCOPE_FILTER_HIGHPASS	1
#define NISCOPE_FILTER_BANDPASS	2
#define NISCOPE_FILTER_BANDSTOP	3
#define INIT_SCOPE_FILTER_CUTOFF	(1000000.0)
#define INIT_SCOPE_FILTER_CENTER    (1000000.0)
#define INIT_SCOPE_FILTER_BNDWIDTH	(1000.0)




/*

typedef struct {
	void		*ptaskConfig;
	void		*pchannelConfig;
	void		*pERefConfig;
	void		*precord;
	int		channelID;
	double		setValue;
	char		setStr[256];
} execParam;

typedef void (*EXECFUNCQUEUE) (ST_execParam *pParam);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	execParam         param;
} controlThreadQueueData;
*/

enum {
   NO_TRIGGER,
   SOFTWARE_TRIGGER,
   DIGITAL_TRIGGER 
};


char sysName[16];


#endif
