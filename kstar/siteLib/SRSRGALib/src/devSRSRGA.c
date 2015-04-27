/*
 * SRSRGA device support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <epicsStdio.h>
#include <devCommonGpib.h>

/******************************************************************************
 *
 * The following define statements are used to declare the names to be used
 * for the dset tables.   
 *
 * A DSET_AI entry must be declared here and referenced in an application
 * database description file even if the device provides no AI records.
 *
 ******************************************************************************/
#define DSET_AI     devAiSRSRGA
#define DSET_AO     devAoSRSRGA
#define DSET_BI     devBiSRSRGA
#define DSET_BO     devBoSRSRGA
#define DSET_EV     devEvSRSRGA
#define DSET_LI     devLiSRSRGA
#define DSET_LO     devLoSRSRGA
#define DSET_MBBI   devMbbiSRSRGA
#define DSET_MBBID  devMbbidSRSRGA
#define DSET_MBBO   devMbboSRSRGA
#define DSET_MBBOD  devMbbodSRSRGA
#define DSET_SI     devSiSRSRGA
#define DSET_SO     devSoSRSRGA
#define DSET_WF     devWfSRSRGA

#include <devGpib.h> /* must be included after DSET defines */

# define WEOL "\r\n"
# define REOL "\n\r"

#define TIMEOUT     5.0    /* I/O must complete within this time */
#define TIMEWINDOW  3.0    /* Wait this long after device timeout */


static int convertData();
static int devSRSRGADebug = 0;
epicsExportAddress(int, devSRSRGADebug);


/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {

    /* Param 0 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "IN0"WEOL,"%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 1 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "CA"WEOL, "%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 2 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "FL1"WEOL,"%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 3 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "FL0"WEOL,"%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 4 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "HV1400"WEOL,"%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 5 */
    {&DSET_BI, GPIBREAD, IB_Q_LOW, "HV0"WEOL,"%d", 0, 30, NULL, 0, 0, NULL, NULL, REOL},
    /* Param 6 */
    {&DSET_BO, GPIBWRITE, IB_Q_LOW, NULL,"MR0"WEOL,0, 30, NULL, 0, 0, NULL, NULL, NULL},
    /* Param 7 */
    {&DSET_SI, GPIBREAD, IB_Q_LOW,"ID?"WEOL, "%s",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 8 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"AP?"WEOL,"%lf",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 9 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"FL?"WEOL,"%lf",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 10 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"IE?"WEOL,"%lf",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 11 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"HV?"WEOL,"%lf",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 12 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"SP?"WEOL,"%lf",0, 32,0,0,0,NULL,NULL,REOL},
    /* Param 13 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR2"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 14 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR4"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 15 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR12"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 16 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR18"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 17 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR28"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 18 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR32"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 19 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR40"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 20 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR44"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 21 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR55"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 22 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR20"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 23 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR26"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 24 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR3"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 25 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR16"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},
    /* Param 26 */
    {&DSET_BO, GPIBWRITE, IB_Q_LOW, NULL,"DG0"WEOL,0, 30, NULL, 0, 0, NULL, NULL, NULL},
    /* Param 27 */
    {&DSET_BO, GPIBWRITE, IB_Q_LOW, NULL,"DG*"WEOL,0, 30, NULL, 0, 0, NULL, NULL, NULL},
    /* Param 28 */
    {&DSET_AI, GPIBREAD, IB_Q_LOW,"MR29"WEOL,0,0,4,convertData,0,0,NULL,NULL,NULL},

    
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
        devSupParms.name = "devSRSRGA";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
        devSupParms.debugFlag = &devSRSRGADebug;
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

  union { long val; unsigned char bytes[4];} value;


if(pdpvt->msgInputLen ==4){


         value.bytes[0] =       pdpvt->msg[0];
          value.bytes[1] =      pdpvt->msg[1];
          value.bytes[2] =      pdpvt->msg[2];
          value.bytes[3] =      pdpvt->msg[3];

       pai->val =(double) (value.val * 1.0E-16 );
       if (pai->val <= 0)
          {
             pai->val = 0;
          }

   }
 else
   epicsSnprintf(pdpvt->pasynUser->errorMessage,
                      pdpvt->pasynUser->errorMessageSize,
                      "Invalid reply");
    return 0;
  }


