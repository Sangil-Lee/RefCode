/*
 * TPG256 device support
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
#define DSET_AI     devAiTPG256
#define DSET_MBBI   devMbbiTPG256

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     1.0    /* I/O must complete within this time */
#define TIMEWINDOW  2.0    /* Wait this long after device timeout */

static int TPG_read();
static int TPG_status();

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static struct gpibCmd gpibCmds[] = {
 /* Param 0 */
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR1", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 1 */ 
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR1", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0},
 /* Param 2 */
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR2", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 3 */
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR2", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0},
 /* Param 4 */
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR3", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 5 */
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR3", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0},
 /* Param 6 */
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR4", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 7 */
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR4", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0},
 /* Param 8 */
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR5", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 9 */
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR5", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0},
 /* Param 10 */ 
  {&DSET_AI, GPIBCVTIO, IB_Q_LOW, "PR6", 0, 0, 100, TPG_read, 0, 0, 0, 0, 0},
 /* Param 11 */ 
  {&DSET_MBBI, GPIBCVTIO, IB_Q_LOW, "PR6", 0, 0, 100, TPG_status, 0, 0, 0, 0, 0}
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
        devSupParms.name = "devTPG256";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
    }
    return(0);
}

static int TPG_read(gpibDpvt *pgpibDpvt,int P1, int P2, char **P3)
{
    asynUser *pasynUser = pgpibDpvt->pasynUser;
	aiRecord *prec = (aiRecord*)pgpibDpvt->precord;
    gpibCmd *pgpibCmd = gpibCmdGet(pgpibDpvt);
	asynOctet *pasynOctet = pgpibDpvt->pasynOctet;
	void *asynOctetPvt = pgpibDpvt->asynOctetPvt;
	asynStatus status;
	
    double      value;
    int         sts;
	char        readBuffer[32];
	size_t		ntrans;
    
    status = pasynOctet->setInputEos(asynOctetPvt,pasynUser,"\r\n",2); 
	status = pasynOctet->write(asynOctetPvt,pasynUser,pgpibCmd->cmd, 3, &ntrans);
	status = pasynOctet->read(asynOctetPvt,pasynUser,readBuffer, 32, &ntrans, 0);
	status = pasynOctet->write(asynOctetPvt,pasynUser,"\05", 1, &ntrans);
	status = pasynOctet->read(asynOctetPvt,pasynUser,readBuffer, 32, &ntrans, 0);
	
	
	/* scan response string for gaufe status & value  */

	status = sscanf(readBuffer, "%d,%lf", &sts, &value);
	
	/* assign new value to value field*/
	prec->val = value;
	prec->udf = FALSE;
	/* if sesor error detected : MAJOR ALARM*/
	if(sts >2)
	{
		prec->hhsv= 2;}
	else
	{	
	    prec->hhsv= 0;}

	return(0);
}

static int TPG_status(gpibDpvt *pgpibDpvt,int P1, int P2, char **P3)
{
    asynUser *pasynUser = pgpibDpvt->pasynUser;
    mbbiRecord *prec = (mbbiRecord*)pgpibDpvt->precord;
	gpibCmd *pgpibCmd = gpibCmdGet(pgpibDpvt);
	asynOctet *pasynOctet = pgpibDpvt->pasynOctet;
	void *asynOctetPvt = pgpibDpvt->asynOctetPvt;
	asynStatus status;
	double      value;
	int         sts;
	char        readBuffer[32];
	size_t      ntrans;

	status = pasynOctet->setInputEos(asynOctetPvt,pasynUser,"\r\n",2);
	status = pasynOctet->write(asynOctetPvt,pasynUser,pgpibCmd->cmd, 3, &ntrans);
	status = pasynOctet->read(asynOctetPvt,pasynUser,readBuffer, 32, &ntrans, 0);
	status = pasynOctet->write(asynOctetPvt,pasynUser,"\05", 1, &ntrans);
	status = pasynOctet->read(asynOctetPvt,pasynUser,readBuffer, 32, &ntrans, 0);
																    
																    
	/* scan response string for gaufe status & value  */
															 
	status = sscanf(readBuffer, "%d,%lf", &sts, &value);
																	    
	/* assign new value to value field*/
	prec->val = sts;
	prec->udf = FALSE;
	return(0);
} 

