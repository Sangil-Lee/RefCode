#ifndef __OZ_CLTU_H__
#define __OZ_CLTU_H__

/*********************************************/

#include "drvCLTU.h"

#define CMD_WAIT_TIME_OUT (5)
#define MAX_CMD_WAIT_TIME_OUT  (500)

#define OFFSET_32BIT_REG (4)

#define PCI_INFO_MEM_BASEADDR_OFFSET ( 0x0000 )
#define PCI_INFO_MEM_HIGHADDR_OFFSET ( 0x07FF )
#define PCI_INFO_MEM_SIZE            ( PCI_INFO_MEM_HIGHADDR_OFFSET - PCI_INFO_MEM_BASEADDR_OFFSET + 1)

#define PCI_BASE2PMC_BASEADDR_OFFSET ( 0x0800 )
#define PCI_BASE2PMC_HIGHADDR_OFFSET ( 0x0BFF )
#define PCI_BASE2PMC_SIZE            ( PCI_BASE2PMC_HIGHADDR_OFFSET - PCI_BASE2PMC_BASEADDR_OFFSET + 1)

#define PCI_PMC2BASE_BASEADDR_OFFSET ( 0x0C00 )
#define PCI_PMC2BASE_HIGHADDR_OFFSET ( 0x0FFF )
#define PCI_PMC2BASE_SIZE            ( PCI_PMC2BASE_HIGHADDR_OFFSET - PCI_PMC2BASE_BASEADDR_OFFSET + 1)



int cltu_cmd_endian( ST_drvCLTU * );
int cltu_cmd_write32(ST_drvCLTU *,  int address, int data );
int cltu_cmd_read32( ST_drvCLTU *,  int address );
int cltu_cmd_set_mode( ST_drvCLTU *, int mode );
int cltu_cmd_set_shot_start( ST_drvCLTU * );
int cltu_cmd_set_shot_term(ST_drvCLTU *, unsigned int term_msb, unsigned int term_lsb );
int cltu_cmd_set_ct_ss(ST_drvCLTU *, int portnum, int trigger_level, int clock_hz,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1 );
int cltu_cmd_set_ct_ss_rfm(ST_drvCLTU *, int portnum, int rfm_en, int rfm_clk_hz );
int cltu_cmd_set_ct_ms(ST_drvCLTU *,
					int portnum, int trigger_level, 
                                    int clk0, int clk1, int clk2, int clk3, int clk4, 
									int trg0, int trg1, int trg2, int trg3, int trg4, 
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1,
                                    int msb_t2, int lsb_t2,
                                    int msb_t3, int lsb_t3,
                                    int msb_t4, int lsb_t4,
                                    int msb_t5, int lsb_t5 );
int cltu_cmd_cal_ltu(ST_drvCLTU *, int ltu_num );

int check_ctu( ST_drvCLTU * );
int cltu_cmd_set_ref_clk( ST_drvCLTU *pCfg,  int ref_clk_sel );




#ifdef FEATURE_SINGLE_UPDATE
int cltu_cmd_single_mt_update( void );
#endif
int cltu_pmc_info_show( ST_drvCLTU * );


#endif /* __drvCLTU_H__ */

