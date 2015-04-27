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
Function  : xtu_GetCompensationTime
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_GetCompensationTime( ST_drvCLTU *pCfg , unsigned int *pCMPST_Time )
{
  unsigned int read_value;

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  read_value = READ32(pCfg->pciDevice.base0 + XTUR018_);

  *pCMPST_Time = read_value & 0x0000ffff; 
  
  return OK;
}

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
Function  : xtu_endian_set
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SetEndian( ST_drvCLTU *pCfg )
{

  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  /* Write Endian control register */
  WRITE32(pCfg->pciDevice.base0 + XTUR000_ENDIAN, 0x01234567);
  DELAY_WAIT(0);
  printf("[XTU] xtu_endian_set .....OK!\n");

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
Function  : xtu_SetMode
Author    : laykim
Parameter : .
Return    : .
Desc      : 
Date      : 
*******************************************************************************/
STATUS xtu_SetMode( ST_drvCLTU *pCfg, unsigned int xtu_mode, unsigned char XTU_ID)
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }   
  
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
  

  PRINTF(("[XTU] xtu_SetMode as mode : %d .....OK!\n",xtu_mode));
  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : xtu_SetShotTime
Author    : laykim
Parameter : .
Return    : .
Desc      : shot start time set.
Date      : 
*******************************************************************************/
STATUS xtu_SetShotTime( ST_drvCLTU *pCfg, unsigned int shot_Start_Time, unsigned int shot_Start_Tick, unsigned int shot_End_Time, unsigned int shot_End_Tick)
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  WRITE32(pCfg->pciDevice.base0 + XTUR008_TSSTM, shot_Start_Time);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR009_TSSTL, shot_Start_Tick);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR00A_TSETM, shot_End_Time);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR00B_TSETL, shot_End_Tick);
  DELAY_WAIT(0);

  return OK;
}

/*******************************************************************************
Company   : Olzetek
Function  : ctu_SerdesTxImmediate
Author    : laykim
Parameter : .
Return    : .
Desc      :
Date      : 
*******************************************************************************/
STATUS ctu_SerdesTxImmediate( ST_drvCLTU *pCfg)
{
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }
  
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, BIT31);
  DELAY_WAIT(0);
  WRITE32(pCfg->pciDevice.base0 + XTUR00E_, 0);
  DELAY_WAIT(0);

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
    set_value = BIT31 | BIT30;
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

/*******************************************************************************
Company   : Olzetek
Function  : ltu_SetCompensationTime
Author    : laykim
Parameter : .
Return    : .
Desc      : ltu time delay set.
Date      : 
*******************************************************************************/
STATUS ltu_SetCompensationTime( ST_drvCLTU *pCfg, unsigned int cmpst_value )
{
  unsigned int time_update_tick;
    
  if(pCfg->pciDevice.init != 0xFFFFFFFF)
  {
    PRINT_ERR(); return ERROR;
  }

  time_update_tick = TICK_SEC - cmpst_value;
  
  WRITE32(pCfg->pciDevice.base0 + XTUR005_XTUDelay, time_update_tick);
	DELAY_WAIT(0);

  return OK;
}










