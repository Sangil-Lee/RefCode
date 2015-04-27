/******************************************************************************
 *  Copyright (c) 2010 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 11. 23.
first created by woong.

*/


#ifndef _IOCTL_CLTU_H_
#define _IOCTL_CLTU_H_


#define IOCTL_CLTU_MAGIC 't'

typedef struct
{
	unsigned long size;
	unsigned char buff[128];
} __attribute__((packed)) ioctl_intLTU_info;

/*
typedef struct int_arg_t
{
	struct semaphore sem_cb;
	int nVal;
	
} CLTU_INTERRUPT_CALLBACK;
*/


#define IOCTL_CLTU_GETSTATE		_IO( IOCTL_CLTU_MAGIC, 0 )

#define IOCTL_CLTU_READ        _IO( IOCTL_CLTU_MAGIC, 1 )
#define IOCTL_CLTU_WRITE       _IO( IOCTL_CLTU_MAGIC, 2 )
#define IOCTL_CLTU_INTERRUPT_ATTACH       _IO( IOCTL_CLTU_MAGIC, 3 )
#define IOCTL_CLTU_INTERRUPT_ENABLE   _IO( IOCTL_CLTU_MAGIC, 4 )
#define IOCTL_CLTU_INTERRUPT_DISABLE   _IO( IOCTL_CLTU_MAGIC, 5 )

#define IOCTL_CLTU_R2_SELECT_PORT   _IO( IOCTL_CLTU_MAGIC, 6 )
#define IOCTL_CLTU_R2_SETUP_PORT   _IO( IOCTL_CLTU_MAGIC, 7 )
#define IOCTL_CLTU_R2_SHOT_START   _IO( IOCTL_CLTU_MAGIC, 8 )
#define IOCTL_CLTU_R2_SET_MODE   _IO( IOCTL_CLTU_MAGIC, 9 )
#define IOCTL_CLTU_R2_NODE_ID   _IO( IOCTL_CLTU_MAGIC, 10 )
#define IOCTL_CLTU_R2_SET_ENDIAN   _IO( IOCTL_CLTU_MAGIC, 11 )
#define IOCTL_CLTU_R2_SET_CAL_VALUE   _IO( IOCTL_CLTU_MAGIC, 12 )
#define IOCTL_CLTU_R2_GET_CAL_TICK   _IO( IOCTL_CLTU_MAGIC, 13 )
#define IOCTL_CLTU_R2_GET_CAL_ID   _IO( IOCTL_CLTU_MAGIC, 14 )
#define IOCTL_CLTU_R2_SHOT_END   _IO( IOCTL_CLTU_MAGIC, 15 )


#define IOCTL_CLTU_R2_GET_FPGA_VER   _IO( IOCTL_CLTU_MAGIC, 16 )
#define IOCTL_CLTU_R2_GET_ENDIAN   _IO( IOCTL_CLTU_MAGIC, 17 )
#define IOCTL_CLTU_R2_GET_REF_CLOCK   _IO( IOCTL_CLTU_MAGIC, 18 )
#define IOCTL_CLTU_R2_GET_IRIGB  _IO( IOCTL_CLTU_MAGIC, 19 )
#define IOCTL_CLTU_R2_GET_IRIGB_UNLOCK_CNT   _IO( IOCTL_CLTU_MAGIC, 20 )
#define IOCTL_CLTU_R2_GET_SERDES	_IO( IOCTL_CLTU_MAGIC, 21 )
#define IOCTL_CLTU_R2_GET_FOT_RXLINK	_IO( IOCTL_CLTU_MAGIC, 22 )


#define IOCTL_CLTU_R2_GET_CURRENT_TIME	_IO( IOCTL_CLTU_MAGIC, 23 )
#define IOCTL_CLTU_R2_GET_CURRENT_TICK	_IO( IOCTL_CLTU_MAGIC, 24 )

#define IOCTL_CLTU_R2_GET_SERDES_RX_CNT	_IO( IOCTL_CLTU_MAGIC, 25 )
#define IOCTL_CLTU_R2_GET_SERDES_TX_CNT	_IO( IOCTL_CLTU_MAGIC, 26 )


#define IOCTL_CLTU_MAXNR			27



#endif /* _IOCTL_CLTU_H_ */

