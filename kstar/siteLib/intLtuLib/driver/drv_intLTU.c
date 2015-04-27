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

/*#include "intLTU_driver.h" */
#include "drv_intLTU.h"
#include "intLTU_debug.h"
#include "intLTU_Def.h"


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetPortConfig
Author    : laykim
Parameter : .
Return    : .
Desc      : set xtu_SetPortConfig. 
          portN (0 ~7), 
          enable( 1: on, 0: off), 
          activeLow( 1: activeLow, 0: activeHigh)
Date      : 
*******************************************************************************/
int r2_SetPortConfig(  CLTU_Device *device, unsigned char portN, unsigned char enable, unsigned char activeLow, unsigned char trgOn)
{
  unsigned int rVal = 0;
  unsigned int wVal = 0;
  unsigned int pConfig = 0;

  unsigned int pConfig_Enable     = 0;
  unsigned int pConfig_ActiveLow    = 0;
  unsigned int pConfig_TrrigerOn  = 0;
  
  unsigned int  pMask = 0;

/*	info( "%s: port:%d, enable: %d, ativeLow: %d, trigOn: %d",device->ldev.drv_name,  portN, enable, activeLow, trgOn ); */

	if( portN > 8)
	{
		err("port num: %d", portN);
		return (-1);
	}

  rVal = READ32(device->virt_addr0 + XTUR004_P_Config);
/*  printf("-rVal    = 0x%08x\n", rVal); */

  if( enable )
  {
    pConfig_Enable     = BIT24 << portN;
  }

  if( activeLow )
  {
    pConfig_ActiveLow  = BIT16 << portN;
  }

  if( trgOn )
  {
    pConfig_TrrigerOn  = BIT8  << portN;
  }

  pMask = (BIT24 | BIT16 | BIT8)  << portN;


  pConfig = pConfig_Enable | pConfig_ActiveLow | pConfig_TrrigerOn;
/*  printf("-pConfig = 0x%08x\n", pConfig); */
  

  
  wVal = (rVal & (~ pMask)) | pConfig;

  WRITE32(device->virt_addr0  + XTUR004_P_Config, wVal);

	udelay(100);

  rVal = READ32(device->virt_addr0 + XTUR004_P_Config);
/*
  printf("pConfig = 0x%08x\n", pConfig);
  printf("wVal    = 0x%08x\n", wVal);
  printf("rVal    = 0x%08x\n", rVal);
*/
  return 0;
  
}


int r2_SetIntervalConfig( CLTU_Device *device, unsigned char portN, 
                               unsigned char intervalN, 
                               unsigned long long int start_offset, 
                               unsigned long long int end_offset, 
                               unsigned int clockVal )
{

  unsigned int  addr_clk;
  unsigned int  start_offset_msb,start_offset_lsb;
  unsigned int  end_offset_msb,  end_offset_lsb;
  
  

  if( portN > 8)
  {
    err("port num: %d", portN);
		return (-1);
  }
  
  if( intervalN > 8)
  {
   err("interval num: %d", intervalN);
		return (-1);
  }
  
  addr_clk         = XTUR080_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x04);
  start_offset_msb = XTUR100_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  start_offset_lsb = XTUR101_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  end_offset_msb   = XTUR180_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  end_offset_lsb   = XTUR181_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);

  WRITE32(device->virt_addr0 + addr_clk        , (unsigned int)(clockVal          ));  udelay(100);
  WRITE32(device->virt_addr0 + start_offset_msb, (unsigned int)(start_offset >>32 ));  udelay(100);
  WRITE32(device->virt_addr0 + start_offset_lsb, (unsigned int)(start_offset      ));  udelay(100);
  WRITE32(device->virt_addr0 + end_offset_msb  , (unsigned int)(end_offset   >>32 ));  udelay(100);
  WRITE32(device->virt_addr0 + end_offset_lsb  , (unsigned int)(end_offset        ));  udelay(100);
#if 0
  PRINTF(("[XTU] Write Address clk value    : 0x%08x \n",addr_clk));
  PRINTF(("[XTU] Write Address start_offset : 0x%08x - 0x%08x\n",start_offset_msb,start_offset_lsb));
  PRINTF(("[XTU] Write Address end_offset   : 0x%08x - 0x%08x\n",end_offset_msb,end_offset_lsb));
#endif
  return 0;
}

int r2_GetCurrentTime( CLTU_Device *device,   unsigned int *pCurrentTime, unsigned int *pCurrentTick )
{

   *pCurrentTime      = READ32(device->virt_addr0  + XTUR010_);
  *pCurrentTick      = READ32(device->virt_addr0  + XTUR011_);

  return 0;
}

int r2_SetShotTime( CLTU_Device *device,  unsigned int shot_Start_Time, unsigned int shot_Start_Tick, unsigned int shot_End_Time, unsigned int shot_End_Tick)
{

  WRITE32(device->virt_addr0  + XTUR008_TSSTM, shot_Start_Time);
  udelay(100);
  WRITE32(device->virt_addr0  + XTUR009_TSSTL, shot_Start_Tick);
  udelay(100);
  WRITE32(device->virt_addr0  + XTUR00A_TSETM, shot_End_Time);
  udelay(100);
  WRITE32(device->virt_addr0  + XTUR00B_TSETL, shot_End_Tick);
  udelay(100);

  return 0;
}

