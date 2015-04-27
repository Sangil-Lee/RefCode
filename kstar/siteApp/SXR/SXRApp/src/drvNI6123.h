#include <NIDAQmx.h>

int drvNI6123_notify_InitCondition(ST_STD_device *);
void threadFunc_DAQ_NI6123(void *param);

#define INIT_MAX_CHAN_SAMPLES    100000
#define INIT_MAX_CHAN_NUM	8
#define INIT_TOT_EVENT_SAMPLES  (INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

typedef struct NI6123_dev {

	int myStatusDev;


	/* NI 6123 *************************/


	TaskHandle taskHandle;
	char physical_channel[8][30];
	char name_to_assignchannel[30];
	char triggerSource[30];
	char strTagName[8][30];
	int32 terminal_config;
	float64 minVal;
	float64 maxVal;
	float64 smp_rate;
	int32 units;
	int32 sample_time;
	char customScaleName[255];
	int32 smp_mode;
	uInt64 sampPerChanToAcquie;
	float64 *Acq_data;
	char Local_data_name[100];
	int reset_switch;
	int auto_run_flag;
	int db_write_flag;
	int32 totalRead;
	char file_name[8][100];
	FILE *Local_fp;
	FILE *write_fp[8];
	ELLLIST *data_buf; 

	/* NI 6123 *************************/
	short readArray[INIT_TOT_EVENT_SAMPLES];
	

	DBADDR db_SOFT_SYS_RUN;
	DBADDR db_SOFT_DATA_PROCESS;
	DBADDR db_SYS_FETCH_DATA;



}ST_NI6123_dev;

typedef struct NI6123_data_buf {

		ELLNODE node;
		short data[8000];

}NI6123_data_buf;

