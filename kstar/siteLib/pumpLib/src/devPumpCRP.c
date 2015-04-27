/*
 * CRP device support
 */

#include <epicsStdio.h>
#include <devCommonGpib.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************
 *
 * The following define statements are used to declare the names to be used
 * for the dset tables.   
 *
 * A DSET_AI entry must be declared here and referenced in an application
 * database description file even if the device provides no AI records.
 *
 ******************************************************************************/
#define DSET_AI     devAiCRP
#define DSET_AO     devAoCRP
#define DSET_BI     devBiCRP
#define DSET_BO     devBoCRP
#define DSET_EV     devEvCRP
#define DSET_LI     devLiCRP
#define DSET_LO     devLoCRP
#define DSET_MBBI   devMbbiCRP
#define DSET_MBBID  devMbbidCRP
#define DSET_MBBO   devMbboCRP
#define DSET_MBBOD  devMbbodCRP
#define DSET_SI     devSiCRP
#define DSET_SO     devSoCRP
#define DSET_WF     devWfCRP

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     5.0    /* I/O must complete within this time */
#define TIMEWINDOW  2.0   /* Wait this long after device timeout */

static int convertData();
static int devPumpCRPDebug = 0;
epicsExportAddress(int, devPumpCRPDebug);

#define EOS1 "\r"
#define EOS2 "\r\n"

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
     FILL10,  /* dummy componets */
     /* First Controller */
	/* Param 10 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$01R00\r","#01R00:%x",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 11 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$01R01\r","#01R01:%x",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 12 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$01R02\r","#01R02:%x",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 13 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$01R03\r","#01R03:%x",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 14 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$01R04\r","#01R04:%x",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 15 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$01R31\r","#01R31:%lf",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 16 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$01R32\r","#01R32:%lf",0,32,0,0,0,NULL,NULL,EOS2},
	/* Param 17 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$01R33\r",0,0,32,convertData,0,0,NULL,NULL,EOS2},
        /* Param 18, 19 */  FILL, FILL,   /* dummy componets */

     /* Second Controller */
        /* Param 20 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$02R00\r","#02R00:%x",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 21 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$02R01\r","#02R01:%x",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 22 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$02R02\r","#02R02:%x",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 23 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$02R03\r","#02R03:%x",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 24 */
     {&DSET_MBBID,GPIBREAD,IB_Q_LOW,"$02R04\r","#02R04:%x",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 25 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$02R31\r","#02R31:%lf",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 26 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$02R32\r","#02R32:%lf",0,32,0,0,0,NULL,NULL,EOS2},
        /* Param 27 */
     {&DSET_AI,GPIBREAD,IB_Q_LOW,"$02R33\r",0,0,32,convertData,0,0,NULL,NULL,EOS2},
        /* Param 28, 29 */ FILL,    /* dummy compoents */
};

/* The following is the number of elements in the command array above.  */
#define NUMPARAMS sizeof(gpibCmds)/sizeof(struct gpibCmd)

/******************************************************************************
 * Initialize device support parameters
 *
 *****************************************************************************/
static long init_ai(int parm)
{
    if(parm==0) {
        devSupParms.name = "devCRP";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
        devSupParms.debugFlag = &devPumpCRPDebug;
    }
    return(0);
}

static int convertData(pdpvt, P1, P2, P3)
  struct gpibDpvt *pdpvt;
  int P1;
  int P2;
  char **P3;

{ 
  
  aiRecord *pai = ((struct aiRecord *)(pdpvt->precord));

  double a,c ;
  int p;
  char c_tmp[10];
  
if(pdpvt->msgInputLen ==14){

       sscanf(pdpvt->msg,"#%2cR33:%lf%d",c_tmp, &a,&p);
        c=pow(10.,p);
	pai->val = a * c;

      }

 else
   epicsSnprintf(pdpvt->pasynUser->errorMessage,
                      pdpvt->pasynUser->errorMessageSize,
                      "Invalid reply");

    return 0;
  }

