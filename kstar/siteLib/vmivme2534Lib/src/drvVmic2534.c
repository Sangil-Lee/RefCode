/* drvVmi2534.c */

/*
 *	routines used to test and interface with V.M.I.C. VMIVME-2534
 *	digital i/o module
 *
 * 	Author:      Bill Brown
 * 	Date:        03/22/94
 *	Experimental Physics and Industrial Control System (EPICS)
 *
 *	Copyright 1994, the Regents of the University of California,
 *	and the University of Chicago Board of Governors.
 *
 *	This software was produced under  U.S. Government contracts:
 *	(DE-AC03-76SF00) at Lawrence Berkeley Laboratory.
 *	(W-7405-ENG-36) at the Los Alamos National Laboratory,
 *	and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *	Initial development by:
 *		Control System Group
 *		Advanced Light Source
 *		Lawrence Berkeley Laboratory
 *
 *	Co-developed with
 *		The Controls and Automation Group (AT-8)
 *		Ground Test Accelerator
 *		Accelerator Technology Division
 *		Los Alamos National Laboratory
 *
 *		The Controls and Computing Group
 *		Accelerator Systems Division
 *		Advanced Photon Source
 *		Argonne National Laboratory
 *
 *	NOTES:
 *	    This code is/was cloned from "drvXy240.c" of EPICS R3.11
 *
 *	    Data direction is dynamically assigned at ioInit() time.
 *	    The definition of the first bit(s) in a given byte will
 *	    determine the data direction of all bits within that byte.
 *
 * Modification Log:
 *
 *
    @(#) drvVmi2534.c,v
    @(#) Revision 1.1  1996/01/09 19:54:35  lewis
    @(#) Add PAL record.  Remove APS eg/er records.
    @(#) Add 4 STAR drivers, LBL 2534, and preliminaries for LBL/SLAC 9210/9325 drivers.
    @(#)
 * Revision 1.1.1.1  1995/07/25  21:49:57  lewis
 * Baseline import R3.12 w/LBL drivers and mods.
 *
 * Revision 1.5  1994/09/28  09:05:51  wbrown
 * at beginning of init function, ctrl reg is set to match
 *   SYS_RESET* condition.
 *
 * Revision 1.4  94/09/27  09:44:31  wbrown
 * changed order of "writes to '2534 hardware during initialization
 * to prevent undesired output transistions when calculating
 * "output correction factor."
 * 
 * Revision 1.3  94/08/19  15:16:14  wbrown
 * added debug #ifdef, printf statements
 * fixed initialization of record definition structure
 * 
 * Revision 1.2  94/07/20  09:25:26  wbrown
 * modified to dynamically assign data direction at
 *   record init time.
 * 
 * Revision 1.1  94/05/27  09:57:51  wbrown
 * Initial revision
 * 
 * 
 */

#include <vxWorks.h>
#include <stdio.h>
#include <sysLib.h>
#include <logLib.h>
#include <taskLib.h>
#include <wdLib.h>
#include <types.h>
#include <vme.h>
#include <vxLib.h>
#include <tickLib.h>
#include <rebootLib.h>

#include  "dbDefs.h"
#include  "errlog.h"
#include  "errlog.h"
                                                               /* #include "module_types.h"     */
#include "task_params.h"
#include "epicsThread.h"
#include "dbAccess.h"
#include "drvSup.h"
#include "dbDefs.h"
#include "dbScan.h"
#include "taskwd.h"
#include "devLib.h"                                            /* for devRegisterAddress         */
#include "vmic2534.h"

/* #include "fast_lock.h" */                                       /* for FASTLOCK,etc */

#include "epicsMutex.h"
#include "epicsExport.h"


#if (CPU_FAMILY==PPC)
# define EIEIO       __asm__ volatile ("eieio")
# define EIEIO_SYNC  __asm__ volatile ("eieio;sync")
#define SYNC         __asm__ volatile ("sync")
#else
#define SYNC 
#define EIEIO
#define EIEIO_SYNC 
#endif 

int debugthis = 0;

#define VMIC_2534_ADDR0     0xc00                              /* (bi_addrs[VMIC_2534_BI])        */
#define VMIC_2534_MAX_CARDS 2                                  /* (bi_num_cards[VMIC_2534_BI])    */
#define VMIC_2534_MAX_CHANS 32                                 /* (bi_num_channels[VMIC_2534_BI]) */
#define IOR_MAX_COLS        4

