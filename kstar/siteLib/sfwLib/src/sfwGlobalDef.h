#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef WR_ERROR
#define WR_ERROR	0
#endif

#ifndef WR_OK
#define WR_OK		1
#endif

#ifndef int8
typedef	signed char		int8;
#endif
#ifndef uint8
typedef	unsigned char		uint8;
#endif
#ifndef int16
typedef	signed short		int16;
#endif
#ifndef uint16
typedef	unsigned short		uint16;
#endif
#ifndef int32
typedef	signed int		int32;
#endif
#ifndef uint32
typedef	unsigned int		uint32;
#endif
#ifndef float32
typedef	float			float32;
#endif
#ifndef float64
typedef	double			float64;
#endif


#define INIT_SAMPLING   1000  /* unit:Hz */
#define INIT_BLIP_TIME  (16.0)  /* unit:sec   */
#define INIT_STEP_TIME  (0.001) /* unit: sec */


#define SIZE_STR_ARG          40
#define SIZE_PV_NAME          61   /* Record Name */
#define SIZE_TASK_NAME        16
#define SIZE_TREE_NAME        16
#define SIZE_TREE_IP_PORT     24
#define SIZE_TAG_NAME         30
#define SIZE_DPVT_ARG         40
#define SIZE_STRINGOUT_VAL    40
#define SIZE_CNT_MULTI_TRIG   50
#define SIZE_ENV_TOP   64
#define SIZE_ENV_IOC   16



#define CNT_OPMODE               4
#define OPMODE_INIT           0
#define OPMODE_REMOTE         1
#define OPMODE_LOCAL          2
#define OPMODE_CALIBRATION    3


/* It can be used  in case of couldn't find TOP environment */
#define STR_LOG_LOCATION  "/usr/local/epics/siteApp"

#define STR_LOG_FNAME  "sfwLog.txt"


#define NUM_SYS_OP_MODE      0
#define NUM_SYS_ARMING       1
#define NUM_SYS_RUN          2
#define NUM_CA_SHOT_NUMBER   3
#define NUM_CA_BLIP_TIME     4
#define NUM_SYS_CREATE_LOCAL_TREE     5

#define NUM_PV_STR_MAX          6


#define ERR_SCN		0
#define ERR_SCN_PNT		1
#define ERR_PNT		2


/********************************************************/
#define TASK_NOT_INIT                  0x0
/*****************/
#define TASK_STANDBY             (0x0001<<0)
#define TASK_ARM_ENABLED              (0x0001<<1)
#define TASK_WAIT_FOR_TRIGGER              (0x0001<<2)
#define TASK_IN_PROGRESS              (0x0001<<3)
#define TASK_POST_PROCESS       (0x0001<<4)
/*****************/
#define TASK_DATA_TRANSFER         (0x0001<<5)
#define TASK_SYSTEM_IDLE              (0x0001<<6)
#define TASK_RUN_TRIGGERED              (0x0001<<7)	/* 2012. 5. 31 user for auto save function */
/********************************************************/

/********************************************************/
#define STATUS_NOT_INIT                  0
#define STATUS_STANDBY             1
#define STATUS_ARM_ENABLED              2
#define STATUS_WAIT_FOR_TRIGGER              3
#define STATUS_IN_PROGRESS              4
#define STATUS_POST_PROCESS       5
#define STATUS_DATA_TRANSFER        6
#define STATUS_SYSTEM_IDLE              7
/********************************************************/


#define ERROR_NONE                       0x0
#define ERROR_AFTER_SHOT_PROCESS        (0x0001<<0)
#define ERROR_DATA_PUT_STORAGE          (0x0001<<1)



#ifdef __cplusplus
}
#endif

#endif



