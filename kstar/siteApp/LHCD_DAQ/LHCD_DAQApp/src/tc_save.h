#ifndef __TCMDSPLUS__
#define __TCMDSPLUS__

typedef short           int16;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

#define MAX_SYS_TC		8
#define LTU_CMD_HEAD    0x10
#define MAX_CARD_NUM    10
#define TC_MAX_CH       8

#define USE_CARD_NUM    4

#define BUF_SIZE        32
#define CARD_SIZE       10
#define CHANNEL_SIZE    8

#define SWAP_UINT(x) \
                ((uint32)( \
                (((uint32)(x) & (uint32)0x000000ff) << 24) | \
                (((uint32)(x) & (uint32)0x0000ff00) <<  8) | \
                (((uint32)(x) & (uint32)0x00ff0000) >>  8) | \
                (((uint32)(x) & (uint32)0xff000000) >> 24) ))

typedef struct __LTU_HEAD
{
	char pvName[CARD_SIZE][CHANNEL_SIZE][BUF_SIZE];
	int nDataCnt;
} LTU_HEAD;

typedef struct __MDS_NODE
{
	int mdsput;
	char node[10];
} TC_MDS_NODE;

typedef struct _tagTCsave {
	char ip[20];
	int sock;
	int card;

	int t0;
	int t1;

	int localshot;
	int kstarshot;
	int shot;

	int status;   /* MDSPLUS PUT Ok, Fault */

	int msg;

	TC_MDS_NODE mdsinfo[MAX_CARD_NUM][TC_MAX_CH];

	FILE *fp[MAX_CARD_NUM];
} TCMDS_INFO;


int acqMdsPutTCData(TCMDS_INFO *tcmds, int mode);

extern TCMDS_INFO tcmds_info[MAX_SYS_TC];

#endif
