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


#define VERSION_1    0  /* old CTU, LTU */
#define VERSION_2    1  /* new XTU */


#define MMAP_SIZE_R1 0x8000
#define MMAP_SIZE_R2 0x1000


#define FILE_CALIB_VALUE   ".LTU_"


#define TASK_FINISH  1
	
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

#define PRINT_ERR()                                                          \
    printf("[ERROR] file:%s line:%d\n", __FILE__, __LINE__)

#define PRINTF(args)
 #define PRINT_ERR()														  \
   printf("[ERROR] file:%s line:%d\n", __FILE__, __LINE__)


#define NUM_PORT_CNT    8
#define NUM_SECTION_CNT    8
#define EPICS_CLOCK_100M   (100000000)
#define EPICS_CLOCK_200M   (200000000)

 typedef struct
 {
	 unsigned char cOnOff;
	 unsigned char cTrigOnOff;
	 unsigned char cActiveLow;
 
	 unsigned long long int startOffset[NUM_SECTION_CNT];
	 unsigned long long int stopOffset[NUM_SECTION_CNT];
	 unsigned int  clock[NUM_SECTION_CNT];
   
 } CLTU2_PORT_CONFIG;
 

#define LittleEndian 0
#define BigEndian    1


#define XTU_DEVICE_DRIVER_VER          (0x02)
#define XTU_DEVICE_DRIVER_VER_REVISION (0x21)
#define XTU_DEVICE_DRIVER_SUB_VER      (0x0C)


#if 0
#define __XTU_PCI_WRITE_DELAY__
#ifdef __XTU_PCI_WRITE_DELAY__
  #define _XTU_TASK_DELAY(args) taskDelay(args)
#else
  #define _XTU_TASK_DELAY(args) 
#endif
#endif

#define XTU_READ_CTU_MODE   (BIT28)

#define XTU_MODE_CTU        (0)  
#define XTU_MODE_CTU_CMPST  (1)  /*ctu-ltu compensation mode */
#define XTU_MODE_LTU        (2)  /*ltu stand-alone mode*/
#define XTU_MODE_LTU_FREE   (BIT27)  /*ltu stand-alone mode*/






#define XTU_MODE_SELECT_CTU   (BIT31)
#define XTU_MODE_SELECT_LTU   (0)

#define XTU_CLK_SEL_ONB       (BIT28)
#define XTU_CLK_SEL_REF       (0)
#define XTU_CLK_SEL_SDRX      (0)

#define XTU_SERDES_TX_ONLY    (BIT31)
#define XTU_SERDES_LOOPTEST   (BIT29)

#define XTU_IRIG_B_STATUS     (BIT31)
#define XTU_REF_CLK_STATUS    (BIT30)
#define XTU_FOT_RxLINK_STATUS (BIT29)

#define PORT_ENABLE           (0x08)
#define PORT_ACTIVEH          (0x04)
#define PORT_TRIGGERON        (0x02)

#define TICK_SEC              (0x0BEBC1FF)



