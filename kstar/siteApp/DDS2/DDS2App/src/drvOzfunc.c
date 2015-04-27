
#include "drvOzfunc.h"

#include <linux/vme_io.h>
#include <sys/ioctl.h>
#include <fcntl.h>



#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <epicsMutex.h>


#include <dbCommon.h>	/* for epicsPrintf() */




/*************  for open VME device *********************/
#define		STR_FILE_DEV	"/dev/vme_a24a16_user"
#define		NUM_OF_BYTE		32

#define DUMMY_DATA	0x0

#define	MM_PGA_ARRAY_1		0
#define	MM_PGA_ARRAY_2		2
#define	MM_PGA_ARRAY_3		4
#define	MM_PGA_ARRAY_4		6
#define	MM_ADC_HPFE			8
#define	MM_ADC_ZCAL			10
#define	MM_CNTRL_REG_1		12
#define	MM_CNTRL_REG_2		14

#define  MASK(k)    (0x0001<<(k))

int		_fd;
unsigned int	gran_loss;
unsigned short	buf_with_dummy[16];

/********************************************************/



/*******************************************************************************
Company   : Olzetek
Function  : ozInitDevice
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozInitDevice(unsigned short vme_addr)
{
	int i;

	unsigned long real_vme_addr;
	unsigned int granularity;
	unsigned int gran_loss;

	volatile unsigned char *ptr = 0;
	volatile unsigned char *mapped_data = 0;
	unsigned short value = 0x0;


	struct vme_mapping_ctrl Mapping = {
		VMEMAP_DWIDTH_16, VMEMAP_ASPACE_A16,
		VMEMAP_SUPUSERAM_USER, VMEMAP_PRGDATAAM_DATA};
	
	Mapping.max_datawidth = VMEMAP_DWIDTH_16;
	Mapping.supuseram = VMEMAP_SUPUSERAM_USER;
	Mapping.address_space = VMEMAP_ASPACE_A16;

	_fd = open(STR_FILE_DEV, O_RDWR);	
	if (_fd == -1) {
		fprintf(stderr,"\n>>> ozInitDevice : Can't open \"%s\"\n", STR_FILE_DEV);
		return ERROR;
	}
	else {
		fprintf(stderr,">>> Open device %s OK.\n", STR_FILE_DEV);
	}

	if (ioctl(_fd, VMEIMG_SETMAPPING, &Mapping)) {
		perror("ioctl VMEIMG_SETMAPPING");
		return ERROR;
	}

	if (ioctl(_fd, VMEIMG_SETVMEADDR, vme_addr)) {
		perror("ioctl VMEIMG_SETVMEADDR");
		return ERROR;
	}
			
	if (ioctl(_fd, VMEIMG_GETVMEADDR, &real_vme_addr)) {
		perror("ioctl VMEIMG_GETVMEADDR");
		return ERROR;
	}

	if (ioctl(_fd, VMEIMG_GETGRANULARITY, &granularity)) {
		perror("ioctl VMEIMG_GETVMEADDR");
		return ERROR;
	}

	gran_loss = vme_addr - real_vme_addr;

	ptr = mapped_data + gran_loss;
#if 1  
		printf( "Register gran_loss TG.Lee val: 0x%X \n", gran_loss );
		printf( "Register vme_addr TG.Lee val: 0x%X \n", vme_addr );
		printf( "Register real_vme_addr TG.Lee val: 0x%X \n", real_vme_addr );
		printf( "Register mapped_data TG.Lee val: 0x%X \n", mapped_data );
		printf( "Register ptr TG.Lee val: 0x%X \n", ptr );
#endif

	for( i=0; i< 16 ; i++) 
		buf_with_dummy[i] = DUMMY_DATA;
	

	for( i=0; i<8; i++) { 
		if (lseek(_fd, (ptr - mapped_data), SEEK_SET) == -1) {
			perror("lseek failed");
			return ERROR;
		}
		if (read(_fd,&value,sizeof(value)) != sizeof(value)) {
			perror("read failed");
			return ERROR;
		}
		buf_with_dummy[i*2] = value;
		ptr += 4;
#if 1 
		printf( "Register TG.Lee: %d  val: 0x%X \n",i, buf_with_dummy[i*2] );
#endif
	}

	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozWriteRegister
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozWriteRegister(drvM6802_taskConfig *ptaskConfig)
{
	int bytes_written;
	if (ioctl(_fd, VMEIMG_SETVMEADDR, ptaskConfig->vme_addr)) {
		perror("\n>>> ozWriteRegister : ioctl VMEIMG_SETVMEADDR\n");
		return ERROR;
	}
	lseek(_fd, gran_loss, SEEK_SET);
	bytes_written = write(_fd, ptaskConfig->register_m6802, NUM_OF_BYTE);
	if (bytes_written != NUM_OF_BYTE) {
		fprintf(stderr,"write failed, only %d of %d bytes read", bytes_written, NUM_OF_BYTE);
		perror("");
		return ERROR;
	}
#if 0
	epicsPrintf("ozWriteRegister(MM_CNTRL_REG_1): 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_1] );
	epicsPrintf("ozWriteRegister(MM_CNTRL_REG_2): 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_2] );
#endif
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetGlobalReset
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetGlobalReset(drvM6802_taskConfig *ptaskConfig)
{
	int i;
	/* GLOBAL reset */
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = 0x4;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	if (ioctl(_fd, VMEIMG_SETVMEADDR, ptaskConfig->vme_addr)) {
		perror("\n>>> ozSetGlobalReset : ioctl VMEIMG_SETVMEADDR\n");
		return ERROR;
	}
	lseek(_fd, gran_loss, SEEK_SET);
	write(_fd, ptaskConfig->register_m6802, NUM_OF_BYTE);

	for( i=0; i< 16 ; i++){ 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[i] = DUMMY_DATA;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : ozSetZeroCalibration
Author    : TGLEE
Parameter : Calibration reference signal : 0(default) = Common Voltag Pins VCOML, 1= Analog Input Pin 
Return    : .
Desc      : 
Date      : 2008.11.15 (initial create.)
*******************************************************************************/
#if 1
int ozSetZeroCalibration(drvM6802_taskConfig *ptaskConfig, int chan, uint16 val)
{
	unsigned short  mask=0x0000, oldData=0x0000;
	int k;
  /*         MASK(k)    (0x0001<<(k))  */
	unsigned short shift=0;
	oldData = ptaskConfig->register_m6802[MM_ADC_ZCAL];
	
	if(chan<15){
		shift = 8;
		if(chan==0)
	  		k=0+shift;
        	else k=chan/2 + shift;
	}
 	else if(chan>15 && chan<33){      
		 shift =0;
		 k=(chan/2 - 8) + shift;
	}
	else {
	epicsPrintf("\n>>> Error Channel Over  Zero Calibration \n");
	return ERROR;
	}

/* Want Position Value is Make Init (zero) State */	
	mask |= MASK(k);
	
/* 	val <<= shift;  Not Shift Move. shift K */
 	val <<= k;

	epicsPrintf("\n>>> Zero Calibration Mask : 0x%X \n",mask);
	epicsPrintf("\n>>> Zero Calibration val : 0x%X \n",val);
	epicsPrintf("\n>>> Zero Calibration shift : 0x%X \n",shift);
	epicsPrintf("\n>>> Zero Calibration chan : %d Value: %d \n",chan, val);
	epicsPrintf("\n>>> Zero Calibration chan : %d Value: 0x%X \n",chan, val);
/* epicsMutex Data Protection TG.Lee */	
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_ADC_ZCAL] = (oldData &(~mask)) | val ;  /* reset on High */
	epicsMutexUnlock(ptaskConfig->bufferLock);

	epicsPrintf("\n>>> Zero Calibration MM_ADC_ZCAL : 0x%X \n",ptaskConfig->register_m6802[MM_ADC_ZCAL]);
	
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerPolarity : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	return OK;
}
#endif


