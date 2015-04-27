/*
 * ESP300 device support
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
#define DSET_AI     devAiESP300
#define DSET_AO     devAoESP300
#define DSET_BI     devBiESP300
#define DSET_BO     devBoESP300
#define DSET_EV     devEvESP300
#define DSET_LI     devLiESP300
#define DSET_LO     devLoESP300
#define DSET_MBBI   devMbbiESP300
#define DSET_MBBID  devMbbidESP300
#define DSET_MBBO   devMbboESP300
#define DSET_MBBOD  devMbbodESP300
#define DSET_SI     devSiESP300
#define DSET_SO     devSoESP300
#define DSET_WF     devWfESP300

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     5    /* I/O must complete within this time */
#define TIMEWINDOW  2   /* Wait this long after device timeout */

static int convertData();

#define EOS1 "\r"
#define EOS2 "\r\n"

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
         /* Param 0, Serial number */
     {&DSET_SI, GPIBREAD, IB_Q_LOW, "VE?\r",  "%s", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 1, Serial number */
     {&DSET_SI, GPIBREAD, IB_Q_LOW, "1ID?\r", "%s", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 2, Serial number */
     {&DSET_SI, GPIBREAD, IB_Q_LOW, "2ID?\r", "%s", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 3, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "1TP\r", "%lf", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 4, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "2TP\r", "%lf", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 5, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "1PA%.2f\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 6, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "2PA%.2f\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 7, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "1PR%.2f\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 8, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "2PR%.2f\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 9, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1ST\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 10, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2ST\r", 0, 32, 0,0,0, NULL, NULL, NULL}, 
         /* Param 11, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1OR1\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 12, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2OR1\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 13, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "AB\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 14, Serial number */
     {&DSET_BI, GPIBREAD, IB_Q_LOW, "1MD?\r", "%ld", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 15, Serial number */
     {&DSET_BI, GPIBREAD, IB_Q_LOW, "2MD?\r", "%ld", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 16, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1MV+\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 17, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2MV+\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 18, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1MV-\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 19, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2MV-\r",  0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 20, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "1VA?\r", "%lf", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 21, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "2VA?\r", "%lf", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 22, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "1VA%d\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 23, Serial number */
     {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "2VA%d\r", 0, 32, 0,0,0, NULL, NULL, NULL},  
         /* Param 24, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1MF\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 25, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2MF\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 26, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "1MO\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 27, Serial number */
     {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "2MO\r", 0, 32, 0,0,0, NULL, NULL, NULL},
         /* Param 28, Serial number */
     {&DSET_BI, GPIBREAD, IB_Q_LOW, "1MO?\r", "%ld", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 29, Serial number */
     {&DSET_BI, GPIBREAD, IB_Q_LOW, "2MO?\r", "%ld", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 30, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "TB?\r", "%lf", 0, 32, 0,0,0, NULL, NULL, EOS2},
         /* Param 31, Serial number */
     {&DSET_AI, GPIBREAD, IB_Q_LOW, "I?\r", "%ld", 0, 32,convertData,0,0, NULL, NULL, "\r\n>"}
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
        devSupParms.name = "devESP300";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
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

  double data ;
/*  char str1[13]; */
  char str2[6];

if(pdpvt->msgInputLen ==13){
/*       str1 = pdpvt->msg; */
       strncpy(str2, (pdpvt->msg)+4, 6);
	   data = atof(str2);
       pai->val = data;

      }

 else
   epicsSnprintf(pdpvt->pasynUser->errorMessage,
                      pdpvt->pasynUser->errorMessageSize,
                      "Invalid reply");

    return 0;
  }
