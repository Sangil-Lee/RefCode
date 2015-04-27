/******************************************************************************
 *  Copyright (c) 2009 ~ by NFRI. All Rights Reserved.                     *
 *                                                                            *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

when       who    what, where, why
YY/MM/DD
--------   ---    --------------------------------------------------------------
2011/04/22  wrlee  Created.
==============================================================================*/

#include <linux/delay.h> /* for udelay */

/*#include "cltu_driver.h" */
#include "drv_xtu2.h"
#include "cltu_debug.h"
#include "cltu_Def.h"






int r2_hms_HtoD( unsigned int* phmsH, unsigned char* phD, unsigned char* pmD, unsigned char* psD  )
{
  *phD = (unsigned char)((*phmsH & 0x00ff0000) >> 16); 
  *pmD = (unsigned char)((*phmsH & 0x0000ff00) >> 8 ); 
  *psD = (unsigned char)((*phmsH & 0x000000ff)      );
/*  printf("hms_HtoD : %d hour : %d min : %d sec %08x\n",*phD,*pmD,*psD,*phmsH); */
  return 0;
}



int r2_hms_DtoH( unsigned char* phD, const unsigned char* pmD, const unsigned char* psD, unsigned int* phmsH )
{
  *phmsH = (*phD << 16) | ( *pmD << 8) | ( *psD );  
/*  printf("msTick_DtoH : 0x%08x\n",*phmsH); */
  return 0;
}

int r2_ms100MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD )
{
  unsigned int ms100;
  unsigned int ms10;
  unsigned int ms1; 
  
  ms100   = (*pmsTickH & 0xf0000000) >> 28; 
  ms10    = (*pmsTickH & 0x0f000000) >> 24; 
  ms1     = (*pmsTickH & 0x00f00000) >> 20; 
  *pmsD   = (ms100 * 100) + (ms10 * 10)  + ms1;
  *pnsD   = (*pmsTickH & 0x000fffff) * 10; 
/*  PRINT_MSG0("msTick_HtoD : %03d ms - %7d ns 0x%08x\n",*pmsD,*pnsD,*pmsTickH,0,0,0); */
  return 0;
}



int r2_ms100MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH )
{
  unsigned int ms100;
  unsigned int ms10;
  unsigned int ms1; 
  unsigned int ns_tmp; 
  
  ms1   = (unsigned int)((*pmsD/1   )%10);
  ms10  = (unsigned int)((*pmsD/10  )%10);
  ms100 = (unsigned int)((*pmsD/100 )%10);

  ns_tmp = (unsigned int)((*pnsD)/10);

  *pmsTickH = (ms100 << 28) | (ms10 << 24) | (ms1 << 20) | (ns_tmp & 0x000fffff);
/*  PRINT_MSG0("msTick_DtoH : 0x%08x\n",*pmsTickH,0,0,0,0,0); */
  return 0;
}


int r2_ms200MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD )
{
  unsigned int ms100;
  unsigned int ms10;
  unsigned int ms1; 
  
  ms100   = (*pmsTickH & 0xf0000000) >> 28; 
  ms10    = (*pmsTickH & 0x0f000000) >> 24; 
  ms1     = (*pmsTickH & 0x00f00000) >> 20; 
  *pmsD   = (ms100 * 100) + (ms10 * 10)  + ms1;
  *pnsD = (*pmsTickH & 0x000fffff) * 5; 
/*  PRINT_MSG0("msTick_HtoD : %03d ms - %7d ns 0x%08x\n",*pmsD,*pnsD,*pmsTickH,0,0,0); */
  return 0;
}



int r2_ms200MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH )
{
  unsigned int ms100;
  unsigned int ms10;
  unsigned int ms1; 
  unsigned int ns_tmp; 
  
  ms1    = (unsigned int)((*pmsD/1   )%10);
  ms10   = (unsigned int)((*pmsD/10  )%10);
  ms100  = (unsigned int)((*pmsD/100 )%10);
  ns_tmp = (unsigned int)((*pnsD)/5);

  *pmsTickH = (ms100 << 28) | (ms10 << 24) | (ms1 << 20) | (ns_tmp & 0x000fffff);
/*  PRINT_MSG0("msTick_DtoH : 0x%08x\n",*pmsTickH,0,0,0,0,0); */
  return 0;
}

