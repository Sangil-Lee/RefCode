/****************************************************************************
 * kuCommon.h
 * --------------------------------------------------------------------------
 * common interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuCommon.h
 * \ingroup kutil
 * \brief   kutil 라이브러리 소스들의 공통 헤더 및 상수 정의
 * 
 * 본 파일은 kutil 라이브러리를 구성하는 소스들에서 공통으로 사용하는 
 * 시스템 또는, EPICS 헤더를 포함하고 매크로 상수를 정의한다.
 */

#ifndef __KUTIL_COMMON_H
#define __KUTIL_COMMON_H

#if !defined(WIN32)
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include <epicsStdio.h>
#include <epicsTime.h>
#include <epicsPrint.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>

#include <iocsh.h>
#include <errlog.h>
#include <shareLib.h>
#include <ellLib.h>

#include "kuTypes.h"

#if !defined(kuOK)
#define kuOK	0					///< 상태 또는, 처리 결과가 정상임
#endif

#if !defined(kuNOK)
#define kuNOK	-1					///< 상태 또는, 처리 결과가 정상이 아님
#endif

#define	KU_PATH_NAME_LEN	128		///< 경로명의 최대 길이
#define	KU_MAX_LINE_SIZE	1024	///< 한 줄 내의 최대 문자열 길이
#define	KU_MAX_RECORD_SIZE	8192	///< 파일 읽기/쓰기 단위
#define	KU_DIR_CREAT_MODE	0755	///< 디렉터리 생성 모드 : rwxr-xr-x

// gets size of array
#define	KU_DIM(a)	(sizeof(a) / sizeof(a[0]))	///< 1차원 배열 내의 데이터 개수 얻기

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \fn     kuInt32 kuDoubleEqual (double v1, double v2, double gap)
 * \brief  두 개의 실수 값이 오차 범위 이내에 있는지 비교
 * \param  v1     비교되는 실수 값 1
 * \param  v2     비교되는 실수 값 2
 * \param  gap    실수 값의 오차 범위
 * \return kuTRUE(같거나 오차 범위 이내), kuFALSE(두 실수 값의 차가 오차 범위를 초과함)
 * \see    kuInt32 kuStd::equal(T v1, T v2, T gap)
 *
 * 부동 소수점 연산의 경우, 그 표현 방식으로 인하여 오차가 발생할 수 있다. \n
 * 이로 인하여, 두 개의 부동 소수점을 '=' 연산으로 값이 같은지를 비교하는 것은 정확하지 않을 수 있다. \n
 * 따라서, 부동 소수점의 값이 같은지를 비교할 때는 오차지정 방식을 사용하여야 한다. \n
 * 즉, 두 값의 차가 오차 범위 내에 있는지를 확인 하여야 한다.
 */
kuShareFunc kuInt32 kuShareAPI kuDoubleEqual (double v1, double v2, double gap);

#ifdef __cplusplus
}
#endif

#endif	// __KUTIL_COMMON_H

