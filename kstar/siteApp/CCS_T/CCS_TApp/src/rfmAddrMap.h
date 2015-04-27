#ifndef _RFM_ADDR_MAP_H_
#define _RFM_ADDR_MAP_H_

/*
2013. 5. 27.   created by woong.
2013. 6. 13.   structure modified for reserved item.
2013. 7. 11.   modify rtscope and add PCS faults.
2013. 7. 16    PCS fault notify map changed.



*/



/*********************************************************************************/
/* CCS managed address                     */


#define RFM_BASE_ADDR         0x95fe0000

#define RFM_PCS_TO_PF    0x100
#define RFM_PCS_TO_IVC   0x190

#define RFM_ID_PF1    0x300
#define RFM_ID_PF2    0x380
#define RFM_ID_PF3U   0x400
#define RFM_ID_PF3L   0x440
#define RFM_ID_PF4U   0x480
#define RFM_ID_PF4L   0x4C0
#define RFM_ID_PF5U   0x500
#define RFM_ID_PF5L   0x540
#define RFM_ID_PF6U   0x580
#define RFM_ID_PF6L   0x5C0
#define RFM_ID_PF7    0x600
#define RFM_ID_TF     0x680
#define RFM_ID_IVC        0x6A0  /* 0x95fe06A0  */
#define RFM_CCS_SHARE     0x3800000  /* 0x997e0000, start at 56MB */
#define RFM_CCS_TO_PCS     0x3810000  
#define RFM_PCS_RTSCOPE   0x4000000  /* 0x99fe0000, start at 64MB */

#define  MAGIC_ID_PCS_PF     (0x01ab01ab)
#define  MAGIC_ID_PCS_IVC    (0x02ab02ab)
#define  MAGIC_ID_PF1      (0x01ab0001)
#define  MAGIC_ID_PF2      (0x01ab0003)
#define  MAGIC_ID_PF3U     (0x01ab0005)
#define  MAGIC_ID_PF3L     (0x01ab0006)
#define  MAGIC_ID_PF4U     (0x01ab0007)
#define  MAGIC_ID_PF4L     (0x01ab0008)
#define  MAGIC_ID_PF5U     (0x01ab0009)
#define  MAGIC_ID_PF5L     (0x01ab000a)
#define  MAGIC_ID_PF6U     (0x01ab000b)
#define  MAGIC_ID_PF6L     (0x01ab000c)
#define  MAGIC_ID_PF7      (0x01ab000d)
#define  MAGIC_ID_TF       (0x01ab000f)
#define  MAGIC_ID_IVC      (0x01cd0001)
#define MAGIC_ID_CCS_SHARE        (0x01abfe01)
#define MAGIC_ID_PCS_RTSCOPE      (0xabdcabdc)

#define RT_CRASH_CNT   4

#define WATCHDOG_INTV_10HZ    (.1)  /* 10Hz*/
#define WATCHDOG_INTV         (.01)  /* 100Hz*/
#define WATCHDOG_INTV_FAST    (.002) /* 500Hz */
#define MEASUREMENT_INTV      (.01)  /* 100Hz*/
#define MONITOR_INTV          (.2)   /* 5Hz */

#define  RT_NORMAL            0
#define  RT_ABNORMAL          1
#define  WATCHDOG_STOP        0
#define  WATCHDOG_RUN         1



/*********************************************************************************/
/* RMP related address                     */
#define RM_RMP_TOP_MODE	0x6ec
#define RM_RMP_MID_MODE	0x6fc
#define RM_RMP_BTM_MODE	0x70c

#define RM_RMP_TOP_I	0x6e8
#define RM_RMP_MID_I	0x6f8
#define RM_RMP_BTM_I	0x708

#define RM_RMP_TOP_V    0x6e4
#define RM_RMP_MID_V    0x6f4
#define RM_RMP_BTM_V    0x704

