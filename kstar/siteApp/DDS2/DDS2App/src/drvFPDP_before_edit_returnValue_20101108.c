/****************************************************************************

Module      : drvFPDP.c

Copyright(c): 2007 OLZETEK. All Rights Reserved.

Revision History:
  

*****************************************************************************/


/* Includes */
#include "stdio.h"      /* printf, ... */
#include "stdlib.h"     /* malloc,.. */
#include "asm/page.h"   /* PAGE_SIZE */
#include "pthread.h"    /* pthread_attr_init ,pthread_create, .... */ 
#include "time.h"       /* nanosleep */
#include "unistd.h"     /* sleep, close, ... */
#include <malloc.h>     /* memalign */
#include <sys/time.h>


#include "vmosa.h"

#include "dpio2.h"



#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <getopt.h>

#include <linux/vme_io.h>

#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#include "drvFPDP.h"


#define _DEBUG		1


static DPIO2_INFO_T  info;
DPIO2_DMA    dmaInfo_in;
DPIO2_DMA   *pPageInfo_in; /* = &dmaInfo_in;	 */



	STATUS       status;
	int          devno_in;
	int          freq;
	int	numberOfDpio2Modules;


#if 0
/* Message Queue */
typedef struct {
	long mtype;
	int msgVal;
	int ch;
} myMsgBuf;

int pid;
myMsgBuf msgbuf;
int msgID;
#endif


/* Functions */
/*unsigned long long int drvM6802_getCurrentUsec(void); */


void dmaDoneNotifier (int  pDmaDoneSemaphore)
{
	sem_post ((sem_t*)pDmaDoneSemaphore); 

/*	fprintf(stdout, "in DMA down envent \n"); */
}


STATUS scanForDevices (int* pNumDpio2Modules)
{
  int           numOfDpio2Modules;

  numOfDpio2Modules = dpio2Scan (&info);
  if (numOfDpio2Modules ==  ERROR) {
    printf ("Failed to scan for DPIO2 Devices\n");
    *pNumDpio2Modules = 0;
    return (ERROR);
  }

  if (numOfDpio2Modules < 1) {
    printf("Failed find any DPIO2 Modules\n");
    *pNumDpio2Modules = numOfDpio2Modules;
    return (ERROR);
  }

  *pNumDpio2Modules = numOfDpio2Modules;
  
  return (OK);
}


STATUS openDpio2ForInput (int dpio2DeviceNumber)
{
  STATUS   status;

  status = dpio2Open (dpio2DeviceNumber, DPIO2_INPUT);
  if (status != OK) {
    printf("Failed to initialize the DPIO2 for input\n");
    return (ERROR);
  }

  return (OK);
}


STATUS openDpio2ForOutput (int dpio2DeviceNumber)
{
  STATUS   status;

  status = dpio2Open(dpio2DeviceNumber, DPIO2_OUTPUT);
  if (status != OK) {
    printf("Failed to initialize the DPIO2 for output\n");
    return (ERROR);
  }

  return (OK);
}


STATUS configureDmaDoneInterrupt (int dpio2DeviceNumber, FUNCPTR pFunction, sem_t *pDmaDoneSemaphore)
{

  STATUS   status;
  DPIO2_INTERRUPT_CALLBACK   callbackSetting;

  callbackSetting.condition = DPIO2_INT_COND_DMA_DONE_BLOCK;
  callbackSetting.pCallbackFunction = pFunction;
  callbackSetting.argument = (int) pDmaDoneSemaphore;

  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_INTERRUPT_CALLBACK_ATTACH, (int) &callbackSetting);
  if (status != OK) {
    printf("Failed to attach callback action!\n");
    return (ERROR);
  }


  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_INTERRUPT_ENABLE, DPIO2_INT_COND_DMA_DONE_BLOCK);
  if (status != OK) {
    printf("Failed to enable interrupt!\n");
    return (ERROR);
  }

  return (OK);

}

