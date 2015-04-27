/***** devAiVmi3122.c       *****/
/***** Author: Marie Keesee *****/
/***** Date: 09May96        *****/

#include	<vxWorks.h>
#include	<types.h>
#include	<stdioLib.h>
#include	<string.h>

#include        <recGbl.h>
#include	<alarm.h>
#include	<cvtTable.h>
#include	<dbDefs.h>
#include	<dbAccess.h>
#include        <recSup.h>
#include	<devSup.h>
#include	<link.h>
#include	<dbScan.h>
#include	<module_types.h>
#include	<aiRecord.h>

#include        <epicsExport.h>

static long init_record();
static long read_ai();
static long special_linconv();

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
        DEVSUPFUN       get_ioint_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiVmi3122={
	6,
	NULL,
	NULL,
	init_record,
	NULL,
	read_ai,
	special_linconv};

epicsExportAddress(dset, devAiVmi3122);

/* Prototypes */
long read_Vmi3122( unsigned short card,
                   unsigned short signal,
                   unsigned long *pval );


/************************************************************************
 * init_record
 ************************************************************************/
static long init_record(pai)
struct aiRecord	*pai;
{
    /* ai.inp must be an VME_IO */
    switch (pai->inp.type) {
    case (VME_IO) :
        pai->udf = FALSE;
	break;
    default :
	recGblRecordError(S_db_badField,(void *)pai,
		"devAiVmi3122 (init_record) Illegal INP field");
	return(S_db_badField);
    }

    /* set linear conversion slope*/
    pai->eslo = (pai->eguf - pai->egul)/65535.0;

    return(0);
}

/************************************************************************
 * read_ai
 ************************************************************************/
static long read_ai(pai)
struct aiRecord	*pai;
{
   struct vmeio *pvmeio;
   long          status;
   unsigned long value;

   pvmeio = (struct vmeio *)&(pai->inp.value);
   status=read_Vmi3122(pvmeio->card,pvmeio->signal,&value);
   if(status==0) 
      pai->rval = value;
      /* driver only returns 0 right now */
   else
      recGblSetSevr(pai,READ_ALARM,INVALID_ALARM);

   return(status);

}

/************************************************************************
 * special_linconv
 ************************************************************************/
static long special_linconv(pai,after)
    struct aiRecord	*pai;
    int after;
{

    if(!after) return(0);
    /* set linear conversion slope*/
    pai->eslo = (pai->eguf - pai->egul)/65535.0;
    return(0);
}
