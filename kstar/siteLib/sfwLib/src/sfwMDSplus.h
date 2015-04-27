#ifndef _SFW_MDS_PLUS_H
#define _SFW_MDS_PLUS_H

#include <math.h>
#include <mdslib.h>
#include "sfwDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

/* for C++  not support MdsGetMsg() 2013. 9. 10
#include <mdsdescrip.h>
#include <mdsshr.h>
*/

#define MDS_INIT                      0x0
#define MDS_CONNECTED          ( 0x01<<0)
#define MDS_OPEN_OK              ( 0x01<<1)
 

SfwShareFunc int mds_Connect(ST_STD_device *);
SfwShareFunc int mds_Connect_master(ST_MASTER *);
SfwShareFunc int mds_Disconnect(ST_STD_device *);
SfwShareFunc int mds_Disconnect_master(ST_MASTER *);
SfwShareFunc int mds_Open(ST_STD_device *);
SfwShareFunc int mds_Close(ST_STD_device *);
SfwShareFunc int mds_Open_withFetch(ST_STD_device *);
SfwShareFunc int mds_Close_withFetch(ST_STD_device *);
SfwShareFunc int mds_createNewShot(ST_STD_device *);
SfwShareFunc int mds_createNewShot_master(ST_MASTER *);
SfwShareFunc int mds_notify_EndOfTreePut(ST_STD_device *);
SfwShareFunc int mds_copy_master_to_STD(ST_STD_device *);
SfwShareFunc int env_MDSinfo_to_STDdev(ST_STD_device *, char *, char *);

SfwShareFunc int fnc_open(ST_STD_device *, int );
SfwShareFunc int fnc_close(ST_STD_device *, int );


#ifdef __cplusplus
}
#endif

#endif



