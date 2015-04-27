#ifndef _SFW_MDS_PLUS_H
#define _SFW_MDS_PLUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sfwAdminHead.h"


#define MDS_INIT                      0x0
#define MDS_CONNECTED          ( 0x01<<0)
#define MDS_OPEN_OK              ( 0x01<<1)
 

int mds_Connect(ST_STD_device *);
int mds_Disconnect(ST_STD_device *);
int mds_Open(ST_STD_device *);
int mds_Close(ST_STD_device *);
int mds_Open_withFetch(ST_STD_device *);
int mds_Close_withFetch(ST_STD_device *);
int mds_createNewShot(ST_STD_device *);
int mds_notify_EndOfTreePut(ST_STD_device *);
int mds_assign_from_Env(ST_STD_device *);
int callFunc_set_MDSplus(ST_STD_device *, char *, char *);

int fnc_open(ST_STD_device *, int );
int fnc_close(ST_STD_device *, int );


#ifdef __cplusplus
}
#endif

#endif



