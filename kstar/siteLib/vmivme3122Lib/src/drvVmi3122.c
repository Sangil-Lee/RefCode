/***** drvVmi3122.c             *****/
/***** Author: Marie Keesee     *****/
/***** Date: 08May96            *****/
/***** Modder: Alicia Hofler    *****/
/***** Date: 14Aug97            *****/

#include        <vxWorks.h>
#include	<types.h>
#include        <vme.h>
#include        <vxLib.h>
#include        <sysLib.h>
#include        <cacheLib.h>
#include        <stdioLib.h>
#include        <drvSup.h>

#include	<dbDefs.h>
#include	<link.h>

int DEBUGVMI3122 = 0;

/****************************************************************************
 * In a given VME crate, there can be at most 2 VMI3122 cards.
 * The VMI3122 has 64 16-bit Analog inputs
 *
 * To set up a VMI3122 for standard address 0xC00000 in supervisory
 * access with ADC conversions for -10 to +10 Volts and all inputs
 * differential:
 *    supervisory access:
 *      E13    put in both jumpers (pins 1-4)
 *    standard address space:
 *      E11    remove both jumpers (pins 1-4)
 *    address 0xC00000:
 *      E5     A24-A31 jumpers installed
 *      E8     A16-A21 jumpers installed
 *             A22 and A23 jumpers removed
 *      E6     A12-A15 installed
 *    voltage range -10 to +10 Volts
 *      E12    jumper installed across pins 2 and 3
 *             (Note: pin 1 is documented
 *             as a square in Fig 5.5-1 on page 5-9)
 *      E10    jumper removed
 *      E7     jumper installed across pins 1 and 2
 *             (Note: pin 1 is documented
 *             as a square in Fig 5.5-1 on page 5-9)
 *      E9     jumper removed
 *    differential inputs:
 *      E1     jumper removed
 *      E2     jumper removed
 ****************************************************************************/
#define CARDS          2
#define CHANS          64
#define REGS           5
 
#define C1BASEADDR     0xC00000
#define C1BASEADDR_BUF 0xC00080
#define C2BASEADDR     0xC01000
#define C2BASEADDR_BUF 0xC01080

unsigned short *vmiai_Add[CARDS][CHANS];
unsigned short *vmireg_Add[CARDS][REGS];

/* local prototypes */
long init_Vmi3122(void);

/* Global structures - Driver Entry Table */
struct drvet  drvVmi3122 = { 2,NULL,init_Vmi3122 };

/****************************************************************************
 * Init VMI3122
 ****************************************************************************/
long init_Vmi3122(void){
   int status, card, reg, chan;
   unsigned long addr;
   char *vmiai_addr;
   char *vmireg_addr;
   char testV = 0;
   long mbbid_addrs[CARDS][REGS] = {
            {C1BASEADDR+12, C1BASEADDR+4, C1BASEADDR+2, C1BASEADDR+6, C1BASEADDR+8},
            {C2BASEADDR+12, C2BASEADDR+4, C2BASEADDR+2, C2BASEADDR+6, C2BASEADDR+8}
          };
   long buf_addrs[CARDS] = {C1BASEADDR_BUF, C2BASEADDR_BUF};


   for ( card = 0 ; card < CARDS ; card++ ) {
      /* Loop through the configuration/control registers. */
      for ( reg = 0 ; reg < REGS ; reg++ ) {
         status=sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, 
                                  (char *)mbbid_addrs[card][reg], 
                                  &vmireg_addr);
         if (status != OK){
            printf("Addressing error in Vmi3122 driver Card %d Register #%d\n", card, reg);
            return ERROR;
         }
         if (vxMemProbe(vmireg_addr, VX_WRITE, 1, &testV) == OK) {
            vmireg_Add[card][reg] = (unsigned short *)vmireg_addr;
            if (DEBUGVMI3122)
               printf("Vmi3122 is present; card %d vmireg_Add = %p\n",card, vmireg_Add[card][reg]);
            if (reg == 0) *vmireg_Add[card][reg] = 0;       /* software reset -src  offset: 12 */
            if (reg == 1) *vmireg_Add[card][reg] = 0x5800;  /* configuration  -ccr  offset:  4 */
            if (reg == 2) *vmireg_Add[card][reg] = 0x8000;  /* control        -csr  offset:  2 */
            if (reg == 3) *vmireg_Add[card][reg] = 0x0000;  /* rate control   -rcr  offset:  6 */
            if (reg == 4) *vmireg_Add[card][reg] = 0xFF00;  /* interrupt con. -icr  offset:  8 */
         }
      }

      /* Loop through the readback channels. */
      addr = buf_addrs[card];
      for ( chan = 0 ; chan < CHANS ; chan++ ) {
         status = sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, 
                                    (char *)addr, 
                                     &vmiai_addr);
         if (status != OK) {
            if (DEBUGVMI3122)
               printf("Addressing error in Vmi3122 card %d channel #%d\n",card,chan);
	    return(ERROR);
         }
         vmiai_Add[card][chan] = (unsigned short *)vmiai_addr;
         if (vxMemProbe(vmiai_addr, VX_READ, 1, &testV) == OK) {
            if (DEBUGVMI3122)
               printf( "Vmi3122 card %d is present; vmiai_Add = %p\n", card, vmiai_Add[card][chan]);
         }
         addr += 2;
      }
   }

   return(0);

}


/******************************************************************************
* Write Vmi3122
*****************************************************************************/
long write_Vmi3122( unsigned short card,
                    unsigned short signal,
                    unsigned long *pval )
{
   long           status = ERROR;

   if (DEBUGVMI3122)
      printf("write_Vmi3122: card %hd signal %hd *pval %ld\n", card, signal, *pval);

     /* write new output */
   if ( (card<CARDS) && (signal<CHANS) && vmireg_Add[card][0] ) {
     *vmiai_Add[card][signal] = *pval;   /* implicit casting */
      status = OK;
   }
   return(status);
}

/******************************************************************************
* Read Vmi3122
*****************************************************************************/
long read_Vmi3122(unsigned short card,
                  unsigned short signal,
	          unsigned long *pval )
{
   unsigned short temp;
   long           status = ERROR;


   if ( (card<CARDS) && (signal<CHANS) && vmireg_Add[card][0] ) {
     cacheInvalidate(DATA_CACHE, vmiai_Add[card][signal], 2);
     temp = *vmiai_Add[card][signal];
     *pval = (unsigned long)temp;
     status = OK;
   }

   if (DEBUGVMI3122)
      printf("read_Vmi3122: card %hd signal %hd *pval %ld status=%ld\n",
              card, signal, *pval,status);
   return(status);
}
