#if !defined(__DRV_XTU_2ND_H__)
#define __DRV_XTU_2ND_H__
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
#ifdef __cplusplus
	extern "C" {
#endif

#include "cltu_header.h"


int r2_hms_HtoD( unsigned int* phmsH, unsigned char* phD, unsigned char* pmD, unsigned char* psD  );
int r2_hms_DtoH( unsigned char* phD, const unsigned char* pmD, const unsigned char* psD, unsigned int* phmsH );
int r2_ms100MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD );
int r2_ms100MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH );
int r2_ms200MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD );
int r2_ms200MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH );
int r2_200MTimeSubtraction( ozTIME_T* ptimeA, unsigned int nsB, ozTIME_T* ptimeOUT);




int r2_SetEndian( CLTU_Device *device );
int r2_SetMode( CLTU_Device *device, unsigned int xtu_mode, unsigned char XTU_ID);
int r2_SetDBGMode( CLTU_Device *device, ozXTU_DBGM_T* pxtu_dbgm);
int r2_SetLocalTime( CLTU_Device *, ozTIME_T* pltime );
int r2_SetCTUShotDelay( CLTU_Device *);
int r2_SetLTUShotDelay( CLTU_Device *pCfg );
int r2_SetPortActiveLevel( CLTU_Device *pCfg, unsigned int port_al );
int r2_SetPortCtrl( CLTU_Device *device, unsigned int port );
int r2_SetPortHLI( CLTU_Device *device, unsigned int port, unsigned int hli_id, ozPCONFIG_HLI_T* pphli);
int r2_SetPortLLI( CLTU_Device *device, unsigned int port, unsigned int lli_d, ozPCONFIG_LLI_T* pplli);
int r2_SetShotTime( CLTU_Device *device, ozSHOT_TIME_T* pshot_time);
int r2_StartShotNow( CLTU_Device *device);
int r2_StopShotNow( CLTU_Device *device);
int r2_SetIRIG_B_Src( CLTU_Device *device, unsigned int irigb_src);
int r2_SetClkConfig(CLTU_Device *device, unsigned int tlkRxClk_en, unsigned int tlkTxClk_sel, unsigned int mclk_sel );
int r2_SetPermClk( CLTU_Device *device, unsigned int value);


int r2_GetGshotLog(  CLTU_Device *device, unsigned int *pbuf);
int r2_GetPortLog( CLTU_Device *device, unsigned char port_num, unsigned int *pbuf);


int r2_GET_ENDIAN_CHECK(CLTU_Device *device, unsigned int *value );
int r2_GET_CLK_CONFIG(CLTU_Device *device, unsigned int *value );
int r2_GET_DBG_CONFIG(CLTU_Device *device, unsigned int *value );
int r2_GET_CLK_STATUS(CLTU_Device *device, unsigned int *value );
int r2_GET_SEL_XTU_MODE(CLTU_Device *device, unsigned int *value );
int r2_GET_SEL_IRIGB(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_CURR_TIME(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_CURR_TIME_4(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_CURR_TIME_8(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_COMP_DELAY(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME_4(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME_8(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME_16(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME_20(CLTU_Device *device, unsigned int *value );
int r2_GET_TX_SHOT_TIME_24(CLTU_Device *device, unsigned int *value );

int r2_GET_RX_CURR_TIME(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_CURR_TIME_4(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_CURR_TIME_8(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_COMP_DELAY(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME_4(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME_8(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME_16(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME_20(CLTU_Device *device, unsigned int *value );
int r2_GET_RX_SHOT_TIME_24(CLTU_Device *device, unsigned int *value );
int r2_GET_VENDOR_ID(CLTU_Device *device, unsigned int *value );
int r2_GET_DEVICE_ID(CLTU_Device *device, unsigned int *value );
int r2_GET_VERSION(CLTU_Device *device, unsigned int *value );



#ifdef __cplusplus
}
#endif 
#endif /* __DRV_XTU_2ND_H__ */
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
