#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_


#ifdef __cplusplus
	extern "C" {
#endif


#ifndef WR_ERROR
#define WR_ERROR	0
#endif

#ifndef WR_OK
#define WR_OK		1
#endif

#define PRINT_PHASE_INFO          0
#define PRINT_PMC_INFO          0

#define USE_MANUAL_FIND_PCI   0
#define PCI_BUS_NO     0x10
#define PCI_DEVICE_NO    0x1  /* it should be get from start script */
#define PCI_FUNC_NO     0x0

#define CLTU_VENDOR_ID (0x10B5)
#define CLTU_DEVICE_ID (0x9601)
#if 0  /* old FPGA */
#define CLTU_VENDOR_ID_2 (0x10EE)
#define CLTU_DEVICE_ID_2 (0x0007)
#else
#define CLTU_VENDOR_ID_2 (0x012E)
#define CLTU_DEVICE_ID_2 (0x0540)
#endif

#define VERSION_1    0  /* old CTU, LTU */
#define VERSION_2    1  /* new XTU */


#define MMAP_SIZE_R1 0x8000
#define MMAP_SIZE_R2 0x1000


#define FILE_CALIB_VALUE   ".LTU_"


#define TASK_STATE_GSHOT_ON		(0x1 << 0)
#define TASK_STATE_CALIB_ON		(0x1 << 1)


	
#define PORT_NOT_INIT		0x0000
#define PORT_TRIG_OK  		(0x0001<<0)
#define PORT_CLOCK_OK		(0x0001<<1)
#define PORT_T0_OK  		(0x0001<<2)
#define PORT_T1_OK			(0x0001<<3)

#define ACTIVE_HIGHT_VAL		0




#define T_YEAR_DIVIDER  (365*24*60*60)
#define T_DAY_DIVIDER  (24*60*60)
#define T_HOUR_DIVIDER (60*60)
#define T_MIN_DIVIDER  (60)
#define T_SEC_DIVIDER  (1)


#define READ8(port)   (*((volatile unsigned char*)(port)))
#define READ16(port)  (*((volatile unsigned short int*)(port)))
#define READ32(port)  (*((volatile unsigned int*)(port)))

#define WRITE8( port, val )   (*((volatile unsigned char*)(port))=((unsigned char)(val)))
#define WRITE16( port, val )  (*((volatile unsigned short int*)(port))=((unsigned short int)(val)))
/* #define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(  ((val>>24)&0x000f) | ((val>>16)&0x00f0) | ((val<<16)&0x0f0) | ((val<<24)&0xf000)             )))
*/
#define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(val)))

#define E_SWAP_16(A)    ((((UINT16)(A)&0xff00)>>8)|(((UINT16)(A)&0x00ff)<<8))
#define E_SWAP_32(A)    ((((UINT32)(A)&0xff000000)>>24)|\
                         (((UINT32)(A)&0x00ff0000)>>8)|\
                         (((UINT32)(A)&0x0000ff00)<<8)|\
                         (((UINT32)(A)&0x000000ff)<<24))

enum 
{
    BIT0  = 0x00000001,
    BIT1  = 0x00000002,
    BIT2  = 0x00000004,
    BIT3  = 0x00000008,
    BIT4  = 0x00000010,
    BIT5  = 0x00000020,
    BIT6  = 0x00000040,
    BIT7  = 0x00000080,
    BIT8  = 0x00000100,
    BIT9  = 0x00000200,
    BIT10 = 0x00000400,
    BIT11 = 0x00000800,
    BIT12 = 0x00001000,
    BIT13 = 0x00002000,
    BIT14 = 0x00004000,
    BIT15 = 0x00008000,
    BIT16 = 0x00010000,
    BIT17 = 0x00020000,
    BIT18 = 0x00040000,
    BIT19 = 0x00080000,
    BIT20 = 0x00100000,
    BIT21 = 0x00200000,
    BIT22 = 0x00400000,
    BIT23 = 0x00800000,
    BIT24 = 0x01000000,
    BIT25 = 0x02000000,
    BIT26 = 0x04000000,
    BIT27 = 0x08000000,
    BIT28 = 0x10000000,
    BIT29 = 0x20000000,
    BIT30 = 0x40000000,
    BIT31 = 0x80000000
};

