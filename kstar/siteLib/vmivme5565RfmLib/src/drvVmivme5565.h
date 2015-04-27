/*
**    ==============================================================================
**    
**        Abs: Include file for the VMIVME5565 Reflect Memory Module
**       
**        Name: drvVmivme5565.h
**
**        Side: Must included the following header files
**                    dbScan.h  - for IOSCANPVT
**                    devLib.h  - for epicsAddressType
**
**        First Auth:  17-Jan-2004, Kukhee Kim (KHKIM)
**        Second Auth: dd-mmm-yyyy, First  Lastname (USERNAME)
**        Rev:         dd-mmm-yyyy, Reviewer's name (USERNAME) 
**    
**    -------------------------------------------------------------------------------
**        Mod:
**                     dd-mmm-yyy, First Lastname   (USERNAME):
**                        comments
**
**    ===============================================================================
*/

#ifndef  drvVmivme5565_H
#define  drvVmivme5565_H

#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"

#define   VMIVME5565_SCAN_INTERVAL           1.    /* 1 second scan */
#define   VMIVME5565_RNGBUFF_FIFO_DEPTH      127

#define   VMIVME5565_NETWORK_INT_1           1
#define   VMIVME5565_NETWORK_INT_2           2
#define   VMIVME5565_NETWORK_INT_3           3
#define   VMIVME5565_NETWORK_INT_4           4

#define   VMIVME5565_NIC_INT1_BITPTN         0x01
#define   VMIVME5565_NIC_INT2_BITPTN         0x02
#define   VMIVME5565_NIC_INT3_BITPTN         0x03
#define   VMIVME5565_NIC_INT4_BITPTN         0x07
#define   VMIVME5565_NIC_INTALL_BITPTN       0x08

#define   VMIVME5565_USR_INIT                0
#define   VMIVME5565_USR_INIT_MASK           (0x01 << VMIVME5565_USR_INIT)
#define   VMIVME5565_SCN_INIT                1
#define   VMIVME5565_SCN_INIT_MASK           (0x01 << VMIVME5565_SCN_INIT)
#define   VMIVME5565_DRV_INIT                2
#define   VMIVME5565_DRV_INIT_MASK           (0x01 << VMIVME5565_DRV_INIT)
#define   VMIVME5565_DEV_INIT                3
#define   VMIVME5565_DEV_INIT_MASK           (0x01 << VMIVME5565_DEV_INIT)
#define   VMIVME5565_REC_INIT                4
#define   VMIVME5565_REC_INIT_MASK           (0x01 << VMIVME5565_REC_INIT)

#define   VMIVME5565_DRV_INIT_AFTER          5
#define   VMIVME5565_DRV_INIT_AFTER_MASK     (0x01 << VMIVME5565_DRV_INIT_AFTER)
#define   VMIVME5565_DEV_INIT_AFTER          6
#define   VMIVME5565_DEV_INIT_AFTER_MASK     (0x01 << VMIVME5565_DEV_INIT_AFTER)
#define   VMIVME5565_REC_INIT_AFTER          7
#define   VMIVME5565_REC_INIT_AFTER_MASK     (0x01 << VMIVME5565_REC_INIT_AFTER)

#define   VMIVME5565_PRODUCT_BID             0x65        /* desired value */

