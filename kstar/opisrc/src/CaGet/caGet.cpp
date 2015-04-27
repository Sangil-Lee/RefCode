#include <stdio.h>

#include "cadef.h"
#include "dbDefs.h"

int main (int argc, char *argv[])
{
	ca_context_create(ca_disable_preemptive_callback);
	struct dbr_time_long shotdata;
	chid shot_id;
	ca_create_channel("MDS_SHOT_NUMBER",NULL, NULL,20,&shot_id);
	ca_pend_io(0.5);
	//int state = ca_state(shot_id);
	ca_get(DBR_TIME_LONG, shot_id, (void *)&shotdata);
	int status = ca_pend_io(0.5);
	if(status != ECA_NORMAL)
	{
		printf("SHOT number: %s denied access\n",ca_name(shot_id));
		return -1;
	};
	fprintf(stdout, "%d\n",shotdata.value);

	ca_clear_channel(shot_id);
	ca_pend_io(0.5);
	ca_context_destroy();
	return 0;
}
