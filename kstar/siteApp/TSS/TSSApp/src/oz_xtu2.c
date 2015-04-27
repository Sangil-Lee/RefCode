/******************************************************************************
 * Copyright (c) 2011 ~ by olzetek.         All Rights Reserved.              *
 * Write detail description about this nfrc.                                *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR NFRI

when        who     what, where, why
YY/MM/DD
--------  -------  -------------------------------------------------------------
11/01/22  laykim  Created.
==============================================================================*/

/*#include "comdef.h" */
#include "oz_xtu2.h"
#include "drvCLTU.h"


/*
#include <vxWorks.h>
#include <stdio.h>
#include <intLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <iv.h>
#include <string.h>
#include "drv\pci\pciConfigLib.h"
*/

/*XTU_PCI_DEVICE pciDevice; */

#if 0

/*******************************************************************************
Company   : Olzetek
Function  : xtu_isr
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
void xtu_isr( void )
{

}

#if 0
/*******************************************************************************
Company   : Olzetek
Function  : xtu_dev_start
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_dev_start( void )
{

  STATUS vstatus;

  /* 1. PCI device struct init. */
  memset( &pciDevice, 0x00, sizeof(XTU_PCI_DEVICE));

  /* 2. PCI find device */
  vstatus = pciFindDevice( XTU_VENDOR_ID, XTU_DEVICE_ID, 0, &pciDevice.BusNo, &pciDevice.DeviceNo, &pciDevice.FuncNo );
  if( vstatus == ERROR )
  {
    PRINT_ERR();
    return ERROR;
  }
  else
  {
    printf(("[XTU] pci device find .....OK!\n"));
  }

  /* 3. Get the PCI bar0 address. */
  vstatus = pciConfigInLong( pciDevice.BusNo, pciDevice.DeviceNo, pciDevice.FuncNo, PCI_CFG_BASE_ADDRESS_0, &pciDevice.base0);
  if( vstatus == ERROR )
  {
    PRINT_ERR();
    return ERROR;
  }
  else
  {
    printf(("[XTU] Get bar0 address = 0x%08x  .....OK!\n", pciDevice.base0));
  }


  if(pciDevice.base0 == 0x00000000)
  {
    return ERROR;
  }

  /* 6. Set PCI Device Init OK */
  pciDevice.init = 0xFFFFFFFF;

  printf("[XTU] xtu_dev_start .....OK!\n");
  
  return OK;
}
#endif

