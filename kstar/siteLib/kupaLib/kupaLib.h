///////////////////////////////////////////////////////////
//  kupaLib.h
//  Interface for C program
//  Created on:      26-4-2013 ¿H 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaLib.h
 * \ingroup kupa
 * \brief   EPICS PV 값 저장을 위한 C 함수 정의
 * 
 * 본 파일은, EPICS의 PV 값을 취득하여 CSV와 MDSplus에 저장하는
 * kupa 라이브러리의 기능을 제공하는 것으로 C 함수를 정의한다. \n
 * 여기에서 제공하는 C 함수들은, kupa 라이브러리의 C++ 클래스들을 호출하는 방식으로 구현 되었다. \n
 */

#ifndef __KUPA_TREE_H
#define __KUPA_TREE_H

#include <stdio.h>
#include "kutil.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \fn     void kupaStart ()
 * \brief  EPICS PV 값의 취득 시작
 * \return 없음
 *
 * kupaSetShotInfo 등의 함수를 통한 설정 이후, 그 설정에 따라 EPICS PV 값의 취득 기능을 시작한다.
 */
void kupaStart ();

/**
 * \fn     void kupaSetShotInfo (int opMode, int shotNum, int samplingRate, 
 * 									double startTime, double duration, double delaySec)
 * \brief  EPICS PV 저장 기능의 설정
 * \param  opMode			제어시스템 (또는, IOC 서버)의 운전 모드. 예) 원격 또는, 로컬
 * \param  shotNum			PV 값을 저장할 MDS 트리의 Shot 번호
 * \param  samplingRate		취득 주기 (Hz)
 * \param  startTime		취득 데이터의 시작 시각 (초)
 * \param  duration			취득 시간 (초)
 * \param  delaySec			취득 대기 시간 (초)
 * \return 없음
 *
 * EPICS PV 저장 기능의 수행 관련 정보들을 설정한다.
 */
void kupaSetShotInfo (int opMode, int shotNum, int samplingRate, double startTime, double duration, double delaySec);

/**
 * \fn     void kupaSetOpMode (int opMode)
 * \brief  EPICS PV 저장 기능의 운전 모드 설정
 * \param  opMode			제어시스템 (또는, IOC 서버)의 운전 모드. 예) 원격 또는, 로컬
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetOpMode (int opMode);

/**
 * \fn     void kupaSetShotNum (int shotNum)
 * \brief  EPICS PV 저장 기능의 Shot 번호 설정
 * \param  shotNum			PV 값을 저장할 MDS 트리의 Shot 번호
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetShotNum (int shotNum);

/**
 * \fn     void kupaSetSamplingRate (int samplingRate)
 * \brief  EPICS PV 저장 기능의 취득 주기 설정
 * \param  samplingRate		취득 주기 (Hz)
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetSamplingRate (int samplingRate);

/**
 * \fn     void kupaSetStartTime (double startTime)
 * \brief  EPICS PV 저장 기능의 운전 모드 설정
 * \param  startTime		취득 데이터의 시작 시각 (초)
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetStartTime (double startTime);

/**
 * \fn     void kupaSetDuration (double duration)
 * \brief  EPICS PV 저장 기능의 운전 모드 설정
 * \param  duration			취득 시간 (초)
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetDuration (double duration);

/**
 * \fn     void kupaSetDelaySec (double delaySec)
 * \brief  EPICS PV 저장 기능의 취득 대기 시간 설정
 * \param  delaySec			취득 대기 시간 (초)
 * \return 없음
 * \see    kupaSetShotInfo
 */
void kupaSetDelaySec (double delaySec);

/**
 * \fn     double kupaGetValue (char *pszPvName)
 * \brief  입력 PV 의 마지막 값 반환
 * \param  pszPvName		EPICS PV명
 * \return 없음
 * \see    kupaStart, kupaUpdateValues
 *
 * 입력으로 받은 이름에 해당하는 PV의 내부 값을 반환한다. \n 
 * 반환되는 내부 값은 저장 기능의 수행 (kupaStart) 또는, kupaUpdateValues() 함수의 호출을 통해 취득된 마지막 값을 나타낸다.
 */
