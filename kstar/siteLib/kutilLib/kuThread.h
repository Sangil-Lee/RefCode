/****************************************************************************
 * kuThread.h
 * --------------------------------------------------------------------------
 * debug kuInt32erface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuThread.h
 * \ingroup kutil
 * \brief   쓰레드 관리를 수행하는 클래스인 kuThread 정의
 * 
 * 본 파일은 쓰레드 관리를 수행하는 클래스인 kuThread를 정의한다.
 */

#ifndef __KUTIL_THREAD_H
#define __KUTIL_THREAD_H

#include "kuDebug.h"

#define	KU_THR_CLEANUP_DROP		0		///< KU_THR_POP_THREAD_CLEANUP 호출 시, CLEANUP 핸들러의 삭제
#define	KU_THR_CLEANUP_EXEC		1		///< KU_THR_POP_THREAD_CLEANUP 호출 시, CLEANUP 핸들러의 수행

#if !defined(WIN32)

// push cleanup handlers : system, user
#define	KU_THR_PUSH_THREAD_CLEANUP(handler, arg) \
	pthread_cleanup_push(handler, arg);				///< 사용자 CLEANUP 핸들러의 등록

// for the thread as RtThread sub class
#define	KU_THR_PUSH_THREAD_CLEANUP_C() \
	pthread_cleanup_push(kuThreadCleanup, this);	///< 기본 CLEANUP 핸들러의 등록

// pop cleanup handlers : match with push
#define	KU_THR_POP_THREAD_CLEANUP(execute) \
	pthread_cleanup_pop ((kuInt32)execute);			///< 등록된 CLEANUP 핸들러의 처리 (삭제 또는, 수행)
#else

typedef struct {
	void (*routine) (void *);
	void *arg;
} kuTheadCleanup_t;

#define pthread_cleanup_push( _rout, _arg ) \
	{ \
		kuTheadCleanup_t	_cleanup; \
		_cleanup.routine	= _rout; \
		_cleanup.arg		= _arg;

#define pthread_cleanup_pop( _execute ) \
		if( _execute) { \
			  (*(_cleanup.routine))( _cleanup.arg ); \
		} \
	}

// push cleanup handlers : system, user
#define	KU_THR_PUSH_THREAD_CLEANUP(handler, arg) \
	pthread_cleanup_push(handler, arg);				///< 사용자 CLEANUP 핸들러의 등록

// for the thread as RtThread sub class
#define	KU_THR_PUSH_THREAD_CLEANUP_C() \
	pthread_cleanup_push(kuThreadCleanup, this);	///< 기본 CLEANUP 핸들러의 등록

// pop cleanup handlers : match with push
#define	KU_POP_THREAD_CLEANUP(execute) \
	pthread_cleanup_pop ((kuInt32)execute); 

#endif

#ifdef __cplusplus

/**
 * \class   kuThread
 * \brief   쓰레드 관리를 수행하는 클래스
 * 
 * 본 클래스는 쓰레드의 생성 및 종료, 상태 검사 등의 기능을 제공 한다. \n
 */
 
class kuThread
{
public :

	#define KU_THR_NAME_LEN	 60						///< 쓰레드 이름의 최대 길이

	kuThread (const char *pszThrName = NULL);		///< 생성자. 애트리뷰트의 초기화

	virtual ~kuThread ();							///< 소멸자. 생성된 객체의 인스턴스 개수 감소

	/**
     * \fn     const kuThreadId start()
     * \brief  해당 객체를 위한 쓰레드 생성
     * \return 쓰레드 식별자 반환. 오류시는 NULL 또는, 0 을 반환
     *
     * 본 함수를 호출한 객체의 run() 함수를 호출하는 쓰레드를 생성한다. \n
     * 만일, 해당 객체에서 run() 함수를 구현하지 않은 경우는 NULL을 반환 한다. \n
     */
	const kuThreadId start ();