#define VMIC_2534_NAME		"vmic2534IoScan"
#define VMIC_2534_PRI		50
#define VMIC_2534_OPT		NULL
#define VMIC_2534_STACK		4096




/*  control-status register
	bit definiitions						 */

#define BYTE_0_OUT_ENA	1<<0
#define BYTE_1_OUT_ENA	1<<1
#define BYTE_2_OUT_ENA	1<<2
#define BYTE_3_OUT_ENA	1<<3

#define FAIL_LED_OFF	1<<5
#define TM1_ENABLE	1<<7
#define TM2_ENABLE	1<<6

/*  Refer to VMIVME-2534 manual for hardware options			 */
/*	These boards are available in two configurations;		 */
/*	"POSITIVE-TRUE Option" - writing a "1" to the output register	 */
/*	    "TURNS ON the output driver, ie pulls the output LOW	 */
/*	"NEGATIVE-TRUE Option" - writing a "1" to the output register	 */
/*	    "TURNS OFF" the output driver, allowing the load to pull	 */
/*	    the output HIGH.						 */

/*	The initialization routine recognizes which type of board is	 */
/*	installed and generates a mask which is used by the device-	 */
/*	driver to cause BOTH board types to appear to have the		 */
/*	POSITIVE-TRUE Option installed.					 */

#define FAIL_FLAG	1<<5
				/*  Test Mode				 */
#define TM2		1<<6	/*  TM2 - "1" = disable test buffers	 */
#define TM1		1<<7	/*  TM1 - "1" = enable output drivers	 */

#define masks( K ) ((1<<K))


/*  VMIVME-2534 memory structure					 */

/*	    Data direction is dynamically assigned at ioInit() time.	 */
/*	    The definition of the first bit(s) in a given byte will	 */
/*	    determine the data direction of all bits within that byte.	 */


typedef struct
    {
    volatile u_long	pioData;
    volatile u_char	brdIdReg;	
    volatile u_char	ctlStatReg;
    volatile u_char	endPad[2];
    } VMIC_2534 ;


/*  VMIVME-2534 control structure record				 */

typedef struct
    {
    VMIC_2534 *vmicDevice;	/*  pointer to the hardware		 */
    short	deviceNr;	/*  device number			 */
    u_long	savedInputs;	/*  previous input value		 */
    u_long      savedOutputs;   /*  previous output value                */
    u_long	outputCorrector;	/*  polarity correction factor	 */
    u_long	inputMask;	/*  and-mask for input bytes		 */
    u_long	outputMask;	/*  and-mask for output bytes		 */
    IOSCANPVT	ioscanpvt;
    int         present;        /* 0=module not present, 1=module present */
    char       *sysBusAdrs;     /* system Bus A16 address                 */
/*
    FAST_LOCK   lock;
*/
    epicsMutexId lock;

    } VMIC_2534_REC ;


static short   vmic_2534_num_cards = 0;
VMIC_2534_REC *vmic2534Rec;	/*  points to control structures	 */

/* Local Prototypes */
static long report(int level);
static long init(void );


struct 
    {
    long	number;
    DRVSUPFUN	report;
    DRVSUPFUN	init;
    }
     drvVmic2534 =
	{
	2,
	report,
	init
	} ;

epicsExportAddress(drvet,drvVmic2534);


static long report(int level)
    {
    vmic2534_io_report( level );
    return( 0 );
    }


static long init(void)
    {
    vmic2534Init();
    return( 0 );
    }

/*
 * This function is called once per VMIC 2534 module,
 * from the vxWorks shell before iocInit.
 * Its function is assign a card number (referenced in 
 * the pv) with each module base address and to verify
 * that the address is valid and the module is present.
 * The actual module initialization is NOT performed here
 * but rather in the driver initialization task vmic2534Init.
 * Below is an example as to how this function should be called
 * from a vxWorks script.
 *
 *  ex) 
 *        vmic2534_init( 0,0xc00 )
 *  This will setup a local structure indicating that 
 *   card 0 is located at the A16 system Bus adddress of 0xc00
 *  A maximum of two TDC cards (0 and 1) is allowed per crate.
 *  If this is not enough the user will have to change the
 *  following definition found in this file to reflect
 *  the maximum number of cards allowed per crate.
 *
 *    #define VMIC_2534_MAX_CARDS 2
 *
 *  Once the symbol has been changed the souce will
 *  need tobe recompiled and the library relinked.
 *
 */
