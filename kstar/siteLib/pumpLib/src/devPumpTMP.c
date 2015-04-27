/*
 * TMP device support
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
#define DSET_AI     devAiTMP
#define DSET_AO     devAoTMP
#define DSET_BI     devBiTMP
#define DSET_BO     devBoTMP
#define DSET_EV     devEvTMP
#define DSET_LI     devLiTMP
#define DSET_LO     devLoTMP
#define DSET_MBBI   devMbbiTMP
#define DSET_MBBID  devMbbidTMP
#define DSET_MBBO   devMbboTMP
#define DSET_MBBOD  devMbbodTMP
#define DSET_SI     devSiTMP
#define DSET_SO     devSoTMP
#define DSET_WF     devWfTMP

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     5.0    /* I/O must complete within this time */
#define TIMEWINDOW  2.0    /* Wait this long after device timeout */


static int convertData();
static int devPumpTMPDebug = 0;
epicsExportAddress(int, devPumpTMPDebug);


/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
    /* Param 0 */   /* read pump status */
    {&DSET_MBBI,GPIBREAD,IB_Q_LOW,"RSS\r", "%d", 0, 32, 0, 0, 0, NULL, NULL, "\r"},
    
    /* Param 1 */   /* read the failure details */
    {&DSET_MBBI,GPIBREAD,IB_Q_LOW,"RSA\r", 0, 0, 32, convertData, 0, 0, NULL, NULL, "\r"},

    /* Param 2 */   /* read rotating speed  Hz */
    {&DSET_AI,  GPIBREAD,IB_Q_LOW,"RRS\r", 0, 0, 32, 0, 0, 0, NULL, NULL, "\r"},

    /* Param 3 */   /* read the total operation time  Hour */
    {&DSET_AI,  GPIBREAD,IB_Q_LOW,"RDT\r", 0, 0, 32, 0, 0, 0, NULL, NULL, "\r"}

    
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
        devSupParms.name = "devTMP";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
        devSupParms.debugFlag = &devPumpTMPDebug;
    }
    return(0);
}



static int convertData(pdpvt, P1, P2, P3)
  struct gpibDpvt *pdpvt;
  int P1;
  int P2;
  char **P3;

{ 
  
  mbbiRecord *psi = ((struct mbbiRecord *)(pdpvt->precord));

  char str_in[4];
  int i;
  
  if(pdpvt->msgInputLen)
  {
     
	strncpy(str_in,&pdpvt->msg[0],4);
	
	if(atoi(str_in) == 1) psi->rval = 1;
        else
        {
         sscanf(str_in,"#%d",&i);
	
         switch(i)
	 {
		case 3:   psi->rval = 2;
        	break;
		case 12:  psi->rval = 3;
        	break;
		case 20:  psi->rval = 4;
        	break;
		case 23:  psi->rval = 5;
        	break;
		case 30:  psi->rval = 6;
        	break;
		case 31:  psi->rval = 7;
        	break;
		case 32:  psi->rval = 8;
        	break;
		case 33:  psi->rval = 9;
        	break;
		case 34:  psi->rval = 10;
        	break;
		case 35:  psi->rval = 11;
        	break;
		case 55:  psi->rval = 12;
        	break;
		case 60:  psi->rval = 13;
        	break;
		case 61:  psi->rval = 14;
        	break;
        	default:   psi->rval = 0;
        	break;
	 }
        }
    }	
   else
   {
	epicsSnprintf(pdpvt->pasynUser->errorMessage,
                      pdpvt->pasynUser->errorMessageSize,
                      "Invalid reply");
   }
}
