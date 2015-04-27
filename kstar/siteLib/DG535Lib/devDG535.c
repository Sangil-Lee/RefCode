/*
 * DG535 device support
 */

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
#define DSET_AI     devAiDG535
#define DSET_AO     devAoDG535
#define DSET_BI     devBiDG535
#define DSET_BO     devBoDG535
#define DSET_MBBI   devMbbiDG535
#define DSET_MBBO   devMbboDG535
#define DSET_SI     devSiDG535
#define DSET_SO     devSoDG535

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     1.0    /* I/O must complete within this time */
#define TIMEWINDOW  2.0    /* Wait this long after device timeout */

static int rddelay();

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
 /* Param 0 */
  {&DSET_SI, GPIBREAD, IB_Q_LOW, "DL", "%s", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 1 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "ES", "%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 2 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 0", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 3 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 1", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 4 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 2", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 5 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 3", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 6 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 4", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 7 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 5", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 8 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "ES 6", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 9 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "TZ 4", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 10 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "TZ 7", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 11 */
  {&DSET_MBBI, GPIBREAD, IB_Q_LOW, "OM 4", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 12 */
  {&DSET_MBBI, GPIBREAD, IB_Q_LOW, "OM 7", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 13 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "OP 4", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 14 */
  {&DSET_BI, GPIBREAD, IB_Q_LOW, "OP 7", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 15 */
  {&DSET_MBBI, GPIBREAD, IB_Q_LOW, "TM", "%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 16 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "TR 0", "%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 17 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "TR 1", "%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 18 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "DT 2", "1,%lf", 0, 100, rddelay, 0, 0, 0, 0, 0},
 /* Param 19 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "DT 3", "2,%lf", 0, 100, rddelay, 0, 0, 0, 0, 0},
 /* Param 20 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "DT 5", "1,%lf", 0, 100, rddelay, 0, 0, 0, 0, 0},
 /* Param 21 */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "DT 6", "5,%lf", 0, 100, rddelay, 0, 0, 0, 0, 0},
  
 /* Param 22 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "TZ 4,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 23 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "TZ 7,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 24 */
  {&DSET_MBBO, GPIBWRITE, IB_Q_LOW, 0, "OM 4,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 25 */
  {&DSET_MBBO, GPIBWRITE, IB_Q_LOW, 0, "OM 7,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 26 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "OP 4,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 27 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "OP 7,%d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 28 */
  {&DSET_MBBO, GPIBWRITE, IB_Q_LOW, 0, "TM %d", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 29 */
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "TR 0,%ilf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 30 */
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "TR 1,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 31 */ 
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "DT 1,0,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 32 */  
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "DT 2,1,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 33 */  
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "DT 3,2,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 34 */  
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "DT 5,1,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 35 */  
  {&DSET_AO, GPIBWRITE, IB_Q_LOW, 0, "DT 6,5,%lf", 0, 100, 0, 0, 0, 0, 0, 0},
  
 /* Param 36 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "SS", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 37 */
  {&DSET_BO, GPIBWRITE, IB_Q_LOW, 0, "CL", 0, 100, 0, 0, 0, 0, 0, 0},
 /* Param 38 */
  {&DSET_SO, GPIBWRITE, IB_Q_LOW, 0, "%s", 0, 100, 0, 0, 0, 0, 0, 0}

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
        devSupParms.name = "devDG535";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
    }
    return(0);
}


static int rddelay(struct gpibDpvt *pgpibDpvt,int P1, int P2, char **P3)
{
	asynUser *pasynUser = pgpibDpvt->pasynUser;
	int         status;
	double      delay;
	unsigned long chan;
    aiRecord *prec = (aiRecord*)pgpibDpvt->precord;

	/* scan response string for chan reference & delay value  */

	  status = sscanf(pgpibDpvt->msg, "%ld,%lf", &chan, &delay);
	  asynPrint(pasynUser,ASYN_TRACE_FLOW," sscanf status = %d\n",status);
	  if(status!=2) {
	      epicsSnprintf(pasynUser->errorMessage,pasynUser->errorMessageSize,
	        	          "rdDelay sscanf returned %d should be 2\n", status);
	     return(-1);
	  }
	 /* assign new delay to value field*/
	    prec->val = delay;
	    prec->udf = FALSE;
	     return(0);
}
