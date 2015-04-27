#ifndef __RFANT__
#define __RFANT__

#include "rfglobal.h"

#define RFANT_PORT			50001

#define RFANT_MAX_DATA		100

#define RFANT_MAX_POW		8
#define RFANT_MAX_OFF		7

#define RFANT_MAX_DATAFILE	30

/******************************************************/
/*  MDSDATA Define                                    */
/******************************************************/
#define MDSDATA_INIT_HEAD   0x20
#define MDSDATA_ETC_DATA    0x21
#define MDSDATA_ARRY_DATA   0x22

#define RFANT_DATA_GET_OK			10

#pragma pack(1)
typedef struct _RFANT_MDSDATA_RAW {
#if 0  /* IQ - old */
	uint16	iq_i[7];
	uint16	iq_q[7];
#endif
	uint16	fwd[8];
	uint16	rev[8];
} RFANT_MDS_RAW;

typedef struct _RFANT_MDSDATA_HEAD {
    int             count;
    int             samp_clk;
				    
    float           fwd_1st[8];
    float           fwd_2st[8];
    float           fwd_its[8];
    float           fwd_ofs[8];
    float           fwd_loss[8];
									    
    float           rev_1st[8];
    float           rev_2st[8];
    float           rev_its[8];
    float           rev_ofs[8];
    float           rev_loss[8];

	float           iqi_offset[7];
	float           iqq_offset[7];
	float           iq_gain[7];
				
} RFANT_MDS_HEAD;
#pragma pack()

typedef struct __RFANT_INFO__
{
    char                ip[16];
    int                 port;
    int                 sock;
	int					con_stat;
	float				samplclk;
	int					mds_status;
	int					data_count;
	int					shot;
    double              t0;
    double              t1;

	float				fwd_1st[8];
	float				fwd_2st[8];
	float				fwd_its[8];
	float				fwd_ofs[8];
	float				fwd_loss[8];

	float				rev_1st[8];
	float				rev_2st[8];
	float				rev_its[8];
	float				rev_ofs[8];
	float				rev_loss[8];

	float				iq_i_offset[7];
	float				iq_q_offset[7];
	float				iq_gain[7];

	FILE				*iq_i_fp[7];
	FILE				*iq_q_fp[7];
	FILE				*fwd_fp[8];
	FILE				*rev_fp[8];
} RFANT_INFO;

int drvRFANT_Configure(int start, char *ip, int port);

extern RFANT_INFO lhcd_rfant;

#endif