#define RM_RMP_ICMD_TOP   0x1bc
#define RM_RMP_ICMD_MID   0x1c0
#define RM_RMP_ICMD_BTM   0x1c4
/*********************************************************************************/
/*
#define PCS_FAULT_IP_MINIMUM       (0x00000001<<0)
#define PCS_FAULT_NE_ERROR         (0x00000001<<1)
#define PCS_FAULT_PF_ERROR         (0x00000001<<2)
#define PCS_FAULT_PF_OVERCURRENT   (0x00000001<<3)
#define PCS_FAULT_PF_VOLTAGE_ERROR (0x00000001<<4)
#define PCS_FAULT_PF_OVERVOLTAGE   (0x00000001<<5)
#define PCS_FAULT_EXT_FAULT        (0x00000001<<6)
#define PCS_FAULT_PS_FAULT         (0x00000001<<7)
#define PCS_FAULT_IP_ERROR         (0x00000001<<8)
#define PCS_FAULT_IC_OVERCURRENT   (0x00000001<<9)
*/
#define RM_PCS_FAULT_CODE      0x3800008
#define RM_PCS_FAST_INTLOCK    0x3800010
#define RM_PCS_FORCE_ABORT     0x3810000
#define RM_PCS_PFC_FAULT       0x3810004



#define RM_PF1_I_VAL     0x308
#define RM_PF2_I_VAL     0x388
#define RM_PF3U_I_VAL    0x408
#define RM_PF3L_I_VAL    0x448
#define RM_PF4U_I_VAL    0x488
#define RM_PF4L_I_VAL    0x4c8
#define RM_PF5U_I_VAL    0x508
#define RM_PF5L_I_VAL    0x548
#define RM_PF6U_I_VAL    0x588
#define RM_PF6L_I_VAL    0x5c8
#define RM_PF7_I_VAL     0x608

#define RM_PF1_V_VAL     0x304
#define RM_PF2_V_VAL     0x384
#define RM_PF3U_V_VAL    0x404
#define RM_PF3L_V_VAL    0x444
#define RM_PF4U_V_VAL    0x484
#define RM_PF4L_V_VAL    0x4c4
#define RM_PF5U_V_VAL    0x504
#define RM_PF5L_V_VAL    0x544
#define RM_PF6U_V_VAL    0x584
#define RM_PF6L_V_VAL    0x5c4
#define RM_PF7_V_VAL     0x604


/*********************************************************************************/
/* ECCD control related address                     */
/* 2013. 4. 12 */
#define RM_ECCD_x_A	0x2000008
#define RM_ECCD_x_B 	0x200000c
#define RM_ECCD_x_C 	0x2000010




/*********************************************************************************/
/* RT-SCOPE control related address                     */
/* 2013. 6. 28 */
#define RM_SCOPE_ID                  0x04000000
#define RM_SCOPE_BUF_INDEX     0x4000034
#define RM_SCOPE_BUF_DATA      0x4002114 

/*********************************************************************************/


#define RM_PCS_CNT    0x10c
#define RM_PF1_CNT    0x30c
#define RM_PF2_CNT    0x38c
#define RM_PF3U_CNT    0x40c
#define RM_PF3L_CNT    0x44c
#define RM_PF4U_CNT    0x48c
#define RM_PF4L_CNT    0x4cc
#define RM_PF5U_CNT    0x50c
#define RM_PF5L_CNT    0x54c
#define RM_PF6U_CNT    0x58c
#define RM_PF6L_CNT    0x5cc
#define RM_PF7_CNT    0x60c
#define RM_IVC_CNT    0x6b0




typedef struct
{
	volatile int    id;
	volatile int    control_method;
	volatile int    current_direction;
	volatile unsigned int    comm_counter;
	volatile int    rt_mode;
	volatile int    pf_command[14]; /*  11 -> 14   2010.6.14 woong */
	volatile int    testvar;
	volatile int    pf_current_traj[14]; /*  new   2010.6.14 woong */
} PCS_cmd_PF;

typedef struct
{
	volatile int    id;
	volatile int    control_method;
	volatile int    current_type;
	volatile unsigned int    comm_IVC;
	volatile int    current_target_IVC;
	volatile int    command_IRC;
	volatile int    current_target_IRC;
} PCS_cmd_IVC;