/*******************************************************************************
Company   : Olzetek
Function  : ozSetADsFIFOrun
Author    : woong
Parameter : .
Return    : .
Desc      : val == 0 : run , vla == 1 reset
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetADsFIFOrun(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  value & 0xFFBF ;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	epicsPrintf("\n>>> ozSetADsFIFOrun : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetADsFIFOrun : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetADsFIFOrun
Author    : woong
Parameter : .
Return    : .
Desc      : val == 0 : run , vla == 1 reset
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetADsFIFOreset(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
/*	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFFBF ) | 0x40 ; */
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value | 0x40 ; 
	epicsMutexUnlock(ptaskConfig->bufferLock);
	epicsPrintf("\n>>> ozSetADsFIFOreset : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetADsFIFOreset : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTriggerPolarity
Author    : woong
Parameter : nPolarity : 0 = reset on high, 1= reset on low
Return    : .
Desc      : 
Date      : 2007.08.29 (initial create.)
*******************************************************************************/
#if 1
int ozSetTriggerPolarity(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	if ( ptaskConfig->nTrigPolarity == 0 ) {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value & 0xFFEF ;  /* reset on High */
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value | 0x10 ;  /* reset on Low */
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerPolarity : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetTriggerPolarity : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}
#endif

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTriggerReset
Author    : woong
Parameter : .
Return    : .
Desc      : 0x20 - when TRIG POLARITY : reset on high 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
#if 1
int ozSetTriggerReset(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];

	if ( ptaskConfig->nTrigPolarity == 0 ) {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value | 0x20 ;  
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value & 0xFFDF ; /* 1101 */
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerReset : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetTriggerReset : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTriggerRelease
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetTriggerRelease(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];

	if ( ptaskConfig->nTrigPolarity == 0 ) {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFFDF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else {
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value | 0x20 );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerRelease : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetTriggerRelease : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}
#endif 


/*******************************************************************************
Company   : Olzetek
Function  : ozSetTriggerExternal
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.30(initial create.)
*******************************************************************************/
int ozSetTriggerExternal(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];

	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value | 0x8 );
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerExternal : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetTriggerExternal : Ctrl()1 taskName:%s,value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;

}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTriggerInternal
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.30(initial create.)
*******************************************************************************/
int ozSetTriggerInternal(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];

	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFFF7 );
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetTriggerInternal : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetTriggerInternal : Ctrl()1 value: 0x%X  \n",ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}