int vmic2534_init( int card, unsigned long busAdrs )
{
    int         status    = ERROR;
    uchar_t     bdata     = 0;
    size_t      max_cards = VMIC_2534_MAX_CARDS;
    size_t      elemSize  = sizeof(VMIC_2534_REC);
    ulong_t     dummy;
    VMIC_2534  *theHardware;



    /*
     *  allow for runtime reconfiguration of the addr map
     */
    if ( !vmic2534Rec ) {
       vmic2534Rec = (VMIC_2534_REC *)calloc(max_cards,elemSize);
       if ( !vmic2534Rec ) {
          logMsg("Attempt to calloc \"VMIC_2534_REC\" failed\n",0,0,0,0,0,0);
	  return( status );
       }
    }
    /* Valid A16 address? */
    status = sysBusToLocalAdrs( VME_AM_SUP_SHORT_IO,(char *)busAdrs,(char **)&theHardware );
    if ( status != OK ) {
      logMsg("Unable to map VMIC_2534 card %d A16 base addr\n",card,0,0,0,0,0);
      return(status);
    }
    /* Is the module there? */
    status = vxMemProbe( (char *)theHardware, READ,sizeof(dummy),(char *)&dummy ); 
    if ( status!=OK ) {
       vmic2534Rec[card].vmicDevice = NULL;
       logMsg("VMIC 2534 Card %d not found at A16 addr 0x%8.8x\n",card,(int)theHardware,0,0,0,0);
       return(status);
    }
    /* Valid card number? */
    if ( (card<0) || (card>=max_cards) ) {
       logMsg("VMIC 2534 card number %d is Out-of-Range (0-%d)\n",card,max_cards-1,0,0,0,0);
       return(status);
    }
    /*  SPECIAL FOR POWER PC:
     *  This is done in the driver initialization routine because
     *  some PowerPC's don't always give a bus error when
     *  READing an address that does not exist. However, the same is
     *  not the case for a WRITE operation.
     *
     *  initialize hardware			      
     *  Outputs/Inputs are undetermined at this time		
     *  Disable output drivers in case of no SYS_RESET*
     */
    bdata = 0;
    status = vxMemProbe((char *)&theHardware->ctlStatReg,WRITE,sizeof(bdata),(char *)&bdata ); 
    if ( status!=OK ) {
       vmic2534Rec[card].vmicDevice = NULL;
       logMsg("VMIC 2534 Card %d not found at A16 addr 0x%8.8x\n",card,(int)theHardware,0,0,0,0);
    }
    else {
       vmic2534Rec[card].present    = TRUE;
       vmic2534Rec[card].sysBusAdrs = (char *)busAdrs;
       vmic2534Rec[card].vmicDevice = theHardware;
/*
       FASTLOCKINIT( &vmic2534Rec[card].lock );
*/
	vmic2534Rec[card].lock = epicsMutexCreate();

       vmic_2534_num_cards++;
    }
    return(status);
}

/*
 *
 *  This hardware does not support interrupts
 *
 *  NO interrupt service routine is applicable
 */


/*
 *
 *  vmic2534IOScan
 *
 *  task to check of a change of state
 *	(used to simulate "change-of-state" interrupts)
 *
 */

