#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H

#include "shareLib.h"

#if defined(WIN32)
#define SfwShareFunc	epicsShareFunc
#else
#define SfwShareFunc	
#endif

#define LEV4LSB(x) ((x) & 0xf )
#define SFT4MSK4(x) (((x) >> 4) & 0xf )
#define SFT8MSK4(x) (((x) >> 8) & 0xf )
#define SFT12MSK4(x) (((x) >> 12) & 0xf )

#define LEV8LSB(x) ((x) & 0xff )
#define SFT8MSK8(x) (((x) >> 8) & 0xff )
#define SFT16MSK8(x) (((x) >> 16) & 0xff )
#define SFT24MSK8(x) (((x) >> 24) & 0xff )
#define SWAP16(x)	( (LEV8LSB(x) << 8) | SFT8MSK8(x) )
#define SWAP32(x)	(  (SFT24MSK8(x)) | (SFT16MSK8(x)<<8) | (SFT8MSK8(x)<<16) | ( LEV8LSB(x)<<24 ) ) 

#define HSWAP32(x)	( ( ((x) >> 16) & 0xffff) | (((x) << 16) & 0xffff0000 ))

#define PRINT_ERR()     printf("file:%s line:%d ERROR!!\n",__FILE__,__LINE__)

#ifndef MIN
#define MIN(x,y)  (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)  (((x) < (y)) ? (y) : (x))
#endif

#ifndef _PI_
#define _PI_	3.141592653589793
#endif

#define READ8(port)   (*((volatile unsigned char*)(port)))
#define READ16(port)  (*((volatile unsigned short int*)(port)))
#define READ32(port)  (*((volatile unsigned int*)(port)))


#define WRITE8( port, val )   (*((volatile unsigned char*)(port))=((unsigned char)(val)))
#define WRITE16( port, val )  (*((volatile unsigned short int*)(port))=((unsigned short int)(val)))
/* #define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(  ((val>>24)&0x000f) | ((val>>16)&0x00f0) | ((val<<16)&0x0f0) | ((val<<24)&0xf000)             )))
*/
#define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(val)))

#ifdef __cplusplus
extern "C" {
#endif


SfwShareFunc int wf_print_date();
SfwShareFunc void printlog(const char *fmt, ... );

SfwShareFunc void kfwPrint(int level, const char *fmt, ... );


SfwShareFunc unsigned long long int wf_getCurrentUsec(void);
SfwShareFunc double                 wf_intervalUSec(unsigned long long int start, unsigned long long int stop);


SfwShareFunc int env_SYS_pvname_assign(char *);
SfwShareFunc int env_MDS_Info_assign();
SfwShareFunc int env_Get_EPICS_ENV();


SfwShareFunc void notify_refresh_master_status();

/******************************************************************************
*
*  FUNCTION:   notify_error
*  PURPOSE:   error status update 
*  PARAMETERS: 
		level: 
			0-> just call scanIoRequest, 
			1-> both call scanIoRequest and print
			else -> just printf.
		*fmt:
******************************************************************************/
SfwShareFunc void notify_error(int level, const char *fmt, ...);

SfwShareFunc void admin_all_taskStatus_reset();

SfwShareFunc int admin_check_Arming_condition();
SfwShareFunc int admin_check_Release_condition();
SfwShareFunc int admin_check_Run_condition();
SfwShareFunc int admin_check_Stop_condition();

SfwShareFunc int check_dev_arming_condition(void *);
SfwShareFunc int check_dev_release_condition(void *);
SfwShareFunc int check_dev_run_condition(void *);
SfwShareFunc int check_dev_stop_condition(void *);

SfwShareFunc int master_is_locked();

SfwShareFunc void call_PRE_SEQSTART(double setValue);
SfwShareFunc void call_PRE_SEQSTOP(double setValue);
SfwShareFunc void call_POST_SEQSTOP(double setValue);
SfwShareFunc void call_PRE_SHOTSTART(double setValue);
SfwShareFunc void call_TREE_CREATE(double setValue);
SfwShareFunc void call_FAULT_IP_MINIMUM(double setValue);
SfwShareFunc void call_PLASMA_STATUS(double setValue);





/* it will be erased.  please don't use this function. Instead, use db_get, db_put*/
SfwShareFunc int DBproc_put(const char *pname, const char *pvalue);
SfwShareFunc int DBproc_get(const char *pname, char *pvalue);
/****************************************/
SfwShareFunc int db_put(const char *pname, const char *pvalue);
SfwShareFunc int db_get(const char *pname, char *pvalue);
#if 0
SfwShareFunc int CAproc_get(const char *pname, char *pvalue);
SfwShareFunc int CAproc_put(const char *pname, const char *pvalue);
#endif


SfwShareFunc int send_master_stop_event( );

SfwShareFunc void set_master_status_manually(unsigned int mask, unsigned int enable);
SfwShareFunc int get_status_auto_save(); /* not used */


/* This is called every time at MDS tree creation */
SfwShareFunc int reset_event_flag();  /* 2013. 8. 29 */

SfwShareFunc int get_plasma_duration_sec();  /* 2013. 5. 23 request by TGLEE */
SfwShareFunc int get_plasma_live_time();  /* 2013. 8. 28 */





#ifdef __cplusplus
}
#endif

#endif

