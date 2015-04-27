#ifndef __ETOS_TCP__
#define __ETOS_TCP__

#include <epicsEvent.h>

#define ETOS_PORT       50001

#define LAKE_1		0
#define LAKE_2		1
#define LAKE_3		2
#define LAKE_4		3
#define VG_FG		4
#define VG_PG		5
#define VG_CG		6
#define VG_ION		7
#define MFC			8
#define VG_4		9

#define MFC_SET_NEU		1
#define MFC_SET_ION		2

#define MFC_SET_NEU2	3
#define MFC_SET_ION2	4

#pragma pack(1)
typedef struct __LAKE {
    float val1[8];
    float val2[8];
    float val3[8];
    float val4[8];
    char  tail[2];
} tagLake;

typedef struct __VAC_G {
    float pg[7];
    float fg[7];
    float cg[4];
    float ion[2];
	float vg4[6];
    char  tail[2];
} tagVacG;

typedef struct __MFC {
    float val[4];
    char  status[4];
    char  tail[2];
} tagMfc;

typedef struct __MFC_SET {
    char head[2];
    float val;
    char tail[2];
} tagMfcSet;
#pragma pack()

extern int softtrigger;
extern epicsEventId softEvent;
extern int mfc_write;
extern tagLake lake;
extern tagVacG vg;
extern tagMfc  mfc;
extern tagMfcSet mfc_set_ion;
extern tagMfcSet mfc_set_neu;

extern tagMfcSet mfc_set_ion2;
extern tagMfcSet mfc_set_neu2;

#endif
