#ifndef drvERSC_H
#define drvERSC_H


#include "sfwDriver.h"
#include "sfwTree.h" /* segmented archiving */

#include "dbAccess.h"   



#define STR_FILE_PATH_NFS   "/nfriapp"
#define STR_FILE_PATH_JFFS   "/nfriapp"


#define STR_DEVICE_TYPE_1   "scFMC150"


#define MAX_CH_SIZE_ADC	0x10000000  // 256MB per channel
//#define MAX_ADC_BD_CNT		8  // 2013. 8. 5
//#define MAX_DAC_BD_CNT		8
#define MAX_ADC_BD_CNT		100
#define MAX_DAC_BD_CNT		100



#define INIT_NUM_OF_RECORD	8	// 32 MB per each section
#define INIT_SIZE_OF_RECORD	0x2000000	// 32 MB per each section
#define INIT_SAMPLE_CLOCK_A		(200000000.0)	// 200MHz
#define INIT_SAMPLE_CLOCK_B		(100000000.0)	// 100MHz

/*
 *  Address of control register
 */

#define MAP_ADC_SIZE	0x10000000	// 256 MB

#define MAP_SIZE	65536UL
#define MAP_MASK 	(MAP_SIZE - 1)

#define MAP_BD_SIZE		4096UL
#define MAP_BD_MASK 	(MAP_BD_SIZE - 1)


#define C_BASEADDR			0xC7A00000

#define C_MEM0_BASEADDR			0xCC800000
#define C_MEM0_DATAADDR			0xCC810000
#define C_MEM1_BASEADDR			0xCC820000
#define C_MEM2_BASEADDR			0xCC840000
#define C_MEM3_BASEADDR			0xCC860000


/* board control resister */
#define BCR_RESET_GLOBAL		0x01
#define BCR_DAC_C_RUN		0x100
#define BCR_ADC_A_RUN		0x200
#define BCR_DAC_D_RUN		0x400
#define BCR_ADC_B_RUN		0x800
#define BCR_ALL_TXRX_RUN		0xF00
#define BCR_RX_A_EXT_TRIG		0x10000
#define BCR_RX_B_EXT_TRIG		0x20000


#define CH_TX_C		0
#define CH_TX_D		1
#define CH_RX_A		0
#define CH_RX_B		1

#define ADDR_START_RX_A	0x04000000      // 64 MB offset
#define ADDR_START_RX_B	0x14000000      // 256 + 64 MB


typedef struct
{
	volatile unsigned int gpRevision;			// 00	
	volatile unsigned int gpBuildDate;			// 04
	volatile unsigned int gpSerialNum;			// 08
	volatile unsigned int gpReadWrite;			// 0C
	volatile unsigned int gpBoardControl;			// 10
	volatile unsigned int gpIntMask;			// 14
	volatile unsigned int gpIntStatus;			// 18
	volatile unsigned int gpA_RX_Desc;			// 1C
	volatile unsigned int gpB_RX_Desc;			// 20
	volatile unsigned int gpC_TX_Desc;			// 24
	volatile unsigned int gpD_TX_Desc;			// 28
	volatile unsigned int gpVfbc1_tloop_cnt;		// 2C
	volatile unsigned int gpVfbc1_txfer_cnt;		// 30
	volatile unsigned int gpVfbc1_tfread_cnt;		// 34
	volatile unsigned int gpVfbc2_ttrg_cnt;			// 38
	volatile unsigned int gpVfbc2_txfer_cnt ;		// 3C
	volatile unsigned int gpVfbc3_tloop_cnt;		// 40
	volatile unsigned int gpVfbc3_txfer_cnt;		// 44
	volatile unsigned int gpVfbc3_tfread_cnt;		// 48
	volatile unsigned int gpVfbc4_ttrg_cnt;			// 4C
	volatile unsigned int gpVfbc4_txfer_cnt;		// 50

}
ST_REG_MAP;


typedef struct
{
//	volatile unsigned int xBD[16];  // 2013. 8. 5
//	volatile unsigned int reserved[1008]; //1024 - 16
	volatile unsigned int xBD[200];
	volatile unsigned int reserved[824]; //1024 - 200
}
ST_BD_MAP;



typedef struct {

	int  fd;
	ST_REG_MAP *pBaseAddr;

	ST_BD_MAP *pTxC_BD;
	signed int *pTxC_DATA;
	
	ST_BD_MAP *pTxD_BD;
	signed int *pTxD_DATA;

	ST_BD_MAP *pRxA_BD;
//	signed int *pRxA_DATA;
	signed short *pRxA_DATA;
	
	ST_BD_MAP *pRxB_BD;
	signed short *pRxB_DATA;


	char strTx_filePath[64];
	char strTxC_fileName[16];
	char strTxD_fileName[16];

	unsigned int ch_mask;

	unsigned int recordNum; /* : 8 */
	unsigned int recordSize; /* : 32 MB, unit: Byte */

	char strTagName[2][SIZE_TAG_NAME];
	float run_period; /* default: 10 sec */

	/* segmented Archiving */
	sfwTreePtr		pSfwTree;
	sfwTreeNodePtr	pSfwTreeNode[2];
	sfwTreeNodePtr	pSfwTreeScaleNode[2];
	

} 
ST_ERSC;



int init_my_sfwFunc_ERSC( ST_STD_device *pSTDdev);
int create_ERSC_taskConfig( ST_STD_device *pSTDdev);
void clear_ERSC_taskConfig(void *pArg, double arg1, double arg2);


void threadFunc_ERSC_DAQ(void *param);

void func_ERSC_OP_MODE(void *, double, double);
void func_ERSC_BOARD_SETUP(void *, double, double);
void func_ERSC_SYS_ARMING(void *, double, double);
void func_ERSC_SYS_RUN(void *, double , double );
void func_ERSC_SAMPLING_RATE(void *, double , double );
void func_ERSC_DATA_SEND(void *, double , double );
void func_ERSC_SYS_RESET(void *, double , double );
void func_ERSC_POST_SEQSTOP(void *, double , double );




/*****************************************/
/* application */
int drvERSC_load_pattern(ST_STD_device *, int);
void drvERSC_print_BufferDescriptor(ST_STD_device *);
int drvERSC_set_ADC_BD(ST_STD_device *, int);
int drvERSC_clear_ADC_BD(ST_STD_device *, int);
int drvERSC_get_Multi_Trig_Info(ST_STD_device *);


#endif /* drvERSC_H */