int r2_200MTimeSubtraction( ozTIME_T* ptimeA, unsigned int nsB, ozTIME_T* ptimeOUT)
{
	unsigned char subcarry;

	if((ptimeA->ns < nsB) && (ptimeA->ms == 0) && (ptimeA->sec == 0) && (ptimeA->min == 0) && (ptimeA->hour == 0) && (ptimeA->day == 0))
	{
//		err("xtu_200MTimeSubtraction subcarry error\n");
		return (-1);
	}

	if(ptimeA->ns < nsB)
	{
		ptimeOUT->ns = XTU_1000000ns + ptimeA->ns - nsB;
		subcarry = 1;
	}
	else
	{
		ptimeOUT->ns = ptimeA->ns - nsB;
		subcarry = 0;
	}
//	printf("ptimeA %dh.%dm.%ds.%dms.%dns <- %dns\n",ptimeA->hour,ptimeA->min,ptimeA->sec,ptimeA->ms,ptimeA->ns,nsB);
//	printf("subcarry ns %d\n",subcarry);

	if(ptimeA->ms < subcarry)
	{
		ptimeOUT->ms = XTU_1000ms + ptimeA->ms - 1;
		subcarry = 1;
	}
	else
	{
		ptimeOUT->ms = ptimeA->ms - subcarry;
		subcarry = 0;
	}
//	printf("subcarry ms %d\n",subcarry);



	if(ptimeA->sec < subcarry)
	{
		ptimeOUT->sec = XTU_60sec + ptimeA->sec - 1;
		subcarry = 1;
	}
	else
	{
		ptimeOUT->sec = ptimeA->sec - subcarry;
		subcarry = 0;
	}
//	printf("subcarry sec %d <= %d %d\n",subcarry,ptimeOUT->sec,ptimeA->sec);

	if(ptimeA->min < subcarry)
	{
		ptimeOUT->min = XTU_60min + ptimeA->min - 1;
		subcarry = 1;
	}
	else
	{
		ptimeOUT->min = ptimeA->min - subcarry;    
		subcarry = 0;
	}  
//	printf("subcarry min %d\n",subcarry);

	if(ptimeA->hour < subcarry)
	{
		ptimeOUT->hour = XTU_24hour+ ptimeA->hour - 1;
		subcarry = 1;
	}
	else
	{
		ptimeOUT->hour = ptimeA->hour - subcarry;    
		subcarry = 0;
	}  
//	printf("subcarry hour %d\n",subcarry);

//	printf("xtu_200MTimeSubtraction : %dh.%dm.%ds.%dms.%dns <- %dns\n",ptimeOUT->hour,ptimeOUT->min,ptimeOUT->sec,ptimeOUT->ms,ptimeOUT->ns,nsB);

	return 0;
}




int r2_SetEndian( CLTU_Device *device )
{
	WRITE32(device->virt_addr + XTU_ADDR_ENDIAN_SET, 0x01234567);
	return 0;
}

int r2_SetMode( CLTU_Device *device, unsigned int xtu_mode, unsigned char XTU_ID)
{
	WRITE32(device->virt_addr  + XTU_ADDR_SEL_XTU_MODE, xtu_mode);
	return 0;
}

int r2_SetDBGMode( CLTU_Device *device, ozXTU_DBGM_T* pxtu_dbgm)
{
	unsigned int wd_tmp;
	wd_tmp =   pxtu_dbgm->cdc_sel | pxtu_dbgm->jtag_sel;
	WRITE32(device->virt_addr  + XTU_ADDR_DBG_CONFIG, wd_tmp);
	return 0;
}

int r2_SetLocalTime( CLTU_Device *device, ozTIME_T* pltime )
{
	unsigned int wd_tmp;

	WRITE32(device->virt_addr + XTU_ADDR_SET_LOCAL_TIME, pltime->day);

	udelay(100);
	r2_hms_DtoH( &(pltime->hour), &(pltime->min), &(pltime->sec), &wd_tmp );

	WRITE32(device->virt_addr + XTU_ADDR_SET_LOCAL_TIME + 4, wd_tmp);

	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_CMD, XTU_CMD_SET_LOCAL_TIME);

	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 1);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 0);

	return 0;
}

int r2_SetCTUShotDelay( CLTU_Device *device )
{
	WRITE32(device->virt_addr + XTU_ADDR_SET_SHOT_DELAY, XTU_SHOT_DEFAULT_DELAY_TICK);
	return 0;
}
int r2_SetLTUShotDelay( CLTU_Device *device )
{
	unsigned int wd_tmp;
	wd_tmp = XTU_SHOT_DEFAULT_DELAY_TICK - (device->shot_delay_ns + 210) / 20;
	WRITE32(device->virt_addr + XTU_ADDR_SET_SHOT_DELAY, wd_tmp);
	return 0;
}
int r2_SetPortActiveLevel( CLTU_Device *device, unsigned int port_al )
{
	WRITE32(device->virt_addr  + XTU_ADDR_PORT_ACTIVE_LEVEL, port_al);
	return 0;
}