/*******************************************************************************
Company   : Olzetek
Function  : ozSetArmOut
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetArmOut(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_2] = value | 0x0010 ;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetArmOut : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetArmOut : Ctrl()2 taskName:%s,value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);
	return OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : ozSetArmReset
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetArmReset(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value=0;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_2] = value & 0xFFEF  ;
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetArmReset : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetArmReset : Ctrl()2 taskName:%s,value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTermEnable
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetTermEnable(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	
	printf("ozSetTermEnable Function TG.Lee \n");
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  value & 0xFFFE ;
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetTermEnable : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetTermEnable : Ctrl()1 taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetTermDisable
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetTermDisable(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	
	printf("ozSetTermDisable Function TG.Lee \n");

	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  value | 0x0001 ;
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetTermEnable : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetTermDisable : Ctrl()1 taskName:%s,value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetMasterOrSlave
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetMasterOrSlave(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value, value2;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	value2 = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	
	if( ptaskConfig->masterMode ) 
	{
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0xF3FF ) | 0x0800;
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  value2 | 0x0020;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetMasterOrSlave : set Master mode\n");
	}
	else 
	{
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0xF3FF ) | 0x0400;
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  value2 & 0xFFDF;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetMasterOrSlave : set Slave mode\n");
	}

	
	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetMasterOrSlave : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetMasterOrSlave : Ctrl_1() taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	epicsPrintf("\n>>> ozSetMasterOrSlave : Ctrl_2() taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);
	return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : ozSetClockInterExter
Author    : woong
Parameter : .
Return    : .
Desc      : val == 0 : internal , vla == 1 external
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetClockInterExter(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	uint16 val;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	val = ptaskConfig->clockSource;

#if 0
/* use always internal clock source */
	val = 0;
#endif

	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = (val == 1) ?  value | 0x8000 : value & 0x7FFF;
	epicsMutexUnlock(ptaskConfig->bufferLock);


/*	epicsPrintf("clock set: 0x%X\n", ptaskConfig->register_m6802[MM_CNTRL_REG_1] ); */

	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetClockInterExter : ozWriteRegister() ... fail\n");
		return ERROR;
	}