/*******************************************************************************
Company   : Olzetek
Function  : xtu_dev_stop
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_dev_stop( ST_drvCLTU *pCfg  )
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  memset( &pCfg->pciDevice, 0x00, sizeof(CLTU_PCI_DEVICE));

  printf("[XTU] xtu_dev_stop .....OK!\n");
  return OK;  
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_reset
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_reset( ST_drvCLTU *pCfg )
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}

	WRITE32(pCfg->pciDevice.base0 + XTUR006_, 0xFFFFFFFF);
	DELAY_WAIT(7);
	WRITE32(pCfg->pciDevice.base0 + XTUR006_, 0x00000000);
	DELAY_WAIT(0);

	printf("[XTU] xtu_reset .....OK!\n");   
	return OK;

}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttEndian
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SttEndian( ST_drvCLTU *pCfg, unsigned char *SttEndian )
{
  unsigned int value;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  value = READ32(pCfg->pciDevice.base0 + XTUR00E_);

  if( BIT0 == value)
  {
    *SttEndian = BigEndian;
  }
  else
  {
    *SttEndian = LittleEndian;
  }

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttIRIGB
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 2011.01.25
*******************************************************************************/
STATUS xtu_SttIRIGB( ST_drvCLTU *pCfg, unsigned char *boolValue )
{
	unsigned int value;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  value = READ32(pCfg->pciDevice.base0 + XTUR013_);
  *boolValue = (XTU_IRIG_B_STATUS == (value & XTU_IRIG_B_STATUS)); 


  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetIRIGBUnlockCount
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 2011.01.25
*******************************************************************************/
STATUS xtu_GetIRIGBUnlockCount( ST_drvCLTU *pCfg, unsigned int *value )
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  *value = READ32(pCfg->pciDevice.base0 + XTUR013_) & 0x0000ffff;

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttRefClock
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SttRefClock(ST_drvCLTU *pCfg,  unsigned char *boolValue )
{
  unsigned int value;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
    
  value = READ32(pCfg->pciDevice.base0 + XTUR013_);
  *boolValue = (BIT30 != (value & XTU_REF_CLK_STATUS)); /* 1: ref clk in Error  0: ref clk OK */

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttFOTRxLink
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SttFOTRxLink( ST_drvCLTU *pCfg,  unsigned char *boolValue )
{
  unsigned int value;
  
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  value = READ32(pCfg->pciDevice.base0 + XTUR013_);
  *boolValue = (BIT29 == (value & BIT29));

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttSerdes
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SttSerdes( ST_drvCLTU *pCfg,  unsigned char *boolValue )
{
  unsigned int value;
  
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  value = READ32(pCfg->pciDevice.base0 + XTUR013_);
  *boolValue = (BIT28 == (value & BIT28));

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetFPGA_Ver
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetFPGA_Ver( ST_drvCLTU *pCfg,  unsigned int *pVer )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   

  *pVer      = READ32(pCfg->pciDevice.base0 + XTUR01F_);

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetCurrentTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetCurrentTime( ST_drvCLTU *pCfg,  unsigned int *pCurrentTime, unsigned int *pCurrentTick )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   

  *pCurrentTime      = READ32(pCfg->pciDevice.base0 + XTUR010_);
  *pCurrentTick      = READ32(pCfg->pciDevice.base0 + XTUR011_);

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetIRIGBTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetIRIGBTime( ST_drvCLTU *pCfg, unsigned int *pCurrentTime )
{

 if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  *pCurrentTime      = READ32(pCfg->pciDevice.base0 + XTUR016_);

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetSerdesRxTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetSerdesRxTime( ST_drvCLTU *pCfg,  unsigned int *pCurrentTime )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  *pCurrentTime      = READ32(pCfg->pciDevice.base0 + XTUR019_);

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetCTU_ShotTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetCTU_ShotTime(ST_drvCLTU *pCfg , unsigned int *CTU_ShotStartTime, unsigned int *CTU_ShotStartTick, unsigned int *CTU_ShotEndTime, unsigned int *CTU_ShotEndTick)
{

   if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  
  *CTU_ShotStartTime = READ32(pCfg->pciDevice.base0 + XTUR008_TSSTM);
  *CTU_ShotStartTick = READ32(pCfg->pciDevice.base0 + XTUR009_TSSTL);

  *CTU_ShotEndTime = READ32(pCfg->pciDevice.base0 + XTUR00A_TSETM);
  *CTU_ShotEndTick = READ32(pCfg->pciDevice.base0 + XTUR00B_TSETL);

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttSerdes
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetSerdesRxCnt(ST_drvCLTU *pCfg,   unsigned int *RxCnt )
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  *RxCnt = READ32(pCfg->pciDevice.base0 + XTUR012_) & 0x0000ffff;
  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttSerdes
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetSerdesTxCnt(ST_drvCLTU *pCfg,   unsigned int *TxCnt )
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  *TxCnt = (READ32(pCfg->pciDevice.base0 + XTUR012_) >> 16) & 0x0000ffff;
  return OK;
}



/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetLTU_ShotTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetLTU_ShotTime(ST_drvCLTU *pCfg , unsigned int *CTU_ShotStartTime, unsigned int *CTU_ShotStartTick, unsigned int *CTU_ShotEndTime, unsigned int *CTU_ShotEndTick)
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  
  *CTU_ShotStartTime = READ32(pCfg->pciDevice.base0 + XTUR01A_);
  *CTU_ShotStartTick = READ32(pCfg->pciDevice.base0 + XTUR01B_);

  *CTU_ShotEndTime   = READ32(pCfg->pciDevice.base0 + XTUR01C_);
  *CTU_ShotEndTick   = READ32(pCfg->pciDevice.base0 + XTUR01D_);
  
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetCompensationValue
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/


/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetCompensationID
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetCompensationID( ST_drvCLTU *pCfg ,unsigned int *pCMPST_ID )
{
  unsigned int read_value;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  read_value = READ32(pCfg->pciDevice.base0 + XTUR018_) >> 16;

  *pCMPST_ID = read_value & 0x0000ffff; 
  
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetLastUpdateTick
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetLastUpdateTick( ST_drvCLTU *pCfg , unsigned int *pLast_Tick )
{

 if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   

  *pLast_Tick      = READ32(pCfg->pciDevice.base0 + XTUR017_);
  
  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : xtu_SttSerdes
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetSerdesRxUnLockCnt( ST_drvCLTU *pCfg,  unsigned int *RxCnt )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  *RxCnt = READ32(pCfg->pciDevice.base0 + XTUR029_SD_ULCNT) & 0x0000ffff;
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetPortOutStatus
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetPortOutStatus( ST_drvCLTU *pCfg, unsigned char PortNum, unsigned int *PortCnt )
{
  unsigned int read_value;

  if( PortNum > 8)
  {
    PRINT_ERR(); return ERROR;    
  }

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
  read_value = READ32(pCfg->pciDevice.base0 + XTUR00F_);
    
  *PortCnt = (read_value >> (4*PortNum)) & 0x0000000F;
    
  return OK;
}


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
STATUS xtu_SetPortConfig(  ST_drvCLTU *pCfg, unsigned char portN, unsigned char enable, unsigned char activeLow, unsigned char trgOn)
{
  unsigned int rVal = 0;
  unsigned int wVal = 0;
  unsigned int pConfig = 0;

  unsigned int pConfig_Enable     = 0;
  unsigned int pConfig_ActiveLow    = 0;
  unsigned int pConfig_TrrigerOn  = 0;
  
  unsigned int  pMask = 0;
  
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  if( portN > 8)
  {
    PRINT_ERR(); return ERROR;    
  }

  rVal = READ32(pCfg->pciDevice.base0 + XTUR004_P_Config);
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

  WRITE32(pCfg->pciDevice.base0 + XTUR004_P_Config, wVal);
  DELAY_WAIT(0);

  rVal = READ32(pCfg->pciDevice.base0 + XTUR004_P_Config);
/*
  printf("pConfig = 0x%08x\n", pConfig);
  printf("wVal    = 0x%08x\n", wVal);
  printf("rVal    = 0x%08x\n", rVal);
*/
  return OK;
  
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetPortConfigDirect
Author    : laykim
Parameter : .
Return    : .
Desc      : set xtu_SetPortConfigDirect. 
          portN (0 ~7), 
          enable( 1: on, 0: off), 
          activeLow( 1: activeLow, 0: activeHigh)
Date      : 
*******************************************************************************/
STATUS xtu_SetPortConfigDirect( ST_drvCLTU *pCfg, unsigned char enable_HEX, unsigned char activeLow_HEX, unsigned char trgOn_HEX)
{
  unsigned int pConfig = 0;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  pConfig = (unsigned int)(enable_HEX << 24) | (unsigned int)(activeLow_HEX << 16) | (unsigned int)(trgOn_HEX << 8);
    
  WRITE32(pCfg->pciDevice.base0 + XTUR004_P_Config, pConfig);
	DELAY_WAIT(0);
 return OK;
  
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetPortRegister
Author    : laykim
Parameter : .
Return    : .
Desc      : ltu time delay set.
Date      : 
*******************************************************************************/
STATUS xtu_SetPortRegister( ST_drvCLTU *pCfg )
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  WRITE32(pCfg->pciDevice.base0 + XTUR00D_, 0xffffffff);
  DELAY_WAIT(1);
  WRITE32(pCfg->pciDevice.base0 + XTUR00D_, 0x00000000);
  DELAY_WAIT(0);
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetIntervalConfig
Author    : laykim
Parameter : .
Return    : .
Desc      : 

Date      : 
*******************************************************************************/
STATUS xtu_SetIntervalConfig( ST_drvCLTU *pCfg, unsigned char portN, 
                               unsigned char intervalN, 
                               unsigned long long int start_offset, 
                               unsigned long long int end_offset, 
                               unsigned int clockVal )
{

  unsigned int  addr_clk;
  unsigned int  start_offset_msb,start_offset_lsb;
  unsigned int  end_offset_msb,  end_offset_lsb;
  
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  if( portN > 8)
  {
    PRINT_ERR(); return ERROR;    
  }
  
  if( intervalN > 8)
  {
    PRINT_ERR(); return ERROR;    
  }
  
  addr_clk         = XTUR080_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x04);
  start_offset_msb = XTUR100_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  start_offset_lsb = XTUR101_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  end_offset_msb   = XTUR180_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);
  end_offset_lsb   = XTUR181_ + (unsigned int)(portN * 0x40) + (unsigned int)(intervalN * 0x08);

  WRITE32(pCfg->pciDevice.base0 + addr_clk        , (unsigned int)(clockVal          ));  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + start_offset_msb, (unsigned int)(start_offset >>32 ));  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + start_offset_lsb, (unsigned int)(start_offset      ));  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + end_offset_msb  , (unsigned int)(end_offset   >>32 ));  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + end_offset_lsb  , (unsigned int)(end_offset        ));  DELAY_WAIT(0);
#if 0
  PRINTF(("[XTU] Write Address clk value    : 0x%08x \n",addr_clk));
  PRINTF(("[XTU] Write Address start_offset : 0x%08x - 0x%08x\n",start_offset_msb,start_offset_lsb));
  PRINTF(("[XTU] Write Address end_offset   : 0x%08x - 0x%08x\n",end_offset_msb,end_offset_lsb));
#endif
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetTestPoint
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SetTestPoint( ST_drvCLTU *pCfg, unsigned int select_tp )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  /* Write Endian control register */
  WRITE32(pCfg->pciDevice.base0 + XTUR002_, select_tp);
  DELAY_WAIT(0);
  PRINTF(("[XTU] Select Test point : %d\n",select_tp));

  return OK;
}


/*******************************************************************************
Company   : Olzetek
Function  : ctu_SetIRIG_B_Enable
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS ctu_SetIRIG_B_Enable( ST_drvCLTU *pCfg, unsigned char enable )
{
  unsigned int set_value;
  
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  if(enable == 0)
  {
/*	BIT31: IRIG_B on/off, BIT30: Serdes Lock on/off, --> BIT30 must be always 0 */  
/* BIT30: 0 -> 1 second update by CTU time code
		 1-> update by LTU self clock */
/*    set_value = BIT31 | BIT30; 2012. 8. 9 */ 
	set_value = BIT31;
  }
  else
  {
    set_value = 0;
  }

  /* Write Endian control register */
  WRITE32(pCfg->pciDevice.base0 + XTUR003_P_CNTRst, set_value);
  DELAY_WAIT(0);

  return OK;
}

#endif








STATUS xtu_SetEndian( ST_drvCLTU *pCfg )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  /* Write Endian control register */
  WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_ENDIAN_SET, 0x01234567);
  printf("[XTU] xtu_endian_set .....OK!\n");

  return OK;
}