	/**
     * \fn     const kuThreadId start( char *pszThrName, void * (*thread)(void *), void *arg )
     * \brief  입력 함수를 수행하는 쓰레드 생성
     * \param  pszThrName	쓰레드의 이름
     * \param  thread  		쓰레드로 실행될 함수의 주소
     * \param  arg     		실행 쓰레드에 전달 될 인자의 주소
     * \return 쓰레드 식별자 반환. 오류시는 NULL 또는, 0 을 반환
     *
     * 입력 함수 \a thread 에 대한 쓰레드를 생성 하고, 입력 \a arg 를 해당 쓰레드의 인자로 전달한다.\n
     * 입력 \a thread 는, 함수의 주소 원형을 가져야 한다. \n
     * 본 함수는, static 함수로서 kuThread 또는, 그 상속 객체 없이 지정 함수에 대한 쓰레드를 생성할 수 있다.
     */
	static const kuThreadId start (char *pszThrName, void * (*thread)(void *), void *arg);

	/**
     * \fn     const kuBoolean stop ()
     * \brief  해당 객체의 쓰레드 종료
     * \return 쓰레드 종료 결과 반환. 
     * \return kuTRUE(정상 종료), kuFALSE(오류 발생)
     *
     * 본 함수를 호출한 객체의 쓰레드를 종료한다. \n
     */
	const kuBoolean stop ();
	
	/**
     * \fn     const kuBoolean stop (kuThreadId threadId)
     * \brief  해당 쓰레드의 종료 (미구현)
     * \param  threadId  종료 할 쓰레드의 식별자
     * \return kuTRUE(정상 종료), kuFALSE(오류 발생)
     *
     * 입력된 \a threadId 에 해당 하는 쓰레드를 종료한다. \n
     * 본 함수는, static 함수로서 kuThread 또는, 그 상속 객체 없이 지정 함수에 대한 쓰레드를 종료할 수 있다.
     */
	static const kuBoolean stop (kuThreadId threadId);

	/**
     * \fn     kuBoolean kill (const kuInt32 sigNo)
     * \brief  해당 객체의 쓰레드에 시그널 전송
     * \param  sigNo     쓰레드에 전송할 시그널 번호
     * \return kuTRUE(정상), kuFALSE(오류 발생)
     *
     * 본 함수를 호출한 객체의 쓰레드에게 입력 시그널 \a sigNo 를 전송한다. \n
     */
	const kuBoolean kill (const kuInt32 sigNo);

	/**
     * \fn     kuBoolean kill (kuThreadId threadId, const kuInt32 sigNo)
     * \brief  해당 쓰레드에 시그널 전송 (미구현)
     * \param  threadId  쓰레드의 식별자
     * \param  sigNo     쓰레드에 전송할 시그널 번호
     * \return kuTRUE(정상), kuFALSE(오류 발생)
     *
     * 입력된 \a threadId 에 해당 하는 쓰레드에게 입력 시그널 \a sigNo 를 전송한다. \n
     * 본 함수는, static 함수로서 kuThread 또는, 그 상속 객체 없이 지정 쓰레드에 대해 시그널을 전송할 수 있다.
     */
	static const kuBoolean kill (kuThreadId threadId, const kuInt32 sigNo);

	/**
     * \fn     void *run( void )
     * \brief  실행될 쓰레드의 함수 (몸체)
     * \return 쓰레드의 반환값에 대한 포인터
     *
     * 해당 객체의 쓰레드로 실행될 함수로서, 본 클래스를 상속 받은 객체에서 반드시 그 내용을 작성 하여야 한다. \n
     * 만일, 상속 받은 객체에서 본 함수를 구현 하지 않으면 NULL을 반환 한다. \n
     */
	virtual void *run (void);

	/**
     * \fn     void clean( void *arg)
     * \brief  쓰레드 종료 시 호출 되는 정리 함수
     * \param  arg   쓰레드에 전달할 인자의 주소
     * \return 없음
     *
     * 본 함수를 호출한 객체의 쓰레드가 종료될 때, 호출 되는 함수로서 자원 등의 해제등을 위해 사용될 수 있다. \n
     * 만일, 상속 받은 객체에서 본 함수를 구현 하지 않으면 kuThread 기본 함수가 수행 된다. \n
     */
	virtual void clean( void *arg);