int r2_SetPortCtrl( CLTU_Device *device, unsigned int port )
{
	unsigned int addr_tmp;
	unsigned int addr_nhli;  

	switch(port)
	{
		case 0:
			addr_tmp = XTU_ADDR_PORTMEM_0;
			break;
		case 1:
			addr_tmp = XTU_ADDR_PORTMEM_1;
			break;
		case 2:
			addr_tmp = XTU_ADDR_PORTMEM_2;
			break;
		case 3:
			addr_tmp = XTU_ADDR_PORTMEM_3;
			break;
		case 4:
			addr_tmp = XTU_ADDR_PORTMEM_4;
			break;
		case 5:
			addr_tmp = XTU_ADDR_PORTMEM_5;
			break;
		case 6:
			addr_tmp = XTU_ADDR_PORTMEM_6;
			break;
		case 7:
			addr_tmp = XTU_ADDR_PORTMEM_7;
			break;
		default :
			info("[XTU] r2_SetPortCtrl .....error!"); 
			return (-1);
	}

	addr_nhli = XTU_ADDR_BRAM_PHLI_0 + (device->xtu_pcfg[port].nhli_num * 4);
	WRITE32(device->virt_addr + addr_tmp + XTU_ADDR_PCTRL, (device->xtu_pcfg[port].port_en | ((addr_nhli & 0x000001ff) << 16) ));

	return 0;
}

int r2_SetPortHLI( CLTU_Device *device, unsigned int port, unsigned int hli_id, ozPCONFIG_HLI_T* pphli)
{
	unsigned int wd_tmp;
	ozTIME_T time_tmp;
	void *base_addr;
	unsigned int addr_tmp;
	unsigned int addr_nhli;
	unsigned int addr_nlli;

	switch(port)
	{
	case 0:
		addr_tmp = XTU_ADDR_PORTMEM_0;
		break;
	case 1:
		addr_tmp = XTU_ADDR_PORTMEM_1;
		break;
	case 2:
		addr_tmp = XTU_ADDR_PORTMEM_2;
		break;
	case 3:
		addr_tmp = XTU_ADDR_PORTMEM_3;
		break;
	case 4:
		addr_tmp = XTU_ADDR_PORTMEM_4;
		break;
	case 5:
		addr_tmp = XTU_ADDR_PORTMEM_5;
		break;
	case 6:
		addr_tmp = XTU_ADDR_PORTMEM_6;
		break;
	case 7:
		addr_tmp = XTU_ADDR_PORTMEM_7;
		break;
	default :
		info("[XTU] r2_SetPortHLI .....error!"); 
		return (-1);
	}

	if(hli_id >= XTU_ADDR_PHLI_MAX_NUM)
	{
		info("[XTU] r2_SetPortHLI .....error!"); 
		return (-1);
	}

//	base_addr = (unsigned int)(device->virt_addr + addr_tmp + XTU_ADDR_PHLI_0 + (hli_id*16));
	base_addr = device->virt_addr + addr_tmp + XTU_ADDR_PHLI_0 + (hli_id*16);
//	addr_nhli = XTU_ADDR_BRAM_PHLI_0 + (pCfg->xtu_pcfg[port].phli[hli_num].nhli_num* 4);
//	addr_nlli = XTU_ADDR_BRAM_PLLI_0 + (pCfg->xtu_pcfg[port].phli[hli_num].nlli_num * 4); 
	addr_nhli = XTU_ADDR_BRAM_PHLI_0 + (pphli->nhli_num * 4);
	addr_nlli = XTU_ADDR_BRAM_PLLI_0 + (pphli->nlli_num * 4); 

	if(XTU_MAX_INF_NS == pphli->intv.ns)
	{
		WRITE32(base_addr    , 0);
		udelay(100);
		WRITE32(base_addr + 4, 0x000fffff);
	}
	else
	{
		r2_200MTimeSubtraction( &(pphli->intv), 55, &time_tmp);
		wd_tmp = (time_tmp.hour<< 16) | (time_tmp.min << 8) | (time_tmp.sec);
		udelay(100);
		WRITE32(base_addr    , wd_tmp);
		r2_ms200MTick_DtoH( &time_tmp.ms, &time_tmp.ns, &wd_tmp);
		WRITE32(base_addr + 4, wd_tmp);
	}

	udelay(100);
	WRITE32(base_addr + 12, ((addr_nhli & 0x000001ff) << 16) |  (addr_nlli & 0x000001ff) );

	return 0;
}