STATUS xtu_SetMode( ST_drvCLTU *pCfg, unsigned int xtu_mode, unsigned char XTU_ID)
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
#if 0  
  switch (xtu_mode)
  {
    case XTU_MODE_CTU:
      WRITE32(pCfg->pciDevice.base0 + XTUR001_XTU_CONF, XTU_SERDES_TX_ONLY  | (unsigned int)XTU_ID);
      DELAY_WAIT(0);
      break;
    case XTU_MODE_CTU_CMPST:
      WRITE32(pCfg->pciDevice.base0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID);
      DELAY_WAIT(0);
      break;
    case XTU_MODE_LTU_FREE:
      WRITE32(pCfg->pciDevice.base0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID | XTU_MODE_LTU_FREE );
      DELAY_WAIT(0);
      break;
    default: /* Default LTU Mode */
      WRITE32(pCfg->pciDevice.base0 + XTUR001_XTU_CONF, (unsigned int)XTU_ID);
      DELAY_WAIT(0);
      break;
  }
#endif
	WRITE32(pCfg->pciDevice.base0  + XTU_ADDR_SEL_XTU_MODE, xtu_mode);

	printf("[XTU] xtu_SetMode as mode : %d .....OK!\n", xtu_mode);
	return OK;
}

STATUS xtu_SetDBGMode( ST_drvCLTU *pCfg, ozXTU_DBGM_T* pxtu_dbgm)
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}

  wd_tmp =   pxtu_dbgm->cdc_sel | pxtu_dbgm->jtag_sel;
  WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_DBG_CONFIG, wd_tmp);
           
  return OK;
}

STATUS xtu_SetLocalTime( ST_drvCLTU *pCfg, ozTIME_T* pltime )
{
	unsigned int wd_tmp;
	
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	

	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_LOCAL_TIME, pltime->day);

	DELAY_WAIT(0);
	hms_DtoH( &(pltime->hour), &(pltime->min), &(pltime->sec), &wd_tmp );

	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_LOCAL_TIME + 4, wd_tmp);

	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_SET_LOCAL_TIME);

	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);

	return OK;
}

STATUS xtu_SetCTUShotDelay( ST_drvCLTU *pCfg )
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_SHOT_DELAY, XTU_SHOT_DEFAULT_DELAY_TICK);
	return OK;
}

STATUS xtu_SetLTUShotDelay( ST_drvCLTU *pCfg )
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	wd_tmp = XTU_SHOT_DEFAULT_DELAY_TICK - (pCfg->compPutTime.ns + 210) / 20;
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_SHOT_DELAY, wd_tmp);

	return OK;
}

/* 0: ative high, 1: active low */
STATUS xtu_SetPortActiveLevel( ST_drvCLTU *pCfg, unsigned int port_al )
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_PORT_ACTIVE_LEVEL, port_al);

	return OK;
}

STATUS xtu_SetPortCtrl( ST_drvCLTU *pCfg, unsigned char port )
{
	unsigned int addr_tmp;
	unsigned int addr_nhli;  
	
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	
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
			PRINT_ERR();
			return ERROR;
	}

	addr_nhli = XTU_ADDR_BRAM_PHLI_0 + (pCfg->xtu_pcfg[port].nhli_num * 4);
	WRITE32(pCfg->pciDevice.base0 + addr_tmp + XTU_ADDR_PCTRL, (pCfg->xtu_pcfg[port].port_en | ((addr_nhli & 0x000001ff) << 16) ));

	return OK;
}

STATUS xtu_SetPortHLI( ST_drvCLTU *pCfg, unsigned int port, unsigned int hli_num, ozPCONFIG_HLI_T* pphli)
{
	unsigned int wd_tmp;
	ozTIME_T time_tmp;
#if defined(_CLTU_VXWORKS_)
	unsigned int base_addr;
#elif defined(_CLTU_LINUX_)
	unsigned int *base_addr;
#else
#error plaease select OS!
#endif

	unsigned int addr_tmp;
	unsigned int addr_nhli;
	unsigned int addr_nlli;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}

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
		PRINT_ERR();
		return ERROR;
	}

	if(hli_num >= XTU_ADDR_PHLI_MAX_NUM)
	{
		PRINT_ERR();
		return ERROR;
	}
#if defined(_CLTU_VXWORKS_)
	base_addr = (unsigned int )(pCfg->pciDevice.base0 + addr_tmp + XTU_ADDR_PHLI_0 + (hli_num*16));
#elif defined(_CLTU_LINUX_)
	base_addr = (unsigned int *)(pCfg->pciDevice.base0 + addr_tmp + XTU_ADDR_PHLI_0 + (hli_num*16));