/*****************  2013. 3. 7 **************************************************/
#if 0
#define _1_KILO_ (0x00000400)
#define _1_MEGA_ (0x00100000)
#define _1_GIGA_ (0x40000000)

#define _1KB_    (0x00000400)
#define _2KB_    (0x00000800)
#define _4KB_    (0x00001000)
#define _8KB_    (0x00002000)
#define _16KB_   (0x00004000)
#define _32KB_   (0x00008000)
#define _64KB_   (0x00010000)
#define _128KB_  (0x00020000)
#define _256KB_  (0x00040000)
#define _512KB_  (0x00080000)
#define _1MB_    (0x00100000)
#define _2MB_    (0x00200000)
#define _4MB_    (0x00400000)
#define _8MB_    (0x00800000)
#define _16MB_   (0x01000000)
#define _32MB_   (0x02000000)
#define _64MB_   (0x04000000)
#define _128MB_  (0x08000000)
#define _256MB_  (0x10000000)
#define _512MB_  (0x20000000)
#define _1GB_    (0x40000000)
#define _2GB_    (0x80000000)

#define _CTYPE_TX_ "Tx"
#define _CTYPE_RX_ "Rx"

#define _NULL_         (-1)
#define _UNLINKED_     (0)
#define _LINKED_       (1)
#endif
/*****************  2013. 3. 7 ************************************************* until here*/



#define PRINT_ERR()                                                          \
    printf("[ERROR] file:%s line:%d\n", __FILE__, __LINE__)

#define PRINTF(args)
 #define PRINT_ERR()														  \
   printf("[ERROR] file:%s line:%d\n", __FILE__, __LINE__)


#define NUM_PORT_CNT    8
#define NUM_SECTION_CNT    50
#define EPICS_CLOCK_100M   (100000000)
#define EPICS_CLOCK_200M   (200000000)

 typedef struct
 {
	 unsigned char cOnOff;
	 unsigned char cTrigOnOff;
	 unsigned char cActiveLow;
 
/*	 unsigned long long int startOffset[NUM_SECTION_CNT]; 
	 unsigned long long int stopOffset[NUM_SECTION_CNT];
	 unsigned int  clock[NUM_SECTION_CNT]; */
	 char cFlag0[NUM_SECTION_CNT+3]; /* 2013. 8. 30 */
	 char cFlag1[NUM_SECTION_CNT+3]; /* 2013. 8. 30 */
	 double dT0[NUM_SECTION_CNT+3]; /* 2013. 3. 18 for updated FPGA */
	 double dT1[NUM_SECTION_CNT+3]; /* 2013. 3. 18 for updated FPGA */
	 unsigned int  r2Clock[NUM_SECTION_CNT+3];/* 2013. 3. 19 for updated FPGA */

 } CLTU2_PORT_CONFIG;


 /*****************  2013. 3. 7 **************************************************/
 typedef struct
{
  unsigned int s_time;
  unsigned int e_time;
  unsigned int d_time;
  unsigned int DTP_i;
  unsigned int DTP_f;
  unsigned int DTP_i_Tenth;
  double DTP;
}ozTIME_CHECK_T;

#define MAXNUM_ERRLOGCNT (100)
#define MAXNUM_DTP       (1000)

typedef struct
{
  unsigned int RunCntNum;
  int    ErrRtn;  
}ozCOMERR_LOG1_T;

typedef struct
{
  unsigned int RunCntNum;
  unsigned int DataSize;
  unsigned int DTP;   
}ozCOMDTP_LOG_T;