/* Bit definition for LCSR */
#define   VMIVME5565_LCSR_LED                31
#define   VMIVME5565_LCSR_LED_MASK           (0x00000001 << VMIVME5565_LCSR_LED)
#define   VMIVME5565_LCSR_TRMDIS             30
#define   VMIVME5565_LCSR_TRMDIS_MASK        (0x00000001 << VMIVME5565_LCSR_TRMDIS)
#define   VMIVME5565_LCSR_DKODKENB           29
#define   VMIVME5565_LCSR_DKODKENB_MASK      (0x00000001 << VMIVME5565_LCSR_DKODKENB)
#define   VMIVME5565_LCSR_LPBKENB            28
#define   VMIVME5565_LCSR_LPBKENB_MASK       (0x00000001 << VMIVME5565_LCSR_LPBKENB)
#define   VMIVME5565_LCSR_LBPRTENB           27
#define   VMIVME5565_LCSR_LBPRTENB_MASK      (0x00000001 << VMIVME5565_LCSR_LBPRTENB)
#define   VMIVME5565_LCSR_RDTMDENB           26
#define   VMIVME5565_LCSR_RDTMDENB_MASK      (0x00000001 << VMIVME5565_LCSR_RDTMDENB)
#define   VMIVME5565_LCSR_RQMST0ENB          25
#define   VMIVME5565_LCSR_RQMST0ENB_MASK     (0x00000001 << VMIVME5565_LCSR_RQMST0ENB)
#define   VMIVME5565_LCSR_RQMST1ENB          24
#define   VMIVME5565_LCSR_RQMST1ENB_MASK     (0x00000001 << VMIVME5565_LCSR_RQMST1ENB)
#define   VMIVME5565_LCSR_CONFIG             20
#define   VMIVME5565_LCSR_CONFIG_MASK        (0x00000003 << VMIVME5565_LCSR_CONFIG)
#define   VMIVME5565_LCSR_OFFSET             16
#define   VMIVME5565_LCSR_OFFSET_MASK        (0x00000003 << VMIVME5565_LCSR_OFFSET)
#define   VMIVME5565_LCSR_TXFIFOEMPT         7
#define   VMIVME5565_LCSR_TXFIFOEMPT_MASK    (0x00000001 << VMIVME5565_LCSR_TXFIFOEMPT)
#define   VMIVME5565_LCSR_TXFIFOAFULL        6
#define   VMIVME5565_LCSR_TXFIFOAFULL_MASK   (0x00000001 << VMIVME5565_LCSR_TXFIFOAFILL)
#define   VMIVME5565_LCSR_RXFIOFOFULL        5
#define   VMIVME5565_LCSR_RXFIOFOFULL_MASK   (0x00000001 << VMIVME5565_LCSR_RXFIFOFULL)
#define   VMIVME5565_LCSR_RXFIFOAFULL        4
#define   VMIVME5565_LCSR_RXFIFOAFULL_MASK   (0x00000001 << VMIVME5565_LCSR_RXFIFOAFULL)
#define   VMIVME5565_LCSR_SYNCLOSS           3
#define   VMIVME5565_LCSR_SYNCLOSS_MASK      (0x00000001 << VMIVME5565_LCSR_SYNCLOSS)
#define   VMIVME5565_LCSR_RXDETECT           2
#define   VMIVME5565_LCSR_RXDETECT_MASK      (0x00000001 << VMIVME5565_LCSR_RXDETECT)
#define   VMIVME5565_LCSR_BADDATA            1
#define   VMIVME5565_LCSR_BADDATA_MASK       (0x00000001 << VMIVME5565_LCSR_BADDATA)
#define   VMIVME5565_LCSR_OWNDATA            0
#define   VMIVME5565_LCSR_OWNDATA_MASK       (0x00000001 << VMIVME5565_LCSR_OWNDATA)

    /* Local Interrupt Control Register (LISR and LIER) */
