/*
=============================================================

  Abs:  Include file for the VMIC 2534 DI/O VME Module

  Name: vmic2534.h

  Side: None

  Auth: 03-Mar-1994, Bill Brown       (USERNAME)
  Rev : dd-mmm-yyyy, Reviewer's Name  (USERNAME)

-------------------------------------------------------------
  Mod: 
        21-Feb-2001. K. Luchini (LUCHINI):
          moved prototypes from drvVmic2534.c to this file

=============================================================
*/
#ifndef VMIC_2534_H
#define VMIC_2534_H
                         
int  vmic2534_init( int card, u_long adrs );
int  vmic2534Init(void);
int  vmic2534IOScan(void);
int  vmic2534_getioscanpvt( short card, IOSCANPVT *scanpvt );
void vmic2534_io_report(int level);
void vmic2534_bi_io_report(int);
void vmic2534_bo_io_report(int);
void vmic2534Shutdown(int startType);

long  vmic2534_setIOBitDirection( short card, u_long mask, u_long direction );
long  vmic2534_bi_driver( short card,u_long mask,u_long *prval );
long  vmic2534_bo_read( short card,u_long mask,u_long *prval );
long  vmic2534_bo_driver( short card,u_long val,u_long  mask );
int   vmic2534_found( short card,u_long **pcard);

/* test routines */
int  vmiv2534Out( short	card, u_long  val );
int  vmic2534Write( short  card,u_long val );
int  vmic2534_readId( short card,u_char *pid);
int  vmic2534_readCsr( short card,u_char *pcsr);

#endif /* VMIC_2534_H    */
