/***** devAoVmi3122.c       *****/
/***** Author: Marie Keesee *****/
/***** Date: 09May96        *****/

#include	<vxWorks.h>
#include	<types.h>
#include	<stdioLib.h>
#include	<string.h>

#include        <recGbl.h>
#include	<alarm.h>
#include	<dbDefs.h>
#include	<dbAccess.h>
#include        <recSup.h>
#include	<devSup.h>
#include	<special.h>
#include	<module_types.h>
#include	<aoRecord.h>
#include        <epicsExport.h>

/* Create the dset for devAoVmi1322 */
static long init_record();
static long write_ao();
static long special_linconv();

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
}devAoVmi3122={
	6,
	NULL,
	NULL,
	init_record,
	NULL,
	write_ao,
	special_linconv};

epicsExportAddress(dset, devAoVmi3122);

/* Prototype */
long write_Vmi3122( unsigned short card,
                    unsigned short signal,
                    unsigned long *pval );

/******************************************************************
 * init_record
 ******************************************************************/
static long init_record(pao)
    struct aoRecord	*pao;
{

    /* ao.out must be an VME_IO */
    switch (pao->out.type) {
    case (VME_IO) :
        pao->udf = FALSE;
	break;
    default :
	recGblRecordError(S_db_badField,(void *)pao,
		"devAoVmi3122 (init_record) Illegal OUT field");
	return(S_db_badField);
    }

    /* set linear conversion slope*/
    pao->eslo = (pao->eguf - pao->egul)/65535.0;

    return(2);
}

/******************************************************************
 * write_ao
 ******************************************************************/
static long write_ao(pao)
    struct aoRecord	*pao;
{
	struct vmeio 	*pvmeio;
	long	    	status;
	/* unsigned short  value,rbvalue; */
	unsigned long value;

	pvmeio = (struct vmeio *)&(pao->out.value);
	value = pao->rval;
	printf("dev writing %ld\n", value);
	status = write_Vmi3122(pvmeio->card,pvmeio->signal,&value);
	if(status==0) pao->rbv = value;
	/* only returns 0 right now */
	else {
		status = 0;
                recGblSetSevr(pao,WRITE_ALARM,INVALID_ALARM);
	}

	return(status);
}

/******************************************************************
 * special_linconv
 ******************************************************************/
static long special_linconv(pao,after)
    struct aoRecord	*pao;
    int after;
{

    if(!after) return(0);
    /* set linear conversion slope*/
    pao->eslo = (pao->eguf - pao->egul)/65535.0;
    return(0);
}
