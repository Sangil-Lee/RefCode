#ifndef __RFCTRL__
#define __RFCTRL__

#include "rfglobal.h"

#define RFCTRL_PORT			50001

#define RFCTRL_MAX_DATA		100

/******************************************************/
/*  MDSDATA Define                                    */
/******************************************************/
#define MDSDATA_INIT_HEAD   0x20
#define MDSDATA_ETC_DATA    0x21
#define MDSDATA_ARRY_DATA   0x22

#define DATA_GET_OK			10

typedef struct _MDSDATA_HEAD {
    int             count;
    int             samp_clk;
				    
    float           fwd_1st;
    float           fwd_2st;
    float           fwd_its;
    float           fwd_ofs;
    float           fwd_loss;
									    
    float           rev_1st;
    float           rev_2st;
    float           rev_its;
    float           rev_ofs;
    float           rev_loss;

	float           phase_offset;
	float           iq_i_offset;
	float           iq_q_offset;
				
} MDSDATA_HEAD;

typedef struct __RFCTRL_INFO__
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

	float				fwd_1st;
	float				fwd_2st;
	float				fwd_its;
	float				fwd_ofs;
	float				fwd_loss;

	float				rev_1st;
	float				rev_2st;
	float				rev_its;
	float				rev_ofs;
	float				rev_loss;

	float				phase_offset;
	float				iq_i_offset;
	float				iq_q_offset;

    FILE                *fp[4];
} RFCTRL_INFO;

int set_up_rf( char *ip, int mode );
int recv_data_get( int sock, char *temp, int count);
int drvRFCTRL_Configure(int start, char *ip, int port);

void rf_datafileOpen(RFCTRL_INFO *lhcd_info);
void rf_datafileClose(RFCTRL_INFO *lhcd_info);

extern RFCTRL_INFO lhcd_rfctrl;

#endif