typedef struct
{
  unsigned int          RunCnt;                   
  unsigned int          OKCnt;                  
  unsigned int          ErrCnt;                  
  int                   ErrLog1[MAXNUM_ERRLOGCNT];
  ozCOMERR_LOG1_T       ErrLog2[MAXNUM_ERRLOGCNT];  
  unsigned int          DTP[MAXNUM_DTP];          /* 10MB/s */
  ozCOMDTP_LOG_T* pDTPLog;
}ozCOMLOG_T;

/*==============================================================================
 Performace Measure Macro
==============================================================================*/
#define TIMEING_CHK( eventNum, val ) { int evtBuf; evtBuf = (int)val; wvEvent(eventNum, (char*)&evtBuf, sizeof(evtBuf));}


/*****************  2013. 3. 7 ************************************************* untill here*/
/******************************************************************************/
/* all contents of below from oz_xtu2.h 

*/
 
#define XTU_DEVICE_DRIVER_VER          (0x03)
#define XTU_DEVICE_DRIVER_VER_REVISION (0x31)
#define XTU_DEVICE_DRIVER_SUB_VER      (0x0C)/*update to 3.31C laykim 2012.03.13 */


#define XTU_VENDOR_ID (0x012E)
#define XTU_DEVICE_ID (0x0540)

#define LittleEndian        0
#define BigEndian           1

#define XTU_MODE_SEL           BIT0
#define XTU_MODE_LTU           0
#define XTU_MODE_CTU           XTU_MODE_SEL

#define XTU_IRIGB_SEL          BIT0
#define XTU_IRIGB_EXT          0
#define XTU_IRIGB_GEN          XTU_IRIGB_SEL

#define XTU_REF_CLK_LOSS       BIT2
#define XTU_TLKRX_CLK_LOSS     BIT1
#define XTU_MCLK_LOCK          BIT0

#define XTU_TLKRX_CLK          BIT8
#define XTU_TLKRX_CLK_DISABLE  0
#define XTU_TLKRX_CLK_ENABLE   XTU_TLKRX_CLK

#define XTU_TLKTX_CLK          BIT4
#define XTU_TLKTX_CLK_ONBOARD  0
#define XTU_TLKTX_CLK_REF      XTU_TLKTX_CLK

#define XTU_MCLK_SEL           (BIT0|BIT1)
#define XTU_MCLK_ONBOARD       0
#define XTU_MCLK_REF           BIT0
#define XTU_MCLK_TLK_RX        BIT1

/*for debug--------------------------------*/
#define XTU_CDC_SEL         BIT1
#define XTU_CDC_SEL_0       0
#define XTU_CDC_SEL_1       XTU_CDC_SEL
#define XTU_JTAG_SEL        BIT0
#define XTU_JTAG_SEL_0      0
#define XTU_JTAG_SEL_1      XTU_JTAG_SEL
/*----------------------------------------- */

#define XTU_TASK_DELAY_VALUE 0


#define XTU_ENDIAN_CHK_VALUE       (0x01234567)
#define XTU_ADDR_ENDIAN_SET        (0x0C)

#define XTU_ADDR_ENDIAN_CHECK      (0x10)
#define XTU_ADDR_CLK_CONFIG        (0x14)
#define XTU_ADDR_DBG_CONFIG        (0x18)
#define XTU_ADDR_PORT_INTR         (0x1C)
#define XTU_ADDR_PORT_ACTIVE_LEVEL (0x24)
#define XTU_ADDR_CLK_STATUS        (0x2C)

#define XTU_BMEM_SIZE              (512) /*512*4byte =2kbyte (18kbit-spartan6 bram size) */

#define XTU_TRX_UDATA_SIZE         (32)

#define XTU_ADDR_SET_LOCAL_TIME    (0x804)
#define XTU_ADDR_SEL_IRIGB         (0x80C)
#define XTU_ADDR_SEL_XTU_MODE      (0x810)
#define XTU_ADDR_SET_SHOT_DELAY    (0x814)
#define XTU_ADDR_TX_SHOT_TIME      (0x820)
#define XTU_ADDR_TX_CURR_TIME      (0x840)
#define XTU_ADDR_TX_COMP_DELAY     (0x84C) 
#define XTU_ADDR_TX_UDATA          (0x880)