#else
#error plaease select OS!
#endif

	addr_nhli = XTU_ADDR_BRAM_PHLI_0 + (pphli->nhli_num * 4);
	addr_nlli = XTU_ADDR_BRAM_PLLI_0 + (pphli->nlli_num * 4); 

	if(XTU_MAX_INF_NS == pphli->intv.ns)
	{
		DELAY_WAIT(0);
		WRITE32(base_addr    , 0);
		DELAY_WAIT(0);
		WRITE32(base_addr + 4, 0x000fffff);
	}
	else
	{
		xtu_200MTimeSubtraction( &(pphli->intv), 55, &time_tmp);
		wd_tmp = (time_tmp.hour<< 16) | (time_tmp.min << 8) | (time_tmp.sec);
		DELAY_WAIT(0);
		WRITE32(base_addr    , wd_tmp);
		ms200MTick_DtoH( &time_tmp.ms, &time_tmp.ns, &wd_tmp);
		WRITE32(base_addr + 4, wd_tmp);
	}

	DELAY_WAIT(0);
	WRITE32(base_addr + 12, ((addr_nhli & 0x000001ff) << 16) |  (addr_nlli & 0x000001ff) );

	return OK;
}

STATUS xtu_SetPortLLI( ST_drvCLTU *pCfg, unsigned int port_num, unsigned int lli_num, ozPCONFIG_LLI_T* pplli)
{
	unsigned int wd_tmp;
	ozTIME_T time_tmp;
	unsigned int clk_time_tmp1;
	unsigned int clk_time_tmp2;
	unsigned int clk_ms_tmp;
	unsigned int clk_ns_tmp;
	unsigned int addr_tmp;
	unsigned int addr_nlli;
#if defined(_CLTU_VXWORKS_)
	unsigned int base_addr;
#elif defined(_CLTU_LINUX_)
	unsigned int *base_addr;
#else
#error plaease select OS!
#endif

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}

	switch(port_num)
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
		PRINT_ERR();
		return ERROR;
	}
  
	if(lli_num >= XTU_ADDR_PLLI_MAX_NUM)
	{
		PRINT_ERR();
		return ERROR;
	}
	
#if defined(_CLTU_VXWORKS_)
	base_addr = (unsigned int )(pCfg->pciDevice.base0 + addr_tmp + XTU_ADDR_PLLI_0 + (lli_num*16));
#elif defined(_CLTU_LINUX_)
	base_addr = (unsigned int *)(pCfg->pciDevice.base0 + addr_tmp + XTU_ADDR_PLLI_0 + (lli_num*16));
#else
#error plaease select OS!
#endif
	addr_nlli = XTU_ADDR_BRAM_PLLI_0 + (pplli->nlli_num * 4); 

	if(XTU_MAX_INF_NS == pplli->intv.ns)
	{
		DELAY_WAIT(0);
		WRITE32(base_addr    , 0);
		DELAY_WAIT(0);
		WRITE32(base_addr + 4, 0x000fffff);
	}
	else
	{
		xtu_200MTimeSubtraction( &(pplli->intv), 65, &time_tmp);
		wd_tmp = (time_tmp.hour<< 16) | (time_tmp.min << 8) | (time_tmp.sec);
		DELAY_WAIT(0);
		WRITE32(base_addr    , wd_tmp);
		ms200MTick_DtoH( &time_tmp.ms, &time_tmp.ns, &wd_tmp);
		DELAY_WAIT(0);
		WRITE32(base_addr + 4, wd_tmp);
	}

	if(pplli->clk_freq == 0)
	{
		clk_time_tmp1 = 0;
		clk_ms_tmp    = 0;
		clk_ns_tmp  = 0;
	}
	else
	{
/*		clk_time_tmp1 = (unsigned int)(1/((float)(pplli->clk_freq) / XTU_200MHz * 2 )); // original */
		clk_time_tmp1 = XTU_100MHz / pplli->clk_freq;
		clk_time_tmp2 = clk_time_tmp1 - 1;
		clk_ms_tmp    = (unsigned int)(clk_time_tmp2 / XTU_MAX_200M_Tick);
		clk_ns_tmp    = (unsigned int)(clk_time_tmp2 % XTU_MAX_200M_Tick) * 5;
	}

	if((pplli->option & XTU_PORT_CLK_OPT_ENABLE) == XTU_PORT_CLK_OPT_ENABLE)
	{
		DELAY_WAIT(0);
		WRITE32(base_addr + 8, 0x000fffff);
		DELAY_WAIT(0);
		WRITE32(base_addr + 12, (pplli->option & XTU_PORT_LLI_OPT_LOG_ON) |XTU_PORT_CLK_OPT_VARFREQ | (addr_nlli & 0x000001ff) );
	}
	else
	{
		ms200MTick_DtoH( &clk_ms_tmp, &clk_ns_tmp, &wd_tmp);
		DELAY_WAIT(0);
		WRITE32(base_addr + 8, wd_tmp);
		DELAY_WAIT(0);
		WRITE32(base_addr + 12, (pplli->option) | (addr_nlli & 0x000001ff) );
	}

	return OK;
}


STATUS xtu_SetShotTime( ST_drvCLTU *pCfg, ozSHOT_TIME_T* pshot_time)
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	

	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME, pshot_time->start.day);

	DELAY_WAIT(0);
	wd_tmp = (pshot_time->start.hour << 16) | (pshot_time->start.min << 8) | (pshot_time->start.sec);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 4, wd_tmp);

	DELAY_WAIT(0);
	ms100MTick_DtoH( &(pshot_time->start.ms), &(pshot_time->start.ns), &wd_tmp);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 8, wd_tmp);

	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 16, pshot_time->stop.day);

	DELAY_WAIT(0);
	wd_tmp = (pshot_time->stop.hour << 16) | (pshot_time->stop.min << 8) | (pshot_time->stop.sec);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 20, wd_tmp);

	DELAY_WAIT(0);
	ms100MTick_DtoH( &(pshot_time->stop.ms), &(pshot_time->stop.ns), &wd_tmp);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 24, wd_tmp);


	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_SET_SHOT_STIME | XTU_CMD_SET_SHOT_ETIME);

	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
	DELAY_WAIT(10);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);

	return OK;
}


STATUS xtu_StartShotNow( ST_drvCLTU *pCfg)
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
/*  
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, BIT31);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, 0);
  DELAY_WAIT(0);
*/
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_IMM_START);
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);

	return OK;
}

STATUS xtu_StopShotNow( ST_drvCLTU *pCfg)
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
/*  
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, BIT31);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, 0);
  DELAY_WAIT(0);
*/
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_IMM_STOP);
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
	DELAY_WAIT(0);
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);

	return OK;
}


/* 0: XTU_IRIGB_EXT,   1: XTU_IRIGB_GEN */
STATUS xtu_SetIRIG_B_Src( ST_drvCLTU *pCfg)
{
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SEL_IRIGB, pCfg->xtuMode.irigb_sel);
	return OK;
}

