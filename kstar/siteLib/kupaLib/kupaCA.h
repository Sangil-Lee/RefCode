///////////////////////////////////////////////////////////
//  kupaCA.h
//  Implementation of the Class kupaCA
//  Created on:      23-4-2013 오후 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaCA.h
 * \ingroup kupa
 * \brief   EPICS CA (Channel Access) 관련 Callback 함수 정의
 * 
 * 본 파일은, EPICS의 PV 값을 취득하기 위하여 사용하는 Channel Access 관련한 Callback 함수들을 정의한다.
 */

#ifndef __KUPA_CA_H
#define __KUPA_CA_H

#include <stdio.h>

#define	KUPA_CA_PRIORITY		20		///< 우선순위 레벨 (0 ~ 99). ca_create_channel()에서 사용됨

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*kupaConnectionHandler) (struct connection_handler_args args);	///< CA 연결 처리용 사용자 콜백 함수 타입 정의
typedef void (*kupaEventHandler) (struct event_handler_args eha);				///< CA 이벤트처리용 사용자 콜백 함수 타입 정의

void kupaExceptionCallback (struct exception_handler_args args);		///< CA 예외 처리를 위한 콜백 함수
void kupaConnectionCallback (struct connection_handler_args args);		///< CA 연결 처리를 위한 콜백 함수
void kupaAccessRightsCallback (struct access_rights_handler_args args);	///< CA 접근권한 오류 처리를 위한 콜백 함수
void kupaEventCallback (struct event_handler_args eha);					///< CA 이벤트의 비동기 처리를 위한 콜백 함수
void kupaTriggerCallback (struct event_handler_args eha);				///< CA 이벤트 콜백 함수로서, 내부 처리용 PV들의 이벤트들을 처리함 (OpMode, ShotNum, Arming, SysRun)

#ifdef __cplusplus
}
#endif

#endif	// __KUPA_CA_H