#define   VMIVME5565_LICR_ACLEAR             15
#define   VMIVME5565_LICR_ACLEAR_MASK        (0x00000001 << VMIVME5565_LICR_ACLEAR)
#define   VMIVME5565_LICR_GBLINT             14
#define   VMIVME5565_LICR_GBLINT_MASK        (0x00000001 << VMIVME5565_LICR_GBLINT)
#define   VMIVME5565_LICR_LCLMEMPARITY       13
#define   VMIVME5565_LICR_LCLMEMPARITY_MASK  (0x00000001 << VMIVME5565_LICR_LCLMEMPARITY)
#define   VMIVME5565_LICR_MEMWRTINHIBIT      12
#define   VMIVME5565_LICR_MEMWRTINHIBIT_MASK (0x00000001 << VMIVME5565_LICR_MEMWRTINHIBIT)
#define   VMIVME5565_LICR_SYNCLOSS           11
#define   VMIVME5565_LICR_SYNCLOSS_MASK      (0x00000001 << VMIVME5565_LICR_SYNCLOSS)
#define   VMIVME5565_LICR_RXFIFOFULL         10
#define   VMIVME5565_LICR_RXFIFOFULL_MASK    (0x00000001 << VMIVME5565_LICR_RXFIFOFULL)
#define   VMIVME5565_LICR_RXFIFOAFULL        9
#define   VMIVME5565_LICR_RXFIFOAFULL_MASK   (0x00000001 << VMIVME5565_LICR_RXFIFOAFULL)
#define   VMIVME5565_LICR_BADDATA            8
#define   VMIVME5565_LICR_BADDATA_MASK       (0x00000001 << VMIVME5565_LICR_BADDATA)
#define   VMIVME5565_LICR_INT4               7  
#define   VMIVME5565_LICR_INT4_MASK          (0x00000001 << VMIVME5565_LICR_INT4)
#define   VMIVME5565_LICR_ROQPCKFLT          6
#define   VMIVME5565_LICR_ROQPCKFLT_MASK     (0x00000001 << VMIVME5565_LICR_ROQPCKFLT)
#define   VMIVME5565_LICR_INT3               2
#define   VMIVME5565_LICR_INT3_MASK          (0x00000001 << VMIVME5565_LICR_INT3)
#define   VMIVME5565_LICR_INT2               1
#define   VMIVME5565_LICR_INT2_MASK          (0x00000001 << VMIVME5565_LICR_INT2)
#define   VMIVME5565_LICR_INT1               0
#define   VMIVME5565_LICR_INT1_MASK          (0x00000001 << VMIVME5565_LICR_INT1)

#define   VMIVME5565_NETINTMASK_ALL          (VMIVME5565_LICR_INT1_MASK | VMIVME5565_LICR_INT2_MASK | \
                                              VMIVME5565_LICR_INT3_MASK | VMIVME5565_LICR_INT4_MASK)





/* 
**   Bit definition for Universe II Contro and Status Register
*/
#define   VMIVME5565_VINT_SWINT7             31       /* VME Interrupt 7 */
#define   VMIVME5565_VINT_SWINT7_MASK        (0x00000001 << VMIVME5565_VINT_SWINT7)
#define   VMIVME5565_VINT_SWINT6             30       /* VME Interrupt 6 */
#define   VMIVME5565_VINT_SWINT6_MASK        (0x00000001 << VMIVME5565_VINT_SWINT6)
#define   VMIVME5565_VINT_SWINT5             29       /* VME Interrupt 5 */
#define   VMIVME5565_VINT_SWINT5_MASK        (0x00000001 << VMIVME5565_VINT_SWINT5)
#define   VMIVME5565_VINT_SWINT4             28       /* VME Interrupt 4 */
#define   VMIVME5565_VINT_SWINT4_MASK        (0x00000001 << VMIVME5565_VINT_SWINT4)
#define   VMIVME5565_VINT_SWINT3             27       /* VME Interrupt 3 */
#define   VMIVME5565_VINT_SWINT3_MASK        (0x00000001 << VMIVME5565_VINT_SWINT3)
#define   VMIVME5565_VINT_SWINT2             26       /* VME Interrupt 2 */
#define   VMIVME5565_VINT_SWINT2_MASK        (0x00000001 << VMIVME5565_VINT_SWINT2)
#define   VMIVME5565_VINT_SWINT1             25       /* VME Interrupt 1 */
#define   VMIVME5565_VINT_SWINT1_MASK        (0x00000001 << VMIVME5565_VINT_SWINT1)

