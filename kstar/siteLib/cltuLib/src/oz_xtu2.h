#if !defined(__XTU_2ND_VDD_H__)
#define __XTU_2ND_VDD_H__
/******************************************************************************
 *  Copyright (c) 2009 ~ by OLZETEK. All Rights Reserved.                     *
 *                                                                            *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

when       who    what, where, why
YY/MM/DD
--------   ---    --------------------------------------------------------------
08/12/23  dhpark  Created.
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "drvCLTU.h"
#include "cltu_Def.h"

#if 0
void xtu_isr( void );
/*STATUS xtu_dev_start( ST_drvCLTU *  ); */
STATUS xtu_dev_stop( ST_drvCLTU *  );
STATUS xtu_reset( ST_drvCLTU * );
STATUS xtu_SttEndian( ST_drvCLTU *, unsigned char *SttEndian );
STATUS xtu_SttIRIGB( ST_drvCLTU *pCfg, unsigned char *boolValue );
STATUS xtu_GetIRIGBUnlockCount( ST_drvCLTU *pCfg, unsigned int *value );
STATUS xtu_SttSerdes( ST_drvCLTU * ,unsigned char *boolValue );
STATUS xtu_SttFOTRxLink(ST_drvCLTU * , unsigned char *boolValue );
STATUS xtu_GetFPGA_Ver( ST_drvCLTU * , unsigned int *pVer );
STATUS xtu_GetSerdesRxUnLockCnt(ST_drvCLTU * , unsigned int *RxCnt );
STATUS xtu_GetSerdesRxCnt(ST_drvCLTU * , unsigned int *RxCnt );
STATUS xtu_GetSerdesTxCnt(ST_drvCLTU * , unsigned int *TxCnt );
STATUS xtu_GetPortOutStatus(ST_drvCLTU * , unsigned char PortNum, unsigned int *PortCnt );
STATUS xtu_GetCTU_ShotTime(ST_drvCLTU * , unsigned int *CTU_ShotStartTime, unsigned int *CTU_ShotStartTick, unsigned int *CTU_ShotEndTime, unsigned int *CTU_ShotEndTick);
STATUS xtu_GetLTU_ShotTime(ST_drvCLTU * , unsigned int *CTU_ShotStartTime, unsigned int *CTU_ShotStartTick, unsigned int *CTU_ShotEndTime, unsigned int *CTU_ShotEndTick);
STATUS xtu_GetCompensationID( ST_drvCLTU * ,unsigned int *pCMPST_ID );
STATUS xtu_GetLastUpdateTick( ST_drvCLTU * ,unsigned int *pLast_Tick );
STATUS xtu_SetTestPoint(ST_drvCLTU * , unsigned int select_tp );
STATUS xtu_GetCurrentTime( ST_drvCLTU * ,unsigned int *pCurrentTime, unsigned int *pCurrentTick );
STATUS xtu_GetIRIGBTime( ST_drvCLTU * ,unsigned int *pCurrentTime );
STATUS xtu_GetSerdesRxTime(ST_drvCLTU * , unsigned int *pCurrentTime );
STATUS xtu_SttRefClock(ST_drvCLTU * , unsigned char *boolValue );

STATUS xtu_SetPortConfig(ST_drvCLTU * , unsigned char portN, unsigned char enable, unsigned char activeLow, unsigned char trgOn );
STATUS xtu_SetPortConfigDirect(ST_drvCLTU * , unsigned char enable_HEX, unsigned char activeLow_HEX, unsigned char trgOn_HEX);
STATUS xtu_SetIntervalConfig(ST_drvCLTU * , unsigned char portN, unsigned char intervalN, 
                                unsigned long long int start_offset, unsigned long long int end_offset,
                                unsigned int clockVal );

STATUS ctu_SetIRIG_B_Enable(ST_drvCLTU * , unsigned char enable );

STATUS xtu_SetPortRegister( ST_drvCLTU *  );

#endif