int vmic2534IOScan(void)
    {
    int	i;
    int	firstScan = TRUE;
    volatile long temp;

    for ( ; ; )
	{
	if ( interruptAccept ) break;
	epicsThreadSleep(.05);
	}

    for ( ; ; )
	{
	for ( i = 0; i < VMIC_2534_MAX_CARDS; i++ )
	    {
	    if ( vmic2534Rec[ i ].vmicDevice )
		{

		epicsMutexLock(vmic2534Rec[i].lock);

		temp = ( vmic2534Rec[ i ].vmicDevice->pioData )
			& ( vmic2534Rec[ i ].inputMask );

		if(vmic2534Rec[i].vmicDevice->ctlStatReg & (u_char) FAIL_FLAG) 
			vmic2534Rec[i].vmicDevice->ctlStatReg &= ~((u_char) FAIL_FLAG);
		else	vmic2534Rec[i].vmicDevice->ctlStatReg |= (u_char) FAIL_FLAG;

		if ( (temp ^ vmic2534Rec[ i ].savedInputs) || firstScan )
		    {
		    scanIoRequest( vmic2534Rec[ i ].ioscanpvt );
		    vmic2534Rec[ i ].savedInputs = temp;
		    }

		epicsMutexUnlock(vmic2534Rec[i].lock);
		}
	    }

	if ( firstScan )
	    firstScan = FALSE;

        epicsThreadSleep(.05);

	}
        return(OK);
    }


/*
 * DIO DRIVER INIT
 *
 */

int vmic2534Init(void)
    {
    epicsThreadId       scanTaskId;
    int                 dummy;
    register short	i;
    VMIC_2534		*theHardware;
    int			status;
    char               *pbase;
    size_t              bcnt = sizeof(VMIC_2534);
    int			atLeastOnePresent = FALSE;
    char                name_c[32];


    if ( !vmic_2534_num_cards ) return(OK);
   
    for ( i = 0; i < VMIC_2534_MAX_CARDS; i++ )
	{
	if ( !vmic2534Rec[i].present ) continue;

	/* Register module address space with EPICS - verify that no conflicts exists */
        sprintf(name_c,"Vmic 2534 Card %hd",i);
        pbase = vmic2534Rec[i].sysBusAdrs;
        if ( devRegisterAddress(name_c,atVMEA16,(void *)pbase,bcnt,(void **)&theHardware)!=OK ) 
	  {
            logMsg("Failed to register Vmic2534 (A16) %8p, bcnt=0x%x w/EPICS - C=%hd\n",
                  (int)theHardware,(int)bcnt,(int)i,0,0,0);
            vmic2534Rec[i].present = FALSE;
            vmic2534Rec[i].vmicDevice = NULL;
            vmic_2534_num_cards--;
	  }
        else
	  {
	    /*  determine "sense" of output and save correction factor	 */
            vmic2534Rec[i].savedOutputs = 0;
	    theHardware->pioData = 0;
	    vmic2534Rec[i].outputCorrector = theHardware->pioData;

	    /*  clear input & output masks, (malloc'ed mem may != 0)	 */
	    vmic2534Rec[i].inputMask = 0;
	    vmic2534Rec[i].outputMask = 0;

	    /*  insure that outputs are initially "OFF", i.e. HIGH	 */
            vmic2534Rec[i].savedOutputs =  vmic2534Rec[i].outputCorrector;
	    theHardware->pioData    = vmic2534Rec[i].outputCorrector;

            if ( debugthis>1 ) 
                   printf("VMIC  2534: Card %d INSURE OUTPUTS ARE INITIALLY OFF (HIGH) 0x%lx\n",
                           i,
                           vmic2534Rec[i].savedOutputs );
	    /* theHardware->ctlStatReg = TM1 | TM2 | FAIL_LED_OFF; */

             theHardware->ctlStatReg = ~(TM1 | TM2 | FAIL_LED_OFF);
	
	    /*
	     *  record structure initalization 
	     */
	    atLeastOnePresent = TRUE;
	    scanIoInit( &(vmic2534Rec[ i ].ioscanpvt) );
	  }
	}/* End of FOR loop */

    if ( atLeastOnePresent)
	{


	scanTaskId = epicsThreadCreate( VMIC_2534_NAME,
				        epicsThreadPriorityHigh,
					epicsThreadGetStackSize(epicsThreadStackSmall),
					(EPICSTHREADFUNC) vmic2534IOScan,
					&dummy);

        }

	
    return( OK );
    }

/*
 * Kill spawned processes beofre reboot takes place
 */
void vmic2534Shutdown(int startType)
{

     int tid;

     tid = taskNameToId( VMIC_2534_NAME );
     if ( tid!=ERROR )
     {
       taskwdRemove( tid );
       taskDelete( tid );
     }
     return;
}


