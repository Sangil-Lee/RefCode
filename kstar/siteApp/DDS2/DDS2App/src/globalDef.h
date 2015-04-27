#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_


#ifndef WR_ERROR
#define WR_ERROR	0
#endif

#ifndef WR_OK
#define WR_OK		1
#endif

#define PRINT_PHASE_INFO          0
#define PRINT_PMC_INFO          0

#define USE_MANUAL_FIND_PCI   1
#define PCI_BUS_NO     0x10
#define PCI_DEVICE_NO    0x1  /* it should be get from start script */
#define PCI_FUNC_NO     0x0


#define TASK_NOT_INIT			0x0000
#define TASK_INITIALIZED		(0x0001<<0)
#define TASK_ADC_STARTED  		(0x0001<<1)
#define TASK_ADC_STOPED			(0x0001<<2)
#define TASK_DAQ_STARTED  		(0x0001<<3)
#define TASK_DAQ_STOPED			(0x0001<<4)
#define TASK_CHANNELIZATION	(0x0001<<5)
#define TASK_LOCAL_STORAGE		(0x0001<<6)
#define TASK_REMOTE_STORAGE	(0x0001<<7)

#define TASK_ARM_OUT				(0x0001<<8)
#define TASK_ARM_RESET			(0x0001<<9)

#define TASK_FINISH  1
	
#define PORT_NOT_INIT		0x0000
#define PORT_TRIG_OK  		(0x0001<<0)
#define PORT_CLOCK_OK		(0x0001<<1)
#define PORT_T0_OK  		(0x0001<<2)
#define PORT_T1_OK			(0x0001<<3)

#define ACTIVE_HIGHT_VAL		0




#define T_YEAR_DIVIDER  (365*24*60*60)
#define T_DAY_DIVIDER  (24*60*60)
#define T_HOUR_DIVIDER (60*60)
#define T_MIN_DIVIDER  (60)
#define T_SEC_DIVIDER  (1)






#endif