STATUS xtu_SetClkConfig( ST_drvCLTU *pCfg)
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	
	wd_tmp =   pCfg->xtuMode.tlkRxClk_en | pCfg->xtuMode.tlkTxClk_sel | pCfg->xtuMode.mclk_sel ;
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CLK_CONFIG, wd_tmp);
	return OK;
}
STATUS xtu_SetPermClk( ST_drvCLTU *pCfg)
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}
	
	wd_tmp =   pCfg->permanentClk;
	WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_PORT_INTR, wd_tmp);
	return OK;
}




STATUS xtu_SetXTUMode( ST_drvCLTU *pCfg)
{
	unsigned int wd_tmp;

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}

  wd_tmp =   pCfg->xtuMode.tlkRxClk_en
           | pCfg->xtuMode.tlkTxClk_sel
           | pCfg->xtuMode.mclk_sel ;
  WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_CLK_CONFIG, wd_tmp);

  DELAY_WAIT(0);
  wd_tmp =   pCfg->xtuMode.mode ;
  WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SEL_XTU_MODE, wd_tmp);

  DELAY_WAIT(0);
  wd_tmp =   pCfg->xtuMode.irigb_sel ;
  WRITE32(pCfg->pciDevice.base0 + XTU_ADDR_SEL_IRIGB, wd_tmp);
  
  return OK;
}






/*
STATUS xtu_time_print( unsigned long long int ll_time )
{
  unsigned int Hex_Sec,Hex_Min,Hex_HR,Hex_Day;
  
  Hex_Sec = (unsigned int)  ((ll_time                          % TIME_MIN ) / TIME_SEC);
  Hex_Min = (unsigned int) (((ll_time - (ll_time % TIME_MIN )) % TIME_HOUR) / TIME_MIN);
  Hex_HR  = (unsigned int) (((ll_time - (ll_time % TIME_HOUR)) % TIME_DAY ) / TIME_HOUR);
  Hex_Day = (unsigned int)  ((ll_time - (ll_time % TIME_DAY ))              / TIME_DAY);

  printf(" %d day, %d hour, %d min, %d sec.\n",Hex_Day,Hex_HR,Hex_Min,Hex_Sec);
  return OK;
}
*/
STATUS xtu_time_print_new( unsigned int time, unsigned int tick )
{
  unsigned int Hex_Sec,Hex_Min,Hex_HR,Hex_Day;
  
  Hex_Sec  = time % 60;
  Hex_Min  = (unsigned int)(time / 60   ) % 60;
  Hex_HR   = (unsigned int)(time / 3600 ) % 24; 
  Hex_Day  = (unsigned int)(time / 86400); 
  
  printf(" %03d day, %02d hour, %02d min, %02d sec",Hex_Day,Hex_HR,Hex_Min,Hex_Sec);
  printf(" 0x%08x tick.\n",tick);
  return OK;
}


STATUS hms_HtoD( unsigned int* phmsH, unsigned char* phD, unsigned char* pmD, unsigned char* psD  )
{
  *phD = (unsigned char)((*phmsH & 0x00ff0000) >> 16); 
  *pmD = (unsigned char)((*phmsH & 0x0000ff00) >> 8 ); 
  *psD = (unsigned char)((*phmsH & 0x000000ff)      );
/*  printf("hms_HtoD : %d hour : %d min : %d sec %08x\n",*phD,*pmD,*psD,*phmsH); */
  return OK;
}



STATUS hms_DtoH( unsigned char* phD, const unsigned char* pmD, const unsigned char* psD, unsigned int* phmsH )
{
  *phmsH = (*phD << 16) | ( *pmD << 8) | ( *psD );  
/*  printf("msTick_DtoH : 0x%08x\n",*phmsH); */
  return OK;
}

STATUS ms100MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD )
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
  return OK;
}



STATUS ms100MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH )
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
  return OK;
}


STATUS ms200MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD )
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
  return OK;
}



STATUS ms200MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH )
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
  return OK;
}

STATUS xtu_200MTimeSubtraction( ozTIME_T* ptimeA, unsigned int nsB, ozTIME_T* ptimeOUT)
{
	unsigned char subcarry;

	if((ptimeA->ns < nsB) && (ptimeA->ms == 0) && (ptimeA->sec == 0) && (ptimeA->min == 0) && (ptimeA->hour == 0) && (ptimeA->day == 0))
	{
		return ERROR;
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

	return OK;
}

STATUS xtu_GetGshotLog(  ST_drvCLTU *pCfg, unsigned int *pbuf)
{
	unsigned int i; 
#if defined(_CLTU_VXWORKS_)
	unsigned int base_addr;
#elif defined(_CLTU_LINUX_)
	unsigned int *base_addr;
#else
#error plaease select OS!
#endif

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}   
	
#if defined(_CLTU_VXWORKS_)
	base_addr = (unsigned int )(pCfg->pciDevice.base0 + XTU_ADDR_GSHOT_LOG_PRN);
#elif defined(_CLTU_LINUX_)
	base_addr = (unsigned int *)(pCfg->pciDevice.base0 + XTU_ADDR_GSHOT_LOG_PRN);
#else
#error plaease select OS!
#endif

	for(i = 0 ; (i < XTU_BMEM_SIZE/4); i++ )
	{
		DELAY_WAIT(0);
		pbuf[i*4 + 0] = READ32(base_addr + (i*16 + 0)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 1] = READ32(base_addr + (i*16 + 4)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 2] = READ32(base_addr + (i*16 + 8)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 3] = READ32(base_addr + (i*16 + 12) );    
/*		printf(" xtu_GetGshotLog loop cnt : %d:%08x|%08x|%08x|%08x\n", i, pbuf[i*4 + 0],pbuf[i*4 + 1],pbuf[i*4 + 2],pbuf[i*4 + 3]); */
	}

	return OK;
}

STATUS xtu_GetPortLog( ST_drvCLTU *pCfg, unsigned char port_num, unsigned int *pbuf)
{
	unsigned int i;  
	unsigned int addr_tmp;  
#if defined(_CLTU_VXWORKS_)
	unsigned int base_addr;
#elif defined(_CLTU_LINUX_)
	unsigned int *base_addr;
#else
#error plaease select OS!
#endif
	

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}   

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
		PRINT_ERR();
		return ERROR;
	}

#if defined(_CLTU_VXWORKS_)
	base_addr = (unsigned int )(pCfg->pciDevice.base0 + addr_tmp);
#elif defined(_CLTU_LINUX_)
	base_addr = (unsigned int *)(pCfg->pciDevice.base0 + addr_tmp);