/*
 *  Set data direction control for specified data bits
 *  Note:
 *    Data direction is controlled on a "per-byte" basis (Jumper JP8 & JP9).
 *    The first bit defined within a byte sets the direction for the entire
 *      byte.  Any attempt to define conflicting bits after the direction
 *      for a byte is set will return an error (-1) response.)
 *    Permmissable values for "direction" are:
 *      direction == 0 indicates input
 *      direction != 0 indicates output
 *
 */
long vmic2534_setIOBitDirection( short card, u_long mask, u_long direction )
    {
    long	status = 0;
    u_long	j;

    static u_long maskTbl[ ] =
		    { 0x000000ff,
		      0x0000ff00,
		      0x00ff0000,
		      0xff000000
		    };
    static u_char portTbl[] = {0x8,0x4,0x2,0x1};



    if ( !vmic_2534_num_cards || (card >= VMIC_2534_MAX_CARDS) || !(vmic2534Rec[ card ].vmicDevice) )
	status = -2;
    else
	{
	if ( direction == 0)
	    {
	    /*  INPUT	 */
#ifdef DEBUG
		    printf("Mask = %08x, outPutMask =%08lx\n",
			    mask, vmic2534Rec[ card ].outputMask);
#endif
	    if ( ((u_long)mask & vmic2534Rec[ card ].outputMask) == 0 )
		{
		for ( j = 0; j < 4; j++ )
		    {

		    if ( ( (u_long)mask & maskTbl[ j ] ) != 0 )
			{
			vmic2534Rec[ card ].inputMask |= maskTbl[ j ];
#ifdef DEBUG
			printf("inputMask = %08lx\n",
				vmic2534Rec[ card ].inputMask);
#endif
                        /*
			vmic2534Rec[ card ].vmicDevice->ctlStatReg
				&= (u_char) ~(portTbl[ j ]);
                        */
                        vmic2534Rec[ card ].vmicDevice->ctlStatReg
                                |= (u_char) (portTbl[ j ]);

			}
		    }
		}
	    else
		{
		status = -1;
		}
	    }
	else
	    {
	    /*  OUTPUT	 */
#ifdef DEBUG
		    printf("Mask = %08x, inputMask =%08lx\n",
			    mask, vmic2534Rec[ card ].inputMask);
#endif
	    if ( ((u_long)mask & vmic2534Rec[ card ].inputMask) == 0 )
		{
		for ( j = 0; j < 4; j++ )
		    {
		    if ( ( (u_long)mask & maskTbl[ j ] ) != 0 )
			{
			vmic2534Rec[ card ].outputMask |= maskTbl[ j ];
#ifdef DEBUG
			printf("outputMask = %08lx\n",
				vmic2534Rec[ card ].outputMask);
#endif
                        /*
			vmic2534Rec[ card ].vmicDevice->ctlStatReg
				|= (u_char)(portTbl[ j ]);
                        */
                        vmic2534Rec[ card ].vmicDevice->ctlStatReg
                                &= (u_char) ~(portTbl[ j ]);
			}
		    }
		}
	    else
		{
		status = -1;
		}
	    }
	}
    return( status );
    }

int vmic2534_getioscanpvt( short card, IOSCANPVT *scanpvt )
    {
    if ( !vmic_2534_num_cards || (card >= VMIC_2534_MAX_CARDS) || !(vmic2534Rec[ card ].vmicDevice) )
	return( ERROR );
    *scanpvt = vmic2534Rec[ card ].ioscanpvt;
    return( 0 );
    }


/*
 *  VMEVMIC_BI_DRIVER
 *
 *  interface binary inputs
 */

long vmic2534_bi_driver( short     card,
                        u_long	  mask,
	                u_long	* prval )
    {
    volatile u_long temp;

    if ( !vmic_2534_num_cards || (card >= VMIC_2534_MAX_CARDS) || !(vmic2534Rec[ card ].vmicDevice) )
	return( ERROR );

    temp = vmic2534Rec[ card ].vmicDevice->pioData;
    EIEIO_SYNC;
    *prval = (u_int)(temp & vmic2534Rec[ card ].inputMask ) & mask;
    EIEIO_SYNC;
    return( 0 );
    }


/*
 *  vmic2534_bo_read
 *
 *  (read) interface to binary outputs
 *
 */

