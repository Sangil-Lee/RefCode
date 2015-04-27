/****************************************************************************
 * kuLock.h
 * --------------------------------------------------------------------------
 * lock interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.05.13  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuLock.h
 * \ingroup kutil
 * \brief   록을 지원하는 kuLock 클래스 정의
 * 
 * 본 파일은 쓰레드 간에 공유되는 자원 - 메모리 또는, 처리 등의 임계영역 - 을
 * 배타적으로 사용하기 위한 록을 제공하는 클래스인 kuLock 을 정의한다.
 */

#ifndef _KU_LOCK_H
#define _KU_LOCK_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <iostream>
#include <string>

using namespace std;

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuCommon.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

/**
 * \class   kuLock
 * \brief   쓰레드 또는 프로세스간 록을 제공하는 클래스
 * 
 * kuLock 은 쓰레드간에 공유되는 자원 – 메모리 또는 처리 등의 임계영역 - 을 
 * 배타적으로 사용하기 위한 록을 제공하는 클래스이다.  \n
 * 따라서, 임계영역으로 들어가기 전에 사전 약속된 록을 설정하게 되면, 
 * 다른 쓰레드 또는 프로세스에서 해당 임계영역으로 들어갈 수가 없고 가능할 때까지 블록 되게 된다. \n
 * 이렇게 록을 설정함으로써 자원에 대해 배타적으로 접근할 수 있다. \n
 * 내부적으로 EPICS에서 제공하는 뮤텍스 API를 사용한다.
 */
 
class kuLock
{
public:

	/**
     * \fn     kuLock (const char *pszLockName)
     * \brief  생성자
     * \param  pszLockName  록 이름
     * \return 없음
     * \see    create
     *
     * 애트리뷰트 초기화를 수행하고, EPICS의 epicsMutexCreate 함수를 이용하여 뮤텍스 록를 생성한다. \n
     */
	kuLock (const char *pszLockName);

	~kuLock ();		///< 소멸자. EPICS의 뮤텍스 록을 제거한다.

	/**
     * \fn     kuInt32 lock ()
     * \brief  록 설정
     * \return epicsMutexLock() 함수를 이용한 록 설정 결과 반환
     * \see    create
     *
     * create를 통해 생성된 EPICS 뮤텍스 록의 ID를 이용하여 록을 설정한다.
     */
	const kuInt32 lock ();

	/**
     * \fn     kuInt32 unlock ()
     * \brief  록 해제
     * \return epicsMutexUnlock() 함수를 이용한 록 해제 결과 반환
     * \see    destroy
     *
     * create를 통해 생성된 EPICS 뮤텍스 록의 ID를 이용하여 설정된 록을 해제한다.
     */
	const kuInt32 unlock ();

	/**
     * \fn     void setLockName (string newVal)
     * \brief  록 이름 설정
     * \param  newVal  록 이름
     * \return 없음
     * \see    getLockName
     *
     * 록 이름은 록 객체를 식별하기 위한 것으로, 이름의 설정은 선택 사항이다. \n
     */
	void setLockName (string newVal) { m_lockName = newVal; }

	/**
     * \fn     string getLockName ()
     * \brief  록 이름 얻기
     * \return 록 이름 반환
     * \see    setLockName
     */
	string getLockName () { return (m_lockName); }

	/**
     * \fn     kuInt32 lock (epicsMutexId id, const char *name)
     * \brief  외부에서 생성된 EPICS 뮤텍스 록의 ID를 이용한 록 설정
     * \param  id  록 ID
     * \param  name  록 이름
     * \return 록 설정 결과 반환
     */
	static const kuInt32 lock (epicsMutexId id, const char *name = NULL);

	/**
     * \fn     kuInt32 unlock (epicsMutexId id, const char *name)
     * \brief  외부에서 생성된 EPICS 뮤텍스 록의 ID를 이용한 록 해제
     * \param  id  록 ID
     * \param  name  록 이름
     * \return 록 해제 결과 반환
     */
	static const kuInt32 unlock (epicsMutexId id, const char *name = NULL);

	/**
     * \fn     epicsMutexId getLockId ()
     * \brief  록 ID 얻기
     * \return 록 ID 반환
     * \see    setLockId
     */
	epicsMutexId getLockId ();

	/**
     * \fn     void setLockId (epicsMutexId id)
     * \brief  록 ID 설정
     * \param  id  설정할 록 ID
     * \return 없음
     * \see    getLockId
     */
	void setLockId (epicsMutexId id);

private:

	const kuInt32 create ();		///< epicsMutexLock() 함수를 이용한 EPICS 뮤텍스 록 생성
	const kuInt32 destroy ();		///< epicsMutexUnlock() 함수를 이용한 EPICS 뮤텍스 록 제거

	epicsMutexId	m_lockId;		///< EPICS의 뮤텍스 록 ID
	string			m_lockName;		///< 록 객체의 이름
	kuBoolean		m_bCreated;		///< EPICS 뮤텍스 록의 생성 여부

};

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// _KU_LOCK_H