STATUS xtu_Status( ST_drvCLTU *pCfg )
{
  unsigned char SttEndian;
  unsigned char boolvalue;

  unsigned int  irigb;
  unsigned int  irigb_current_time;

  unsigned int  value;
  unsigned int  CMPST_ID;
  unsigned int  CMPST_Time;

  unsigned int LTU_ShotStartTime,LTU_ShotEndTime, LTU_ShotStartTick,LTU_ShotEndTick;
  unsigned int CTU_ShotStartTime,CTU_ShotEndTime, CTU_ShotStartTick,CTU_ShotEndTick;
  unsigned int RxCnt,TxCnt;
  unsigned int xtu_current_time, xtu_current_tick;

  unsigned int last_Tick;
  
  
  unsigned int xtu_ver;

  unsigned char xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver;
  

  printf("\n");
  printf("========== XTU Status Report ==========\n\n");

  printf("XTU DEVICE DRIVER VERSION  : %1x.%02x.%1X \n",XTU_DEVICE_DRIVER_VER,XTU_DEVICE_DRIVER_VER_REVISION,XTU_DEVICE_DRIVER_SUB_VER);

  printf("\n");

  /*------------------------------------------------------------- */
  printf("Mode                : ");
  
  
  xtu_GetFPGA_Ver(pCfg, &xtu_ver);  
  
  xtu_fpga_ver      = (xtu_ver & 0x0000F000)>>12;
  xtu_fpga_revision = (xtu_ver & 0x00000FF0)>>4;
  xtu_fpga_sub_ver  = (xtu_ver & 0x0000000F);

  if(XTU_READ_CTU_MODE == (xtu_ver & XTU_READ_CTU_MODE))
  {
    printf("CTU FPGA Version_%1x.%02x.%1X\n",xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver);
  }
  else
  {
    printf("LTU FPGA Version_%1x.%02x.%1X\n",xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver);
  }

  /*------------------------------------------------------------- */
  xtu_SttEndian(pCfg,&SttEndian);
  if(BigEndian == SttEndian)
  {
    printf("Endian              : Big Endian\n");
  }
  else
  {
    printf("Endian              : Little Endian\n");
  }

  printf("\n");

  
  /*------------------------------------------------------------- */
  xtu_SttRefClock(pCfg, &boolvalue );
  if(1 == boolvalue)
  {
    printf("Ref Clk status      : OK\n");/*not connected in fpga 2011.02.25 */
  }
  else
  {
    printf("Ref Clk status      : Fail\n");
  }

  /*------------------------------------------------------------- */
  xtu_SttIRIGB(pCfg, &boolvalue );
  if(1 == boolvalue)
  {
    printf("IRIG-B Status       : OK\n");
  }
  else
  {
    printf("IRIG-B Status       : Fail\n");
  }

  xtu_GetIRIGBUnlockCount(pCfg, &irigb );
  printf("IRIG-B unlock count : %d \n", irigb);
  printf("\n");

  /*------------------------------------------------------------- not complete */
  xtu_SttSerdes(pCfg,&boolvalue);
  if(1 == boolvalue)
  {
    printf("Serdes Rx Link      : Connected\n");
  }
  else
  {
    printf("Serdes Rx Link      : Disconnected\n");
  }

  /*------------------------------------------------------------- */
  xtu_SttFOTRxLink(pCfg,&boolvalue);
  if(1 == boolvalue)
  {
    printf("Fiber Rx Link       : Connected\n");/*not connected in fpga 2011.02.25 */
  }
  else
  {
    printf("Fiber Rx Link       : Disconnected\n");
  }

  printf("\n");

 
  /*------------------------------------------------------------- */

  xtu_GetSerdesTxCnt(pCfg, &TxCnt );
  printf("SERDES Tx Count     : %d\n",TxCnt);
  xtu_GetSerdesRxCnt(pCfg, &RxCnt);
  printf("SERDES Rx Count     : %d\n",RxCnt);
  printf("\n");

  xtu_GetLastUpdateTick(pCfg, &last_Tick );
  printf("Last Update Tick    : %d\n",last_Tick);
  printf("\n");


  xtu_GetCompensationTime( pCfg, &CMPST_Time );
  xtu_GetCompensationID(pCfg,  &CMPST_ID );
  printf("[Compensation Time] : [ID: 0x%04x/%d] %d\n",CMPST_ID,CMPST_ID,CMPST_Time);

  xtu_GetCurrentTime( pCfg,&xtu_current_time, &xtu_current_tick); /*ctu current Time */
  printf("[Current Time]      :");  
  xtu_time_print_new(xtu_current_time, xtu_current_tick);

  xtu_GetIRIGBTime( pCfg,&irigb_current_time );   /*IRIGB Time */
  printf("[IRIGB Time]        :");  
  xtu_time_print_new(irigb_current_time,0);

  xtu_GetSerdesRxTime( pCfg,&value );   /*serdes rx current time [not complete] */
  printf("[Serdes Rx Time]    :");  
  xtu_time_print_new(value,0);

  
  
  

  

  printf("\n");
  xtu_GetCTU_ShotTime(pCfg,&CTU_ShotStartTime,&CTU_ShotStartTick, &CTU_ShotEndTime,&CTU_ShotEndTick);
  printf("CTU ShotStart       : 0x%08x Time 0x%08x Tick\n",CTU_ShotStartTime,CTU_ShotStartTick);  
  printf("CTU ShotEnd         : 0x%08x Time 0x%08x Tick\n",CTU_ShotEndTime,CTU_ShotEndTick);  

  printf("\n");
  xtu_GetLTU_ShotTime(pCfg,&LTU_ShotStartTime,&LTU_ShotStartTick, &LTU_ShotEndTime, &LTU_ShotEndTick);
  printf("LTU Shot Start      : 0x%08x Time 0x%08x Tick\n",LTU_ShotStartTime,LTU_ShotStartTick);  
  printf("LTU Shot End        : 0x%08x Time 0x%08x Tick\n",LTU_ShotEndTime,LTU_ShotEndTick); 

  
  printf("\n");
  printf("========== XTU Status Report End ==========\n");
  
  return OK;
}