int r2_SerdesTxImmediate( CLTU_Device *device)
{
   
  WRITE32(device->virt_addr0 + XTUR00E_, BIT31);
   udelay(100);
  WRITE32(device->virt_addr0 + XTUR00E_, 0);
   udelay(100);

  return 0;
}

int r2_SetMode( CLTU_Device *device, unsigned int xtu_mode, unsigned char XTU_ID)
{
  
  switch (xtu_mode)
  {
    case XTU_MODE_CTU:
      WRITE32(device->virt_addr0 + XTUR001_XTU_CONF, XTU_SERDES_TX_ONLY  | (unsigned int)XTU_ID);
	  udelay(100);
      break;
    case XTU_MODE_CTU_CMPST:
      WRITE32(device->virt_addr0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID);
      udelay(100);
      break;
    case XTU_MODE_LTU_FREE:
      WRITE32(device->virt_addr0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID | XTU_MODE_LTU_FREE );
      udelay(100);
      break;
    default: /* Default LTU Mode */
      WRITE32(device->virt_addr0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID);
      udelay(100);
      break;
  }

/*  info("[XTU] xtu_SetMode as mode : %d .....OK!",xtu_mode);*/
  return 0;
}

int r2_reset( CLTU_Device *device )
{

	WRITE32(device->virt_addr0 + XTUR006_, 0xFFFFFFFF);
	udelay(1000);
	WRITE32(device->virt_addr0 + XTUR006_, 0x00000000);
	udelay(100);

/*	info("[XTU] xtu_reset .....OK!");   */
	return 0;

}

int r2_SetEndian( CLTU_Device *device )
{

  /* Write Endian control register */
  WRITE32(device->virt_addr0 + XTUR000_ENDIAN, 0x01234567);
  udelay(100);

  return 0;
}
int r2_SetCompensationTime( CLTU_Device *device, unsigned int cmpst_value )
{
  unsigned int time_update_tick;
    
  time_update_tick = TICK_SEC - cmpst_value;
  
  WRITE32(device->virt_addr0 + XTUR005_XTUDelay, time_update_tick);
	udelay(100);

  return 0;
}
int r2_GetCompensationTime( CLTU_Device *device , unsigned int *pCMPST_Time )
{
  unsigned int read_value;

  read_value = READ32(device->virt_addr0 + XTUR018_);

  *pCMPST_Time = read_value & 0x0000ffff; 
  
  return 0;
}
int r2_GetCompensationID( CLTU_Device *device ,unsigned int *pCMPST_ID )
{
  unsigned int read_value;

  read_value = READ32(device->virt_addr0 + XTUR018_) >> 16;

  *pCMPST_ID = read_value & 0x0000ffff; 
  
  return 0;
}

int r2_SetPortRegister( CLTU_Device *device )
{
  WRITE32(device->virt_addr0 + XTUR00D_, 0xffffffff);
  udelay(100);
  WRITE32(device->virt_addr0 + XTUR00D_, 0x00000000);
 udelay(100);
  return 0;
}



















int r2_GetFPGA_Ver( CLTU_Device *device,  unsigned int *pVer )
{

  *pVer      = READ32(device->virt_addr0 + XTUR01F_);

  return 0;
}
int r2_SttEndian( CLTU_Device *device, unsigned char *SttEndian )
{
  unsigned int value;
  
  value = READ32(device->virt_addr0 + XTUR00E_);

  if( BIT0 == value)
  {
    *SttEndian = BigEndian;
  }
  else
  {
    *SttEndian = LittleEndian;
  }

  return 0;
}
int r2_SttRefClock(CLTU_Device *device,  unsigned char *boolValue )
{
  unsigned int value;
    
  value = READ32(device->virt_addr0 + XTUR013_);
  *boolValue = (BIT30 != (value & XTU_REF_CLK_STATUS)); /* 1: ref clk in Error  *//* 0: ref clk OK */

  return 0;
}
int r2_SttIRIGB( CLTU_Device *device, unsigned char *boolValue )
{
	unsigned int value;

  value = READ32(device->virt_addr0 + XTUR013_);
  *boolValue = (XTU_IRIG_B_STATUS == (value & XTU_IRIG_B_STATUS)); 

  return 0;
}
int r2_GetIRIGBUnlockCount( CLTU_Device *device, unsigned int *value )
{
  
  *value = READ32(device->virt_addr0 + XTUR013_) & 0x0000ffff;

  return 0;
}
int r2_SttSerdes( CLTU_Device *device,  unsigned char *boolValue )
{
  unsigned int value;
  
   
  value = READ32(device->virt_addr0 + XTUR013_);
  *boolValue = (BIT28 == (value & BIT28));

  return 0;
}
int r2_SttFOTRxLink( CLTU_Device *device,  unsigned char *boolValue )
{
  unsigned int value;

  value = READ32(device->virt_addr0 + XTUR013_);
  *boolValue = (BIT29 == (value & BIT29));

  return 0;
}

int r2_GetSerdesRxCnt(CLTU_Device *device, unsigned int *RxCnt )
{
	*RxCnt = READ32(device->virt_addr0 + XTUR012_) & 0x0000ffff;
	return 0;
}

int r2_GetSerdesTxCnt(CLTU_Device *device, unsigned int *TxCnt )
{
	*TxCnt = (READ32(device->virt_addr0 + XTUR012_) >> 16) & 0x0000ffff;
	return 0;
}