#define XTU_ADDR_RX_SHOT_TIME      (0x1020)
#define XTU_ADDR_RX_CURR_TIME      (0x1040)
#define XTU_ADDR_RX_COMP_DELAY     (0x104C)
#define XTU_ADDR_RX_UDATA          (0x1080)


#define XTU_ADDR_WATCH_STATUS      (0x8D0)
#define XTU_ADDR_IDEC_STATUS       (0x8D4)
#define XTU_ADDR_SHOT_STATUS       (0x8D8)

#define XTU_ADDR_SET_CMD           (0x020)

#define XTU_ADDR_CMD               (0x800)
#define XTU_CMD_IMM_START          (BIT0)
#define XTU_CMD_IMM_STOP           (BIT1)
#define XTU_CMD_SET_SHOT_STIME     (BIT2)
#define XTU_CMD_SET_SHOT_ETIME     (BIT3)
#define XTU_CMD_SET_LOCAL_TIME     (BIT4)

#define XTU_ADDR_GSHOT_LOG_PRN     (0x1800)


#define XTU_ADDR_PORTMEM_0         (0x2000)
#define XTU_ADDR_PORTMEM_1         (0x2800)
#define XTU_ADDR_PORTMEM_2         (0x3000)
#define XTU_ADDR_PORTMEM_3         (0x3800)
#define XTU_ADDR_PORTMEM_4         (0x4000)
#define XTU_ADDR_PORTMEM_5         (0x4800)
#define XTU_ADDR_PORTMEM_6         (0x5000)
#define XTU_ADDR_PORTMEM_7         (0x5800)

#define XTU_ADDR_PORTLOG_0         (0x6000)
#define XTU_ADDR_PORTLOG_1         (0x6800)
#define XTU_ADDR_PORTLOG_2         (0x7000)
#define XTU_ADDR_PORTLOG_3         (0x7800)
#define XTU_ADDR_PORTLOG_4         (0x8000)
#define XTU_ADDR_PORTLOG_5         (0x8800)
#define XTU_ADDR_PORTLOG_6         (0x9000)
#define XTU_ADDR_PORTLOG_7         (0x9800)

#define XTU_ADDR_PCTRL             (0x000)
#define XTU_ADDR_PSIZE             (0x010)
#define XTU_ADDR_PHLI_0            (0x010)
#define XTU_ADDR_BRAM_PHLI_0       (0x004)
#define XTU_ADDR_PHLI_MAX_NUM      (   15) /*0~14 */
#define XTU_ADDR_PLLI_0            (0x100)
#define XTU_ADDR_BRAM_PLLI_0       (0x040)
#define XTU_ADDR_PLLI_MAX_NUM      (  112) /* 0~111 */

#define XTU_PORT_ENABLE            (BIT31)
#define XTU_PORT_DISABLE           (0)

#define XTU_P0_ACT_H     (0)
#define XTU_P1_ACT_H     (0)
#define XTU_P2_ACT_H     (0)
#define XTU_P3_ACT_H     (0)
#define XTU_P4_ACT_H     (0)
#define XTU_P5_ACT_H     (0)
#define XTU_P6_ACT_H     (0)
#define XTU_P7_ACT_H     (0)

#define XTU_P0_ACT_L     (BIT0)
#define XTU_P1_ACT_L     (BIT1)
#define XTU_P2_ACT_L     (BIT2)
#define XTU_P3_ACT_L     (BIT3)
#define XTU_P4_ACT_L     (BIT4)
#define XTU_P5_ACT_L     (BIT5)
#define XTU_P6_ACT_L     (BIT6)
#define XTU_P7_ACT_L     (BIT7)