STATUS xtu_Status_2( ST_drvCLTU *pCfg )
{
#ifdef _CLTU_LINUX_

	int status;
  unsigned char SttEndian;
  unsigned char boolvalue;

  unsigned int  irigb;
/*  unsigned int  irigb_current_time; */

/*  unsigned int  value; */
  unsigned int  CMPST_ID;
  unsigned int  CMPST_Time;

/*  unsigned int LTU_ShotStartTime,LTU_ShotEndTime, LTU_ShotStartTick,LTU_ShotEndTick; */
/*  unsigned int CTU_ShotStartTime,CTU_ShotEndTime, CTU_ShotStartTick,CTU_ShotEndTick; */
  unsigned int RxCnt,TxCnt;
/*  unsigned int xtu_current_time, xtu_current_tick; */

/*  unsigned int last_Tick; */
  
  
  unsigned int xtu_ver;

  unsigned char xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver;


	printf("\n");
	printf("Using \"ioctl()\" method.\n");
	printf("========== XTU Status Report ==========\n\n");

	printf("XTU DEVICE DRIVER VERSION  : %1x.%02x.%1X \n",XTU_DEVICE_DRIVER_VER,XTU_DEVICE_DRIVER_VER_REVISION,XTU_DEVICE_DRIVER_SUB_VER);

	printf("\n");

	/*------------------------------------------------------------- */
	printf("Mode                : ");


	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_FPGA_VER, &xtu_ver);
	if (status == -1) {
		printf("ioctl error!!!!\n");
		return WR_ERROR;
		}
	
  xtu_fpga_ver      = (xtu_ver & 0x0000F000)>>12;
  xtu_fpga_revision = (xtu_ver & 0x00000FF0)>>4;
  xtu_fpga_sub_ver  = (xtu_ver & 0x0000000F);

  if(XTU_READ_CTU_MODE == (xtu_ver & XTU_READ_CTU_MODE))
  {
    printf("CTU FPGA Version_%1x.%02x.%1X\n",xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver);
  }
  else
  {
    printf("LTU FPGA Version_%1x.%02x.%1X\n",xtu_fpga_ver, xtu_fpga_revision, xtu_fpga_sub_ver);
  }

  /*------------------------------------------------------------- */
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_ENDIAN, &SttEndian);
	if (status == -1) return WR_ERROR;

  if(BigEndian == SttEndian)
  {
    printf("Endian              : Big Endian\n");
  }
  else
  {
    printf("Endian              : Little Endian\n");
  }

  printf("\n");

  
  /*------------------------------------------------------------- */
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_REF_CLOCK, &boolvalue);
	if (status == -1) return WR_ERROR;

  if(1 == boolvalue)
  {
    printf("Ref Clk status      : OK\n");/*not connected in fpga 2011.02.25 */
  }
  else
  {
    printf("Ref Clk status      : Fail\n");
  }

  /*------------------------------------------------------------- */
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_IRIGB, &boolvalue);
	if (status == -1) return WR_ERROR;
	if(1 == boolvalue)
	{
		printf("IRIG-B Status       : OK\n");
	}
	else
	{
		printf("IRIG-B Status       : Fail\n");
	}

	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_IRIGB_UNLOCK_CNT, &irigb);
	if (status == -1) return WR_ERROR;

	printf("IRIG-B unlock count : %d \n", irigb);
	printf("\n");

  /*------------------------------------------------------------- not complete */
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_SERDES, &boolvalue);
	if (status == -1) return WR_ERROR;

	if(1 == boolvalue)
	{
		printf("Serdes Rx Link      : Connected\n");
	}
	else
	{
		printf("Serdes Rx Link      : Disconnected\n");
	}

  /*------------------------------------------------------------- */
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_FOT_RXLINK, &boolvalue);
	if (status == -1) return WR_ERROR;

	if(1 == boolvalue)
	{
		printf("Fiber Rx Link       : Connected\n");/*not connected in fpga 2011.02.25 */
	}
	else
	{
		printf("Fiber Rx Link       : Disconnected\n");
	}
	printf("\n");

	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_SERDES_TX_CNT, &TxCnt);
	if (status == -1) return WR_ERROR;
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_SERDES_RX_CNT, &RxCnt);
	if (status == -1) return WR_ERROR;
	
	printf("SERDES Tx Count     : %d\n",TxCnt);
	printf("SERDES Rx Count     : %d\n",RxCnt);
	printf("\n");


	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CAL_TICK, &CMPST_Time);
	if (status == -1) return WR_ERROR;
	status = ioctl(pCfg->pciDevice.fd, IOCTL_CLTU_R2_GET_CAL_ID, &CMPST_ID);
	if (status == -1) return WR_ERROR;
	printf("[Compensation Time] : [ID: 0x%04x/%d] %d\n",CMPST_ID,CMPST_ID,CMPST_Time);


	printf("\n");
	printf("========== XTU Status Report End ==========\n");
  
#endif

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