/*
	if( val == 1 ) epicsPrintf("\n>>> set external clock \n");
	else epicsPrintf("\n>>> set internal clock \n");
*/
	epicsPrintf("\n>>> ozSetClockInterExter : Ctrl_1() taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetSamplingRate
Author    : woong
Parameter : .
Return    : .
Desc      :
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetSamplingRate( drvM6802_taskConfig *ptaskConfig, uint16 samplingRate )
{
	unsigned short value;

	value = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	epicsPrintf("\n>>> ozSetSamplingRate : Ctrl_1()taskName:%s, init value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
/* Delete TG.Lee Modify -  Clock Divider Function Add. So woong Function delete.
	if( samplingRate == 200 )
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFCFF ) | DFS_QUAD_MODE_200KHZ;
	else if( samplingRate == 100 )
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFCFF ) | DFS_DOUBLE_MODE_100KHZ;
	else if(samplingRate == 50 )
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
	else if (samplingRate < 50)
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
	else {
		epicsPrintf("\n>>> ozSetSamplingRate : Can't support %d KHz\n", samplingRate);
		return ERROR;
	}
*/
/* Add TG.Lee Modify -  Clock Divider Function Add  */ 
	if( samplingRate == 200 ){
		value = ( value & 0xFCFF ) | DFS_QUAD_MODE_200KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if ( samplingRate == 100 ){  /* 100kHz - clock divider 1 : real 100KHz - jump Gap 1 */
		value = ( value & 0xFCFF ) | DFS_DOUBLE_MODE_100KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 50 ){    /* 50kHz - clock divider 1 : real 50KHz - jump Gap 1 */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 25){    /* 50kHz - clock divider 2 : real 25KHz - jump Gap 1  */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF ) | 0x1000;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 20){    /* 100kHz - clock divider 1 : real 100KHz - jump Gap 5 */
		value = ( value & 0xFCFF ) | DFS_DOUBLE_MODE_100KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
                ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 10 ){   /* 50kHz - clock divider 1 : real 50KHz - jump Gap 5  */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if	(samplingRate == 5 ){     /* 50kHz - clock divider 2 : real 25KHz - jump Gap 5  */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF ) | 0x1000;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 2 ){     /* 50kHz - clock divider 1 : real 50KHz - jump Gap 25  */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF );
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if (samplingRate == 1 ){    /* 50kHz - clock divider 2 : real 25KHz - jump Gap 25  */
		value = ( value & 0xFCFF ) | DFS_NORMAL_MODE_50KHZ;
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_1] =  ( value & 0x8FFF ) | 0x1000;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else {
		epicsPrintf("\n>>> ozSetSamplingRate : Can't support %d KHz\n", samplingRate);
		return ERROR;
	}
	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetSamplingRate : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetSamplingRate : Ctrl_1()taskName:%s, end value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetSampleClkDivider
Author    : woong
Parameter : .
Return    : .
Desc      :
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetSampleClkDivider(drvM6802_taskConfig *ptaskConfig, uint16 clockDivider )
{
	unsigned short value=0, val=0;
	val = ptaskConfig->register_m6802[MM_CNTRL_REG_1];
	epicsPrintf("\n>>> ozSetSampleClkDivider : Ctrl_1()taskName:%s, init value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);

	switch( clockDivider ) {
	case 1 :	value =  ( val & 0x8FFF );			break;
	case 2 :	value =  ( val & 0x8FFF ) | 0x1000;		break;
	case 4 :	value =  ( val & 0x8FFF ) | 0x2000;		break;
	case 8 :	value =  ( val & 0x8FFF ) | 0x3000;		break;
	case 16 :	value =  ( val & 0x8FFF ) | 0x4000;		break;
	case 32 :	value =  ( val & 0x8FFF ) | 0x5000;		break;
	case 64 :	value =  ( val & 0x8FFF ) | 0x6000;		break;
	case 128 :	value =  ( val & 0x8FFF ) | 0x7000;		break;
	default : epicsPrintf("\n>>> ozSetSampleClkDivider : clockDivider: %d error \n", clockDivider);
		return ERROR;
	}
	
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_1] = value;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	
	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetSampleClkDivider : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetSampleClkDivider : Ctrl_1()taskName:%s, end value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_1]);
	return OK;

}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetBoardIDBit
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetBoardIDBit(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	epicsPrintf("\n>>> ozSetBoardIDBit : Ctrl_2()taskName:%s, init value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);

	switch( ptaskConfig->BoardID ) {
	case 1 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 );
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 1\n");
		break;
	case 2 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0001;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 2\n");
		break;
	case 3 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0002;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 3\n");
		break;
	case 4 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0003;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 4\n");
		break;
	case 5 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0004;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 5\n");
		break;
	case 6 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0005;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 6\n");
		break;
	case 7 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0006;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 7\n");
		break;
	case 8 : 
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value & 0xFFF8 ) | 0x0007;
		epicsMutexUnlock(ptaskConfig->bufferLock);
		epicsPrintf("\n>>> ozSetBoardIDBit : set Board ID : 8\n");
		break;

	default : 
		epicsPrintf("\n>>> ozSetBoardIDBit : Wrong BoardID\n");
		return ERROR;
	}

	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetBoardIDBit : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetBoardIDBit : Ctrl_2()taskName:%s, end value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);
	return OK;

}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetAIGain
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetAIGain(drvM6802_taskConfig *ptaskConfig, int ch, int gain)
{
	unsigned short value, val;

#if 0
/*        epicsPrintf("\n>>> ozSetAIGain : taskName: %s, chan #%d, gain: %d\n", ptaskConfig->taskName, ch, gain);*/
	fprintf(stdout, "\n>>> ozSetAIGain : taskName: %s, chan #%d, gain: %d\n", ptaskConfig->taskName, ch, gain); 
#endif /* DEBUG */

	if( ch < 8 )		value = val = ptaskConfig->register_m6802[MM_PGA_ARRAY_1];
	else if(ch < 16)	value = val = ptaskConfig->register_m6802[MM_PGA_ARRAY_2];
	else if(ch < 24)	value = val = ptaskConfig->register_m6802[MM_PGA_ARRAY_3];
	else if( ch < 32)	value = val = ptaskConfig->register_m6802[MM_PGA_ARRAY_4];
	else {
		epicsPrintf("\n>>> ozSetAIGain : There's no ch: %d\n", ch);
		return ERROR;
		}
		

	switch(gain) {
	case 1:
		/*	MSB	LSB	*/
		/*	0	0	*/
		if	( ch==0 || ch==8 || ch==16 || ch==24 )	value &= 0xFCFF;
		else if( ch==1 || ch==9 || ch==17 || ch==25 )	value &= 0xF3FF;
		else if( ch==2 || ch==10 || ch==18 || ch==26 )	value &= 0xCFFF;
		else if( ch==3 || ch==11 || ch==19 || ch==27 )	value &= 0x3FFF;
		else if( ch==4 || ch==12 || ch==20 || ch==28 )	value &= 0xFFFC;
		else if( ch==5 || ch==13 || ch==21 || ch==29 )	value &= 0xFFF3;
		else if( ch==6 || ch==14 || ch==22 || ch==30 )	value &= 0xFFCF;
		else if( ch==7 || ch==15 || ch==23 || ch==31 )	value &= 0xFF3F;
		break;
		break;
	case 10:
		/*	MSB	LSB	*/
		/*	0	1	*/
		if	( ch==0 || ch==8 || ch==16 || ch==24 )	value = (val &= 0xFCFF) | 0x0100;
		else if( ch==1 || ch==9 || ch==17 || ch==25 )	value = (val &= 0xF3FF) | 0x0400;
		else if( ch==2 || ch==10 || ch==18 || ch==26 )	value = (val &= 0xCFFF) | 0x1000;
		else if( ch==3 || ch==11 || ch==19 || ch==27 )	value = (val &= 0x3FFF) | 0x4000;
		else if( ch==4 || ch==12 || ch==20 || ch==28 )	value = (val &= 0xFFFC) | 0x0001;
		else if( ch==5 || ch==13 || ch==21 || ch==29 )	value = (val &= 0xFFF3) | 0x0004;
		else if( ch==6 || ch==14 || ch==22 || ch==30 )	value = (val &= 0xFFCF) | 0x0010;
		else if( ch==7 || ch==15 || ch==23 || ch==31 )	value = (val &= 0xFF3F) | 0x0040;
		break;
	case 100:
		/*	MSB	LSB	*/
		/*	1	0	*/
		if	( ch==0 || ch==8 || ch==16 || ch==24 )	value = (val &= 0xFCFF) | 0x0200;
		else if( ch==1 || ch==9 || ch==17 || ch==25 )	value = (val &= 0xF3FF) | 0x0800;
		else if( ch==2 || ch==10 || ch==18 || ch==26 )	value = (val &= 0xCFFF) | 0x2000;
		else if( ch==3 || ch==11 || ch==19 || ch==27 )	value = (val &= 0x3FFF) | 0x8000;
		else if( ch==4 || ch==12 || ch==20 || ch==28 )	value = (val &= 0xFFFC) | 0x0002;
		else if( ch==5 || ch==13 || ch==21 || ch==29 )	value = (val &= 0xFFF3) | 0x0008;
		else if( ch==6 || ch==14 || ch==22 || ch==30 )	value = (val &= 0xFFCF) | 0x0020;
		else if( ch==7 || ch==15 || ch==23 || ch==31 )	value = (val &= 0xFF3F) | 0x0080;
		break;
	case 108:
		/*	MSB	LSB	*/
		/*	1	1	*/
		if	( ch==0 || ch==8 || ch==16 || ch==24 )	value |= 0x0300;
		else if( ch==1 || ch==9 || ch==17 || ch==25 )	value |= 0x0C00;
		else if( ch==2 || ch==10 || ch==18 || ch==26 )	value |= 0x3000;
		else if( ch==3 || ch==11 || ch==19 || ch==27 )	value |= 0xC000;
		else if( ch==4 || ch==12 || ch==20 || ch==28 )	value |= 0x0003;
		else if( ch==5 || ch==13 || ch==21 || ch==29 )	value |= 0x000C;
		else if( ch==6 || ch==14 || ch==22 || ch==30 )	value |= 0x0030;
		else if( ch==7 || ch==15 || ch==23 || ch==31 )	value |= 0x00C0;
		break;
	default: 
		epicsPrintf("\n>>> ozSetAIGain : gain: %d error \n", gain);
		break;
	}

	if( ch < 8 ){
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_PGA_ARRAY_1] = value;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if(ch < 16){
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_PGA_ARRAY_2] = value;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if(ch < 24){
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_PGA_ARRAY_3] = value;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}
	else if( ch < 32){
		epicsMutexLock(ptaskConfig->bufferLock);
		ptaskConfig->register_m6802[MM_PGA_ARRAY_4] = value;
		epicsMutexUnlock(ptaskConfig->bufferLock);
	}



	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetAIGain : ozWriteRegister() ... fail\n");
		return ERROR;
	}
