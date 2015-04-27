#include "RBA.h"
#include "drvNI6122.h"

// Global variable for IOC
ST_IOC_VAR	gIocVar;

void request_scan_master_io ()
{
	ST_MASTER	*pMaster	= get_master();

	scanIoRequest (pMaster->ioScanPvt_status);
}

int get_ioc_amp_stat ()
{
	return (gIocVar.amp_stat);
}

void set_ioc_amp_stat (const int stat)
{
	if (stat != gIocVar.amp_stat) {
		gIocVar.amp_stat = stat;

		request_scan_master_io ();
	}
}

int get_ioc_dig_stat ()
{
	return (gIocVar.dig_stat);
}

void set_ioc_dig_stat (const int stat)
{
	if (stat != gIocVar.dig_stat) {
		gIocVar.dig_stat = stat;

		request_scan_master_io ();
	}
}

int get_ioc_mds_stat ()
{
	return (gIocVar.mds_stat);
}

void set_ioc_mds_stat (const int stat)
{
	if (stat != gIocVar.mds_stat) {
		gIocVar.mds_stat = stat;

		request_scan_master_io ();
	}
}

