/****************************************************************************
 * kuDebug.h
 * --------------------------------------------------------------------------
 * debug interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuDebug.h
 * \ingroup kutil
 * \brief   디버그 메시지 출력을 지원하는 함수 제공
 * 
 * 본 파일은 프로그램에서 사용하는 디버그 메시지의 출력을 관리하기 위한 API를 제공한다.  \n
 * 즉, 프로그램에서 출력 하고자 하는 디버그 메시지 각각에 대해 디버그 레벨을 지정하고, 
 * 출력 레벨을 변경함으로써 외부로의 출력을 허용하거나 제약할 수 있도록 하는 API를 제공한다. \n
 * 또한, 실제 운용 중에 프로그램을 종료하고 재컴파일 하는 등의 작업 없이, 
 * iocShell의 변수 <b>kuPrintLevel</b> 및 함수 <b>kuSetPrintLevel()</b>를 통해 
 * 온라인 상에서 프로그램의 출력 메시지를 제어할 수 있다.
 * 또한, iocShell의 변수 <b>kuLogLevel</b> 및 함수 <b>kuSetLogLevel()</b>를 통해 
 * 온라인 상에서 프로그램 출력 메시지의 로그 파일로의 저장을 제어할 수 있다.
 *
 * \see     kuTime.h
 */

#ifndef __KUTIL_DEBUG_H
#define __KUTIL_DEBUG_H

#include "kuCommon.h"
#include "kuTime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum   kuDebugLevel_e
 * \brief  디버그 메시지 레벨 상수 정의
 * 
 * 디버그 메시지의 레벨은, 각 디버그 메시지별로 출력 되어야 할 수위를 지정한다. \n
 * 디버그 메시지의 레벨은, 0 ~ 10 사이의 값을 사용할 수 있다. \n
 * 화면으로의 출력 레벨 (kuPrintLevel) 과 로그 파일로의 출력 레벨 (kuLogLevel)은 별도로 설정.관리 된다. \n
 * \n
 * 여기에서는 애플리케이션에서 주로 사용할 수 있는 디버그 유형 별로 상수를 정의한다. \n
 * 참고로, 디버그 메시지 레벨의 숫자가 작을 수록 우선순위가 높은 것으로 본다.
 */
typedef enum {
	kuOFF	= 0,		///< turns off logging
	kuFATAL	= 1,		///< very severe error events that will presumably lead the application to abort
	kuERR	= 2,		///< error events that might still allow the application to continue running
	kuWARN	= 3,		///< potentially harmful situations
	kuMON	= 4,		///< monitoring events
	kuINFO	= 5,		///< informational messages that highlight the progress of the application at coarse-grained level
	kuDEBUG	= 6,		///< fine-grained informational events that are most useful to debug an application
	kuTRACE	= 7,		///< finer-grained informational events than the DEBUG
	kuDATA	= 8,		///< logging any content of file, comminucation packet and so on
	kuDEL	= 9,		///< most detailed informational events and can be deleted later
	kuALL	= 10		///< turn on all logging
} kuDebugLevel_e;

/**
 * \fn     void kuDebug (const kuInt32 nDebugLevel, const char *fmt, ... )
 * \brief  특정 디버그 레벨을 갖는 메시지의 출력
 * \param  nDebugLevel	디버그 메시지의 레벨
 * \param  fmt			메시지의 출력 형식, printf() 함수에서의 format과 동일함
 * \param  ...			가변 인수. fmt에 따름
 * \return 없음
 * \see	   kuSetPrintLevel, kuSetLogLevel
 *
 * 지정한 디버그 레벨 \a nDebugLevel 을 갖는 디버그 메시지를 출력한다. \n
 * 디버그 레벨 \a nDebugLevel 을 제외 하고는, C 라이브러리인 printf() 함수와 동일한 기능과 사용법을 갖는다. \n
 * 본 함수를 이용하여 출력을 요청한 디버그 메시지는, 
 * 해당 메시지의 디버그 레벨과 출력 레벨에 따라 실제 외부로의 출력(표시) 여부가 결정된다. \n
 * 즉, 디버그 레벨이 출력 레벨의 우선 순위 이상인 경우, 실제 외부로 출력 된다. \n
 */
kuShareFunc void kuShareAPI kuDebug (const kuInt32 nDebugLevel, const char *fmt, ... );

/**
 * \fn     int kuGetPrintLevel ()
 * \brief  애플리케이션 디버그 메시지의 화면 출력 레벨 얻기
 * \return 애플리케이션 디버그 메시지의 화면 출력 레벨
 * \see    kuSetPrintLevel
 */
kuShareFunc int kuShareAPI kuGetPrintLevel ();

/**
 * \fn     void kuSetPrintLevel (const int nPrintLevel)
 * \brief  애플리케이션 디버그 메시지의 화면 출력 레벨 지정
 * \param  nPrintLevel	애플리케이션 디버그 메시지의 화면 출력 레벨 (0 ~ 10)
 * \return 없음
 * \see    kuGetPrintLevel
 *
 * 본 함수는, 화면으로의 출력 레벨 값을 저장하기 위한 내부 전역변수 \c kuPrintLevel의 값을 설정한다. \n 
 * 변수 \c kuPrintLevel은 EPICS iocShell의 \c var 명령을 통해서도 설정이 가능하다.
 */
kuShareFunc void kuShareAPI kuSetPrintLevel (const int nPrintLevel);

/**
 * \fn     int kuCanPrint (const int nDebugLevel)
 * \brief  입력 디버그 메시지 레벨의 화면 출력 가능 여부 얻기
 * \param  nDebugLevel	디버그 메시지의 레벨 
 * \return kuTRUE(출력이 허용됨), kuFALSE(출력이 제한됨)
 * \see    kuGetPrintLevel, kuSetPrintLevel
 */
kuShareFunc int kuShareAPI kuCanPrint (const int nDebugLevel);

/**
 * \fn     int kuGetLogLevel ()
 * \brief  애플리케이션 디버그 메시지의 파일 저장 레벨 얻기
 * \return 애플리케이션 디버그 메시지의 파일 저장 레벨
 * \see    kuSetLogLevel
 */
kuShareFunc int kuShareAPI kuGetLogLevel ();

/**
 * \fn     void kuSetLogLevel (const int nLogLevel)
 * \brief  애플리케이션 디버그 메시지의 파일 저장 레벨 지정
 * \param  nLogLevel	애플리케이션 디버그 메시지의 파일 저장 레벨 (0 ~ 10)
 * \return 없음
 * \see    kuGetLogLevel
 *
 * 본 함수는, 로그 파일로의 저장 레벨 값을 저장하기 위한 내부 전역변수 \c kuLogLevel의 값을 설정한다. \n 
 * 변수 \c kuLogLevel은 EPICS iocShell의 \c var 명령을 통해서도 설정이 가능하다. \n
 * 이와 함께, 디버그 메시지가 저장될 로그 파일의 경로명은 내부 전역변수 \c kuLogFileName에 저장된다. \n
 * \c kuLogLevel에서와 같이, 변수 \c kuLogFileName는 EPICS iocShell의 \c var 명령을 통해서 설정이 가능하다.
 */
kuShareFunc void kuShareAPI kuSetLogLevel (const int nLogLevel);

#ifdef __cplusplus
}
#endif

#endif	// __KUTIL_DEBUG_H

