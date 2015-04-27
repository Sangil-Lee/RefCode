/******************************************************************************
 *  Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved.                     *
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
} __attribute__((packed)) ioctl_cltu_info;

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


#define IOCTL_CLTU_MAXNR			6



#endif /* _IOCTL_CLTU_H_ */