int r2_SetPortLLI( CLTU_Device *device, unsigned int port, unsigned int lli_id, ozPCONFIG_LLI_T* pplli)
{
	unsigned int wd_tmp;
	ozTIME_T time_tmp;
	unsigned int clk_time_tmp1;
	unsigned int clk_time_tmp2;
	unsigned int clk_ms_tmp;
	unsigned int clk_ns_tmp;
	void *base_addr;
	unsigned int addr_tmp;
	unsigned int addr_nlli;
  
	switch(port)
	{
	case 0:
		addr_tmp = XTU_ADDR_PORTMEM_0;
		break;
	case 1:
		addr_tmp = XTU_ADDR_PORTMEM_1;
		break;
	case 2:
		addr_tmp = XTU_ADDR_PORTMEM_2;
		break;
	case 3:
		addr_tmp = XTU_ADDR_PORTMEM_3;
		break;
	case 4:
		addr_tmp = XTU_ADDR_PORTMEM_4;
		break;
	case 5:
		addr_tmp = XTU_ADDR_PORTMEM_5;
		break;
	case 6:
		addr_tmp = XTU_ADDR_PORTMEM_6;
		break;
	case 7:
		addr_tmp = XTU_ADDR_PORTMEM_7;
		break;
	default :
		info("[XTU] r2_SetPortLLI .....error!"); 
		return (-1);
	}
  
	if(lli_id >= XTU_ADDR_PLLI_MAX_NUM)
	{
		info("[XTU] r2_SetPortLLI .....error!"); 
		return (-1);
	}

	base_addr = device->virt_addr + addr_tmp + XTU_ADDR_PLLI_0 + (lli_id*16);
	addr_nlli = XTU_ADDR_BRAM_PLLI_0 + (pplli->nlli_num * 4); 

	
	
	if(XTU_MAX_INF_NS == pplli->intv.ns)
	{
		WRITE32(base_addr	 , 0);
		udelay(100);
		WRITE32(base_addr + 4, 0x000fffff);
	}
	else
	{
		r2_200MTimeSubtraction( &(pplli->intv), 65, &time_tmp);
		wd_tmp = (time_tmp.hour<< 16) | (time_tmp.min << 8) | (time_tmp.sec);
		udelay(100);
		WRITE32(base_addr	 , wd_tmp);
		r2_ms200MTick_DtoH( &time_tmp.ms, &time_tmp.ns, &wd_tmp);
		udelay(100);
		WRITE32(base_addr + 4, wd_tmp);
	}

	if(pplli->clk_freq == 0)
	{
		clk_time_tmp1 = 0;
		clk_ms_tmp	  = 0;
		clk_ns_tmp	= 0;
//		pplli->clk_freq_result = 0;
	}
	else
	{

//		clk_time_tmp1 = (unsigned int)(1/((float)(pplli->clk_freq) / XTU_200MHz * 2 )); original
		clk_time_tmp1 = XTU_100MHz / pplli->clk_freq;

		clk_time_tmp2 = clk_time_tmp1 - 1;
		clk_ms_tmp	  = (unsigned int)(clk_time_tmp2 / XTU_MAX_200M_Tick);
		clk_ns_tmp	  = (unsigned int)(clk_time_tmp2 % XTU_MAX_200M_Tick) * 5;
//		pplli->clk_freq_result = (unsigned int)((float)(1/clk_time_tmp1) / 2 * XTU_200MHz);

	}

	if((pplli->option & XTU_PORT_CLK_OPT_ENABLE) == XTU_PORT_CLK_OPT_ENABLE)
	{
		WRITE32(base_addr + 8, 0x000fffff);
		udelay(100);
		WRITE32(base_addr + 12, (pplli->option & XTU_PORT_LLI_OPT_LOG_ON) |XTU_PORT_CLK_OPT_VARFREQ | (addr_nlli & 0x000001ff) );
	}
	else
	{
		r2_ms200MTick_DtoH( &clk_ms_tmp, &clk_ns_tmp, &wd_tmp);
		udelay(100);
		WRITE32(base_addr + 8, wd_tmp);
#if PRINT_LLI_CONFIG		
		printk(KI "clock freq %d.%d-%d-%d=%d\n", clk_ms_tmp, clk_ns_tmp, clk_time_tmp1, pplli->clk_freq, wd_tmp);
#endif
		udelay(100);
		WRITE32(base_addr + 12, (pplli->option) | (addr_nlli & 0x000001ff) );
	}

	return 0;
}

