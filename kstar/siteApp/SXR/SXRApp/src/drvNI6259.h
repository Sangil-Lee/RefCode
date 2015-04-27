#include <NIDAQmx.h>

void threadFunc_DAQ_NI6259 (void *param);


typedef struct NI6259_dev {

        int myStatusDev;


/* NI 6259 *************************/

		char power_supply_num[30];
        TaskHandle taskHandle;
        char physical_channel[30];
        char name_to_assignchannel[30];
        char triggerSource[30];
		char customScaleName[255];
		float64 temp[4];
		float64 *Acq_data;

/* NI 6123 *************************/



        DBADDR db_SOFT_SYS_RUN;
        DBADDR db_SOFT_DATA_PROCESS;
        DBADDR db_SYS_FETCH_DATA;



}ST_NI6259_dev;