STATUS xtu_SetEndian( ST_drvCLTU *  );
STATUS xtu_SetMode(ST_drvCLTU * , unsigned int xtu_mode, unsigned char XTU_ID);
STATUS xtu_SetDBGMode( ST_drvCLTU *, ozXTU_DBGM_T* pxtu_dbgm);
STATUS xtu_SetLocalTime( ST_drvCLTU *, ozTIME_T* pltime );
STATUS xtu_SetCTUShotDelay( ST_drvCLTU *);
STATUS xtu_SetLTUShotDelay( ST_drvCLTU *pCfg);
STATUS xtu_SetPortActiveLevel( ST_drvCLTU *pCfg, unsigned int port_al );/*update by 3.31c by laykim 2012.03.13 */
STATUS xtu_SetPortCtrl( ST_drvCLTU *pCfg, unsigned char port );
STATUS xtu_SetPortHLI( ST_drvCLTU *pCfg, unsigned int port, unsigned int hli_num, ozPCONFIG_HLI_T* pphli);
STATUS xtu_SetPortLLI( ST_drvCLTU *pCfg, unsigned int port, unsigned int lli_num, ozPCONFIG_LLI_T* pplli);
STATUS xtu_SetShotTime( ST_drvCLTU *pCfg, ozSHOT_TIME_T* pshot_time);
STATUS xtu_StartShotNow( ST_drvCLTU * );
STATUS xtu_StopShotNow( ST_drvCLTU *);
STATUS xtu_SetIRIG_B_Src(ST_drvCLTU *);
STATUS xtu_SetClkConfig( ST_drvCLTU *);
STATUS xtu_SetPermClk( ST_drvCLTU *);
STATUS xtu_SetXTUMode( ST_drvCLTU *);





/* from oz_xtu2_util.h. *********************************/
#define TIME_SEC     (0x000000000BEBC200)
#define TIME_MIN     (0x00000002CB417800)
#define TIME_HOUR    (0x000000A7A3582000)
#define TIME_DAY     (0x00000FB750430000)
/*
STATUS xtu_Status( ST_drvCLTU *pCfg );
STATUS xtu_Status_2( ST_drvCLTU *pCfg );
*/
/*STATUS xtu_time_print( unsigned long long int ll_time );*/
STATUS xtu_time_print_new( unsigned int time, unsigned int tick );

STATUS hms_HtoD( unsigned int* phmsH, unsigned char* phD, unsigned char* pmD, unsigned char* psD  );
STATUS hms_DtoH( unsigned char* phD, const unsigned char* pmD, const unsigned char* psD, unsigned int* phmsH );
STATUS ms100MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD );
STATUS ms100MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH );
STATUS ms200MTick_HtoD( unsigned int* pmsTickH, unsigned int* pmsD, unsigned int* pnsD );
STATUS ms200MTick_DtoH( const unsigned int* pmsD, const unsigned int* pnsD, unsigned int* pmsTickH );
STATUS xtu_200MTimeSubtraction( ozTIME_T* ptimeA, unsigned int nsB, ozTIME_T* ptimeOUT);

STATUS xtu_GetGshotLog(  ST_drvCLTU *pCfg, unsigned int *pbuf);
STATUS xtu_GetPortLog( ST_drvCLTU *pCfg, unsigned char port_num, unsigned int *pbuf);

STATUS xtu_GetXtuInfo( ST_drvCLTU *pCfg );
STATUS xtu_GetXtuInfo_2( ST_drvCLTU *pCfg );


STATUS xtu_GetXtuStatus(ST_drvCLTU *pCfg);
STATUS xtu_GetXtuStatus_2(ST_drvCLTU *pCfg);

STATUS xtu_GetCurrTimeStatus( ST_drvCLTU *pCfg );
STATUS xtu_GetCurrTimeStatus_2( ST_drvCLTU *pCfg );

STATUS xtu_GetTlkRxTimeStatus( ST_drvCLTU *pCfg );
STATUS xtu_GetTlkRxTimeStatus_2( ST_drvCLTU *pCfg );




#ifdef __cplusplus                                    
}
#endif
#endif /* __XTU_2ND_VDD_H__ */
                          
                          