#define   VMIVME5565_VINT_MBOX3              19       /* MailBox Interrupt 3 */
#define   VMIVME5565_VINT_MBOX3_MASK         (0x00000001 << VMIVME5565_VINT_MBOX3)
#define   VMIVME5565_VINT_MBOX2              18       /* MailBox Interrupt 2 */
#define   VMIVME5565_VINT_MBOX2_MASK         (0x00000001 << VMIVME5565_VINT_MBOX2)
#define   VMIVME5565_VINT_MBOX1              17       /* MailBox Interrupt 1 */
#define   VMIVME5565_VINT_MBOX1_MASK         (0x00000001 << VMIVME5565_VINT_MBOX1)
#define   VMIVME5565_VINT_MBOX0              16       /* MailBox Interrupt 0 */
#define   VMIVME5565_VINT_MBOX0_MASK         (0x00000001 << VMIVME5565_VINT_MBOX0)

#define   VMIVME5565_VINT_SWINT              12       /* VME SW interrupt */
#define   VMIVME5565_VINT_SWINT_MASK         (0x00000001 << VMIVME5565_VINT_SWINT)

#define   VMIVME5565_VINT_VERR               10       /* PCI VERR interrupt */
#define   VMIVME5565_VINT_VERR_MASK          (0x00000001 << VMIVME5565_VINT_VERR)
#define   VMIVME5565_VINT_LERR               9        /* PCI LERR interrupt */
#define   VMIVME5565_VINT_LERR_MASK          (0x00000001 << VMIVME5565_VINT_LERR)
#define   VMIVME5565_VINT_DMA                8        /* PCI DMA interrupt */
#define   VMIVME5565_VINT_DMA_MASK           (0x00000001 << VMIVME5565_VINT_DMA)

#define   VMIVME5565_VINT_LINT0              0        /* PCI LINT0 Interrupt */
#define   VMIVME5565_VINT_LINT0_MASK         (0x00000001 << VMIVME5565_VINT_LINT0)

    /* VINT_MAP0 */
#define   VMIVME5565_VINTMAP0_LINT0          0        /* */
#define   VMIVME5565_VINTMAP0_LINT0_MASK     (0x00000007 << VMIVME5565_VINT_LINT0)

    /* VINT_MAP1 */
#define   VMIVME5565_VINTMAP1_VERR           8
#define   VMIVME5565_VINTMAP1_VERR_MASK      (0x00000007 << VMIVME5565_VINTMAP1_VERR)
#define   VMIVME5565_VINTMAP1_LERR           4
#define   VMIVME5565_VINTMAP1_LERR_MASK      (0x00000007 << VMIVME5565_VINTMAP1_LERR)
#define   VMIVME5565_VINTMAP1_DMA            0
#define   VMIVME5565_VINTMAP1_DMA_MASK       (0x00000007 << VMIVME5565_VINTMAP1_DMA)

    /* STATID */
#define   VMIVME5565_STATID_U8BIT             24
#define   VMIVME5565_STATID_U8BIT_MASK        (0x000000ff << VMIVME5565_STATID_U8BIT)
#define   VMIVME5565_STATID_VEC               25
#define   VMIVME5565_STATID_VEC_MASK          (0x000000ff << VMIVME5565_STATID_VEC)
#define   VMIVME5565_STATID_SWIACK            24
#define   VMIVME5565_STATID_SWIACK_MASK       (0x00000001 << VMIVME5565_STATID_SWIACK)

   /* DCTL */
#define   VMIVME5565_DCTL_L2V                 32
#define   VMIVME5565_DCTL_L2V_MASK            (0x00000001 << VMIVME5565_DCTL_L2V)
#define   VMIVME5565_DCTL_VDW                 22
#define   VMIVME5565_DCTL_VDW_MASK            (0x00000003 << VMIVME5565_DCTL_VDW)
#define   VMIVME5565_DCTL_VAS                 16
#define   VMIVME5565_DCTL_VAS_MASK            (0x00000007 << VMIVME5565_DCTL_VAS)
#define   VMIVME5565_DCTL_PGM                 14
#define   VMIVME5565_DCTL_PGM_MASK            (0x00000003 << VMIVME5565_DCTL_PGM)
#define   VMIVME5565_DCTL_SUPER               12
#define   VMIVME5565_DCTL_SUPER_MASK          (0x00000003 << VMIVME5565_DCTL_SUPER)
#define   VMIVME5565_DCTL_VCT                 8
#define   VMIVME5565_DCTL_VCT_MASK            (0x00000001 << VMIVME5565_DCTL_VCT)
#define   VMIVME5565_DCTL_LD64EN              7
#define   VMIVME5565_DCTL_LD64EN_MASK         (0x00000001 << VMIVME5565_DCTL_LD64EN)

   /* DTBC */