#define XTU_PORT_CLK_OPT_DISABLE       (0)
#define XTU_PORT_CLK_OPT_ENABLE      (BIT29)
#define XTU_PORT_CLK_OPT_50MHz     (BIT30)
#define XTU_PORT_CLK_OPT_100MHz    (BIT31)
#define XTU_PORT_CLK_OPT_VARFREQ   (BIT31 | BIT30)

#define XTU_PORT_LLI_OPT_LOG_ON    (BIT28)
#define XTU_PORT_LLI_OPT_LOG_OFF   (0)

#define XTU_SHOT_DEFAULT_DELAY_TICK 100000


#define XTU_1MEGA       (1000000)
#define XTU_100MHz      (100000000)
#define XTU_200MHz      (200000000)


#define XTU_MAX_INF_NS         10485750 
#define XTU_MAX_200M_Tick      199999 
#define XTU_MAX_200M_ns        999995 
#define XTU_MAX_100M_ns        999990
#define XTU_MAX_ms             999
#define XTU_MAX_sec            59
#define XTU_MAX_min            59
#define XTU_MAX_hour           23
#define XTU_MAX_day            364

#define XTU_1000000ns          1000000
#define XTU_1000ms             1000
#define XTU_60sec              60
#define XTU_60min              60
#define XTU_24hour             24
#define XTU_365day             365


#define NUM_END_HLI_14      (   14) /*0~14 , woong add */
#define NUM_END_LLI_111      (  111) /*0~111, woong */



typedef struct
{
  unsigned int init;
  int BusNo;
  int DeviceNo;
  int FuncNo;  
  unsigned int base0;
  unsigned int base1;
  unsigned int base2;
  unsigned int base3;
  unsigned int base4;
  unsigned char int_line;
/*  SEM_ID        int_sem; */
  unsigned char int_count;
  unsigned char int_error;  
}ozXTU_PCI_T;

typedef	struct
{
  unsigned int vendor_id;
  unsigned int device_id;
  unsigned int version;  
} ozXTU_INFO_T;

typedef	struct
{
  unsigned int irigb_sel;
  unsigned int mode;
  unsigned int tlkRxClk_en;
  unsigned int tlkTxClk_sel;
  unsigned int mclk_sel;
} ozXTU_MODE_T;

typedef	struct
{
  unsigned int cdc_sel;
  unsigned int jtag_sel;
} ozXTU_DBGM_T; 


typedef	struct
{
  unsigned int xtu_endian;
  ozXTU_MODE_T xm;
  unsigned int xtu_ref_clk_loss;
  unsigned int xtu_trx_clk_loss;
  unsigned int xtu_mclk_lock;
  ozXTU_DBGM_T dbg;
} ozXTU_STT_T;

typedef	struct
{
  unsigned int  day;
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
  unsigned int  ms;    /* 0~999 */
  unsigned int  ns;  /* 0~99999 */
} ozTIME_T;


typedef	struct
{
  ozTIME_T start;
  ozTIME_T stop;
} ozSHOT_TIME_T;

typedef	struct
{
  ozTIME_T      curr;
  ozTIME_T      comp;
  ozSHOT_TIME_T t_shot;
} ozXTU_TIME_T;


typedef	struct
{
  ozTIME_T     intv;
  unsigned int nhli_num;
  unsigned int nlli_num;
} ozPCONFIG_HLI_T;


typedef	struct
{
  ozTIME_T     intv;
  unsigned int clk_freq;
  unsigned int option;
  unsigned int nlli_num;
} ozPCONFIG_LLI_T;



typedef	struct
{
  unsigned int     port_en;
  unsigned int     nhli_num;  
  ozPCONFIG_HLI_T  phli[XTU_ADDR_PHLI_MAX_NUM];
  ozPCONFIG_LLI_T  plli[XTU_ADDR_PLLI_MAX_NUM];  
} ozPCONFIG_T;



#ifdef __cplusplus
}
#endif 
#endif