#else
#error plaease select OS!
#endif	

	for(i = 0 ; (i < XTU_BMEM_SIZE/4); i++ )
	{
		DELAY_WAIT(0);
		pbuf[i*4 + 0] = READ32(base_addr + (i*16 + 0)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 1] = READ32(base_addr + (i*16 + 4)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 2] = READ32(base_addr + (i*16 + 8)  );    
		DELAY_WAIT(0);
		pbuf[i*4 + 3] = READ32(base_addr + (i*16 + 12) );    
/*		printf(" xtu_GetPortLog loop cnt : %d:%08x|%08x|%08x|%08x\n", i, pbuf[i*4 + 0],pbuf[i*4 + 1],pbuf[i*4 + 2],pbuf[i*4 + 3]); */
	}

  return OK;
}


/***************************************************************************************** new updated FPGA 2013. 3. 7 */

/*******************************************************************************
Company   : Olzetek
Function  : xtu_GetFPGA_Ver
Author    : laykim
Parameter : .
Return    : .
Desc      : Get board version
Date      : 
*******************************************************************************/
STATUS xtu_GetXtuInfo( ST_drvCLTU *pCfg )
{
  unsigned int vendor_id;
  unsigned int device_id;
  unsigned int version;  
  
   if(pCfg->pciDevice.init != 0xFFFFFFFF)
   {
	 PRINT_ERR(); return ERROR;
   }   

   DELAY_WAIT(0);
  vendor_id = READ32(pCfg->pciDevice.base0 + 0);
   DELAY_WAIT(0);
  device_id = READ32(pCfg->pciDevice.base0 + 4);
   DELAY_WAIT(0);
  version   = READ32(pCfg->pciDevice.base0 + 8);

  printf("=== Get XTU Information =========\n");
  printf("Vendor    : 0x%08x\n", vendor_id);
  printf("Device id : 0x%08x\n", device_id);
  printf("Version   : 0x%08x\n", version);

  return OK;
}

STATUS xtu_GetXtuInfo_2( ST_drvCLTU *pCfg )
{
#ifdef _CLTU_LINUX_

	unsigned int vendor_id;
	unsigned int device_id;
	unsigned int version;  
	int status;

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_VENDOR_ID, &vendor_id);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_VENDOR_ID, error\n"); return WR_ERROR; }

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_DEVICE_ID, &device_id);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_DEVICE_ID, error\n"); return WR_ERROR; }

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_VERSION, &version);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_VERSION, error\n"); return WR_ERROR; }
	
	printf("=== Get XTU Information =========\n");
	printf("Vendor    : 0x%08x\n", vendor_id);
	printf("Device id : 0x%08x\n", device_id);
	printf("Version   : 0x%08x\n", version);

#endif

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_ChkBrdStatus
Author    : laykim
Parameter : .
Return    : .
Desc      : check board status
Date      : 
*******************************************************************************/
STATUS xtu_GetXtuStatus( ST_drvCLTU *pCfg )
{
	ozXTU_STT_T pxtu_stt;
	unsigned int rd_tmp;
	
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}   

	DELAY_WAIT(0);
	pxtu_stt.xtu_endian = READ32(pCfg->pciDevice.base0 + XTU_ADDR_ENDIAN_CHECK);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_CLK_CONFIG);
	pxtu_stt.xm.tlkRxClk_en   = rd_tmp & XTU_TLKRX_CLK;
	pxtu_stt.xm.tlkTxClk_sel  = rd_tmp & XTU_TLKTX_CLK;
	pxtu_stt.xm.mclk_sel      = rd_tmp & XTU_MCLK_SEL;

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_DBG_CONFIG);
	pxtu_stt.dbg.cdc_sel      = rd_tmp & XTU_CDC_SEL;
	pxtu_stt.dbg.jtag_sel     = rd_tmp & XTU_JTAG_SEL;

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_CLK_STATUS);
	pxtu_stt.xtu_ref_clk_loss = rd_tmp & XTU_REF_CLK_LOSS;
	pxtu_stt.xtu_trx_clk_loss = rd_tmp & XTU_TLKRX_CLK_LOSS;
	pxtu_stt.xtu_mclk_lock    = rd_tmp & XTU_MCLK_LOCK; 

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_SEL_XTU_MODE);
	pxtu_stt.xm.mode          = rd_tmp & XTU_MODE_SEL;

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_SEL_IRIGB);
	pxtu_stt.xm.irigb_sel     = rd_tmp & XTU_IRIGB_SEL;


	printf("=== Get XTU Status =========\n");

	printf("xtu_endian      : 0x%01x (0:big/1:little)\n"              , pxtu_stt.xtu_endian);   
	printf("xtu_mode        : 0x%01x (0:ltu/1:ctu)\n"                 , pxtu_stt.xm.mode);
	printf("irigb_sel       : 0x%01x (0:irig or tlk rx/1:igen)\n"     , pxtu_stt.xm.irigb_sel);
	printf("tlk_rx_clk_en   : 0x%01x (0:disable/1:enable)\n"          , pxtu_stt.xm.tlkRxClk_en   >> 8);
	printf("tlk_tx_clk_sel  : 0x%01x (0:onboard/1:ref clk)\n"         , pxtu_stt.xm.tlkTxClk_sel  >> 4);
	printf("xtu_mclk_sel    : 0x%01x (0:onboard/1:ref/2:tlk rx clk)\n", pxtu_stt.xm.mclk_sel);
	printf("xtu_ref_clk_loss: 0x%01x (0:detected/1:loss)\n"           , pxtu_stt.xtu_ref_clk_loss >> 2);
	printf("xtu_trx_clk_loss: 0x%01x (0:detected/1:loss)\n"           , pxtu_stt.xtu_trx_clk_loss >> 1);
	printf("xtu_mclk_lock   : 0x%01x (0:unlock/1:lock)\n"             , pxtu_stt.xtu_mclk_lock);
	printf("xtu_cdc_sel     : 0x%01x (0:-/1:-)\n"                     , pxtu_stt.dbg.cdc_sel      >> 1);
	printf("xtu_jtag_sel    : 0x%01x (0:fpga/1:idt)\n"                , pxtu_stt.dbg.jtag_sel);

  return OK;
}