#define   VMIVME5565_DTBC_24BIT               0
#define   VMIVME5565_DTBC_24BIT_MASK          (0x00ffffff << VMIVME5565_DTBC_24BIT)

   /* DLA */
#define   VMIVME5565_DLA_32BIT                0
#define   VMIVME5565_DLA_32BIT_MASK           (0xffffffff << VMIVME5565_DLA_32BIT)

   /* DVA */
#define   VMIVME5565_DVA_32BIT                0
#define   VMIVME5565_DVA_32BIT_MASK           (0xffffffff << VMIVME55565_DVA_32BIT)

  /* DCPP */
#define   VMIVME5565_DCPP_27BIT               5
#define   VMIVME5565_DCPP_27BIT_MASK          (0xffffffff << VMIVME5565_DCPP_27BIT)

    /* DGCS */
#define   VMIVME5565_DGCS_GO                  31
#define   VMIVME5565_DGCS_GO_MASK             (0x00000001 << VMIVME5565_DGCS_GO_MASK)
#define   VMIVME5565_DGCS_STOPREQ             30
#define   VMIVME5565_DGCS_STOPREQ_MASK        (0x00000001 << VMIVME5565_DGCS_STOPREQ)
#define   VMIVME5565_DGCS_HALTREQ             29
#define   VMIVME5565_DGCS_HALTREQ_MASK        (0x00000001 << VMIVME5565_DGCS_HALTREQ)

#define   VMIVME5565_DGCS_CHAIN               27
#define   VMIVME5565_DGCS_CHAIN_MASK          (0x00000001 << VMIVME5565_DGCS_CHAIN)

#define   VMIVME5565_DGCS_VON                 20
#define   VMIVME5565_DGCS_VON_MASK            (0x00000007 << VMIVME5565_DGCS_VON)
#define   VMIVME5565_DGCS_VOFF                16
#define   VMIVME5565_DGCS_VOFF_MASK           (0x0000000f << VMIVME5565_DGCS_VOFF)
#define   VMIVME5565_DGCS_ACT                 15
#define   VMIVME5565_DGCS_ACT_MASK            (0x00000001 << VMIVME5565_DGCS_ACT)
#define   VMIVME5565_DGCS_STOP                14
#define   VMIVME5565_DGCS_STOP_MASK           (0x00000001 << VMIVME5565_DGCS_STOP)
#define   VMIVME5565_DGCS_HALT                13
#define   VMIVME5565_DGCS_HALT_MASK           (0x00000001 << VMIVME5565_DGCS_HALT)

#define   VMIVME5565_DGCS_DONE                11
#define   VMIVME5565_DGCS_DONE_MASK           (0x00000001 << VMIVME5565_DGCS_DONE)
#define   VMIVME5565_DGCS_LERR                10
#define   VMIVME5565_DGCS_LERR_MASK           (0x00000001 << VMIVME5565_DGCS_LERR)
#define   VMIVME5565_DGCS_VERR                9
#define   VMIVME5565_DGCS_VERR_MASK           (0x00000001 << VMIVME5565_DGCS_VERR)
#define   VMIVME5565_DGCS_PERR                8
#define   VMIVME5565_DGCS_PERR_MASK           (0x00000001 << VMIVME5565_DGCS_PERR)

#define   VMIVME5565_DGCS_INTSTOP             6
#define   VMIVME5565_DGCS_INTSTOP_MASK        (0x00000001 << VMIVME5565_DGCS_INTSTOP)
#define   VMIVME5565_DGCS_INTHALT             5
#define   VMIVME5565_DGCS_INTHALT_MASK        (0x00000001 << VMIVME5565_DGCS_INTHALT)