int r2_SetShotTime( CLTU_Device *device, ozSHOT_TIME_T* pshot_time)
{
	unsigned int wd_tmp;

	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME, pshot_time->start.day);
	wd_tmp = (pshot_time->start.hour << 16) | (pshot_time->start.min << 8) | (pshot_time->start.sec);
	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME + 4, wd_tmp);
	r2_ms100MTick_DtoH( &(pshot_time->start.ms), &(pshot_time->start.ns), &wd_tmp);
	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME + 8, wd_tmp);
	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME + 16, pshot_time->stop.day);
	wd_tmp = (pshot_time->stop.hour << 16) | (pshot_time->stop.min << 8) | (pshot_time->stop.sec);
	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME + 20, wd_tmp);
	r2_ms100MTick_DtoH( &(pshot_time->stop.ms), &(pshot_time->stop.ns), &wd_tmp);
	WRITE32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME + 24, wd_tmp);

	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_CMD, XTU_CMD_SET_SHOT_STIME | XTU_CMD_SET_SHOT_ETIME);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 1);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 0);
	
	return 0;
}


int r2_StartShotNow( CLTU_Device *device)
{
/*
  WRITE32(device->virt_addr + XTUR00E_, BIT31);
   udelay(100);
  WRITE32(device->virt_addr + XTUR00E_, 0);
   udelay(100);
*/
	WRITE32(device->virt_addr + XTU_ADDR_CMD, XTU_CMD_IMM_START);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 1);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 0);
	return 0;
}
int r2_StopShotNow( CLTU_Device *device)
{
	WRITE32(device->virt_addr + XTU_ADDR_CMD, XTU_CMD_IMM_STOP);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 1);
	udelay(100);
	WRITE32(device->virt_addr + XTU_ADDR_SET_CMD, 0);

	return 0;
}
int r2_SetIRIG_B_Src( CLTU_Device *device, unsigned int irigb_src)
{
	WRITE32(device->virt_addr  + XTU_ADDR_SEL_IRIGB, irigb_src);
	return 0;
}

int r2_SetClkConfig(CLTU_Device *device, unsigned int tlkRxClk_en, unsigned int tlkTxClk_sel, unsigned int mclk_sel )
{
	unsigned int wd_tmp;
	wd_tmp =   tlkRxClk_en | tlkTxClk_sel | mclk_sel ;
	WRITE32(device->virt_addr + XTU_ADDR_CLK_CONFIG, wd_tmp);
	return 0;
}
int r2_SetPermClk( CLTU_Device *device, unsigned int value)
{
	WRITE32(device->virt_addr + XTU_ADDR_PORT_INTR, value);
	return 0;
}

int r2_GetGshotLog(  CLTU_Device *device, unsigned int *pbuf)
{
	unsigned int i; 
	void *base_addr;
	base_addr = device->virt_addr  + XTU_ADDR_GSHOT_LOG_PRN;

	for(i = 0 ; (i < XTU_BMEM_SIZE/4); i++ )
	{
		udelay(10);
		pbuf[i*4 + 0] = READ32(base_addr + (i*16 + 0)  );    
		udelay(10);
		pbuf[i*4 + 1] = READ32(base_addr + (i*16 + 4)  );    
		udelay(10);
		pbuf[i*4 + 2] = READ32(base_addr + (i*16 + 8)  );    
		udelay(10);
		pbuf[i*4 + 3] = READ32(base_addr + (i*16 + 12) );    
/*		printf(" xtu_GetGshotLog loop cnt : %d:%08x|%08x|%08x|%08x\n", i, pbuf[i*4 + 0],pbuf[i*4 + 1],pbuf[i*4 + 2],pbuf[i*4 + 3]); */
	}

	return 0;
}

