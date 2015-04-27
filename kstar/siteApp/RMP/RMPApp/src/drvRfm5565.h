#ifndef drvRFM5565_H
#define drvRFM5565_H




#include "sfwDriver.h"
#include "rfm2g_api.h"



#define STR_DEVICE_TYPE_2   "Rfm5565"



typedef struct {

	RFM2G_CHAR     device[40];             /* Name of RFM2G device to use          */
	RFM2GHANDLE    Handle;
	RFM2G_NODE     NodeId;

	RFM2G_UINT32 Offset;
	


} 
ST_RFM5565;



int init_RFM5565_taskConfig( ST_STD_device *pSTDdev);
int init_RFM5565_vender( ST_RFM5565 * );
void exit_RFM5565_taskConfig(void *pArg, double arg1, double arg2);



/*ST_threadCfg*  make_controlThreadConfig(ST_RFM5565 *pRFM5565); */




void threadFunc_user_DAQ(void *param);
void threadFunc_user_RingBuf(void *param);
void threadFunc_user_CatchEnd(void *param);



void _bin_show(unsigned char data);



#endif /* drvRFM5565_H */

