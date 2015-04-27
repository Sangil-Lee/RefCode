#ifndef drvEC1_ANT_H
#define drvEC1_ANT_H

#include "kutil.h"   
#include "sfwDriver.h"

#include "rfm2g_api.h"
#include "rfmAddrMap.h"
#include <pxi-6259-lib.h>

#define STR_DEVICE_TYPE_1		"EC1_ANT"
#define STR_RFM_DEV_NAME		"/dev/rfm2g0"
#define STR_DIG_DEV_NAME		"/dev/pxi6259"

#define USE_CPU_AFFINITY_RT		1
#define USE_ECH_POWER_CTRL		1

#if USE_CPU_AFFINITY_RT

#define PRIOTY_EC1_ANT			92

typedef struct epicsThreadOSD {
    ELLNODE            node;
    pthread_t          tid;
    pthread_attr_t     attr;
    struct sched_param schedParam;
    EPICSTHREADFUNC    createFunc;
    void              *createArg;
    epicsEventId       suspendEvent;
    int                isSuspended;
    int                isEpicsThread;
    int                isOnThreadList;
    unsigned int       osiPriority;
    char              *name;
#if defined (SHOW_LWP)
    pid_t              lwpId;
#endif /* SHOW_LWP */
} epicsThreadOSD;

#endif	// USE_CPU_AFFINITY_RT

#define	RFM_EC1_ANT_CMD		0x02000000		// writing section for heating
#define	RFM_EC1_ANT_STS		0x02000200		// reading section for heating

typedef struct {
	unsigned int	id;

	unsigned int	pcs_mode		:  1;	// 1 : controlled by PCS
	unsigned int	pcs_run			:  1;	// 1 : start to move mirror
	unsigned int	reserved		: 30;	// reserved bits for command
	unsigned int	count;					// angle count command
	unsigned int	count_max;				// maximum count
	unsigned int	count_min;				// minimum count
} EC1_ANT_PCS_CMD;	// PCSRT1 -> PCSRT5

typedef struct {
	unsigned int	id;

	unsigned int	count;					// angle count return

	// H/W status
	unsigned int	kstar_mode		:  1;	// 1 : KSTAR mode
	unsigned int	rt_mode			:  1;	// controller allows PCS to control motor
	unsigned int	shutter			:  1;	// 1 : shutter is closed
	unsigned int	ech				:  1;	// 1 : launcher has external problem
	unsigned int	hw_limit_up		:  1;	// 1 : antenna has reached to hardware maximum
	unsigned int	hw_limit_down	:  1;	// 1 : antenna has reached to hardware minimum
	unsigned int	reserved1		: 26;	// reserved bits for status

	// control status
	unsigned int	motor_on		:  1;	// motor on : move
	unsigned int	motor_down		:  1;	// motor direction : down
	unsigned int	motor_up		:  1;	// motor direction : up
	unsigned int	reserved2		: 29;	// reserved bits for status

	// S/W status
	unsigned int	pcs_mode		:  1;	// motor on : start
	unsigned int	pcs_run			:  1;	// 1 : start to move mirror
	unsigned int	sw_limit_up		:  1;	// 1 : antenna has reached to software maximum
	unsigned int	sw_limit_down	:  1;	// 1 : antenna has reached to software minimum
	unsigned int	reserved3		: 28;	// reserved bits for status

} EC1_ANT_PCS_STS;	// PCSRT5 -> PCSRT1

typedef struct {
	unsigned short	count;					// angle count command

	unsigned short	pcs_mode		:  1;	// 1 : controlled by PCS
	unsigned short	pcs_run			:  1;	// 1 : start to move mirror
	unsigned short	motor_on		:  1;	// motor on : move
	unsigned short	motor_down		:  1;	// motor direction : down
	unsigned short	motor_up		:  1;	// motor direction : up
	unsigned short	reserved		: 11;	// reserved bits for status
} EC1_ANT_CTRL_CMD;	// PCSRT5 -> Controller

typedef struct {
	unsigned short	count;					// angle count return

	unsigned short	rt_mode			:  1;	// controller allows PCS to control motor
	unsigned short	shutter			:  1;	// 1 : shutter is closed
	unsigned short	ech				:  1;	// 1 : launcher has external problem
	unsigned short	hw_limit_up		:  1;	// 1 : antenna has reached to hardware maximum
	unsigned short	hw_limit_down	:  1;	// 1 : antenna has reached to hardware minimum
	unsigned short	pcs_run			:  1;	// return of pcs_run of EC1_ANT_CTRL_CMD. 2013/05/14
	unsigned short	reserved		: 10;	// reserved bits for status
} EC1_ANT_CTRL_STS;	// Controller -> PCSRT5

#pragma pack(push, 1)
typedef struct {
	unsigned char		start;		// ENQ_CHR
	EC1_ANT_CTRL_STS	data;		// data
	unsigned char		stop;		// EOT_CHR
} EC1_ANT_CTRL_FRAME;	// communication frame for RS-232C
#pragma pack(pop)

typedef struct {
	// database
	int					iDbBaudRate;
	int					bDbSimulation;
	double				dDbStsRate;
	double				dDbCmdRate;
	char				szDbPortName[64];
	int					bDbRfmStatus;		// RFM communication status
	int					bDbCtrlStatus;		// status of communication with controller (via RS-232C)

	// for PCSRT1
	EC1_ANT_PCS_CMD		stMsgPcsCmd;		// RT1 -> RT5
	EC1_ANT_PCS_STS		stMsgPcsSts;		// RT5 -> RT1
	EC1_ANT_PCS_STS		stMsgPcsStsLocal;	// local status

	// for Controller
	EC1_ANT_CTRL_CMD	stMsgCtrlCmd;
	EC1_ANT_CTRL_STS	stMsgCtrlSts;

	// for NI 6259
	int					devFD;
	int					devNo;
	int					channelNo;
	int					channelFD;
	pxi6259_ai_conf_t	aiConfig;
	epicsEventId		ctrlRunEventId;		// event for schedule

#if USE_ECH_POWER_CTRL
	// for ECH Power Control
	epicsEventId		echRunEventId;		// event for schedule
#endif	// USE_ECH_POWER_CTRL

} ST_EC1_ANT;

#endif /* drvEC1_ANT_H */

