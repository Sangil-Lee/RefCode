/*
**    ===========================================================================
**
**        Abs: EPICS Instrument Device Support for Agilent 6652A DC power supply
**
**        Name: devAgilent6652A
**
**        First Auth:   10-Jun-2004,  Kukhee Kim         (KHKIM)
**        Second Auth:  dd-mmm-yyyy,  First Lastname     (USERNAME)
**        Rev:          dd-mmm-yyyy,  Reviewer's name    (USERNAME)
**
**    -------------------------------------------------------------------------
**        Mod:
**                      dd-mmm-yyyy, First Lastname      (USERNAME)
**                             comments
**
**    =========================================================================
*/

#define DSET_AI           devAiAgilent6652AGpib
#define DSET_AO           devAoAgilent6652AGpib
#define DSET_BI           devBiAgilent6652AGpib
#define DSET_BO           devBoAgilent6652AGpib
#define DSET_LI           devLiAgilent6652AGpib
#define DSET_LO           devLoAgilent6652AGpib
#define DSET_MBBI         devMbbiAgilent6652AGpib
#define DSET_MBBO         devMbboAgilent6652AGpib
#define DSET_SI           devSiAgilent6652AGpib
#define DSET_SO           devSoAgilent6652AGpib

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <alarm.h>
#include <errlog.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recSup.h>
#include <drvSup.h>
#include <link.h>
#include <dbCommon.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <biRecord.h>
#include <boRecord.h>
#include <mbbiRecord.h>
#include <mbboRecord.h>
#include <stringinRecord.h>
#include <stringoutRecord.h>
#include <longinRecord.h>
#include <longoutRecord.h>

#include <devCommonGpib.h>
#include <devGpib.h>


int testDebugAgilent6652A = 0;    /* debugging flag */

#define TIMEOUT           1.0
#define TIMEWINDOW        2.0


static int writeCmdStringout(gpibDpvt *pdpvt, int P1, int P2, char **P3);

static char *tOnOffEfast[] = { "OFF", "ON", 0 };
static char *tOnOffList[] = { "OFF", "ON" };
static unsigned long tOnOffListVal[] = { 0, 1 };
static devGpibNames nameOnOffList = {
    2,                 /* number of elements */
    tOnOffList,        /* pointer to strings */
    tOnOffListVal,     /* pointer to value list */
    1                  /* number of valid bits */
};

static char *tNormTextEfast[] = { "NORM", "TEXT", 0 };
static char *tNormTextList[] = { "NORM", "TEXT" };
static unsigned long tNormTextListVal [] = { 0, 1 };
static devGpibNames nameNormTextList = {
    2,                 /* number of elements */
    tNormTextList,     /* pointer to strings */
    tNormTextListVal,  /* pointer to value list */
    1                  /* number of valid bits */
};

 




static struct gpibCmd gpibCmds [] = {
  /* Param 0, */
  {&DSET_BO, GPIBIFC, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 1, */
  {&DSET_BO, GPIBREN, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 2, */
  {&DSET_BO, GPIBDCL, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 3, */
  {&DSET_BO, GPIBLLO, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 4, */
  {&DSET_BO, GPIBSDC, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 5, */
  {&DSET_BO, GPIBGTL, IB_Q_LOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 6, */
  {&DSET_BO, GPIBRESETLNK, IB_Q_HIGH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 7, */
  {&DSET_LI, GPIBSRQHANDLER, IB_Q_HIGH, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* Param 8 */
  {&DSET_LO, GPIBWRITE, IB_Q_LOW, 0, "DESE %ld", 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 9 */
  {&DSET_LO, GPIBWRITE, IB_Q_LOW, 0, "*ESE %ld", 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 10 */
  {&DSET_LO, GPIBWRITE, IB_Q_LOW, 0, "*SRE %ld", 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 11 */
  {&DSET_LI, GPIBREAD, IB_Q_LOW, "DESE?",  0, 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 12 */
  {&DSET_LI, GPIBREAD, IB_Q_LOW, "*ESR?",  0, 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 13 */
  {&DSET_LI, GPIBREAD, IB_Q_LOW, "*ESE?",  0, 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 14 */
  {&DSET_LI, GPIBREAD, IB_Q_LOW, "*STB?",  0, 0, 20, 0, 0, 0, 0, 0, 0},
  /* Param 15 */
  {&DSET_LI, GPIBREAD, IB_Q_LOW, "*SRE?",  0, 0, 20, 0, 0, 0, 0, 0, 0},

  /* Param 16 (id)   */
  {&DSET_SI, GPIBREAD, IB_Q_LOW, "*IDN?", 0, 0, 200, 0, 0, 0, 0, 0, 0},
  /* Param 17 */
  {&DSET_SI, GPIBREADW, IB_Q_LOW, "*IDN?", 0, 0, 200, 0, 0, 0, 0, 0, 0},

  {&DSET_MBBO, GPIBEFASTO, IB_Q_LOW, "OUTP", 0, 0, 200, 0, 0, &tOnOffEfast, 0, 0},
    
};



static int writeCmdStringout(gpibDpvt *pdpvt, int P1, int P2, char **P3)
{
    gpibCmd          *pGpibCmd = gpibCmdGet(pdpvt);
    char             *pCmd     = pGpibCmd->cmd;
    char             *pMsg     = pdpvt->msg;
    stringoutRecord  *prec     = (stringoutRecord*)pdpvt->precord;

    sprintf(pMsg, pCmd, prec->val);
    return(0);
}

/* The following is the number of elements in the common array above. */
#define NUMPARAMS         sizeof(gpibCmds)/sizeof(struct gpibCmd)

/***********************************************************************
*
*    Initialization for device support
*    This is called one time before any records are initialized with a param
*    value of 0.     And then again AFTER all record-level init is complete
*    with a param value of 1.
*
*    This function will no longer be required after epics 3.3 is released
*    
************************************************************************/
static long init_ai(int parm)
{
    if(parm==0) {
        devSupParms.name = "devAgilent6652A";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
        devSupParms.debugFlag = &testDebugAgilent6652A;
    }
    return (0);
}