STATUS xtu_GetXtuStatus_2( ST_drvCLTU *pCfg )
{
#ifdef _CLTU_LINUX_

	ozXTU_STT_T pxtu_stt;
	unsigned int rd_tmp;
	int status;


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_ENDIAN_CHECK, &rd_tmp );
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_ENDIAN_CHECK, error\n"); return WR_ERROR; }
	pxtu_stt.xtu_endian = rd_tmp == 0 ? 0 : 1;
	

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_CLK_CONFIG, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_CLK_CONFIG, error\n"); return WR_ERROR; }
	pxtu_stt.xm.tlkRxClk_en   = rd_tmp & XTU_TLKRX_CLK;
	pxtu_stt.xm.tlkTxClk_sel  = rd_tmp & XTU_TLKTX_CLK;
	pxtu_stt.xm.mclk_sel      = rd_tmp & XTU_MCLK_SEL;



	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_DBG_CONFIG, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_DBG_CONFIG, error\n"); return WR_ERROR; }
	pxtu_stt.dbg.cdc_sel      = rd_tmp & XTU_CDC_SEL;
	pxtu_stt.dbg.jtag_sel     = rd_tmp & XTU_JTAG_SEL;


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_CLK_STATUS, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_CLK_STATUS, error\n"); return WR_ERROR; }
	pxtu_stt.xtu_ref_clk_loss = rd_tmp & XTU_REF_CLK_LOSS;
	pxtu_stt.xtu_trx_clk_loss = rd_tmp & XTU_TLKRX_CLK_LOSS;
	pxtu_stt.xtu_mclk_lock    = rd_tmp & XTU_MCLK_LOCK; 


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_SEL_XTU_MODE, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_SEL_XTU_MODE, error\n"); return WR_ERROR; }
	pxtu_stt.xm.mode          = rd_tmp & XTU_MODE_SEL;


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_SEL_IRIGB, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_SEL_IRIGB, error\n"); return WR_ERROR; }
	pxtu_stt.xm.irigb_sel     = rd_tmp & XTU_IRIGB_SEL;


	printf("=== Get XTU Status =========\n");

	printf("xtu_endian      : 0x%01x (0:big/1:little)\n"              , pxtu_stt.xtu_endian);   
	printf("xtu_mode        : 0x%01x (0:ltu/1:ctu)\n"                 , pxtu_stt.xm.mode);
	printf("irigb_sel       : 0x%01x (0:irig or tlk rx/1:igen)\n"     , pxtu_stt.xm.irigb_sel);
	printf("tlk_rx_clk_en   : 0x%01x (0:disable/1:enable)\n"          , pxtu_stt.xm.tlkRxClk_en   >> 8);
	printf("tlk_tx_clk_sel  : 0x%01x (0:onboard/1:ref clk)\n"         , pxtu_stt.xm.tlkTxClk_sel  >> 4);
	printf("xtu_mclk_sel    : 0x%01x (0:onboard/1:ref/2:tlk rx clk)\n", pxtu_stt.xm.mclk_sel);
	printf("xtu_ref_clk_loss: 0x%01x (0:detected/1:loss)\n"           , pxtu_stt.xtu_ref_clk_loss >> 2);
	printf("xtu_trx_clk_loss: 0x%01x (0:detected/1:loss)\n"           , pxtu_stt.xtu_trx_clk_loss >> 1);
	printf("xtu_mclk_lock   : 0x%01x (0:unlock/1:lock)\n"             , pxtu_stt.xtu_mclk_lock);
	printf("xtu_cdc_sel     : 0x%01x (0:-/1:-)\n"                     , pxtu_stt.dbg.cdc_sel      >> 1);
	printf("xtu_jtag_sel    : 0x%01x (0:fpga/1:idt)\n"                , pxtu_stt.dbg.jtag_sel);
#endif

  return OK;
}

STATUS xtu_GetCurrTimeStatus( ST_drvCLTU *pCfg )
{
	ozXTU_TIME_T pxtu_time;
	unsigned int rd_tmp;  
	
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}   

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_CURR_TIME      );
	pxtu_time.curr.day              = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_CURR_TIME + 4  );
	hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_CURR_TIME + 8  );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.curr.ms), &(pxtu_time.curr.ns) );


	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_COMP_DELAY     );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.comp.ms), &(pxtu_time.comp.ns) );

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME      );
	pxtu_time.t_shot.start.day   = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 4  );
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.start.hour),&(pxtu_time.t_shot.start.min),&(pxtu_time.t_shot.start.sec));

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 8  );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.start.ms), &(pxtu_time.t_shot.start.ns) );

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 16 );
	pxtu_time.t_shot.stop.day    = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 20 );
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.stop.hour),&(pxtu_time.t_shot.stop.min),&(pxtu_time.t_shot.stop.sec));

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_SHOT_TIME + 24 );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.stop.ms), &(pxtu_time.t_shot.stop.ns) );


	
	printf("\n=== Get XTU Time =========\n");	
	printf("Current Time : %3dday/%3dh.%2dm.%2ds/%3dms.      xns\n",
							pxtu_time.curr.day,
							pxtu_time.curr.hour,
							pxtu_time.curr.min,
							pxtu_time.curr.sec,
							pxtu_time.curr.ms);
#if 0
	printf("--- Tx Shot Time ---\n");
	printf("start time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.start.day
															,pxtu_time.t_shot.start.hour
															,pxtu_time.t_shot.start.min
															,pxtu_time.t_shot.start.sec
															,pxtu_time.t_shot.start.ms
															,pxtu_time.t_shot.start.ns);

	printf("stop time    : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.stop.day
															,pxtu_time.t_shot.stop.hour
															,pxtu_time.t_shot.stop.min
															,pxtu_time.t_shot.stop.sec
															,pxtu_time.t_shot.stop.ms
															,pxtu_time.t_shot.stop.ns);
#endif
	printf("--- checked RX Time ---\n");  
	printf("fiber RX Time : %3dms.%7dns\n",pxtu_time.comp.ms,pxtu_time.comp.ns);

	return OK;
}

STATUS xtu_GetCurrTimeStatus_2( ST_drvCLTU *pCfg )
{
#ifdef _CLTU_LINUX_

	ozXTU_TIME_T pxtu_time;
	unsigned int rd_tmp;
	int status;


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_CURR_TIME, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_CURR_TIME, error\n"); return WR_ERROR; }
	pxtu_time.curr.day              = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_CURR_TIME_4, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_CURR_TIME_4, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_CURR_TIME_8, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_CURR_TIME_8, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.curr.ms), &(pxtu_time.curr.ns) );



	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_COMP_DELAY, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_COMP_DELAY, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.comp.ms), &(pxtu_time.comp.ns) );


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME, error\n"); return WR_ERROR; }
	pxtu_time.t_shot.start.day   = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME_4, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME_4, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.start.hour),&(pxtu_time.t_shot.start.min),&(pxtu_time.t_shot.start.sec));


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME_8, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME_8, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.start.ms), &(pxtu_time.t_shot.start.ns) );



	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME_16, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME_16, error\n"); return WR_ERROR; }
	pxtu_time.t_shot.stop.day    = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME_20, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME_20, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.stop.hour),&(pxtu_time.t_shot.stop.min),&(pxtu_time.t_shot.stop.sec));


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_SHOT_TIME_24, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_TX_SHOT_TIME_24, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.stop.ms), &(pxtu_time.t_shot.stop.ns) );


	printf("\n=== Get XTU Time =========\n");	
	printf("My (LTU) Current Time : %3dday/%3dh.%2dm.%2ds/%3dms.      xns\n",
							pxtu_time.curr.day,
							pxtu_time.curr.hour,
							pxtu_time.curr.min,
							pxtu_time.curr.sec,
							pxtu_time.curr.ms);