long vmic2534_bo_read( short   card,u_long   mask,u_long * prval )
    {
    volatile u_long temp;

    if ( !vmic_2534_num_cards || (card >= VMIC_2534_MAX_CARDS) || !(vmic2534Rec[ card ].vmicDevice) )
	return( ERROR );

     temp = vmic2534Rec[ card ].vmicDevice->pioData;
     EIEIO_SYNC;
    *prval = (u_int)(~((temp & (vmic2534Rec[ card ].outputMask))
		^ vmic2534Rec[card].outputCorrector)) & mask;
     EIEIO_SYNC;
    return( 0 );
    }

                  
/*
 *  vmic2534_bo_driver
 *
 *  (write) interface to binary outputs
 */
long vmic2534_bo_driver( short  card,u_long  val,u_long  mask )
{
    if ( !vmic_2534_num_cards || (card >= VMIC_2534_MAX_CARDS) || !(vmic2534Rec[ card ].vmicDevice) )
	return( ERROR );
    

    /* FASTLOCK( &vmic2534Rec[ card ].lock ); */
    epicsMutexLock(vmic2534Rec[card].lock);

    vmic2534Rec[ card ].savedOutputs &= ~mask;  /* clear bit(s) we are interested in */
    vmic2534Rec[ card ].savedOutputs |= val;    /* set desired bit status */
    EIEIO_SYNC;
    /*
    vmic2534Rec[ card ].vmicDevice->pioData = vmic2534Rec[ card ].savedOutputs;
    */
    vmic2534Rec[card].vmicDevice->pioData = (vmic2534Rec[card].savedOutputs);
    EIEIO_SYNC;

    /* FASTUNLOCK( &vmic2534Rec[ card ].lock ); */
    epicsMutexUnlock(vmic2534Rec[card].lock);

    return( 0 );
    }


/*
 * vmiv2534Out
 *
 * test routine for vm1c2534 output
 */

int vmiv2534Out( short	card,u_long   val )
    {

    /*  check for valid card nr						 */
    if ( card >= VMIC_2534_MAX_CARDS )
	{
	logMsg("VMIC 2534 card %d out of range\n",(int)card,0,0,0,0,0);
	return( -1 );
	}

    /*  check for card actually installed				 */
    if ( !vmic_2534_num_cards || !(vmic2534Rec[ card ].vmicDevice) )
	{
	logMsg("VMIC 2534 card %d not found\n",(int)card,0,0,0,0,0);
	return( -2 );
        }

    /*  set the physical output						 */
    vmic2534Rec[ card ].vmicDevice->pioData
	    = vmic2534Rec[ card ].outputCorrector ^ val;
    return( 0 );
    }


/*
 *  vmic2534Write
 *
 *  command line interface to test bo driver
 */

int vmic2534Write( short card,u_long val )
{
    return( vmic2534_bo_driver( card, val, 0xffff ) );
}



void vmic2534_io_report( int level )
    {
    int card=0;

    if ( !vmic_2534_num_cards ) {
      printf("No VMIC 2534 Modules present\n");
      return;
    }

    for ( card = 0; card < VMIC_2534_MAX_CARDS; card++ )
	{
	if ( vmic2534Rec[ card ].vmicDevice )
	    {
	    printf("AI: Vmic 2534:\tcard %d\n",card);
	    if( level >= 1 )
		{
		vmic2534_bi_io_report( card );
		vmic2534_bo_io_report( card );
		}
	    }
	}
    }

