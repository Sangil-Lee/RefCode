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
/*
#ifndef int8
typedef	signed char		int8;
#endif
*/
#ifndef uint8
typedef	unsigned char		uint8;
#endif
/*
#ifndef int16
typedef	signed short		int16;
#endif
*/
#ifndef uint16
typedef	unsigned short		uint16;
#endif
/*

#ifndef int32
typedef	signed int		int32;
#endif
*/

#ifndef uint32
typedef	unsigned int		uint32;
#endif
/*
#ifndef float32
typedef	float			float32;
#endif
#ifndef float64
typedef	double			float64;
#endif
*/

#define SIZE_PV_NAME          61   /* Record Name */
#define SIZE_TASK_NAME        40
#define SIZE_TREE_NAME        40
#define SIZE_TREE_IP_PORT     40
#define SIZE_TAG_NAME         30
#define SIZE_DPVT_ARG         30
#define SIZE_STRINGOUT_VAL    64
#define SIZE_CNT_MULTI_TRIG   3

#define CNT_OPMODE            4
#define OPMODE_INIT           2
#define OPMODE_LOCAL          0
#define OPMODE_REMOTE         1
#define OPMODE_CALIBRATION    3



#define STR_LOG_FILENAME "/usr/local/epics/siteApp/config/ioc_log.txt"


/********************************************************/
#define TASK_NOT_INIT                  0x0
/*****************/
#define TASK_SYSTEM_READY             (0x0001<<0)
#define TASK_ARM_ENABLED              (0x0001<<1)
#define TASK_DAQ_STARTED              (0x0001<<2)
#define TASK_AFTER_SHOT_PROCESS       (0x0001<<3)
/*****************/
#define TASK_DATA_PUT_STORAGE         (0x0001<<4)
#define TASK_SYSTEM_IDLE              (0x0001<<5)
/********************************************************/

#define ERROR_NONE                       0x0
#define ERROR_AFTER_SHOT_PROCESS        (0x0001<<0)
#define ERROR_DATA_PUT_STORAGE          (0x0001<<1)



#ifdef __cplusplus
}
#endif

#endif



