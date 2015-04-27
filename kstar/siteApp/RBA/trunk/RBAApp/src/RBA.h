#ifndef _RBA_H
#define _RBA_H

typedef enum {
	AMP_RESET		= 0,
	AMP_READY		= 1,
	AMP_STARTED		= 2,
	AMP_ERROR		= 3
} RBA_AMP_STAT;

typedef enum {
	DIG_RESET		= 0,
	DIG_STARTED		= 1,
	DIG_ACQUIRING	= 2,
	DIG_ERROR		= 3
} RBA_DIG_STAT;

typedef enum {
	MDS_NORMAL		= 0,
	MDS_ARCHIVING	= 1,
	MDS_FILE_ERR	= 2,
	MDS_CONN_ERR	= 3
} RBA_MDS_STAT;

// Global variable for IOC
typedef struct {
	int			amp_stat;				// 0(Reset) 1(Started) 2(Error)
	int			dig_stat;				// 0(Reset) 1(Started) 2(Acquiring) 3(Error)
	int			mds_stat;				// 0(Normal) 1(Archiving) 2(File err) 3(Connection err)
} ST_IOC_VAR;

extern ST_IOC_VAR	gIocVar;

extern int get_ioc_amp_stat ();
extern void set_ioc_amp_stat (int stat);

extern int get_ioc_dig_stat ();
extern void set_ioc_dig_stat (int stat);

extern int get_ioc_mds_stat ();
extern void set_ioc_mds_stat (int stat);

#endif	// _RBA_H