	/**
     * \fn     kuInt32 isAlive()
     * \brief  쓰레드의 정상 실행 여부
     * \return kuTRUE(정상), kuFALSE(오류 발생)
     *
     * 해당 객체의 쓰레드가 정상적으로 실행 중인지를 반환한다.
     */
	const kuInt32 isAlive();

	/**
     * \fn     const kuInt32 isAlive( kuThreadId threadId )
     * \brief  쓰레드의 정상 실행 여부
     * \param  threadId  쓰레드의 식별자
     * \return kuTRUE(정상), kuFALSE(오류 발생)
     *
     * 입력된 \a threadId 에 해당 하는 쓰레드가 정상적으로 실행 중인지를 반환한다.
     */
	static const kuInt32 isAlive( kuThreadId threadId );

	/**
     * \fn     const kuThreadId getThreadId()
     * \brief  해당 객체의 쓰레드 식별자 얻기
     * \return 쓰레드 식별자
     *
     * 해당 객체의 쓰레드 식별자를 반환한다.
     */
	inline const kuThreadId getThreadId() { return( m_tid ); };

	/**
     * \fn     const kuInt32 getPriority()
     * \brief  해당 객체의 쓰레드의 우선순위 얻기
     * \return 쓰레드의 우선순위
     *
     * 해당 객체의 쓰레드 우선순위를 반환한다.
     */
	inline const kuInt32 getPriority() { return( m_priority ); };

	/**
     * \fn     void setPriority( const kuInt32 priority )
     * \brief  해당 객체의 쓰레드 우선순위 설정 (미구현)
     * \param  priority  쓰레드의 우선순위
     * \return 없음
     *
     * 해당 객체의 쓰레드 우선순위를 설정한다. (미구현)
     */
	void setPriority( const kuInt32 priority );

	/**
     * \fn     const kuInt32 getActiveCnt()()
     * \brief  해당 클래스의 쓰레드 객체 개수 얻기
     * \return 쓰레드 식별자
     *
     * 해당 클래스의 쓰레드 객체 개수를 반환한다.
     */
	inline static const kuInt32 getActiveCnt() { return( m_activeCnt ); };

	/**
     * \fn     const char * getName()
     * \brief  해당 객체의 쓰레드 이름 얻기
     * \return 쓰레드 이름
     *
     * 해당 객체의 쓰레드 이름을 반환한다.
     */
	inline const char * getName() { return( m_thrName ); };

	// -----------------------------------------------------
	// friend function
	// -----------------------------------------------------

	/**
     * \fn     void *kuThreadStartup (void *tgtObject)
     * \brief  해당 객체의 쓰레드 생성
     * \param  tgtObject  kuThread 객체
     * \return 해당 객체의 run 함수 실행 결과 반환
     *
     * 입력으로 받은 kuThread 객체 \a tgtObject 의 run 함수를 이용하여 쓰레드를 생성한다.
     */
	friend void *kuThreadStartup (void *tgtObject);

	/**
     * \fn     void *kuThreadCleanup (void *tgtObject)
     * \brief  해당 객체의 CLEANUP 핸들러 호출
     * \param  tgtObject  kuThread 객체
     * \return 해당 객체의 clean 함수 실행 결과 반환
     *
     * 입력으로 받은 kuThread 객체 \a tgtObject 의 clean 함수를 이용하여 쓰레드의 CLEANUP 핸들러를 호출한다.
     */
	friend void kuThreadCleanup (void *tgtObject);

private :

	char			m_thrName[KU_THR_NAME_LEN];		///< 쓰레드 이름
	kuThreadId		m_tid;							///< 쓰레드 식별자
	kuInt32			m_priority;						///< 쓰레드 우선순위
	static kuInt32	m_activeCnt;					///< 쓰레드 인스턴스의 개수
	kuBoolean		m_isAlive;						///< 쓰레드의 실행 여부
};

#endif	// __cplusplus

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// __KUTIL_THREAD_H