#define   VMIVME5565_DGCS_INTDONE             3
#define   VMIVME5565_DGCS_INTDONE_MASK        (0x00000001 << VMIVME5565_DGCS_INTDONE)
#define   VMIVME5565_DGCS_INTLERR             2
#define   VMIVME5565_DGCS_INTLERR_MASK        (0x00000001 << VMIVME5565_DGCS_INTLERR)
#define   VMIVME5565_DGCS_INTVERR             1
#define   VMIVME5565_DGCS_INTVERR_MASK        (0x00000001 << VMIVME5565_DGCS_INTVERR)
#define   VMIVME5565_DGCS_INTPERR             0
#define   VMIVME5565_DGCS_INTPERR_MASK        (0x00000001 << VMIVME5565_DGCS_INTPERR)




typedef enum{
    vmivme5565Sram64MB,
    vmivme5565Sram128MB
} vmivme5565SramSize;



typedef volatile unsigned char      devReg1;
typedef volatile unsigned long      devReg4;

typedef struct {
    devReg1       reserved0[0x1200];     /* not used byte pad */

    devReg1       brv;                   /* board revision, read only,  offset 0x0 */
    devReg1       bid;                   /* board ID register, read only, offset 0x1 */

    devReg1       reserved1[2];          /* reserved, offset 0x3, 0x2 */

    devReg1       nid;                   /* node ID register, read only, offset 0x4 */

    devReg1       reserved2[3];          /* reserved, offset 0x7, 0x6, 0x5 */

    devReg4       lcsr;                  /* local control and status reg., read write, offset 0xb to 0x8 */

    devReg4       reserved3;             /* reserved, offset 0xf to 0xc */

    devReg4       lisr;                  /* local interrupt status reg., read write, offset 0x13 to 0x10 */
    devReg4       lier;                  /* local interrupt enable reg., read write, offset 0x17 to 0x14 */
    devReg4       ntd;                   /* network target data, read write, offset 0x1b to 0x18 */
    devReg1       ntn;                   /* network target node, read write, offset 0x1c */
    devReg1       nic;                   /* network interrupt command, read write, offset 0x1d */

    devReg1       reserved4[2];          /* reserved, offset 0x1f, 0x1e */

    devReg4       isd1;                  /* Int. 1 sender data, read write, offset 0x23 to 0x20 */
    devReg1       sid1;                  /* int. 1 sender ID, read write, offset 0x24 */

    devReg1       reserved5[3];          /* reserved, offset 0x27, 0x26, 0x25 */

    devReg4       isd2;                  /* int. 2 sender data, read write, offset 0x2b to 0x28 */
    devReg1       sid2;                  /* int. 2 sender ID, read write, offset 0x2c */
    
    devReg1       reserved6[3];          /* reserved, offset 0x2f, 0x2e, 0x2d */
   
    devReg4       isd3;                  /* int. 3 sender data, read write, offset 0x33 to 0x30 */
    devReg1       sid3;                  /* int. 3 sender ID, read write, offset 0x34 */
  
    devReg1       reserved7[3];          /* reserved, offset 0x37, 0x36, 0x35 */

    devReg4       isd4;                  /* int. 4 sender data, read write, offset 0x3b to 0x38 */
    devReg1       sid4;                  /* int. 4 sender ID, read write, offset 0x3c */

    devReg1       reserved8[3];          /* reserved, offset 0x3f, 0x3e, 0x3d */
} vmivme5565_ts;

typedef struct {
    devReg1      reserved0[0x200];      /* reserved, offset 0x200 */
    devReg4      dctl;                  /* DMA transfer control, offset 0x200 */
    devReg4      dtbc;                  /* DMA transfer byte counter, offset 0x204 */
    devReg4      dla;                   /* DMA PCI Bus address, offset 0x208 */
    devReg4      unknown0;              /* not described by manual?, offset 0x20b */
    devReg4      dva;                   /* DMA VMEbus address, offset 0x210 */
    devReg4      unknown1;              /* not described by manual?, offset 0x214 */
    devReg4      dcpp;                  /* DMA command packet pointer, offset 0x218 */
    devReg4      unknown2;              /* not described by manual?, offset 0x21b */
    devReg4      dgcs;                  /* DMA general controla and status */
} vmivme5565_UniverseII_DMA_ctrl_ts;

