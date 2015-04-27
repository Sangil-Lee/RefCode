/****************************************************************************
 * kuTypes.h
 * --------------------------------------------------------------------------
 * types for kutil library
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.05.12  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuTypes.h
 * \ingroup kutil
 * \brief   kutil 라이브러리에서 사용하는 데이터 유형 정의
 * 
 * 프로그램의 이식성을 고려하여 kutil 라이브러리 자체 데이터 유형을 정의한다.
 */

#ifndef __KUTIL_TYPES_H
#define __KUTIL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef epicsInt8		kuInt8;			///< epicsInt8 재 정의
typedef epicsUInt8		kuUInt8;		///< epicsUInt8 재 정의
typedef epicsInt16		kuInt16;		///< epicsInt16 재 정의
typedef epicsUInt16		kuUInt16;		///< epicsUInt16 재 정의
typedef epicsEnum16		kuEnum16;		///< epicsEnum16 재 정의
typedef epicsInt32		kuInt32;		///< epicsInt32 재 정의
typedef epicsUInt32		kuUInt32;		///< epicsUInt32 재 정의
typedef long int		kuInt64;		///< long int 재 정의
typedef unsigned long	kuUInt64;		///< unsigned long 재 정의
typedef epicsFloat32	kuFloat32;		///< epicsFloat32 재 정의
typedef epicsFloat64	kuFloat64;		///< epicsFloat64 재 정의

// union of all types
typedef epicsAny		kuAny;			///< epicsAny 재 정의

typedef epicsThreadId	kuThreadId;		///< epicsThreadId 재 정의
typedef epicsTimeStamp	kuTimeStamp;	///< epicsTimeStamp 재 정의

#define	kuShareFunc		epicsShareFunc	///< epicsShareFunc 재 정의
#define kuShareAPI		epicsShareAPI	///< epicsShareAPI 재 정의

/**
 * \enum   kuBoolean
 * \brief  kutil 라이브러리용 boolean 타입 정의
 */
typedef enum {
	kuFALSE		= 0,		///< 거짓
	kuTRUE		= 1			///< 참
} kuBoolean;

#ifdef __cplusplus
}
#endif

#endif	// __KUTIL_TYPES_H

