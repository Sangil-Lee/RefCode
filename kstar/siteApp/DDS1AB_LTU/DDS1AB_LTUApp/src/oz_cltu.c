/******************************************************************************
 *  Copyright (c) 2007 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/


/*==============================================================================
                        EDIT HISTORY FOR MODULE
                        
*/

#include "oz_cltu.h"
#include "cltu_Def.h"




#if 0
#if defined(_CLTU_VXWORKS_)
 /* copyed from "$WIN_BASE/target/h/drv/pci/pciConfigLib.h" */
#define	PCI_CFG_BASE_ADDRESS_0	0x10
#define	PCI_CFG_BASE_ADDRESS_2	0x18
#define	PCI_CFG_DEV_INT_LINE	  0x3c
#endif
#endif






/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_endian
Author    : pianist
Parameter : .
Return    : .
Desc      :  ctl = 0x00000000 (PPC) else (x86)
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_endian( ST_drvCLTU *pCfg )
{
  int endian_signal, ctl;
  PCI_COM_S* msg;
  int time_out = 0;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  endian_signal = READ32( pCfg->pciDevice.base2 + PCI_INFO_MEM_BASEADDR_OFFSET );

	if( endian_signal == ENDIAN_SIGNAL ) return WR_OK;
		
  if( endian_signal == ENDIAN_SIGNAL )
  {
    ctl = 0x00000000; /* PPC */
  }
  else
  {
    ctl = 0xFFFFFFFF; /* Intel */
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
  	printf("msg->status = %x\n",msg->status);
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = ctl;
  msg->cmd     = CLTU_CMD_ENDIAN_CTL;

  DELAY_WAIT(1);

  	  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_write32( ST_drvCLTU *pCfg, int address, int data )
{
  PCI_COM_S* msg;
  int time_out = 0;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = address;
  msg->status  = 0;
  msg->data    = data;
  msg->cmd     = CLTU_CMD_WRITE32;
  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_read32( ST_drvCLTU *pCfg, int address )
{

  PCI_COM_S* msg;
  int time_out = 0;
  
  int wait = 1;
  int time_count = 0;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }


  msg->address = address;
  msg->status  = 0;
  msg->data    = 0;
  msg->cmd     = CLTU_CMD_READ32;

  while( wait )
  {
    time_count++;
    if( msg->status == 0xFFFFFFFF || time_count > 1000 )
      wait = 0;
  }

  if( time_count > 10000 )
  {
    CLTU_ASSERT();     /* time out error. */
    return WR_ERROR;
  }

  printf("[CLTU] read val = 0x%x\n", msg->data);

  return msg->data;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_mode
Author    : pianist
Parameter : .
Return    : .
Desc      : synch or test mode setting.
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_mode(ST_drvCLTU *pCfg,  int mode )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = mode;
  msg->cmd     = CLTU_CMD_SET_MODE;
  
	return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_shot_start
Author    : pianist
Parameter : .
Return    : .
Desc      :  CTU only 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_shot_start( ST_drvCLTU *pCfg )
{
  PCI_COM_S* msg;
  
#if 1
  int time_out = 0;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);
  DELAY_WAIT(0);

  while( msg->status != 0xFFFFFFFF)
  {
  	printf("R1, status error in shot start!\n");
	
    if( time_out > 10 )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(0);
  }
#else
  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);
#endif

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->cmd     = CLTU_CMD_SHOT_START;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_shot_term
Author    : pianist
Parameter : .
Return    : .
Desc      : CTU only
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_shot_term( ST_drvCLTU *pCfg, unsigned int term_msb, unsigned int term_lsb )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = term_msb;
  msg->arg[1]  = term_lsb;  
  msg->cmd     = CLTU_CMD_SHOT_TERM;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ct_ss
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ct_ss(ST_drvCLTU *pCfg, 
					int portnum, int trigger_level, int clock_hz,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1 )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
/*    printf("[cltu_cmd_set_ct_ss] msg->status = 0x%x\n", msg->status ); */
/*    printf("[cltu_cmd_set_ct_ss] time_out = 0x%x\n", time_out );     */
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++; 	 
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = trigger_level;
  msg->arg[2]  = clock_hz;
  msg->arg[3]  = msb_t0;
  msg->arg[4]  = lsb_t0;
  msg->arg[5]  = msb_t1;
  msg->arg[6]  = lsb_t1;
  msg->cmd     = CLTU_CMD_SET_CT_SS;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ct_ss_rfm
Author    : pianist
Parameter : .
Return    : .
Desc      : change clock on shot
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ct_ss_rfm( ST_drvCLTU *pCfg,  int portnum, int rfm_en, int rfm_clk_hz )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = rfm_en;
  msg->arg[2]  = rfm_clk_hz;  
  msg->cmd     = CLTU_CMD_SET_CT_SS_RFM;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_write32
Author    : pianist
Parameter : .
Return    : .
Desc      : multi trigger setting  only port 4
Date      : 2007.06.22(initial create.)
			parameter (trg0 ~ trg4) added. { On => !0, Off => 0}..
*******************************************************************************/
int cltu_cmd_set_ct_ms(ST_drvCLTU *pCfg, 
					int portnum, int trigger_level, 
                                    int clk0, int clk1, int clk2, int clk3, int clk4,
									int trg0, int trg1, int trg2, int trg3, int trg4,
                                    int msb_t0, int lsb_t0, 
                                    int msb_t1, int lsb_t1,
                                    int msb_t2, int lsb_t2,
                                    int msb_t3, int lsb_t3,
                                    int msb_t4, int lsb_t4,
                                    int msb_t5, int lsb_t5 )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = portnum;
  msg->arg[1]  = trigger_level;

  msg->arg[2]  = clk0;  

  msg->arg[3]  = clk1;  

  msg->arg[4]  = clk2;  

  msg->arg[5]  = clk3;  

  msg->arg[6]  = clk4;



  msg->arg[7]  = trg0;  

  msg->arg[8]  = trg1;  

  msg->arg[9]  = trg2;  

  msg->arg[10]  = trg3;  

  msg->arg[11]  = trg4;



  

  msg->arg[12]   = msb_t0;  

  msg->arg[13]   = lsb_t0;  

  msg->arg[14]   = msb_t1;  

  msg->arg[15]  = lsb_t1;  

  msg->arg[16]  = msb_t2;  

  msg->arg[17]  = lsb_t2;  

  msg->arg[18]  = msb_t3;  

  msg->arg[19]  = lsb_t3;  

  msg->arg[20]  = msb_t4;  

  msg->arg[21]  = lsb_t4;  

  msg->arg[22]  = msb_t5;  

  msg->arg[23]  = lsb_t5; 
  
  msg->cmd     = CLTU_CMD_SET_CT_MS;
  
  return WR_OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_set_ref_clk
Author    : pianist
Parameter : ref_clk_sel : 0xffffffff -> external mode  or 0x0 -> internal mode
Return    : .
Desc      : only CTU, external 50M clock 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_set_ref_clk( ST_drvCLTU *pCfg,  int ref_clk_sel )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = ref_clk_sel;
  msg->cmd     = CLTU_REF_CLK_SEL;
  
  return WR_OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : cltu_cmd_cal_ltu
Author    : pianist
Parameter : arg(ltu_num) : 2^7 integer number  ,    8-on : run for CTU
Return    : .
Desc      : CTU only , 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_cmd_cal_ltu( ST_drvCLTU *pCfg,  int ltu_num )
{
  PCI_COM_S* msg;
  int time_out = 0;


  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  msg = (PCI_COM_S*) (pCfg->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

  while( msg->status != 0xFFFFFFFF)
  {
    if( time_out > MAX_CMD_WAIT_TIME_OUT )
    {
      CLTU_ASSERT();
      return WR_ERROR;
    }
    time_out++;    
    DELAY_WAIT(1);
  }

  msg->address = 0;
  msg->status  = 0;
  msg->data    = 0;
  msg->arg[0]  = ltu_num;
  msg->cmd     = CLTU_CAL_LTU;
  
  return WR_OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : check_ctu
Author    : pianist
Parameter : .
Return    : TRUE -> CTU else LTU
Desc      : 
Date      : 2007.06.22(initial create.)
			2007.11.21( remove read32.... )
*******************************************************************************/
int check_ctu( ST_drvCLTU *pCfg )
{
	int sw_id;
	System_Info* mSys;
	mSys = (System_Info*) (pCfg->pciDevice.base2);
	
	sw_id = mSys->ltu.id;
/*	sw_id = cltu_cmd_read32( 0x71200000 + ( OFFSET_32BIT_REG * 0)); */

	return (0x00000080 & sw_id );
}


/*******************************************************************************
Company   : Olzetek
Function  : nfrc_mon_update_show
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int cltu_pmc_info_show( ST_drvCLTU *pCfg )
{
  
/*  int i;*/
  System_Info* mSys;
  

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    CLTU_ASSERT(); return WR_ERROR;
  }

  mSys = (System_Info*) (pCfg->pciDevice.base2);
  
  printf("\n=======================================================\n");
  
  printf("mSys->endian_signal               : %d\n", mSys->endian_signal);  
  printf("mSys->ver_info.board              : %d\n", mSys->ver_info.board);
  printf("mSys->ver_info.fpga               : %d\n", mSys->ver_info.fpga);
  printf("mSys->ver_info.sw                 : %d\n\n", mSys->ver_info.sw);
  printf("mSys->single_update               : %d\n", mSys->single_update);
  printf("mSys->single_update_arm           : %d\n\n", mSys->single_update_arm);
  printf("mSys->log_status                  : 0x%x\n", mSys->log_status);
#ifdef FEATURE_HEADER_64_TO_32
  printf("mSys->ShotTerm_msb                : 0x%x\n", mSys->ShotTerm_msb);  
  printf("mSys->ShotTerm_lsb                : 0x%x\n", mSys->ShotTerm_lsb);  
  
  printf("mSys->CurrentTime_msb             : 0x%x\n", mSys->CurrentTime_msb);
  printf("mSys->CurrentTime_lsb             : 0x%x\n", mSys->CurrentTime_lsb);
  
  printf("mSys->ShotStartTime_msb           : 0x%x\n", mSys->ShotStartTime_msb);
  printf("mSys->ShotStartTime_lsb           : 0x%x\n", mSys->ShotStartTime_lsb);
  
  printf("mSys->ShotStopTime_msb            : 0x%x\n", mSys->ShotStopTime_msb);
  printf("mSys->ShotStopTime_lsb            : 0x%x\n", mSys->ShotStopTime_lsb);  
#else
  printf("mSys->ShotTerm                    : 0x%llx\n", mSys->ShotTerm);  
  printf("mSys->CurrentTime                 : 0x%llx\n", mSys->CurrentTime);
  printf("mSys->ShotStartTime               : 0x%llx\n", mSys->ShotStartTime);
  printf("mSys->ShotStopTime                : 0x%llx\n", mSys->ShotStopTime);
#endif  
  printf("mSys->op_mode                     : 0x%x\n", mSys->op_mode);

  printf("\n");
  printf("mSys->serdes.tx_only              : 0x%x\n", mSys->serdes.tx_only);
  printf("mSys->serdes.rx_link_status       : 0x%x\n", mSys->serdes.rx_link_status);
  printf("mSys->serdes.good_cnt             : %d\n", mSys->serdes.good_cnt);
  printf("mSys->serdes.error_cnt            : %d\n", mSys->serdes.error_cnt);



  printf("\n");
  printf("mSys->ctu.irigb_status            : 0x%x\n", mSys->ctu.irigb_status);
  printf("mSys->ctu.ref_clk_source          : 0x%x\n", mSys->ctu.ref_clk_source);
  printf("mSys->ctu.gps_locked              : 0x%x\n", mSys->ctu.gps_locked);
  printf("mSys->ctu.dcm_locked              : 0x%x\n", mSys->ctu.dcm_locked); /* OPI: main ctu M.Clock */
  printf("mSys->ctu.rx_linked_ltu           : 0x%x\n", mSys->ctu.rx_linked_ltu);

  printf("\n");
  printf("mSys->ltu.id                      : 0x%x\n", mSys->ltu.id);  /* LTU ID number : 0 ~ 2^7  */
  printf("mSys->ltu.ref_clk_source          : 0x%x\n", mSys->ltu.ref_clk_source);
  printf("mSys->ltu.dcm_locked              : 0x%x\n", mSys->ltu.dcm_locked);
  printf("mSys->ltu.compensation_offset1    : 0x%x\n", mSys->ltu.compensation_offset1); /* unit; 10 ns */
  printf("mSys->ltu.compensation_offset2    : 0x%x\n", mSys->ltu.compensation_offset2); /* not used */
#if 0
  printf("\n");
  for( i = 0; i < 4; i++ )
  {
  	printf("mSys->ltu.cts[%d].trigger_level    : 0x%x\n",i, mSys->ltu.cts[i].trigger_level);
  	printf("mSys->ltu.cts[%d].clock            : %d\n",i, mSys->ltu.cts[i].clock);
#ifdef FEATURE_HEADER_64_TO_32
  	printf("mSys->ltu.cts[%d].start_offset_msb : 0x%x\n",i, mSys->ltu.cts[i].start_offset_msb);
  	printf("mSys->ltu.cts[%d].start_offset_lsb : 0x%x\n",i, mSys->ltu.cts[i].start_offset_lsb);
    
  	printf("mSys->ltu.cts[%d].stop_offset_msb  : 0x%x\n",i, mSys->ltu.cts[i].stop_offset_msb);
   	printf("mSys->ltu.cts[%d].stop_offset_lsb  : 0x%x\n",i, mSys->ltu.cts[i].stop_offset_lsb);
#else    
  	printf("mSys->ltu.cts[%d].start_offset     : 0x%llx\n",i, mSys->ltu.cts[i].start_offset);
  	printf("mSys->ltu.cts[%d].stop_offset      : 0x%llx\n",i, mSys->ltu.cts[i].stop_offset);
#endif    
  	printf("mSys->ltu.cts[%d].rfmen            : 0x%x\n",i, mSys->ltu.cts[i].rfmen);
  	printf("mSys->ltu.cts[%d].rfmclock         : %d\n",i, mSys->ltu.cts[i].rfmclock);
  }

  printf("\n");
  printf("mSys->ltu.ctm.trigger_level       : 0x%x\n", mSys->ltu.ctm.trigger_level);
  printf("mSys->ltu.ctm.clock1              : %d\n", mSys->ltu.ctm.clock1);
  printf("mSys->ltu.ctm.clock2              : %d\n", mSys->ltu.ctm.clock2);
  printf("mSys->ltu.ctm.clock3              : %d\n", mSys->ltu.ctm.clock3);
  printf("mSys->ltu.ctm.clock4              : %d\n", mSys->ltu.ctm.clock4);
  printf("mSys->ltu.ctm.clock5              : %d\n", mSys->ltu.ctm.clock5);
#ifdef FEATURE_HEADER_64_TO_32
  printf("mSys->ltu.ctm.offset1_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset1_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset2_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset2_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset3_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset3_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset4_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset4_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset5_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset5_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);
  printf("mSys->ltu.ctm.offset6_msb         : 0x%x\n", mSys->ltu.ctm.offset1_msb);
  printf("mSys->ltu.ctm.offset6_lsb         : 0x%x\n", mSys->ltu.ctm.offset1_lsb);  
#else  
  printf("mSys->ltu.ctm.offset1             : 0x%llx\n", mSys->ltu.ctm.offset1);
  printf("mSys->ltu.ctm.offset2             : 0x%llx\n", mSys->ltu.ctm.offset2);
  printf("mSys->ltu.ctm.offset3             : 0x%llx\n", mSys->ltu.ctm.offset3);
  printf("mSys->ltu.ctm.offset4             : 0x%llx\n", mSys->ltu.ctm.offset4);
  printf("mSys->ltu.ctm.offset5             : 0x%llx\n", mSys->ltu.ctm.offset5);
  printf("mSys->ltu.ctm.offset6             : 0x%llx\n", mSys->ltu.ctm.offset6);
#endif  
#endif
  printf("\n=======================================================\n");
  
  return WR_OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : sec_to_10ns_64bit_value
Author    : pianist
Parameter : .
Return    : .
Desc      : 
Date      : 2007.06.22(initial create.)
*******************************************************************************/
int sec_to_10ns_64bit_value( unsigned int sec, unsigned int* msbT, unsigned int* lsbT)
{
  unsigned long long  int time64;

  time64 = sec * 100000000;

  *msbT = (unsigned int) (time64>>32);
  *lsbT = (unsigned int) time64;
  
  return WR_OK;
}




