/****************************************************************************
 * MarteShmType.h
 * --------------------------------------------------------------------------
 * definition of data type used in shared memory
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.04.21  yunsw        Initial revision
 ****************************************************************************/

#ifndef __MARTE_SHM_TYPE_H
#define __MARTE_SHM_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#define	MARTE_SHM_TYPE_STRING_SIZE	32

enum {
	MARTE_SHM_TYPE_INT32	= 0,
	MARTE_SHM_TYPE_FLOAT	= 1,
	MARTE_SHM_TYPE_INT64	= 2,
	MARTE_SHM_TYPE_DOUBLE	= 3,
	MARTE_SHM_TYPE_STRING	= 4,
};

typedef union {
	int			ival;
	float		fval;
	long long	lval;
	double		dval;
	char		sval[32];
} MarteShmTypeUnion;

#if 0
// Use this when other data types wll be supported
#define	MARTE_SHM_TYPE_MAX_SIZE		sizeof(MarteShmTypeUnion)
#else
// Only int32 and float are supported in this version
#define	MARTE_SHM_TYPE_MAX_SIZE		sizeof(int)
#endif

#ifdef __cplusplus
}
#endif

#endif	// __MARTE_SHM_TYPE_H

