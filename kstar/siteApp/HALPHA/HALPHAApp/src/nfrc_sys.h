#if !defined(__NFRC_SYS_H__)
#define __NFRC_SYS_H__
/******************************************************************************
 *  Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved.                     *
 *  This header file contains all the internal definitions of Sound driver.   *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

when       who    what, where, why
YY/MM/DD
--------   ---    --------------------------------------------------------------
06/11/06  dhpark  Created.




2007.10.23   ctu_info;
typedef struct{
-  int arg[20];
+  int arg[40];
} PCI_COM_S;


typedef struct {
+	int is_gps_time_working;
}ctu_info;





==============================================================================*/
#include <stdio.h>

#if 0
#define _CLTU_VXWORK_ 
#else
#define _CLTU_LINUX_KERNEL_26_
#endif



#define FEATURE_HEADER_64_TO_32

#define FEATURE_IMIDATE_SHOT_START

#define FEATURE_SINGLE_UPDATE


#ifdef FEATURE_IMIDATE_SHOT_START
#define SHOT_OFFSET (0x000186A0)  /* 1ms */
#else
#define SHOT_OFFSET (0x05F5E100) /* 1S */
#endif

#define ENDIAN_SIGNAL (0x1234abcd)

/* pci cmd (base card -> pmc) */

#define CLTU_CMD_WRITE32  (0xF0000000)
#define CLTU_CMD_READ32   (0xF0000001)

#define CLTU_CMD_ENDIAN_CTL (0xFF0000FF)


#define CLTU_CMD_SET_MODE         (0x00000001) /* mode change. */
#define CLTU_CMD_SHOT_START       (0x00000002) /* Shot Start. */
#define CLTU_CMD_SET_CT_SS        (0x00000003) /* sigle clock trigger setting. */
#define CLTU_CMD_SET_CT_SS_RFM    (0x00000004) /* sigle clock rfm setting. */
#define CLTU_CMD_SET_CT_MS        (0x00000005) /* multi clock trigger setting. */
#define CLTU_REF_CLK_SEL          (0x00000006) /* cltu ref clock selecting. */
#define CLTU_CAL_LTU              (0x00000007) /* ltu time compensation. */
#define CLTU_CMD_SHOT_TERM        (0x00000008) /* shot term. */
#ifdef FEATURE_SINGLE_UPDATE
#define CLTU_CMD_SINGLE_MT_UD     (0x00000009) /* single mater timer update. */
#endif




/* operation mode */
#define SYS_MODE_NOR  (0x00000000)
#define SYS_MODE_CAL  (0x00000001)
#define SYS_MODE_TEST (0x00000002)



typedef struct{
  int cmd;
  int status;
  int address;
  int data;
  int arg[40];
} PCI_COM_S;



typedef struct
{
  int board;
  int fpga;
  int sw;
}version_info;


typedef struct
{
  int irigb_status;
	int is_gps_time_working;	/* 2007.10.23 add */
  int ref_clk_source;
  int gps_locked;
  int dcm_locked;
  int rx_linked_ltu;
}ctu_info;


typedef struct
{
  int trigger_level;
  int clock1;
  int clock2;
  int clock3;
  int clock4;
  int clock5;
#ifdef FEATURE_HEADER_64_TO_32
  unsigned int offset1_msb;
  unsigned int offset1_lsb;
  unsigned int offset2_msb;
  unsigned int offset2_lsb;
  unsigned int offset3_msb;
  unsigned int offset3_lsb;
  unsigned int offset4_msb;
  unsigned int offset4_lsb;
  unsigned int offset5_msb;
  unsigned int offset5_lsb;
  unsigned int offset6_msb;
  unsigned int offset6_lsb;
#else
  unsigned long long int offset1;
  unsigned long long int offset2;
  unsigned long long int offset3;
  unsigned long long int offset4;
  unsigned long long int offset5;
  unsigned long long int offset6;
#endif  
}MultiTC_info;


typedef struct
{
  int trigger_level;
  int clock;
#ifdef FEATURE_HEADER_64_TO_32
  unsigned int start_offset_msb;
  unsigned int start_offset_lsb;
  unsigned int stop_offset_msb;
  unsigned int stop_offset_lsb;  

#else
  unsigned long long int start_offset;
  unsigned long long int stop_offset;
#endif  
  int rfmen;
  int rfmclock;  
}SingleTC_info;


typedef struct
{
  int id;
  int ref_clk_source;
  int dcm_locked;
  int reserved_0;
  unsigned int compensation_offset1;
  unsigned int compensation_offset2;
  SingleTC_info cts[4];
  MultiTC_info ctm;  
}ltu_info;


typedef struct
{
  int tx_only;
  int rx_link_status;  
  unsigned int good_cnt;
  unsigned int error_cnt;
}serdes_info;


typedef struct
{
  int endian_signal;
  version_info ver_info;
  int single_update;
  int single_update_arm;
  int log_status;
  int reserved_0;
#ifdef FEATURE_HEADER_64_TO_32
  unsigned int ShotTerm_msb;
  unsigned int ShotTerm_lsb;
  unsigned int CurrentTime_msb;
  unsigned int CurrentTime_lsb; 
  unsigned int ShotStartTime_msb;
  unsigned int ShotStartTime_lsb; 
  unsigned int ShotStopTime_msb;
  unsigned int ShotStopTime_lsb;   
#else  
  unsigned long long  int ShotTerm;  
  unsigned long long  int CurrentTime;
  unsigned long long  int ShotStartTime;
  unsigned long long  int ShotStopTime;
#endif  
  int op_mode;
  int reserved_1;  
  serdes_info serdes;
  ctu_info ctu;
  ltu_info ltu;
}System_Info;


#endif /* __NFRC_SYS_H__ */