#if 0
	epicsPrintf("\n>>> ozSetAIGain : register_PGA_Array_1() value: 0x%X  \n",ptaskConfig->register_m6802[MM_PGA_ARRAY_1]);
	epicsPrintf("\n>>> ozSetAIGain : register_PGA_Array_2() value: 0x%X  \n",ptaskConfig->register_m6802[MM_PGA_ARRAY_2]);
	epicsPrintf("\n>>> ozSetAIGain : register_PGA_Array_3() value: 0x%X  \n",ptaskConfig->register_m6802[MM_PGA_ARRAY_3]);
	epicsPrintf("\n>>> ozSetAIGain : register_PGA_Array_4() value: 0x%X  \n",ptaskConfig->register_m6802[MM_PGA_ARRAY_4]);
#endif 
	return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ozSetChannelMask
Author    : woong
Parameter : .
Return    : .
Desc      : 
Date      : 2007.08.28(initial create.)
*******************************************************************************/
int ozSetChannelMask(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value=0, val=0;
	val = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	epicsPrintf("\n>>> ozSetChannelMask : Ctrl_2()taskName:%s, init value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);

#if 1
	fprintf(stdout, "\n>>> ozSetChannelMask : channelMask: %d\n", ptaskConfig->channelMask);
#endif /* DEBUG */


/*	value &= 0x00FF; */
	switch(ptaskConfig->channelMask) {
	case 1: value = (val &= 0x00FF) | 0x0000;		break;
	case 2: value = (val &= 0x00FF) | 0x0100;		break;
	case 3: value = (val &= 0x00FF) | 0x0200;		break;
	case 4: value = (val &= 0x00FF) | 0x0300;		break;
	case 5: value = (val &= 0x00FF) | 0x0400;		break;
	case 6: value = (val &= 0x00FF) | 0x0500;		break;
	case 7: value = (val &= 0x00FF) | 0x0600;		break;
	case 8: value = (val &= 0x00FF) | 0x0700;		break;
	case 9: value = (val &= 0x00FF) | 0x0800;		break;
	case 10: value = (val &= 0x00FF) | 0x0900;		break;
	case 11: value = (val &= 0x00FF) | 0x0A00;		break;
	case 12: value = (val &= 0x00FF) | 0x0B00;		break;
	case 13: value = (val &= 0x00FF) | 0x0C00;		break;
	case 14: value = (val &= 0x00FF) | 0x0D00;		break;
	case 15: value = (val &= 0x00FF) | 0x0E00;		break;
	case 16: value = (val &= 0x00FF) | 0x0F00;		break;
	case 17: value = (val &= 0x00FF) | 0x1000;		break;
	case 18: value = (val &= 0x00FF) | 0x1100;		break;
	case 19: value = (val &= 0x00FF) | 0x1200;		break;
	case 20: value = (val &= 0x00FF) | 0x1300;		break;
	case 21: value = (val &= 0x00FF) | 0x1400;		break;
	case 22: value = (val &= 0x00FF) | 0x1500;		break;
	case 23: value = (val &= 0x00FF) | 0x1600;		break;
	case 24: value = (val &= 0x00FF) | 0x1700;		break;
	case 25: value = (val &= 0x00FF) | 0x1800;		break;
	case 26: value = (val &= 0x00FF) | 0x1900;		break;
	case 27: value = (val &= 0x00FF) | 0x1A00;		break;
	case 28: value = (val &= 0x00FF) | 0x1B00;		break;
	case 29: value = (val &= 0x00FF) | 0x1C00;		break;
	case 30: value = (val &= 0x00FF) | 0x1D00;		break;
	case 31: value = (val &= 0x00FF) | 0x1E00;		break;
	case 32: value = (val &= 0x00FF) | 0x1F00;		break;

	case 33: value = (val &= 0x00FF) | 0x2000;		break;
	case 34: value = (val &= 0x00FF) | 0x2100;		break;
	case 35: value = (val &= 0x00FF) | 0x2200;		break;
	case 36: value = (val &= 0x00FF) | 0x2300;		break;
	case 37: value = (val &= 0x00FF) | 0x2400;		break;
	case 38: value = (val &= 0x00FF) | 0x2500;		break;
	case 39: value = (val &= 0x00FF) | 0x2600;		break;
	case 40: value = (val &= 0x00FF) | 0x2700;		break;
	case 41: value = (val &= 0x00FF) | 0x2800;		break;
	case 42: value = (val &= 0x00FF) | 0x2900;		break;
	case 43: value = (val &= 0x00FF) | 0x2A00;		break;
	case 44: value = (val &= 0x00FF) | 0x2B00;		break;
	case 45: value = (val &= 0x00FF) | 0x2C00;		break;
	case 46: value = (val &= 0x00FF) | 0x2D00;		break;
	case 47: value = (val &= 0x00FF) | 0x2E00;		break;
	case 48: value = (val &= 0x00FF) | 0x2F00;		break;
	case 49: value = (val &= 0x00FF) | 0x3000;		break;
	case 50: value = (val &= 0x00FF) | 0x3100;		break;
	case 51: value = (val &= 0x00FF) | 0x3200;		break;
	case 52: value = (val &= 0x00FF) | 0x3300;		break;
	case 53: value = (val &= 0x00FF) | 0x3400;		break;
	case 54: value = (val &= 0x00FF) | 0x3500;		break;
	case 55: value = (val &= 0x00FF) | 0x3600;		break;
	case 56: value = (val &= 0x00FF) | 0x3700;		break;
	case 57: value = (val &= 0x00FF) | 0x3800;		break;
	case 58: value = (val &= 0x00FF) | 0x3900;		break;
	case 59: value = (val &= 0x00FF) | 0x3A00;		break;
	case 60: value = (val &= 0x00FF) | 0x3B00;		break;
	case 61: value = (val &= 0x00FF) | 0x3C00;		break;
	case 62: value = (val &= 0x00FF) | 0x3D00;		break;
	case 63: value = (val &= 0x00FF) | 0x3E00;		break;
	case 64: value = (val &= 0x00FF) | 0x3F00;		break;

	case 65: value = (val &= 0x00FF) | 0x4000;		break;
	case 66: value = (val &= 0x00FF) | 0x4100;		break;
	case 67: value = (val &= 0x00FF) | 0x4200;		break;
	case 68: value = (val &= 0x00FF) | 0x4300;		break;
	case 69: value = (val &= 0x00FF) | 0x4400;		break;
	case 70: value = (val &= 0x00FF) | 0x4500;		break;
	case 71: value = (val &= 0x00FF) | 0x4600;		break;
	case 72: value = (val &= 0x00FF) | 0x4700;		break;
	case 73: value = (val &= 0x00FF) | 0x4800;		break;
	case 74: value = (val &= 0x00FF) | 0x4900;		break;
	case 75: value = (val &= 0x00FF) | 0x4A00;		break;
	case 76: value = (val &= 0x00FF) | 0x4B00;		break;
	case 77: value = (val &= 0x00FF) | 0x4C00;		break;
	case 78: value = (val &= 0x00FF) | 0x4D00;		break;
	case 79: value = (val &= 0x00FF) | 0x4E00;		break;
	case 80: value = (val &= 0x00FF) | 0x4F00;		break;
	case 81: value = (val &= 0x00FF) | 0x5000;		break;
	case 82: value = (val &= 0x00FF) | 0x5100;		break;
	case 83: value = (val &= 0x00FF) | 0x5200;		break;
	case 84: value = (val &= 0x00FF) | 0x5300;		break;
	case 85: value = (val &= 0x00FF) | 0x5400;		break;
	case 86: value = (val &= 0x00FF) | 0x5500;		break;
	case 87: value = (val &= 0x00FF) | 0x5600;		break;
	case 88: value = (val &= 0x00FF) | 0x5700;		break;
	case 89: value = (val &= 0x00FF) | 0x5800;		break;
	case 90: value = (val &= 0x00FF) | 0x5900;		break;
	case 91: value = (val &= 0x00FF) | 0x5A00;		break;
	case 92: value = (val &= 0x00FF) | 0x5B00;		break;
	case 93: value = (val &= 0x00FF) | 0x5C00;		break;
	case 94: value = (val &= 0x00FF) | 0x5D00;		break;
	case 95: value = (val &= 0x00FF) | 0x5E00;		break;
	case 96: value = (val &= 0x00FF) | 0x5F00;		break;
	
	case 97: value = (val &= 0x00FF) | 0x6000;		break;
	case 98: value = (val &= 0x00FF) | 0x6100;		break;
	case 99: value = (val &= 0x00FF) | 0x6200;		break;
	case 100: value = (val &= 0x00FF) | 0x6300;		break;
	case 101: value = (val &= 0x00FF) | 0x6400;		break;
	case 102: value = (val &= 0x00FF) | 0x6500;		break;
	case 103: value = (val &= 0x00FF) | 0x6600;		break;
	case 104: value = (val &= 0x00FF) | 0x6700;		break;
	case 105: value = (val &= 0x00FF) | 0x6800;		break;
	case 106: value = (val &= 0x00FF) | 0x6900;		break;
	case 107: value = (val &= 0x00FF) | 0x6A00;		break;
	case 108: value = (val &= 0x00FF) | 0x6B00;		break;
	case 109: value = (val &= 0x00FF) | 0x6C00;		break;
	case 110: value = (val &= 0x00FF) | 0x6D00;		break;
	case 111: value = (val &= 0x00FF) | 0x6E00;		break;
	case 112: value = (val &= 0x00FF) | 0x6F00;		break;
	case 113: value = (val &= 0x00FF) | 0x7000;		break;
	case 114: value = (val &= 0x00FF) | 0x7100;		break;
	case 115: value = (val &= 0x00FF) | 0x7200;		break;
	case 116: value = (val &= 0x00FF) | 0x7300;		break;
	case 117: value = (val &= 0x00FF) | 0x7400;		break;
	case 118: value = (val &= 0x00FF) | 0x7500;		break;
	case 119: value = (val &= 0x00FF) | 0x7600;		break;
	case 120: value = (val &= 0x00FF) | 0x7700;		break;
	case 121: value = (val &= 0x00FF) | 0x7800;		break;
	case 122: value = (val &= 0x00FF) | 0x7900;		break;
	case 123: value = (val &= 0x00FF) | 0x7A00;		break;
	case 124: value = (val &= 0x00FF) | 0x7B00;		break;
	case 125: value = (val &= 0x00FF) | 0x7C00;		break;
	case 126: value = (val &= 0x00FF) | 0x7D00;		break;
	case 127: value = (val &= 0x00FF) | 0x7E00;		break;
	case 128: value = (val &= 0x00FF) | 0x7F00;		break;
	default: 
		epicsPrintf("\n>>> ozSetChannelMask : channel number wrong! ... fail\n");
		break;
	}
	
	
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_2] = value;
	epicsMutexUnlock(ptaskConfig->bufferLock);
	if( ozWriteRegister(ptaskConfig) == ERROR)	{
		epicsPrintf("\n>>> ozSetChannelMask : ozWriteRegister() ... fail\n");
		return ERROR;
	}

	epicsPrintf("\n>>> ozSetChannelMask : Ctrl_2()taskName:%s, end value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);
	return OK;
}


int ozSetStrobeTTL(drvM6802_taskConfig *ptaskConfig)
{
	unsigned short value;
	value = ptaskConfig->register_m6802[MM_CNTRL_REG_2];
	epicsMutexLock(ptaskConfig->bufferLock);
	ptaskConfig->register_m6802[MM_CNTRL_REG_2] =  ( value | 0x0040 );
	epicsMutexUnlock(ptaskConfig->bufferLock);

	if( ozWriteRegister(ptaskConfig) == ERROR) {
		epicsPrintf("\n>>> ozSetStrobeTTL : ozWriteRegister() ... fail\n");
		return ERROR;
	}
	epicsPrintf("\n>>> ozSetStrobeTTL : Ctrl_2()taskName:%s, value: 0x%X  \n",ptaskConfig->taskName,ptaskConfig->register_m6802[MM_CNTRL_REG_2]);

	return OK;
}
