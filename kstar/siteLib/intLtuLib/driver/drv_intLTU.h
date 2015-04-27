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

#include "intLTU_header.h"

int r2_SetPortConfig(  CLTU_Device  *device, unsigned char portN, unsigned char enable, unsigned char activeLow, unsigned char trgOn);
int r2_SetIntervalConfig( CLTU_Device *device, unsigned char portN, 
                               unsigned char intervalN, 
                               unsigned long long int start_offset, 
                               unsigned long long int end_offset, 
                               unsigned int clockVal );
int r2_GetCurrentTime( CLTU_Device *device,   unsigned int *pCurrentTime, unsigned int *pCurrentTick );
int r2_SetShotTime( CLTU_Device *device,  unsigned int shot_Start_Time, unsigned int shot_Start_Tick, unsigned int shot_End_Time, unsigned int shot_End_Tick);
int r2_SerdesTxImmediate( CLTU_Device *device);
int r2_SetMode( CLTU_Device *device, unsigned int xtu_mode, unsigned char XTU_ID);
int r2_reset( CLTU_Device *device );
int r2_SetEndian( CLTU_Device *device );
int r2_SetCompensationTime( CLTU_Device *device, unsigned int cmpst_value );
int r2_GetCompensationTime( CLTU_Device *device , unsigned int *pCMPST_Time );
int r2_GetCompensationID( CLTU_Device *device ,unsigned int *pCMPST_ID );
int r2_SetPortRegister( CLTU_Device *device );






int r2_GetFPGA_Ver( CLTU_Device *device,  unsigned int *pVer );
int r2_SttEndian( CLTU_Device *device,unsigned char *SttEndian );
int r2_SttRefClock(CLTU_Device *device,  unsigned char *boolValue );
int r2_SttIRIGB( CLTU_Device *device, unsigned char *boolValue );
int r2_GetIRIGBUnlockCount( CLTU_Device *device, unsigned int *value );
int r2_SttSerdes( CLTU_Device *device,  unsigned char *boolValue );
int r2_SttFOTRxLink( CLTU_Device *device,  unsigned char *boolValue );
int r2_GetSerdesTxCnt(CLTU_Device *device, unsigned int *TxCnt );
int r2_GetSerdesRxCnt(CLTU_Device *device, unsigned int *RxCnt );


#ifdef __cplusplus
}
#endif 
#endif /* __DRV_XTU_2ND_H__ */
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