typedef struct {
    devReg1       reserved0[0x310];     /* reserved, offset 0x310 */
    devReg4       vint_en;              /* VME interrupt enable, offset 0x310 */
    devReg4       vint_stat;            /* VME interrupt status, offset 0x314 */
    devReg4       vint_map0;            /* VME interrupt map0,   offset 0x318 */
    devReg4       vint_map1;            /* VME interrupt map1,   offset 0x31b */
    devReg4       statid;               /* Interrupt status/ID out, offset 0x320 */

} vmivme5565_UniverseII_ctrl_ts;

typedef struct {                        /* type definition for network interrupt FIFFO */
    int            int_num;             /* interrupt number */
    unsigned long  isd;                 /* interrupt sender data */ 
    unsigned char  sid;                 /* sender id for network interrupt */
} vmivme5565_netIntFIFO_ts;

typedef struct {
    ELLNODE           node;              /* linked list */
    vmivme5565_ts*      vmivme5565_io_ps;    /* IO register */
    vmivme5565_UniverseII_DMA_ctrl_ts*
                        vmivme5565_io_UniverseII_DMA_ctrl_ps;   /* UniverseII DMA control */
    vmivme5565_UniverseII_ctrl_ts*
                        vmivme5565_io_UniverseII_ctrl_ps;      /* UniverseII control */
    void*               vmivme5565_sram_ps;  /* SRAM */

    unsigned short    card;              /* card number */
    unsigned char     vint_vec;          /* VMEbus Interrupt vector number */
    unsigned char     vint_lev;          /* VMEbus Interrupt Level */
    epicsAddressType  addr_type_regs;    /* address type for register, default A24 */
    epicsAddressType  addr_type_sram;    /* address type for sram,     default A32 */
    unsigned long     base_addr_regs;    /* board offset for registers */
    unsigned long     local_addr_regs;   /* local address for registers */
    unsigned long     base_addr_sram;    /* board offset for registers */
    unsigned long     local_addr_sram;   /* local address for registers */

    unsigned char     brv;               /* board revision number,  fixed value */
    unsigned char     bid;               /* board id, default 0x65, fixed value */
    unsigned char     nid;               /* board node ID,          fixed value */
    unsigned long     lcsr;              /* local control and status, latched */

    unsigned long     lisr;              /* local interrupt status regs., latched */
    unsigned long     lier;              /* local interrupt enable regs., latched */
    unsigned long     ntd;               /* network interrupt target data */
    unsigned char     ntn;               /* network interrupt target node */
    unsigned char     nic;               /* network interrupt command */

    
    vmivme5565SramSize  sram_size;

    epicsMutexId     lock;
    epicsRingBytesId rngBuffId;
    unsigned long    netIntMask;
    unsigned char    init_flag;
    unsigned long    scan_cnt;
    CALLBACK         vint_callback;

    unsigned long    int_cnt;             /* interrupt counter */
    unsigned long    mod_int_cnt;
    unsigned long    net_int1cmd_cnt;     /* network interrupt1 counter */
    unsigned long    net_int2cmd_cnt;     /* network interrupt2 counter */
    unsigned long    net_int3cmd_cnt;     /* network interrupt3 counter */
    unsigned long    net_int4cmd_cnt;     /* network interrupt4 counter */

    unsigned long    rngBuffFreeBytes;    /* free bytes in the ring buffer */
    unsigned int     rngBuffFreeEvents;   /* free events in the ring buffer */
    
    IOSCANPVT        ioScanPvtRfm;

} vmivme5565_config_ts;


#endif /* drvVmivme5565_H */