#define PCS_S_V_NCPUS      10
#define PCS_S_V_RINGSIZE   1000
#define PCS_S_V_NSIGNALS   50
#define PCS_S_SIGNAMLEN    16

typedef struct 
{
	volatile int     id;
	volatile int     status;
	volatile int     cput_status[PCS_S_V_NCPUS];
	volatile int     shotnumber;
	volatile int     scope_buffer_index;
	volatile int     last_update_time;
	volatile int     update_frequency;
	volatile int     bdry_number;
	volatile int     scope_bdry_xy[800];
	volatile int     scope_sig_names[200];
	volatile int     scope_y_spans[PCS_S_V_NSIGNALS][2];
	volatile int     scope_buffer_time[1000];
	volatile float   scope_buffer_data[PCS_S_V_NSIGNALS][PCS_S_V_RINGSIZE];
	volatile int     grid_nw;
	volatile int     scope_psi_end[2];
	volatile int     scope_psi_midplane[256];	
} PCS_RTscope;


/* 0x3800000 */
typedef struct
{
	volatile int         id;
	volatile unsigned    pcs_current_time;
	volatile int         pcs_fault_code;
/*	volatile int         force_pcs_abort;  move to CCS_to_PCS */
	volatile int         dummy; /* removed 2013. 7. 12 */
	volatile int         force_fast_interlock; /* 2012. 8. 1 request by shhan */
/*	volatile int         pcs_pfc_fault;*/ /* 2013. 6. 20 request by hshan */    /* removed 2013. 7. 12 */
} CCS_from_PCS;

/* 0x3810000 */
typedef struct
{
	volatile int         force_pcs_abort; /* used as the  External fault for NB armor interlock  */
	volatile int         pcs_pfc_fault; /* 2013. 6. 20 request by hshan */    
} CCS_to_PCS;


typedef struct
{
	volatile int       id;
	volatile int       vpf_measured;   /* scale 1000:32767 */
	volatile int       ipf_measured;   /* scale 1000:32767 */
	volatile unsigned int   comm_counter;
	volatile int       ipf_delta_measured;  /* new 2010.6.23 */
	volatile int       ipf_wye_measured;  /* new 2010.6.23 */
	volatile int       ipf_limit_flag;
	volatile int       alpha_delta_measured;  /* scale 1/100 */  /* name changed (alpha_measured) */
	volatile int       alpha_wye_measured;       /* new 2010.6.23 */
	volatile int       vqp_measured;   /* scale 1:1 */
	volatile int       vbris_measured; /* scale 1:1 */
	volatile int       vpf_delta_measured;   /* new 2010.6.23 */
	volatile int       vpf_wye_measured;    /* new 2010.6.23 */
	volatile int       reserved[3];    /* new 2013.6.12 */ 
} PF_return;

typedef struct
{
	volatile int       id;
	volatile int       itf_measured;          /* scale 1:1 */
	volatile int       vtf_measured;          /* scale 1:100 */
	volatile int       vbris_measured;        /* scale 1:100 */  /* slow_discharge_volt */
	volatile int       vqp_measured;          /* scale 1:1 */  /* fast_discharge_volt */
	volatile int       reserved[3];
} TF_return;


typedef struct
{
	volatile int       id;
	volatile int       voltage_measured; 
	volatile int       current_measured;
	volatile int       vdclink_measured;
	volatile unsigned int  comm_counter;    
	volatile int       ivc_limit_flag;
	volatile int       reserved[2];
} IVC_return;

typedef struct
{
	volatile int       id_T;
	volatile int       voltage_T; 
	volatile int       current_T;
	volatile int       connection_T;
	volatile int       id_M;
	volatile int       voltage_M; 
	volatile int       current_M;
	volatile int       connection_M;
	volatile int       id_B;
	volatile int       voltage_B; 
	volatile int       current_B;
	volatile int       connection_B;
} RMP_return;




















#endif

