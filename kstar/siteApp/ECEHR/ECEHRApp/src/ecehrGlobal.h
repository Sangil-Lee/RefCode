#ifndef _DAQ_STATE_H_
#define _DAQ_STATE_H_



#ifndef ERROR
#define ERROR	0
#endif

#ifndef OK
#define OK		1
#endif



#define INIT_GAIN					1
#define INIT_SAMPLING			100		/* unit: KHz must  50, 100, 200 Khz */
#define INIT_CH_MASK				96		/* 32, 64, 96, 128 */
#define INIT_TRIG_POLARITY		1		/* 0: reset on high,  1: reset on low */




#define D07_CLOCK_INTER_EXTER		0x8000
#define DFS_QUAD_MODE_200KHZ		0x0200	/*  1  0 */
#define DFS_DOUBLE_MODE_100KHZ		0x0100	/*  0  1 */
#define DFS_NORMAL_MODE_50KHZ		0x0000	/*  0  0 */


#define TASK_NOT_INIT            0x0000
#define TASK_INITIALIZED          (0x0001<<0)
#define TASK_ADC_STARTED      (0x0001<<1)
#define TASK_ADC_STOPED        (0x0001<<2)
#define TASK_DAQ_STARTED       (0x0001<<3)
#define TASK_DAQ_STOPED        (0x0001<<4)
#define TASK_CHANNELIZATION      (0x0001<<5)
#define TASK_LOCAL_STORAGE     (0x0001<<6)
#define TASK_REMOTE_STORAGE    (0x0001<<7)

#define TASK_ARM_OUT               (0x0001<<8)
#define TASK_ARM_RESET             (0x0001<<9)

#define TASK_CHANNELIZATION_FAIL    (0x0001<<10)
#define TASK_STORAGE_FAIL     (0x0001<<11)


#define CHANNEL_INITIALIZED	(0x0001<<0)


#define FPDP_START		1
#define FPDP_STOP			2
#define FPDP_OPENFILE	3
#define FPDP_KILL			99



#endif