void vmic2534_bi_io_report( int card )
    {
    short int num_chans=VMIC_2534_MAX_CHANS;
    short int j,k,l,m;
    u_long  jval=0,kval=0,lval=0,mval=0;
    u_int  chanNo;
    unsigned long bcnt=sizeof(VMIC_2534);



    if(  !vmic_2534_num_cards || !vmic2534Rec[card].vmicDevice )
	    return;
    printf("\tbase addr (A16)= %8p\tbcnt=0x%4.4lx\n",
           (char *)vmic2534Rec[card].vmicDevice,bcnt);
    printf("\tVMEVMI-2534 BINARY IN CHANNELS:\n");
    for( j=0, k=1, l=2, m=3;
	(j<num_chans) && (k<num_chans) && (l<num_chans) && (m<num_chans);
         j+=IOR_MAX_COLS, k+= IOR_MAX_COLS, l+= IOR_MAX_COLS,m += IOR_MAX_COLS )
	{
	if( j < num_chans )
	    {
            chanNo = masks(j);
	    vmic2534_bi_driver( card,chanNo,&jval );
	    if ( jval != 0 )
		    jval = 1;
	    printf("\tChan %d = %lx\t ", j, jval);
	    }

	if( k < num_chans )
	    {
            chanNo = masks(k);
	    vmic2534_bi_driver( card,chanNo,&kval );
	    if ( kval != 0) 
		    kval = 1;
	    printf("Chan %d = %lx\t ", k, kval);
	    }

	if( l < num_chans )
	    {
            chanNo = masks(l);
	    vmic2534_bi_driver( card,chanNo,&lval );
	    if ( lval != 0 )
		    lval = 1;
	    printf("Chan %d = %lx \t", l, lval);
	    }
	if( m < num_chans) 
	    {
            chanNo = masks(m);
	    vmic2534_bi_driver( card,chanNo,&mval );
	    if ( mval != 0 )
		    mval = 1;
	    printf("Chan %d = %lx \n", m, mval);
	    }
	}
    }


void vmic2534_bo_io_report( int card )
    {
    short int num_chans,j,k,l,m;
    u_long jval=0,kval=0,lval=0,mval=0;
    u_int chanNo;

    num_chans = VMIC_2534_MAX_CHANS;

    if(  !vmic_2534_num_cards || !vmic2534Rec[card].vmicDevice )
	    return;

    printf("\tVMEVMI-2534 BINARY OUT CHANNELS:\n");
    for( j=0, k=1, l=2, m=3;
	 (j<num_chans) && (k<num_chans) && (l<num_chans) && (m<num_chans);
	 j+=IOR_MAX_COLS, k+= IOR_MAX_COLS, l+= IOR_MAX_COLS,
		    m += IOR_MAX_COLS )
	{
	if( j < num_chans )
	    {

            chanNo = masks(j);
	    vmic2534_bo_read( card,chanNo,&jval );
	    if ( jval != 0 )
		    jval = 1;
	    printf("\tChan %d = %lx\t ", j, jval);
	    }

	if( k < num_chans )
	    {
            chanNo = masks(k);
	    vmic2534_bo_read( card,chanNo,&kval );
	    if ( kval != 0) 
		    kval = 1;
	    printf("Chan %d = %lx\t ", k, kval);
	    }

	if( l < num_chans )
	    {
           chanNo = masks(l);
	    vmic2534_bo_read( card,chanNo,&lval );
	    if ( lval != 0 )
		    lval = 1;
	    printf("Chan %d = %lx \t", l, lval);
	    }
	if( m < num_chans) 
	    {
            chanNo = masks(m);
	    vmic2534_bo_read( card,chanNo,&mval );
	    if ( mval != 0 )
		    mval = 1;
	    printf("Chan %d = %lx \n", m, mval);
	    }
	}
    }

int vmic2534_found( short card,u_long **pcard )
{
   int found = TRUE;

   *pcard = NULL;
    if ( !vmic_2534_num_cards          || 
         (card >= VMIC_2534_MAX_CARDS) ||
         !vmic2534Rec[ card ].vmicDevice ) 
      found = FALSE;
    else
      *pcard = (ulong_t *)vmic2534Rec[ card ].vmicDevice;
    return(found);
}
                
int  vmic2534_readId( short card, u_char *pid )
{
    int        status = ERROR;
    VMIC_2534  *pcard = NULL;
    

    if ( vmic2534_found(card,(ulong_t **)&pcard) ) {
       if ( pid ) 
          *pid = pcard->brdIdReg;
       else
          printf("VMIC 2534: card %hd id=%2.2d\n",card,pcard->brdIdReg);
        status = OK;
    }
    return(status);
}

int  vmic2534_readCsr( short card, u_char *pcsr )
{
    int        status = ERROR;
    VMIC_2534  *pcard = NULL;
    
    if ( vmic2534_found( card,(ulong_t **)&pcard ) ) {
       if ( pcsr ) 
            *pcsr   = pcard->ctlStatReg;
        else
            printf("VMIC 2534: card %hd csr=0x%2.2x\n",card,pcard->ctlStatReg);  
        status = OK;
    }
    return(status);
}
