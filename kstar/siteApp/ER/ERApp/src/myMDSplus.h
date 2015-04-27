#ifndef DEV_MDSPLUS_H
#define DEV_MDSPLUS_H


#include "drvNIscope.h"


#define TEST_NODE_NUM		2
#define TEST_SEGMENT_NUM	10
#define TEST_DATA_NUM		10
#define TEST_SAMPLING_RATE	10

#define TEST_TREE_NAME		"r_spectro"
#define TEST_SHOT_NUM		1
#define TEST_MDS_ADDR		"172.17.102.58:8000"
#define TEST_EVENT_NAME		"VBS"
#define TEST_EVENT_ADDR		"127.0.0.1:8000"
#define TEST_DATA_FILE_HOME	"/data/rawData"



int mds_sendDatatoTree( ST_STD_device *pSTDdev, ST_STD_channel *pSTDch);


#endif