#if 0
	printf("--- Tx Shot Time ---\n");
	printf("start time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.start.day
															,pxtu_time.t_shot.start.hour
															,pxtu_time.t_shot.start.min
															,pxtu_time.t_shot.start.sec
															,pxtu_time.t_shot.start.ms
															,pxtu_time.t_shot.start.ns);

	printf("stop time    : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.stop.day
															,pxtu_time.t_shot.stop.hour
															,pxtu_time.t_shot.stop.min
															,pxtu_time.t_shot.stop.sec
															,pxtu_time.t_shot.stop.ms
															,pxtu_time.t_shot.stop.ns);
#endif
	printf("--- checked RX Time ---\n");
	printf("fiber RX Time : %3dms.%7dns\n",pxtu_time.comp.ms,pxtu_time.comp.ns);
#endif

	return OK;
}

STATUS xtu_GetTlkRxTimeStatus( ST_drvCLTU *pCfg )
{

	ozXTU_TIME_T pxtu_time;
	unsigned int rd_tmp;  

	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
		PRINT_ERR(); return ERROR;
	}   

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_CURR_TIME      );
	pxtu_time.curr.day              = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_CURR_TIME + 4  );
	hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));  

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_CURR_TIME + 8  );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.curr.ms), &(pxtu_time.curr.ns) );


	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_COMP_DELAY     );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.comp.ms), &(pxtu_time.comp.ns) );

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME      );
	pxtu_time.t_shot.start.day   = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME + 4  );
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.start.hour),&(pxtu_time.t_shot.start.min),&(pxtu_time.t_shot.start.sec));  

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME + 8  );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.start.ms), &(pxtu_time.t_shot.start.ns) );

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME + 16 );
	pxtu_time.t_shot.stop.day    = (rd_tmp & 0x0000ffff);

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME + 20 );
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.stop.hour),&(pxtu_time.t_shot.stop.min),&(pxtu_time.t_shot.stop.sec));  

	DELAY_WAIT(0);
	rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_SHOT_TIME + 24 );
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.stop.ms), &(pxtu_time.t_shot.stop.ns) );

	printf("=== Get XTU Rx Time =========\n");  
	printf("Current Time : %3dday/%3dh.%2dm.%2ds/%3dms.      xns\n",
											pxtu_time.curr.day,
											pxtu_time.curr.hour,
											pxtu_time.curr.min,
											pxtu_time.curr.sec,
											pxtu_time.curr.ms);

	printf("--- Rx Shot Time ---\n");  
	printf("start time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.start.day
														,pxtu_time.t_shot.start.hour
														,pxtu_time.t_shot.start.min
														,pxtu_time.t_shot.start.sec
														,pxtu_time.t_shot.start.ms
														,pxtu_time.t_shot.start.ns);

	printf("stop time    : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.stop.day
														,pxtu_time.t_shot.stop.hour
														,pxtu_time.t_shot.stop.min
														,pxtu_time.t_shot.stop.sec
														,pxtu_time.t_shot.stop.ms
														,pxtu_time.t_shot.stop.ns);

	printf("--- Rx compensation Time ---\n");  
	printf("compensation Time from CTU : %3dms.%7dns\n",pxtu_time.comp.ms, pxtu_time.comp.ns);

  return OK;
}

STATUS xtu_GetTlkRxTimeStatus_2( ST_drvCLTU *pCfg )
{
#ifdef _CLTU_LINUX_

	ozXTU_TIME_T pxtu_time;
	unsigned int rd_tmp;  
	int status;


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_CURR_TIME, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_CURR_TIME, error\n"); return WR_ERROR; }
	pxtu_time.curr.day              = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_CURR_TIME_4, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_CURR_TIME_4, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));  


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_CURR_TIME_8, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_CURR_TIME_8, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.curr.ms), &(pxtu_time.curr.ns) );


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_COMP_DELAY, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_COMP_DELAY, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.comp.ms), &(pxtu_time.comp.ns) );


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME, error\n"); return WR_ERROR; }
	pxtu_time.t_shot.start.day   = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME_4, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME_4, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.start.hour),&(pxtu_time.t_shot.start.min),&(pxtu_time.t_shot.start.sec));  


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME_8, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME_8, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.start.ms), &(pxtu_time.t_shot.start.ns) );



	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME_16, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME_16, error\n"); return WR_ERROR; }
	pxtu_time.t_shot.stop.day    = (rd_tmp & 0x0000ffff);


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME_20, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME_20, error\n"); return WR_ERROR; }
	hms_HtoD(&rd_tmp,&(pxtu_time.t_shot.stop.hour),&(pxtu_time.t_shot.stop.min),&(pxtu_time.t_shot.stop.sec));  


	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_SHOT_TIME_24, &rd_tmp);
	if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_SHOT_TIME_24, error\n"); return WR_ERROR; }
	ms100MTick_HtoD(&rd_tmp, &(pxtu_time.t_shot.stop.ms), &(pxtu_time.t_shot.stop.ns) );


	printf("=== Get XTU Rx Time =========\n");  
	printf("Current Time : %3dday/%3dh.%2dm.%2ds/%3dms.      xns\n",
											pxtu_time.curr.day,
											pxtu_time.curr.hour,
											pxtu_time.curr.min,
											pxtu_time.curr.sec,
											pxtu_time.curr.ms);

	printf("--- Rx Shot Time ---\n");  
	printf("start time   : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.start.day
														,pxtu_time.t_shot.start.hour
														,pxtu_time.t_shot.start.min
														,pxtu_time.t_shot.start.sec
														,pxtu_time.t_shot.start.ms
														,pxtu_time.t_shot.start.ns);

	printf("stop time    : %3dday/%3dh.%2dm.%2ds/%3dms.%7dns\n",pxtu_time.t_shot.stop.day
														,pxtu_time.t_shot.stop.hour
														,pxtu_time.t_shot.stop.min
														,pxtu_time.t_shot.stop.sec
														,pxtu_time.t_shot.stop.ms
														,pxtu_time.t_shot.stop.ns);

	printf("--- Rx compensation Time ---\n");
        printf("compensation Time from CTU : %3dms.%7dns\n",pxtu_time.comp.ms, pxtu_time.comp.ns);

#endif

  return OK;
}






