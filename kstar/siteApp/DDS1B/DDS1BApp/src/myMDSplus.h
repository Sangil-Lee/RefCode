#ifndef _ACQ_MDSPLUS_H
#define _ACQ_MDSPLUS_H



#include "drvACQ196.h"


int proc_sendData2Tree(ST_STD_device *pSTDdev);
int proc_dataChannelization(ST_STD_device *pSTDdev);
int swap32(int *n);
void clearAllchFiles(int slot);



#endif

