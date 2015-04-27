#include <NIDAQmx.h>

void threadFunc_Power_Supply_IT6322(void *param);

#define CHANGE_DC1_CH1_VOLT 	0x000000000001<<0
#define CHANGE_DC1_CH2_VOLT 	0x000000000001<<1
#define CHANGE_DC1_CH3_VOLT 	0x000000000001<<2
#define CHANGE_DC2_CH1_VOLT 	0x000000000001<<3
#define CHANGE_DC2_CH2_VOLT 	0x000000000001<<4
#define CHANGE_DC2_CH3_VOLT 	0x000000000001<<5
#define CHANGE_DC1_CH1_ON_OFF 	0x000000000001<<6
#define CHANGE_DC1_CH2_ON_OFF 	0x000000000001<<7
#define CHANGE_DC1_CH3_ON_OFF 	0x000000000001<<8
#define CHANGE_DC2_CH1_ON_OFF 	0x000000000001<<9
#define CHANGE_DC2_CH2_ON_OFF 	0x000000000001<<10
#define CHANGE_DC2_CH3_ON_OFF 	0x000000000001<<11












typedef struct IT6322_dev {

        int myStatusDev;


/* NI 6259 *************************/

		char  port_name[255];
		int   baud_rate       ;
		float DC1_CH1_Voltage ;
		float DC1_CH2_Voltage ;
		float DC1_CH3_Voltage ;		
		int   DC1_CH1_ON_OFF  ;
		int   DC1_CH2_ON_OFF  ;
		int   DC1_CH3_ON_OFF  ;
		int   DC1_Comm_status ;
		
		float DC2_CH1_Voltage ;
		float DC2_CH2_Voltage ;
		float DC2_CH3_Voltage ;		
		int   DC2_CH1_ON_OFF  ;
		int   DC2_CH2_ON_OFF  ;
		int   DC2_CH3_ON_OFF  ;
		int   DC2_Comm_status ;
		float64 SENSOR_BLOCK1_MAX_TEMP ;
		float64 SENSOR_BLOCK2_MAX_TEMP ;
		float64 SENSOR_BLOCK3_MAX_TEMP ;
		float64 SENSOR_BLOCK4_MAX_TEMP ;

		unsigned int change_option;

/* NI 6123 *************************/



        DBADDR db_SOFT_SYS_RUN;
        DBADDR db_SOFT_DATA_PROCESS;
        DBADDR db_SYS_FETCH_DATA;



}ST_IT6322_dev;

