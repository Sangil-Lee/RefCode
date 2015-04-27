#ifndef _DAQ_STATE_H_
#define _DAQ_STATE_H_


/********************************************************/
#define TASK_NOT_INIT                  0x0
/*****************/
#define TASK_SYSTEM_READY             (0x0001<<0)
#define TASK_ARM_ENABLED              (0x0001<<1)
#define TASK_WAITING_TRIGGER              (0x0001<<2)
#define TASK_DURING_DAQ              (0x0001<<3)
#define TASK_AFTER_SHOT_PROCESS       (0x0001<<4)
/*****************/
#define TASK_DATA_PUT_STORAGE         (0x0001<<5)
#define TASK_SYSTEM_IDLE              (0x0001<<6)
/********************************************************/




#define TASK_ARM_OUT			(0x0001<<8)
#define TASK_ARM_RESET			(0x0001<<9)

#define TASK_CHANNELIZATION_FAIL    (0x0001<<10)
#define TASK_STORAGE_FAIL     (0x0001<<11)
#define TASK_IN_LOOP_HTSTREAM     (0x0001<<12)
#define TASK_READY_TO_SAVE			(0x0001<<13)
#define TASK_STORAGE_FINISH			(0x0001<<14)
#define TASK_CMD_DAQ_STOPED			(0x0001<<15)


#define CHANNEL_INITIALIZED		(0x0001<<0)


/*  operation mode */
#define SYS_MODE_INIT           0
#define SYS_MODE_REMOTE     1
#define SYS_MODE_LOCAL        2
#define SYS_MODE_CALIB          3




#define SYS_CLOCK_INTERNAL    1
#define SYS_CLOCK_EXTERNAL    2

#define STATE_INIT						1
#define STATE_DAQ_IDLE					2
#define STATE_SETUP_DONE				3
#define STATE_DAQ_RUN					4
#define STATE_DAQ_STOP					5
#define STATE_DATA_CONVT				6
#define STATE_LOCAL_DATA_SAVE			7
#define STATE_SEND_READY_REMOTE		8



#endif
