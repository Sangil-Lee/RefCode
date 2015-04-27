/*
 *      Author:          Sulhee Baek
 *      Date:            4-30-2007
 */

#ifndef INC_KSTAR_errMdefh
#define INC_KSTAR_errMdefh

#define MAXERRNUM 100
#define M_noInterlock	(long)0x00000001 /*No interlock*/
#define M_TFCoilQuench	(long)0x00000002 /**/
#define M_PFCoilQuench	(long)0x0003 /**/
#define M_TFBusQuench	(long)0x0004 /**/
#define M_PFBusQuench	(long)0x0005 /**/
#define M_HRSInterlock	(long)0x0006 /**/

typedef struct 
{
	long errNum;
	char *errMessage;
} ERRTABLE;

ERRTABLE kstar_error_table []=
{
	{(long)M_noInterlock, "No Interlock"}, 
	{(long)M_TFCoilQuench, "TF Coil Quench"},
	{(long)M_PFCoilQuench, "PF Coil Quench"},
	{(long)M_TFBusQuench, "TF Busline Quench"},
	{(long)M_PFBusQuench, "PF Busline Quench"},
	{(long)M_HRSInterlock, "HRS Interlock Quench"}
	//{(long)M_HRSInterlock, "HRS Interlock Quench"},
};

#endif /*INC_KSTAR_errMdefh*/