int r2_GetPortLog( CLTU_Device *device, unsigned char port_num, unsigned int *pbuf)
{
	unsigned int i;  
	void *base_addr;
	unsigned int addr_tmp;  

	switch(port_num)
	{
	case 0:
		addr_tmp = XTU_ADDR_PORTLOG_0;
		break;
	case 1:
		addr_tmp = XTU_ADDR_PORTLOG_1;
		break;
	case 2:
		addr_tmp = XTU_ADDR_PORTLOG_2;
		break;
	case 3:
		addr_tmp = XTU_ADDR_PORTLOG_3;
		break;
	case 4:
		addr_tmp = XTU_ADDR_PORTLOG_4;
		break;
	case 5:
		addr_tmp = XTU_ADDR_PORTLOG_5;
		break;
	case 6:
		addr_tmp = XTU_ADDR_PORTLOG_6;
		break;
	case 7:
		addr_tmp = XTU_ADDR_PORTLOG_7;
		break;
	default :
		err("[XTU] xtu_GetPortLog()"); 
		return (-1);
	}

	base_addr = device->virt_addr + addr_tmp;

	for(i = 0 ; (i < XTU_BMEM_SIZE/4); i++ )
	{
		udelay(10);
		pbuf[i*4 + 0] = READ32(base_addr + (i*16 + 0)  );    
		udelay(10);
		pbuf[i*4 + 1] = READ32(base_addr + (i*16 + 4)  );    
		udelay(10);
		pbuf[i*4 + 2] = READ32(base_addr + (i*16 + 8)  );    
		udelay(10);
		pbuf[i*4 + 3] = READ32(base_addr + (i*16 + 12) );    
/*		printf(" xtu_GetPortLog loop cnt : %d:%08x|%08x|%08x|%08x\n", i, pbuf[i*4 + 0],pbuf[i*4 + 1],pbuf[i*4 + 2],pbuf[i*4 + 3]); */
	}

  return 0;
}






int r2_GET_ENDIAN_CHECK(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_ENDIAN_CHECK);
	return 0;
}
int r2_GET_CLK_CONFIG(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_CLK_CONFIG);
	return 0;
}
int r2_GET_DBG_CONFIG(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_DBG_CONFIG);
	return 0;
}
int r2_GET_CLK_STATUS(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_CLK_STATUS);
	return 0;
}
int r2_GET_SEL_XTU_MODE(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_SEL_XTU_MODE);
	return 0;
}
int r2_GET_SEL_IRIGB(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_SEL_IRIGB);
	return 0;
}
int r2_GET_TX_CURR_TIME(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_CURR_TIME);
	return 0;
}
int r2_GET_TX_CURR_TIME_4(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_CURR_TIME+4);
	return 0;
}
int r2_GET_TX_CURR_TIME_8(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_CURR_TIME+8);
	return 0;
}
int r2_GET_TX_COMP_DELAY(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_COMP_DELAY);
	return 0;
}
int r2_GET_TX_SHOT_TIME(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME);
	return 0;
}
int r2_GET_TX_SHOT_TIME_4(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME+4);
	return 0;
}
int r2_GET_TX_SHOT_TIME_8(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME+8);
	return 0;
}
int r2_GET_TX_SHOT_TIME_16(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME+16);
	return 0;
}
int r2_GET_TX_SHOT_TIME_20(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME+20);
	return 0;
}
int r2_GET_TX_SHOT_TIME_24(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_TX_SHOT_TIME+24);
	return 0;
}

int r2_GET_RX_CURR_TIME(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_CURR_TIME);
	return 0;
}
int r2_GET_RX_CURR_TIME_4(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_CURR_TIME+4);
	return 0;
}
int r2_GET_RX_CURR_TIME_8(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_CURR_TIME+8);
	return 0;
}
int r2_GET_RX_COMP_DELAY(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_COMP_DELAY);
	return 0;
}
int r2_GET_RX_SHOT_TIME(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME);
	return 0;
}
int r2_GET_RX_SHOT_TIME_4(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME+4);
	return 0;
}
int r2_GET_RX_SHOT_TIME_8(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME+8);
	return 0;
}
int r2_GET_RX_SHOT_TIME_16(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME+16);
	return 0;
}
int r2_GET_RX_SHOT_TIME_20(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME+20);
	return 0;
}
int r2_GET_RX_SHOT_TIME_24(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + XTU_ADDR_RX_SHOT_TIME+24);
	return 0;
}

int r2_GET_VENDOR_ID(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + 0);
	return 0;
}
int r2_GET_DEVICE_ID(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + 4);
	return 0;
}
int r2_GET_VERSION(CLTU_Device *device, unsigned int *value )
{
	*value = READ32(device->virt_addr + 8);
	return 0;
}

