/* DPIO2_SECONDARY_STROBE to select the PECL strobe. */
STATUS enableStrobeReception (int dpio2DeviceNumber, UINT32 strobeFrequency)
{
  STATUS   status;
  int   range;

  status = dpio2Ioctl (dpio2DeviceNumber, 
		       DPIO2_CMD_STROBE_RECEPTION_ENABLE, 
		       DPIO2_PRIMARY_STROBE);  /*  TTL strobe  */
  if (status != OK) {
    printf("Failed to enable strobe reception\n");
    return (ERROR);
  } 

  /* Determine which of the predefined ranges,
   * the specfied strobe frequency falls within.
   */
  if (strobeFrequency <= (5*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_BELOW_OR_EQUAL_TO_5MHZ;  
  } else  if (strobeFrequency <= (10*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_5MHZ_BELOW_OR_EQUAL_TO_10MHZ;
  } else  if (strobeFrequency <= (15*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_10MHZ_BELOW_OR_EQUAL_TO_15MHZ;
  } else  if (strobeFrequency <= (20*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_15MHZ_BELOW_OR_EQUAL_TO_20MHZ;
  } else  if (strobeFrequency <= (25*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_20MHZ_BELOW_OR_EQUAL_TO_25MHZ;
  } else  if (strobeFrequency <= (30*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_25MHZ_BELOW_OR_EQUAL_TO_30MHZ;
  } else  if (strobeFrequency <= (35*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_30MHZ_BELOW_OR_EQUAL_TO_35MHZ;
  } else  if (strobeFrequency <= (40*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_35MHZ_BELOW_OR_EQUAL_TO_40MHZ;
  } else  if (strobeFrequency <= (45*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_40MHZ_BELOW_OR_EQUAL_TO_45MHZ;
  } else  if (strobeFrequency <= (50*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_45MHZ_BELOW_OR_EQUAL_TO_50MHZ;
  } else  if (strobeFrequency <= (55*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_50MHZ_BELOW_OR_EQUAL_TO_55MHZ;
  } else  if (strobeFrequency <= (60*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_55MHZ_BELOW_OR_EQUAL_TO_60MHZ;
  } else  if (strobeFrequency <= (65*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_60MHZ_BELOW_OR_EQUAL_TO_65MHZ;
  } else  if (strobeFrequency <= (70*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_65MHZ_BELOW_OR_EQUAL_TO_70MHZ;
  } else  if (strobeFrequency <= (75*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_70MHZ_BELOW_OR_EQUAL_TO_75MHZ;
  } else  if (strobeFrequency <= (80*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_75MHZ_BELOW_OR_EQUAL_TO_80MHZ;
  } else  if (strobeFrequency <= (85*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_80MHZ_BELOW_OR_EQUAL_TO_85MHZ;
  } else  if (strobeFrequency <= (90*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_85MHZ_BELOW_OR_EQUAL_TO_90MHZ;
  } else  if (strobeFrequency <= (95*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_90MHZ_BELOW_OR_EQUAL_TO_95MHZ;
  } else  if (strobeFrequency <= (100*1000*1000)) {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_95MHZ_BELOW_OR_EQUAL_TO_100MHZ;
  } else {
    range = DPIO2_STROBE_FREQUENCY_ABOVE_100MHZ;
  }


  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_STROBE_FREQUENCY_RANGE_SET, range);
  if (status != OK) {
    printf("Failed to set %d as strobe frequency range\n", range);
    return (ERROR);
  }

  dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_CLOCKING_ON_BOTH_STROBE_EDGES_SELECT, FALSE);

  return (OK);
}


STATUS activateFpdpInterface (int dpio2DeviceNumber)
{

  STATUS   status;

  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_FPDP_ACTIVATION_SELECT, TRUE);
  if (status != OK) {
    printf ("Failed to activate the FPDP interface\n");
    return (ERROR);
  }
  
  return (OK);
}



STATUS allocateAndLockBuffer (UINT32 lengthInBytes, UINT32 **buffer, 
			      DPIO2_DMA** ppPageInfo)
{
  STATUS   status;
  int      maxNumPages;

  /* Allocate aligned memory buffer */
  *buffer = memalign (lengthInBytes, (size_t)lengthInBytes);
/*  *buffer = memalign (256, (size_t)lengthInBytes); */
  if (*buffer == NULL) 
  {
    printf("Failed to allocate %d bytes\n", (int) lengthInBytes);
    return (ERROR);
  }


  /* Determine the maximum number of pages the buffer can be mapped to.
   * One is added at the end of the computation to account for the fact
   * that the buffer may not be aligned to a page boundary. */
  maxNumPages = ((lengthInBytes + PAGE_SIZE - 1) / PAGE_SIZE) + 1;

#if 1
	printf(" maxNumPages= %d, PAGE_SIZE= %lu\n", maxNumPages, PAGE_SIZE);
#endif

  /* Allocate structure to store information about all the physical pages
   * the buffer maps to.
   */
  *ppPageInfo = malloc (sizeof(DPIO2_DMA) + maxNumPages * sizeof(DPIO2_DMA_PAGE));
  if (*ppPageInfo == NULL) 
  {
    printf("Failed to allocate Page Information structure\n");
    free (*buffer);
    return (ERROR);
  }
#if _DEBUG
	printf(" sizeof(DPIO2_DMA)= %d, sizeof(DPIO2_DMA_PAGE)= %d\n", sizeof(DPIO2_DMA), sizeof(DPIO2_DMA_PAGE));
#endif


  /* Lock the buffer into physical memory.
   */
  (*ppPageInfo)->pUserAddr = *buffer;
  (*ppPageInfo)->dwBytes = lengthInBytes;
  (*ppPageInfo)->dwPages = maxNumPages;
  status = dpio2DMALock (*ppPageInfo);
  if (status != OK) {
    printf("Failed to lock buffer into physical memory\n");
    free (*ppPageInfo);
    free (*buffer);
    return (ERROR);
  }

  return (OK);
}

#if 0
STATUS configureDmaChain(int dpio2DeviceNumber, DPIO2_DMA* pPageInfo, int desc_num)
{
  STATUS   status;
  UINT32   iPage;
  DPIO2_DMA_DESC   dmaDescriptor;


  if (( pPageInfo->dwBytes & 0x00000003 ) != 0) {
    printf("Length must be multiple of 4 bytes\n");
    return (ERROR);
  } 

  if (( pPageInfo->Page[0].pPhysicalAddr & 0x00000003) != 0) {
    printf("PCI Address must be aligned to 4 bytes boundary\n");
    return (ERROR);
  }
  
  for (iPage = 0; iPage < (pPageInfo->dwPages - 1); iPage++) {
    dmaDescriptor.descriptorId        = iPage + desc_num;
    dmaDescriptor.nextDescriptorId    = iPage + desc_num + 1;
    dmaDescriptor.pciAddress       
      = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
    dmaDescriptor.blockSizeInBytes    
      = pPageInfo->Page[iPage].dwBytes;
    dmaDescriptor.lastBlockInChain    = FALSE;
    dmaDescriptor.useD64              = TRUE;
    dmaDescriptor.notEndOfFrame       = FALSE;
    dmaDescriptor.endOfBlockInterrupt = FALSE;
/*
printf(" dmaDescriptor blockSizeInBytes= %d, pciAddress= 0x%X\n", dmaDescriptor.blockSizeInBytes, dmaDescriptor.pciAddress);
*/
    status = dpio2Ioctl(dpio2DeviceNumber, 
                        DPIO2_CMD_DMA_SET_DESC, 
                        (int) &dmaDescriptor);
    if (status != OK) {
      printf("Failed to set DMA Descriptor\n");
      return (ERROR);
    }
  }
  
  dmaDescriptor.descriptorId        = iPage + desc_num;
/*  dmaDescriptor.nextDescriptorId    = 0; */
  dmaDescriptor.nextDescriptorId    = 0;
  dmaDescriptor.pciAddress          
    = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
  dmaDescriptor.blockSizeInBytes    
    = pPageInfo->Page[iPage].dwBytes;
  dmaDescriptor.lastBlockInChain    = FALSE;
  dmaDescriptor.useD64              = TRUE;
  dmaDescriptor.notEndOfFrame       = FALSE;
  dmaDescriptor.endOfBlockInterrupt = TRUE;

  status = dpio2Ioctl(dpio2DeviceNumber, 
                      DPIO2_CMD_DMA_SET_DESC, 
                      (int) &dmaDescriptor);
  if (status != OK) {
    printf("Failed to set DMA Descriptor\n");
    return (ERROR);
  }

  return (OK);
}
#endif

#if 1
STATUS configureDmaChain(int dpio2DeviceNumber, DPIO2_DMA* pPageInfo, int desc_num)
{
  STATUS   status;
  UINT32   iPage;
  DPIO2_DMA_DESC   dmaDescriptor;


  if (( pPageInfo->dwBytes & 0x00000003 ) != 0) {
    printf("Length must be multiple of 4 bytes\n");
    return (ERROR);
  } 

  if (( pPageInfo->Page[0].pPhysicalAddr & 0x00000003) != 0) {
    printf("PCI Address must be aligned to 4 bytes boundary\n");
    return (ERROR);
  }
  
  for (iPage = 0; iPage < ((pPageInfo->dwPages/2) - 1); iPage++) { 
/*  for (iPage = 0; iPage < ((pPageInfo->dwPages/2) + 8); iPage++) { */
/*  for (iPage = 0; iPage < 576; iPage++) { */
    dmaDescriptor.descriptorId        = iPage + desc_num;
    dmaDescriptor.nextDescriptorId    = iPage + desc_num + 1;
    dmaDescriptor.pciAddress       
      = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
    dmaDescriptor.blockSizeInBytes    
      = pPageInfo->Page[iPage].dwBytes;
    dmaDescriptor.lastBlockInChain    = FALSE;
    dmaDescriptor.useD64              = TRUE;
    dmaDescriptor.notEndOfFrame       = FALSE;
    dmaDescriptor.endOfBlockInterrupt = FALSE;

    status = dpio2Ioctl(dpio2DeviceNumber, 
                        DPIO2_CMD_DMA_SET_DESC, 
                        (int) &dmaDescriptor);
    if (status != OK) {
      printf("Failed to set DMA Descriptor\n");
      return (ERROR);
    }
  }
printf(" iPage= %d,  blockSizeInBytes= %d, pciAddress= 0x%X\n", iPage, dmaDescriptor.blockSizeInBytes, dmaDescriptor.pciAddress);
  
  dmaDescriptor.descriptorId        = iPage + desc_num;
  dmaDescriptor.nextDescriptorId    = iPage + desc_num + 1;
  dmaDescriptor.pciAddress          
    = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
  dmaDescriptor.blockSizeInBytes    
    = pPageInfo->Page[iPage].dwBytes;
  dmaDescriptor.lastBlockInChain    = FALSE;
  dmaDescriptor.useD64              = TRUE;
  dmaDescriptor.notEndOfFrame       = FALSE;
  dmaDescriptor.endOfBlockInterrupt = TRUE;

  status = dpio2Ioctl(dpio2DeviceNumber, 
                      DPIO2_CMD_DMA_SET_DESC, 
                      (int) &dmaDescriptor);
  if (status != OK) {
    printf("Failed to set DMA Descriptor\n");
    return (ERROR);
  }


	iPage += 1;


 for (; iPage < (pPageInfo->dwPages - 1) ; iPage++) {
    dmaDescriptor.descriptorId        = iPage + desc_num;
    dmaDescriptor.nextDescriptorId    = iPage + desc_num + 1;
    dmaDescriptor.pciAddress       
      = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
    dmaDescriptor.blockSizeInBytes    
      = pPageInfo->Page[iPage].dwBytes;
    dmaDescriptor.lastBlockInChain    = FALSE;
    dmaDescriptor.useD64              = TRUE;
    dmaDescriptor.notEndOfFrame       = FALSE;
    dmaDescriptor.endOfBlockInterrupt = FALSE;
/*
printf(" dmaDescriptor blockSizeInBytes= %d, pciAddress= 0x%X\n", dmaDescriptor.blockSizeInBytes, dmaDescriptor.pciAddress);
*/
    status = dpio2Ioctl(dpio2DeviceNumber, 
                        DPIO2_CMD_DMA_SET_DESC, 
                        (int) &dmaDescriptor);
    if (status != OK) {
      printf("Failed to set DMA Descriptor\n");
      return (ERROR);
    }
  }

printf(" iPage= %d,  blockSizeInBytes= %d, pciAddress= 0x%X\n", iPage, dmaDescriptor.blockSizeInBytes, dmaDescriptor.pciAddress);  

  dmaDescriptor.descriptorId        = iPage + desc_num;
  dmaDescriptor.nextDescriptorId    = 0;
  dmaDescriptor.pciAddress          
    = (UINT32) pPageInfo->Page[iPage].pPhysicalAddr;
  dmaDescriptor.blockSizeInBytes    
    = pPageInfo->Page[iPage].dwBytes;
  dmaDescriptor.lastBlockInChain    = FALSE;
  dmaDescriptor.useD64              = TRUE;
  dmaDescriptor.notEndOfFrame       = FALSE;
  dmaDescriptor.endOfBlockInterrupt = TRUE;

  status = dpio2Ioctl(dpio2DeviceNumber, 
                      DPIO2_CMD_DMA_SET_DESC, 
                      (int) &dmaDescriptor);
  if (status != OK) {
    printf("Failed to set DMA Descriptor\n");
    return (ERROR);
  }

  return (OK);
}

#endif

STATUS startDmaTransfer (int dpio2DeviceNumber, int desc_num)
{
  STATUS   status;

  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_DMA_SET_START_DESCRIPTOR, desc_num);
  if (status != OK) {
    printf("Failed to set start descriptor\n");
    return (ERROR);
  }

  status = dpio2Ioctl (dpio2DeviceNumber, DPIO2_CMD_DMA_START, 0);
  if (status != OK) {
    printf("Failed to start DMA transfer\n");
    return (ERROR);
  }

  return (OK);
}


void unlockAndFreeBuffer (DPIO2_DMA* pPageInfo, UINT32 *buffer)
{
  /* Unlock the pages of physical memory that the buffer maps. */
  dpio2DMAUnlock (pPageInfo);

  /* Free the page information structure and the buffer. */
  free (pPageInfo);

  free (buffer);
}


void closeDevice(int dpio2DeviceNumber)
{
  dpio2Close(dpio2DeviceNumber);
}



/*
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
/	main	//	main	//	main	//	main	//	main	//	main	//	main	/
*/

int drvM6802_init_FPDP()
{

	pPageInfo_in = &dmaInfo_in;	
/*	cnt_DMAcallback=0; */


	devno_in  = 1;

	freq = 20 * 1000 * 1000;
	dmaSize = 1024*1024*3;


	status = scanForDevices (&numberOfDpio2Modules);
	if (status != OK) {
		return (-1);
	}
#if _DEBUG
	printf("Found %d DPIO2 modules in system\n", numberOfDpio2Modules);
#endif


/* Initialize the device for input.*/
	status = openDpio2ForInput(devno_in);
	if (status != OK) {
		printf("Failed to initialize device number %d for input\n", devno_in);
		return (-1);
	}

	/* Init semaphore */
	sem_init (&dmaDoneSemaphore_in,  0, 0);


	/* Configure interrupts  */
	status = configureDmaDoneInterrupt(devno_in, (FUNCPTR) dmaDoneNotifier, &dmaDoneSemaphore_in);
	if (status != OK) {
		printf("Failed to configure DMA Done Interrupt\n");
		return (-1);
	}

	/* Configure strobe reception. */
	status = enableStrobeReception(devno_in, freq);
	if (status != OK) {
		printf("Failed to configure Strobe Reception\n");
		return (-1);
	}

  /* Set no swap */
	dpio2Ioctl (devno_in,  DPIO2_CMD_DATA_SWAP_MODE_SELECT, DPIO2_8BIT_SWAP);


	status = dpio2Ioctl(devno_in, DPIO2_CMD_DATA_PACKING_PIPELINE_FLUSH, 0);

/* Flush FIFO */
	status = dpio2Ioctl (devno_in,  DPIO2_CMD_FIFO_FLUSH, 0 );
	if (status != OK) {
		printf("Failed to FIFO flush\n");
		return (-1);
	}

  /* Allocate buffer. */
  status = allocateAndLockBuffer ((UINT32)dmaSize, &buffer_in, &pPageInfo_in);
  if (status != OK) {
    printf("Failed to allocate and lock buffer\n");
    return (-1);
  }

	buffer_A = (int*)malloc(dmaSize/2);
	buffer_B = (int*)malloc(dmaSize/2);
	buffer_write = (int*)malloc(dmaSize);



  /* Configure DMA chain.  */
  status = configureDmaChain (devno_in, pPageInfo_in, 0);
  if (status != OK) {
    printf ("Failed to configure DMA chain\n");
    unlockAndFreeBuffer (pPageInfo_in, (UINT32 *)buffer_in);
    return (-1);
  }


	return (1);
}

void doCacheInvalidate()
{
	dpio2CacheInvalidate (pPageInfo_in);
}


#if 0
void* thread_function(void *arg)
{

	while(1) {
		msgrcv(msgID, &msgbuf, 8, 1, 0);
		msgFPDP = msgbuf.msgVal;

		printf("example: Type: %ld  msg: %d\n", msgbuf.mtype, msgbuf.msgVal);

		if( msgFPDP == FPDP_STOP ) {
			printf("FPDP >>> got msg STOP\n");

		} else if( msgFPDP == FPDP_START ) {
			if( fpRaw != NULL ) fclose(fpRaw);
			ch = msgbuf.ch;

			fpRaw = fopen(FILE_NAME_RAWDATA, "w");
			if( fpRaw == NULL ) {
				printf(" can't open temp.dat \n");
				exit(0);
			}			
			fprintf(stdout, "FPDP >>> got msg START, ch=%d\n", ch);

		} else if ( msgFPDP == FPDP_KILL ) {
/*			if( startFpdp() != OK ) {
				printf(" can't start FPDP \n");
				terminate();
				break;
			}
*/
			printf("FPDP >>> got msg KILL\n");
			break;
		}
	
	} /* while(1) */
	printf("FPDP >>> exit thread\n");
}
#endif

int startFpdp()
{
	status = dpio2Ioctl(devno_in, DPIO2_CMD_DATA_PACKING_PIPELINE_FLUSH, 0);
/* Flush FIFO */
	status = dpio2Ioctl (devno_in,  DPIO2_CMD_FIFO_FLUSH, 0 );
	if (status != OK) {
		printf("Failed to FIFO flush\n");
		return (0);
	}
	
	status = activateFpdpInterface (devno_in);
	if (status != OK) 
	{
		printf("Failed to activate FPDP interface\n");
		return (0);
	}
	
	status = startDmaTransfer (devno_in, 0);
	if (status != OK) 
	{
		printf ("Failed to start DMA transfer\n");
		return (0);
	}
	
	return (1);
}

int stopFpdp()
{
	
	status = dpio2Ioctl(devno_in, DPIO2_CMD_DMA_ABORT, 0);
	if (status != OK) {
		printf("Failed to Abort DMA transfer\n");
		return (0);
	}
	
	status = dpio2Ioctl (devno_in, DPIO2_CMD_FPDP_ACTIVATION_SELECT, FALSE);
	if (status != OK) {
		printf ("Failed to activate the FPDP interface\n");
		return (0);
	}
	
	status = dpio2Ioctl(devno_in, DPIO2_CMD_DATA_PACKING_PIPELINE_FLUSH, 0);

/* Flush FIFO */
	status = dpio2Ioctl (devno_in,  DPIO2_CMD_FIFO_FLUSH, 0 );
	if (status != OK) {
		printf("Failed to FIFO flush\n");
		return (0);
	}
	
	return (1);
}

void terminate()
{
	/* Free buffer. */
	unlockAndFreeBuffer (pPageInfo_in,  (UINT32 *)buffer_in);
  
	/* Close device   */
	closeDevice (devno_in);

	free(buffer_A);
	free(buffer_B);
	free(buffer_write);
	
	printf("FPDP >>> Terminated!\n");
}