#define XTUR000_ENDIAN      (0x00000000)      /*     slv_reg_000  */
#define XTUR001_XTU_CONF     (0x00000004)      /*     slv_reg_001*/
#define XTUR002_            (0x00000008)      /*     slv_reg_002*/
#define XTUR003_P_CNTRst    (0x0000000C)      /*     slv_reg_003*/
#define XTUR004_P_Config    (0x00000010)      /*     slv_reg_004*/
#define XTUR005_XTUDelay    (0x00000014)      /*     slv_reg_005*/
#define XTUR006_               (0x00000018)      /*     slv_reg_006*/
#define XTUR007_               (0x0000001C)      /*     slv_reg_007*/
#define XTUR008_TSSTM       (0x00000020)      /*     slv_reg_008*/
#define XTUR009_TSSTL       (0x00000024)      /*     slv_reg_009*/
#define XTUR00A_TSETM       (0x00000028)      /*     slv_reg_00A*/
#define XTUR00B_TSETL       (0x0000002C)      /*     slv_reg_00B*/
#define XTUR00C_               (0x00000030)      /*     slv_reg_00C*/
#define XTUR00D_               (0x00000034)      /*     slv_reg_00D*/
#define XTUR00E_               (0x00000038)      /*     slv_reg_00E*/
#define XTUR00F_               (0x0000003C)      /*     slv_reg_00F*/
#define XTUR010_     (0x00000040)      /*     slv_reg_010            */ 
#define XTUR011_     (0x00000044)      /*     slv_reg_011             */
#define XTUR012_     (0x00000048)      /*     slv_reg_012             */
#define XTUR013_     (0x0000004C)      /*     slv_reg_013             */
#define XTUR014_     (0x00000050)      /*     slv_reg_014             */
#define XTUR015_     (0x00000054)      /*     slv_reg_015             */
#define XTUR016_     (0x00000058)      /*     slv_reg_016             */
#define XTUR017_     (0x0000005C)      /*     slv_reg_017             */
#define XTUR018_     (0x00000060)      /*     slv_reg_018*/
#define XTUR019_  (0x00000064)      /*     slv_reg_019*/
#define XTUR01A_  (0x00000068)      /*     slv_reg_01A*/
#define XTUR01B_  (0x0000006C)      /*     slv_reg_01B*/
#define XTUR01C_  (0x00000070)      /*     slv_reg_01C*/
#define XTUR01D_  (0x00000074)      /*     slv_reg_01D*/
#define XTUR01E_  (0x00000078)      /*     slv_reg_01E*/
#define XTUR01F_  (0x0000007C)      /*     slv_reg_01F */
#define XTUR020_ENDIAN_STT  (0x00000080)      /*     slv_reg_020 */
#define XTUR021_COMP_STT    (0x00000084)      /*     slv_reg_021 */
#define XTUR022_               (0x00000088)      /*     slv_reg_022 */
#define XTUR023_               (0x0000008C)      /*     slv_reg_023 */
#define XTUR024_               (0x00000090)      /*     slv_reg_024 */
#define XTUR025_               (0x00000094)      /*     slv_reg_025 */
#define XTUR026_               (0x00000098)      /*     slv_reg_026 */
#define XTUR027_               (0x0000009C)      /*     slv_reg_027 */
#define XTUR028_IR_ULCNT    (0x000000A0)      /*     slv_reg_028 */
#define XTUR029_SD_ULCNT    (0x000000A4)      /*     slv_reg_029 */
#define XTUR02A_SD_ERENCNT  (0x000000A8)      /*     slv_reg_02A */
#define XTUR02B_               (0x000000AC)      /*     slv_reg_02B */
#define XTUR02C_               (0x000000B0)      /*     slv_reg_02C */
#define XTUR02D_               (0x000000B4)      /*     slv_reg_02D */
#define XTUR02E_               (0x000000B8)      /*     slv_reg_02E */
#define XTUR02F_               (0x000000BC)      /*     slv_reg_02F */
#define XTUR030_BCD_TIME    (0x000000C0)      /*     slv_reg_030 */
#define XTUR031_BCD_DAY     (0x000000C4)      /*     slv_reg_031 */
#define XTUR032_CTM         (0x000000C8)      /*     slv_reg_032 */
#define XTUR033_CTL         (0x000000CC)      /*     slv_reg_033 */
#define XTUR034_SD_RSSTM    (0x000000D0)      /*     slv_reg_034 */
#define XTUR035_SD_RSSTL    (0x000000D4)      /*     slv_reg_035 */
#define XTUR036_SD_RSETM    (0x000000D8)      /*     slv_reg_036 */
#define XTUR037_SD_RSETL    (0x000000DC)      /*     slv_reg_037 */
#define XTUR038_IR_CTM      (0x000000E0)      /*     slv_reg_038 */
#define XTUR039_IR_CTL      (0x000000E4)      /*     slv_reg_039 */
#define XTUR03A_MAINCLK_CNT (0x000000E8)      /*     slv_reg_03A */
#define XTUR03B_IR_DE_CNT   (0x000000EC)      /*     slv_reg_03B */
#define XTUR03C_SD_DE_CNT   (0x000000F0)      /*     slv_reg_03C */
#define XTUR03D_SSF_CNT     (0x000000F4)      /*     slv_reg_03D */
#define XTUR03E_            (0x000000F8)      /*     slv_reg_03E */
#define XTUR03F_            (0x000000FC)      /*     slv_reg_03F */
#define XTUR040_            (0x00000100)      /*     slv_reg_040 */
#define XTUR041_            (0x00000104)      /*     slv_reg_041 */
#define XTUR042_            (0x00000108)      /*     slv_reg_042 */
#define XTUR043_            (0x0000010C)      /*     slv_reg_043 */
#define XTUR044_            (0x00000110)      /*     slv_reg_044 */
#define XTUR045_            (0x00000114)      /*     slv_reg_045 */
#define XTUR046_            (0x00000118)      /*     slv_reg_046 */
#define XTUR047_            (0x0000011C)      /*     slv_reg_047 */
#define XTUR048_            (0x00000120)      /*     slv_reg_048 */
#define XTUR049_            (0x00000124)      /*     slv_reg_049 */
#define XTUR04A_            (0x00000128)      /*     slv_reg_04A */
#define XTUR04B_            (0x0000012C)      /*     slv_reg_04B */
#define XTUR04C_            (0x00000130)      /*     slv_reg_04C */
#define XTUR04D_            (0x00000134)      /*     slv_reg_04D */
#define XTUR04E_            (0x00000138)      /*     slv_reg_04E */
#define XTUR04F_            (0x0000013C)      /*     slv_reg_04F */
#define XTUR050_            (0x00000140)      /*     slv_reg_050 */
#define XTUR051_            (0x00000144)      /*     slv_reg_051 */
#define XTUR052_            (0x00000148)      /*     slv_reg_052 */
#define XTUR053_            (0x0000014C)      /*     slv_reg_053 */
#define XTUR054_            (0x00000150)      /*     slv_reg_054 */
#define XTUR055_            (0x00000154)      /*     slv_reg_055 */
#define XTUR056_            (0x00000158)      /*     slv_reg_056 */
#define XTUR057_            (0x0000015C)      /*     slv_reg_057 */
#define XTUR058_            (0x00000160)      /*     slv_reg_058 */
#define XTUR059_            (0x00000164)      /*     slv_reg_059 */
#define XTUR05A_            (0x00000168)      /*     slv_reg_05A */
#define XTUR05B_            (0x0000016C)      /*     slv_reg_05B */
#define XTUR05C_            (0x00000170)      /*     slv_reg_05C */
#define XTUR05D_            (0x00000174)      /*     slv_reg_05D */
#define XTUR05E_            (0x00000178)      /*     slv_reg_05E */
#define XTUR05F_            (0x0000017C)      /*     slv_reg_05F */
#define XTUR060_            (0x00000180)      /*     slv_reg_060 */
#define XTUR061_            (0x00000184)      /*     slv_reg_061 */
#define XTUR062_            (0x00000188)      /*     slv_reg_062 */
#define XTUR063_            (0x0000018C)      /*     slv_reg_063 */
#define XTUR064_            (0x00000190)      /*     slv_reg_064 */
#define XTUR065_            (0x00000194)      /*     slv_reg_065 */
#define XTUR066_            (0x00000198)      /*     slv_reg_066 */
#define XTUR067_            (0x0000019C)      /*     slv_reg_067 */
#define XTUR068_            (0x000001A0)      /*     slv_reg_068 */
#define XTUR069_            (0x000001A4)      /*     slv_reg_069 */
#define XTUR06A_            (0x000001A8)      /*     slv_reg_06A */
#define XTUR06B_            (0x000001AC)      /*     slv_reg_06B */
#define XTUR06C_            (0x000001B0)      /*     slv_reg_06C */
#define XTUR06D_            (0x000001B4)      /*     slv_reg_06D */
#define XTUR06E_            (0x000001B8)      /*     slv_reg_06E */
#define XTUR06F_            (0x000001BC)      /*     slv_reg_06F */
#define XTUR070_            (0x000001C0)      /*     slv_reg_070 */
#define XTUR071_            (0x000001C4)      /*     slv_reg_071 */
#define XTUR072_            (0x000001C8)      /*     slv_reg_072 */
#define XTUR073_            (0x000001CC)      /*     slv_reg_073 */
#define XTUR074_            (0x000001D0)      /*     slv_reg_074 */
#define XTUR075_            (0x000001D4)      /*     slv_reg_075 */
#define XTUR076_            (0x000001D8)      /*     slv_reg_076 */
#define XTUR077_            (0x000001DC)      /*     slv_reg_077 */
#define XTUR078_            (0x000001E0)      /*     slv_reg_078 */
#define XTUR079_            (0x000001E4)      /*     slv_reg_079 */
#define XTUR07A_            (0x000001E8)      /*     slv_reg_07A */
#define XTUR07B_            (0x000001EC)      /*     slv_reg_07B */
#define XTUR07C_            (0x000001F0)      /*     slv_reg_07C */
#define XTUR07D_            (0x000001F4)      /*     slv_reg_07D */
#define XTUR07E_            (0x000001F8)      /*     slv_reg_07E */
#define XTUR07F_            (0x000001FC)      /*     slv_reg_07F */
#define XTUR080_            (0x00000200)      /*     slv_reg_080 */
#define XTUR081_            (0x00000204)      /*     slv_reg_081 */
#define XTUR082_            (0x00000208)      /*     slv_reg_082 */
#define XTUR083_            (0x0000020C)      /*     slv_reg_083 */
#define XTUR084_            (0x00000210)      /*     slv_reg_084 */
#define XTUR085_            (0x00000214)      /*     slv_reg_085 */
#define XTUR086_            (0x00000218)      /*     slv_reg_086 */
#define XTUR087_            (0x0000021C)      /*     slv_reg_087 */
#define XTUR088_            (0x00000220)      /*     slv_reg_088 */
#define XTUR089_            (0x00000224)      /*     slv_reg_089 */
#define XTUR08A_            (0x00000228)      /*     slv_reg_08A */
#define XTUR08B_            (0x0000022C)      /*     slv_reg_08B */
#define XTUR08C_            (0x00000230)      /*     slv_reg_08C */
#define XTUR08D_            (0x00000234)      /*     slv_reg_08D */
#define XTUR08E_            (0x00000238)      /*     slv_reg_08E */
#define XTUR08F_            (0x0000023C)      /*     slv_reg_08F */
#define XTUR090_            (0x00000240)      /*     slv_reg_090 */
#define XTUR091_            (0x00000244)      /*     slv_reg_091 */
#define XTUR092_            (0x00000248)      /*     slv_reg_092 */
#define XTUR093_            (0x0000024C)      /*     slv_reg_093 */
#define XTUR094_            (0x00000250)      /*     slv_reg_094 */
#define XTUR095_            (0x00000254)      /*     slv_reg_095 */
#define XTUR096_            (0x00000258)      /*     slv_reg_096 */
#define XTUR097_            (0x0000025C)      /*     slv_reg_097 */
#define XTUR098_            (0x00000260)      /*     slv_reg_098 */
#define XTUR099_            (0x00000264)      /*     slv_reg_099 */
#define XTUR09A_            (0x00000268)      /*     slv_reg_09A */
#define XTUR09B_            (0x0000026C)      /*     slv_reg_09B */
#define XTUR09C_            (0x00000270)      /*     slv_reg_09C */
#define XTUR09D_            (0x00000274)      /*     slv_reg_09D */
#define XTUR09E_            (0x00000278)      /*     slv_reg_09E */
#define XTUR09F_            (0x0000027C)      /*     slv_reg_09F */
#define XTUR0A0_            (0x00000280)      /*     slv_reg_0A0 */
#define XTUR0A1_            (0x00000284)      /*     slv_reg_0A1 */
#define XTUR0A2_            (0x00000288)      /*     slv_reg_0A2 */
#define XTUR0A3_            (0x0000028C)      /*     slv_reg_0A3 */
#define XTUR0A4_            (0x00000290)      /*     slv_reg_0A4 */
#define XTUR0A5_            (0x00000294)      /*     slv_reg_0A5 */
#define XTUR0A6_            (0x00000298)      /*     slv_reg_0A6 */
#define XTUR0A7_            (0x0000029C)      /*     slv_reg_0A7 */
#define XTUR0A8_            (0x000002A0)      /*     slv_reg_0A8 */
#define XTUR0A9_            (0x000002A4)      /*     slv_reg_0A9 */
#define XTUR0AA_            (0x000002A8)      /*     slv_reg_0AA */
#define XTUR0AB_            (0x000002AC)      /*     slv_reg_0AB */
#define XTUR0AC_            (0x000002B0)      /*     slv_reg_0AC */
#define XTUR0AD_            (0x000002B4)      /*     slv_reg_0AD */
#define XTUR0AE_            (0x000002B8)      /*     slv_reg_0AE */
#define XTUR0AF_            (0x000002BC)      /*     slv_reg_0AF */
#define XTUR0B0_            (0x000002C0)      /*     slv_reg_0B0 */
#define XTUR0B1_            (0x000002C4)      /*     slv_reg_0B1 */
#define XTUR0B2_            (0x000002C8)      /*     slv_reg_0B2 */
#define XTUR0B3_            (0x000002CC)      /*     slv_reg_0B3 */
#define XTUR0B4_            (0x000002D0)      /*     slv_reg_0B4 */
#define XTUR0B5_            (0x000002D4)      /*     slv_reg_0B5 */
#define XTUR0B6_            (0x000002D8)      /*     slv_reg_0B6 */
#define XTUR0B7_            (0x000002DC)      /*     slv_reg_0B7 */
#define XTUR0B8_            (0x000002E0)      /*     slv_reg_0B8 */
#define XTUR0B9_            (0x000002E4)      /*     slv_reg_0B9 */
#define XTUR0BA_            (0x000002E8)      /*     slv_reg_0BA */
#define XTUR0BB_            (0x000002EC)      /*     slv_reg_0BB */
#define XTUR0BC_            (0x000002F0)      /*     slv_reg_0BC */
#define XTUR0BD_            (0x000002F4)      /*     slv_reg_0BD */
#define XTUR0BE_            (0x000002F8)      /*     slv_reg_0BE */
#define XTUR0BF_            (0x000002FC)      /*     slv_reg_0BF */
#define XTUR0C0_            (0x00000300)      /*     slv_reg_0C0 */
#define XTUR0C1_            (0x00000304)      /*     slv_reg_0C1 */
#define XTUR0C2_            (0x00000308)      /*     slv_reg_0C2 */
#define XTUR0C3_            (0x0000030C)      /*     slv_reg_0C3 */
#define XTUR0C4_            (0x00000310)      /*     slv_reg_0C4 */
#define XTUR0C5_            (0x00000314)      /*     slv_reg_0C5 */
#define XTUR0C6_            (0x00000318)      /*     slv_reg_0C6 */ 
#define XTUR0C7_            (0x0000031C)      /*     slv_reg_0C7 */
#define XTUR0C8_            (0x00000320)      /*     slv_reg_0C8 */
#define XTUR0C9_            (0x00000324)      /*     slv_reg_0C9 */
#define XTUR0CA_            (0x00000328)      /*     slv_reg_0CA */
#define XTUR0CB_            (0x0000032C)      /*     slv_reg_0CB */
#define XTUR0CC_            (0x00000330)      /*     slv_reg_0CC */
#define XTUR0CD_            (0x00000334)      /*     slv_reg_0CD */
#define XTUR0CE_            (0x00000338)      /*     slv_reg_0CE */
#define XTUR0CF_            (0x0000033C)      /*     slv_reg_0CF */
#define XTUR0D0_            (0x00000340)      /*     slv_reg_0D0 */
#define XTUR0D1_            (0x00000344)      /*     slv_reg_0D1 */
#define XTUR0D2_            (0x00000348)      /*     slv_reg_0D2 */
#define XTUR0D3_            (0x0000034C)      /*     slv_reg_0D3 */
#define XTUR0D4_            (0x00000350)      /*     slv_reg_0D4 */
#define XTUR0D5_            (0x00000354)      /*     slv_reg_0D5 */
#define XTUR0D6_            (0x00000358)      /*     slv_reg_0D6 */ 
#define XTUR0D7_            (0x0000035C)      /*     slv_reg_0D7 */
#define XTUR0D8_            (0x00000360)      /*     slv_reg_0D8 */
#define XTUR0D9_            (0x00000364)      /*     slv_reg_0D9 */
#define XTUR0DA_            (0x00000368)      /*     slv_reg_0DA */
#define XTUR0DB_            (0x0000036C)      /*     slv_reg_0DB */
#define XTUR0DC_            (0x00000370)      /*     slv_reg_0DC */
#define XTUR0DD_            (0x00000374)      /*     slv_reg_0DD */
#define XTUR0DE_            (0x00000378)      /*     slv_reg_0DE */
#define XTUR0DF_            (0x0000037C)      /*     slv_reg_0DF */
#define XTUR0E0_            (0x00000380)      /*     slv_reg_0E0 */
#define XTUR0E1_            (0x00000384)      /*     slv_reg_0E1 */
#define XTUR0E2_            (0x00000388)      /*     slv_reg_0E2 */
#define XTUR0E3_            (0x0000038C)      /*     slv_reg_0E3 */
#define XTUR0E4_            (0x00000390)      /*     slv_reg_0E4 */
#define XTUR0E5_            (0x00000394)      /*     slv_reg_0E5 */
#define XTUR0E6_            (0x00000398)      /*     slv_reg_0E6 */
#define XTUR0E7_            (0x0000039C)      /*     slv_reg_0E7 */
#define XTUR0E8_            (0x000003A0)      /*     slv_reg_0E8 */
#define XTUR0E9_            (0x000003A4)      /*     slv_reg_0E9 */ 
#define XTUR0EA_            (0x000003A8)      /*     slv_reg_0EA */
#define XTUR0EB_            (0x000003AC)      /*     slv_reg_0EB */
#define XTUR0EC_            (0x000003B0)      /*     slv_reg_0EC */
#define XTUR0ED_            (0x000003B4)      /*     slv_reg_0ED */ 
#define XTUR0EE_            (0x000003B8)      /*     slv_reg_0EE */
#define XTUR0EF_            (0x000003BC)      /*     slv_reg_0EF */
#define XTUR0F0_            (0x000003C0)      /*     slv_reg_0F0 */
#define XTUR0F1_            (0x000003C4)      /*     slv_reg_0F1 */
#define XTUR0F2_            (0x000003C8)      /*     slv_reg_0F2 */
#define XTUR0F3_            (0x000003CC)      /*     slv_reg_0F3 */
#define XTUR0F4_            (0x000003D0)      /*     slv_reg_0F4 */
#define XTUR0F5_            (0x000003D4)      /*     slv_reg_0F5 */
#define XTUR0F6_            (0x000003D8)      /*     slv_reg_0F6 */
#define XTUR0F7_            (0x000003DC)      /*     slv_reg_0F7 */
#define XTUR0F8_            (0x000003E0)      /*     slv_reg_0F8 */
#define XTUR0F9_            (0x000003E4)      /*     slv_reg_0F9 */
#define XTUR0FA_            (0x000003E8)      /*     slv_reg_0FA */
#define XTUR0FB_            (0x000003EC)      /*     slv_reg_0FB */
#define XTUR0FC_            (0x000003F0)      /*     slv_reg_0FC */
#define XTUR0FD_            (0x000003F4)      /*     slv_reg_0FD */
#define XTUR0FE_            (0x000003F8)      /*     slv_reg_0FE */
#define XTUR0FF_            (0x000003FC)      /*     slv_reg_0FF */
#define XTUR100_            (0x00000400)      /*     slv_reg_100 */
#define XTUR101_            (0x00000404)      /*     slv_reg_101 */
#define XTUR102_            (0x00000408)      /*     slv_reg_102 */
#define XTUR103_            (0x0000040C)      /*     slv_reg_103 */
#define XTUR104_            (0x00000410)      /*     slv_reg_104 */
#define XTUR105_            (0x00000414)      /*     slv_reg_105 */
#define XTUR106_            (0x00000418)      /*     slv_reg_106 */
#define XTUR107_            (0x0000041C)      /*     slv_reg_107 */
#define XTUR108_            (0x00000420)      /*     slv_reg_108 */
#define XTUR109_            (0x00000424)      /*     slv_reg_109 */
#define XTUR10A_            (0x00000428)      /*     slv_reg_10A */
#define XTUR10B_            (0x0000042C)      /*     slv_reg_10B */
#define XTUR10C_            (0x00000430)      /*     slv_reg_10C */
#define XTUR10D_            (0x00000434)      /*     slv_reg_10D */
#define XTUR10E_            (0x00000438)      /*     slv_reg_10E */
#define XTUR10F_            (0x0000043C)      /*     slv_reg_10F */
#define XTUR110_            (0x00000440)      /*     slv_reg_110 */
#define XTUR111_            (0x00000444)      /*     slv_reg_111 */
#define XTUR112_            (0x00000448)      /*     slv_reg_112 */
#define XTUR113_            (0x0000044C)      /*     slv_reg_113 */
#define XTUR114_            (0x00000450)      /*     slv_reg_114 */
#define XTUR115_            (0x00000454)      /*     slv_reg_115 */
#define XTUR116_            (0x00000458)      /*     slv_reg_116 */
#define XTUR117_            (0x0000045C)      /*     slv_reg_117 */
#define XTUR118_            (0x00000460)      /*     slv_reg_118 */
#define XTUR119_            (0x00000464)      /*     slv_reg_119 */
#define XTUR11A_            (0x00000468)      /*     slv_reg_11A */
#define XTUR11B_            (0x0000046C)      /*     slv_reg_11B */
#define XTUR11C_            (0x00000470)      /*     slv_reg_11C */
#define XTUR11D_            (0x00000474)      /*     slv_reg_11D */
#define XTUR11E_            (0x00000478)      /*     slv_reg_11E */
#define XTUR11F_            (0x0000047C)      /*     slv_reg_11F */
#define XTUR120_            (0x00000480)      /*     slv_reg_120 */
#define XTUR121_            (0x00000484)      /*     slv_reg_121 */
#define XTUR122_            (0x00000488)      /*     slv_reg_122 */
#define XTUR123_            (0x0000048C)      /*     slv_reg_123 */
#define XTUR124_            (0x00000490)      /*     slv_reg_124 */
#define XTUR125_            (0x00000494)      /*     slv_reg_125 */
#define XTUR126_            (0x00000498)      /*     slv_reg_126 */
#define XTUR127_            (0x0000049C)      /*     slv_reg_127 */
#define XTUR128_            (0x000004A0)      /*     slv_reg_128 */
#define XTUR129_            (0x000004A4)      /*     slv_reg_129 */
#define XTUR12A_            (0x000004A8)      /*     slv_reg_12A */
#define XTUR12B_            (0x000004AC)      /*     slv_reg_12B */
#define XTUR12C_            (0x000004B0)      /*     slv_reg_12C */
#define XTUR12D_            (0x000004B4)      /*     slv_reg_12D */
#define XTUR12E_            (0x000004B8)      /*     slv_reg_12E */
#define XTUR12F_            (0x000004BC)      /*     slv_reg_12F */
#define XTUR130_            (0x000004C0)      /*     slv_reg_130 */
#define XTUR131_            (0x000004C4)      /*     slv_reg_131 */
#define XTUR132_            (0x000004C8)      /*     slv_reg_132 */
#define XTUR133_            (0x000004CC)      /*     slv_reg_133 */
#define XTUR134_            (0x000004D0)      /*     slv_reg_134 */
#define XTUR135_            (0x000004D4)      /*     slv_reg_135 */
#define XTUR136_            (0x000004D8)      /*     slv_reg_136 */
#define XTUR137_            (0x000004DC)      /*     slv_reg_137 */
#define XTUR138_            (0x000004E0)      /*     slv_reg_138 */
#define XTUR139_            (0x000004E4)      /*     slv_reg_139 */
#define XTUR13A_            (0x000004E8)      /*     slv_reg_13A */
#define XTUR13B_            (0x000004EC)      /*     slv_reg_13B */
#define XTUR13C_            (0x000004F0)      /*     slv_reg_13C */
#define XTUR13D_            (0x000004F4)      /*     slv_reg_13D */
#define XTUR13E_            (0x000004F8)      /*     slv_reg_13E */
#define XTUR13F_            (0x000004FC)      /*     slv_reg_13F */
#define XTUR140_            (0x00000500)      /*     slv_reg_140 */
#define XTUR141_            (0x00000504)      /*     slv_reg_141 */
#define XTUR142_            (0x00000508)      /*     slv_reg_142 */
#define XTUR143_            (0x0000050C)      /*     slv_reg_143 */
#define XTUR144_            (0x00000510)      /*     slv_reg_144 */
#define XTUR145_            (0x00000514)      /*     slv_reg_145 */
#define XTUR146_            (0x00000518)      /*     slv_reg_146 */
#define XTUR147_            (0x0000051C)      /*     slv_reg_147 */
#define XTUR148_            (0x00000520)      /*     slv_reg_148 */
#define XTUR149_            (0x00000524)      /*     slv_reg_149 */
#define XTUR14A_            (0x00000528)      /*     slv_reg_14A */
#define XTUR14B_            (0x0000052C)      /*     slv_reg_14B */
#define XTUR14C_            (0x00000530)      /*     slv_reg_14C */
#define XTUR14D_            (0x00000534)      /*     slv_reg_14D */
#define XTUR14E_            (0x00000538)      /*     slv_reg_14E */
#define XTUR14F_            (0x0000053C)      /*     slv_reg_14F */
#define XTUR150_            (0x00000540)      /*     slv_reg_150 */
#define XTUR151_            (0x00000544)      /*     slv_reg_151 */
#define XTUR152_            (0x00000548)      /*     slv_reg_152 */
#define XTUR153_            (0x0000054C)      /*     slv_reg_153 */
#define XTUR154_            (0x00000550)      /*     slv_reg_154 */
#define XTUR155_            (0x00000554)      /*     slv_reg_155 */
#define XTUR156_            (0x00000558)      /*     slv_reg_156 */
#define XTUR157_            (0x0000055C)      /*     slv_reg_157 */
#define XTUR158_            (0x00000560)      /*     slv_reg_158 */
#define XTUR159_            (0x00000564)      /*     slv_reg_159 */
#define XTUR15A_            (0x00000568)      /*     slv_reg_15A */
#define XTUR15B_            (0x0000056C)      /*     slv_reg_15B */ 
#define XTUR15C_            (0x00000570)      /*     slv_reg_15C */
#define XTUR15D_            (0x00000574)      /*     slv_reg_15D */
#define XTUR15E_            (0x00000578)      /*     slv_reg_15E */
#define XTUR15F_            (0x0000057C)      /*     slv_reg_15F */ 
#define XTUR160_            (0x00000580)      /*     slv_reg_160 */
#define XTUR161_            (0x00000584)      /*     slv_reg_161 */
#define XTUR162_            (0x00000588)      /*     slv_reg_162 */
#define XTUR163_            (0x0000058C)      /*     slv_reg_163 */
#define XTUR164_            (0x00000590)      /*     slv_reg_164 */
#define XTUR165_            (0x00000594)      /*     slv_reg_165 */
#define XTUR166_            (0x00000598)      /*     slv_reg_166 */
#define XTUR167_            (0x0000059C)      /*     slv_reg_167 */
#define XTUR168_            (0x000005A0)      /*     slv_reg_168 */
#define XTUR169_            (0x000005A4)      /*     slv_reg_169 */
#define XTUR16A_            (0x000005A8)      /*     slv_reg_16A */
#define XTUR16B_            (0x000005AC)      /*     slv_reg_16B */
#define XTUR16C_            (0x000005B0)      /*     slv_reg_16C */
#define XTUR16D_            (0x000005B4)      /*     slv_reg_16D */
#define XTUR16E_            (0x000005B8)      /*     slv_reg_16E */
#define XTUR16F_            (0x000005BC)      /*     slv_reg_16F */
#define XTUR170_            (0x000005C0)      /*     slv_reg_170 */
#define XTUR171_            (0x000005C4)      /*     slv_reg_171 */
#define XTUR172_            (0x000005C8)      /*     slv_reg_172 */
#define XTUR173_            (0x000005CC)      /*     slv_reg_173 */
#define XTUR174_            (0x000005D0)      /*     slv_reg_174 */
#define XTUR175_            (0x000005D4)      /*     slv_reg_175 */
#define XTUR176_            (0x000005D8)      /*     slv_reg_176 */
#define XTUR177_            (0x000005DC)      /*     slv_reg_177 */
#define XTUR178_            (0x000005E0)      /*     slv_reg_178 */
#define XTUR179_            (0x000005E4)      /*     slv_reg_179 */
#define XTUR17A_            (0x000005E8)      /*     slv_reg_17A */
#define XTUR17B_            (0x000005EC)      /*     slv_reg_17B */
#define XTUR17C_            (0x000005F0)      /*     slv_reg_17C */
#define XTUR17D_            (0x000005F4)      /*     slv_reg_17D */
#define XTUR17E_            (0x000005F8)      /*     slv_reg_17E */
#define XTUR17F_            (0x000005FC)      /*     slv_reg_17F */
#define XTUR180_            (0x00000600)      /*     slv_reg_180 */
#define XTUR181_            (0x00000604)      /*     slv_reg_181 */
#define XTUR182_            (0x00000608)      /*     slv_reg_182 */
#define XTUR183_            (0x0000060C)      /*     slv_reg_183 */
#define XTUR184_            (0x00000610)      /*     slv_reg_184 */ 
#define XTUR185_            (0x00000614)      /*     slv_reg_185 */
#define XTUR186_            (0x00000618)      /*     slv_reg_186 */
#define XTUR187_            (0x0000061C)      /*     slv_reg_187 */
#define XTUR188_            (0x00000620)      /*     slv_reg_188 */
#define XTUR189_            (0x00000624)      /*     slv_reg_189 */
#define XTUR18A_            (0x00000628)      /*     slv_reg_18A */
#define XTUR18B_            (0x0000062C)      /*     slv_reg_18B */
#define XTUR18C_            (0x00000630)      /*     slv_reg_18C */
#define XTUR18D_            (0x00000634)      /*     slv_reg_18D */
#define XTUR18E_            (0x00000638)      /*     slv_reg_18E */
#define XTUR18F_            (0x0000063C)      /*     slv_reg_18F */
#define XTUR190_            (0x00000640)
#define XTUR191_            (0x00000644)
#define XTUR192_            (0x00000648)
#define XTUR193_            (0x0000064C)
#define XTUR194_            (0x00000650)
#define XTUR195_            (0x00000654)
#define XTUR196_            (0x00000658)
#define XTUR197_            (0x0000065C)
#define XTUR198_            (0x00000660)
#define XTUR199_            (0x00000664)
#define XTUR19A_            (0x00000668)
#define XTUR19B_            (0x0000066C)
#define XTUR19C_            (0x00000670)
#define XTUR19D_            (0x00000674)
#define XTUR19E_            (0x00000678)
#define XTUR19F_            (0x0000067C)
#define XTUR1A0_            (0x00000680)
#define XTUR1A1_            (0x00000684)
#define XTUR1A2_            (0x00000688)
#define XTUR1A3_            (0x0000068C)
#define XTUR1A4_            (0x00000690)
#define XTUR1A5_            (0x00000694)
#define XTUR1A6_            (0x00000698)
#define XTUR1A7_            (0x0000069C)
#define XTUR1A8_            (0x000006A0)
#define XTUR1A9_            (0x000006A4)
#define XTUR1AA_            (0x000006A8)
#define XTUR1AB_            (0x000006AC)
#define XTUR1AC_            (0x000006B0)
#define XTUR1AD_            (0x000006B4)
#define XTUR1AE_            (0x000006B8)
#define XTUR1AF_            (0x000006BC)
#define XTUR1B0_            (0x000006C0)
#define XTUR1B1_            (0x000006C4)
#define XTUR1B2_            (0x000006C8)
#define XTUR1B3_            (0x000006CC)
#define XTUR1B4_            (0x000006D0)
#define XTUR1B5_            (0x000006D4)
#define XTUR1B6_            (0x000006D8)
#define XTUR1B7_            (0x000006DC)
#define XTUR1B8_            (0x000006E0)
#define XTUR1B9_            (0x000006E4)
#define XTUR1BA_            (0x000006E8)
#define XTUR1BB_            (0x000006EC)
#define XTUR1BC_            (0x000006F0)
#define XTUR1BD_            (0x000006F4)
#define XTUR1BE_            (0x000006F8)
#define XTUR1BF_            (0x000006FC)
#define XTUR1C0_            (0x00000700)
#define XTUR1C1_            (0x00000704)
#define XTUR1C2_            (0x00000708)
#define XTUR1C3_            (0x0000070C)
#define XTUR1C4_            (0x00000710)
#define XTUR1C5_            (0x00000714)
#define XTUR1C6_            (0x00000718)
#define XTUR1C7_            (0x0000071C)
#define XTUR1C8_            (0x00000720)
#define XTUR1C9_            (0x00000724)
#define XTUR1CA_            (0x00000728)
#define XTUR1CB_            (0x0000072C)
#define XTUR1CC_            (0x00000730)
#define XTUR1CD_            (0x00000734)
#define XTUR1CE_            (0x00000738)
#define XTUR1CF_            (0x0000073C)
#define XTUR1D0_            (0x00000740)
#define XTUR1D1_            (0x00000744)
#define XTUR1D2_            (0x00000748)
#define XTUR1D3_            (0x0000074C)
#define XTUR1D4_            (0x00000750)
#define XTUR1D5_            (0x00000754)
#define XTUR1D6_            (0x00000758)
#define XTUR1D7_            (0x0000075C)
#define XTUR1D8_            (0x00000760)
#define XTUR1D9_            (0x00000764)
#define XTUR1DA_            (0x00000768)
#define XTUR1DB_            (0x0000076C)
#define XTUR1DC_            (0x00000770)
#define XTUR1DD_            (0x00000774)
#define XTUR1DE_            (0x00000778)
#define XTUR1DF_            (0x0000077C)
#define XTUR1E0_            (0x00000780)
#define XTUR1E1_            (0x00000784)
#define XTUR1E2_            (0x00000788)
#define XTUR1E3_            (0x0000078C)
#define XTUR1E4_            (0x00000790)
#define XTUR1E5_            (0x00000794)
#define XTUR1E6_            (0x00000798)
#define XTUR1E7_            (0x0000079C)
#define XTUR1E8_            (0x000007A0)
#define XTUR1E9_            (0x000007A4)
#define XTUR1EA_            (0x000007A8)
#define XTUR1EB_            (0x000007AC)
#define XTUR1EC_            (0x000007B0)
#define XTUR1ED_            (0x000007B4)
#define XTUR1EE_            (0x000007B8)
#define XTUR1EF_            (0x000007BC)
#define XTUR1F0_            (0x000007C0)
#define XTUR1F1_            (0x000007C4)
#define XTUR1F2_            (0x000007C8)
#define XTUR1F3_            (0x000007CC)
#define XTUR1F4_            (0x000007D0)
#define XTUR1F5_            (0x000007D4)
#define XTUR1F6_            (0x000007D8)
#define XTUR1F7_            (0x000007DC)
#define XTUR1F8_            (0x000007E0)
#define XTUR1F9_            (0x000007E4)
#define XTUR1FA_            (0x000007E8)
#define XTUR1FB_            (0x000007EC)
#define XTUR1FC_            (0x000007F0)
#define XTUR1FD_            (0x000007F4)
#define XTUR1FE_            (0x000007F8)
#define XTUR1FF_            (0x000007FC)

#ifdef __cplusplus
}
#endif 
#endif



