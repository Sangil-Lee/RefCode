#include <stdio.h>

#include "cadef.h"
#include "dbDefs.h"

int main ()
{
	ca_context_create(ca_disable_preemptive_callback);
	chid shsum_reset_chid;
	//ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",NULL, NULL,20,&shsum_reset_chid);
	int resetval = 0;
	int status = ca_search("CCS_PERFORM_SHOT_SUMMARY", &shsum_reset_chid);
	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(shsum_reset_chid);
		printf("Not Found %s\n", "CCS_PERFORM_SHOT_SUMMARY");
		return -1;
	};
	ca_put(DBR_INT, shsum_reset_chid, &resetval);
	status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		printf("Reset Shot Summary failed: %s denied access\n",ca_name(shsum_reset_chid));
		return -1;
	};
	ca_clear_channel(shsum_reset_chid);
	ca_pend_io(0.5);
	ca_context_destroy();
	return 0;
}