double kupaGetValue (char *pszPvName);

/**
 * \fn     void kupaUpdateValues ()
 * \brief  명시적인 PV 값 갱신
 * \return 없음
 *
 * iocShell을 통해 제공되는 kupaAddPV()를 이용하여 등록된 PV들의 값을 현재 값으로 갱신한다. \n 
 * 이는, kupaStart() 호출 시 취득 기간동안 설정 주기로 PV 값이 내부에서 자동으로 갱신 되는데 반해,
 * kupaStart() 함수가 호출되기 이전이라도 갱신된 값을 얻고자 할 때 사용될 수 있다. \n 
 * 즉, kupaStart()가 호출되지 않아 데이터 취득이 미 수행인 상태에서 본 함수를 호출한 뒤 등록된 PV의 최신 값을 얻을 수 있다.
 */
void kupaUpdateValues ();

/**
 * \fn     void kupaSetLockId (epicsMutexId id)
 * \brief  록 ID 설정
 * \param  id				설정할 록 ID
 * \return 없음
 * \see    kupaGetLockId
 *
 * mdslib.h에서 제공하는 API들은 MDSplus 서버와의 단일 연결만을 지원한다. \n 
 * 이에 따라, 동일 프로그램 내의 여러 쓰레드에서 동시에 MDSplus 서버에 대한 연결을 맺는 경우, 연결 정보의 혼선으로 인해 오류가 발생할 수 있다. \n 
 * 이를 해결하기 위하여, MDSplus 서버와의 연결을 필요로 하는 쓰레드들 사이의 동기화를 위하여 MDSplus 접근을 제어하기 위한 록을 공유할 필요가 있다. \n
 * 따라서, 본 함수는 외부에서 생성된 록을 내부에 설정함으로써, 다른 쓰레드와의 동기화된 처리를 수행할 수 있도록 한다.
 */
void kupaSetLockId (epicsMutexId id);

/**
 * \fn     epicsMutexId kupaGetLockId ()
 * \brief  내부의 록 ID 얻기
 * \return 록 ID
 * \see    kupaSetShotInfo
 *
 * 본 함수는, 내부에서 생성된 록 ID를 외부에서 사용할 수 있도록 제공하는 기능을 수행한다.
 */
epicsMutexId kupaGetLockId ();

/**
 * \fn     int kupaMdsPut (char *szTreeName, char *szMdsAddr, int nShotNum,
 *  						char *szTagName, char *szUnit, kuFloat64 *pValues, 
 *							int nSamples, kuFloat64 fStartTime, kuFloat64 fSamplingRate)
 * \brief  MDSplus에 데이터 저장
 * \param  szTreeName		트리명
 * \param  szMdsAddr		MDSplus 서버의 주소
 * \param  nShotNum			Shot 번호
 * \param  szTagName		노드명 또는, 태그명
 * \param  szUnit			단위
 * \param  pValues			저장할 데이터의 주소
 * \param  nSamples			저장할 데이터의 개수
 * \param  fStartTime		저장할 데이터의 시작 시각 (초)
 * \param  fSamplingRate	저장할 데이터의 주기 (Hz)
 * \return 성공(kuOK), 실패(kuNOK)
 *
 * 본 함수는, MDSplus에 데이터를 저장하기 위한 기능으로서 다른 함수들과는 독립적으로 수행된다. \n
 * 즉, 입력으로 받은 MDSplus 서버에 대한 연결을 이용하여 해당 트리에 데이터를 저장한다. 
 */
int kupaMdsPut (char *szTreeName, char *szMdsAddr, int nShotNum,
	char *szTagName, char *szUnit, kuFloat64 *pValues, int nSamples, kuFloat64 fStartTime, kuFloat64 fSamplingRate);

#ifdef __cplusplus
}
#endif

#endif	// __KUPA_LIB_H

