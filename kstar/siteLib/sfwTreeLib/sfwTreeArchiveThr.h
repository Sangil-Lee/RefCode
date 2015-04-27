/****************************************************************************
 * sfwTreeArchiveThr.h
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.08.17  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    sfwTreeArchiveThr.h
 * \ingroup sfwtree
 * \brief   MDSplus로의 데이터 저장을 수행하는 클래스인 sfwTreeArchiveThr 정의
 * 
 * 본 파일은 사용자 애플리케이션으로부터 데이터를 받아 MDSplus에 저장하는 클래스인 sfwTreeArchiveThr 을 정의한다.
 */
 
#ifndef __SFW_TREE_ARCHIVE_THR_H
#define __SFW_TREE_ARCHIVE_THR_H

#include "kuThread.h"
#include "sfwTreeObj.h"

class sfwTree;
class sfwTreeNode;

/**
 * \class   sfwTreeArchiveThr
 * \brief   MDSplus로의 데이터 저장을 수행하는 클래스
 * 
 * 본 클래스는 사용자 애플리케이션으로부터 받은 데이터를 MDSplus에 저장하는 쓰레드를 위한 클래스로서,
 * 사용자 애플리케이션의 요청을 수행한다. \n
 * 사용자 애플리케이션은 C 함수 또는, C++ 클래스를 이용하여
 * 데이터 저장의 시작 및 종료, 데이터의 저장, 데이터의 변경을 알리기 위한 이벤트 전송 등을 요청한다. \n
 * sfwTreeArchiveThr는 이러한 요청에 따른 메시지를 수신하고, 각 요청에 대한 서비스를 수행한다. \n
 * \n
 * sfwTreeArchiveThr 는, 쓰레드를 생성하기 위하여 kutil 라이브러리 내의 kuThread 클래스를 상속하였다.
 */
 
class sfwTreeArchiveThr : public kuThread
{
public :
	/**
     * \fn     sfwTreeArchiveThr (const char *pszThrName, sfwTree *pSfwTree)
     * \brief  문자열로부터 첫 번째 토큰 얻기
     * \param  pszThrName	쓰레드 명
	 * \param  pSfwTree		sfwTree 클래스의 트리 객체
     * \return 없음
     *
     * 입력 받은 문자열 \a pszThrName 의 이름을 갖는 데이터 저장을 수행하는 쓰레드의 객체를 생성한다. \n
     * sfwTreeInit() 또는, new sfwTree()를 이용하여 생성된 트리 객체를 입력으로 받아 애트리뷰트에 보관한다. \n
     * 또한, 사용자 애플리케이션과의 요청 메시지 전달을 위한 통신 수단으로 사용되는 메시지 큐를 생성한다. \n
     */
	sfwTreeArchiveThr (const char *pszThrName, sfwTree *pSfwTree);

	~sfwTreeArchiveThr ();		///< 소멸자

	/**
     * \fn     void *run ()
     * \brief  쓰레드에서 호출되는 처리 함수
     * \return 프로그램 종료 시까지 처리를 반복함
     *
     * kuThread를 상속받아 생성되는 sfwTreeArchiveThr의 쓰레드 처리 함수이다. \n
     * 본 함수는 생성된 쓰레드에 의해 호출 되어, 프로그램의 종료 시까지 사용자 애플리케이션으로부터의 요청 메시지를 처리한다.
     * 요청 메시지의 종류에 따라, processBeginArchiving, processReceivedData, processEndArchiving, processUpdateArchiving 등의 함수를 호출한다.
     */
	void *run ();

	/**
     * \fn     epicsMessageQueueId getQueueId ()
     * \brief  메시지 큐의 식별자 얻기
     * \return 메시지 큐의 식별자
     *
     * 사용자 애플리케이션과 데이터 저장 처리 쓰레드 간의 요청 메시지는 EPICS의 메시지 큐를 이용해서 전달된다.
     * 본 함수는, 통신을 위해 사용되는 EPICS 메시지 큐의 식별자를 반환한다.
     * 만일, 메시지 큐가 생성되어 있지 않은 경우는, 메시지 큐를 생성한 뒤 그 식별자를 반환한다.
     */
	const epicsMessageQueueId getQueueId () {
		if (NULL == m_queueId) {
			m_queueId	= createQueue ();
		}

		return (m_queueId);
	}

	/**
     * \fn     epicsUInt32 getQueueCapacity ()
     * \brief  메시지의 최대 대기 개수 얻기
     * \return 메시지의 최대 대기 개수
     */
	const epicsUInt32 getQueueCapacity () {
		return (SFW_TREE_MQ_CAPACITY);
	}

	/**
     * \fn     epicsUInt32 getQueueMaxMsgSize ()
     * \brief  메시지의 헤더 크기 얻기
     * \return 메시지의 헤더 크기 (바이트)
     */
	const epicsUInt32 getQueueMaxMsgSize () {
		return (SFW_TREE_MQ_MAX_SIZE);
	}

	/**
     * \fn     sfwTree *getTree ()
     * \brief  내부의 트리 객체 얻기
     * \return 내부의 트리 객체 반환
     * \see	   sfwTreeArchiveThr
     *
     * 생성자 sfwTreeArchiveThr의 매개변수로 입력된 트리 객체 반환한다.
     */
	sfwTree *getTree () { return (m_pSfwTree); }
	
	/**
     * \fn     sfwTreeNode *getNode (const char *pszNodeName)
     * \brief  이름으로 노드 찾기
     * \param  pszNodeName	객체를 찾고자 하는 노드의 이름
     * \return 노드의 객체에 대한 포인터
     *
     * 트리 객체로부터 이름 /a pszNodeName 과 일치하는 노드 객체를 찾아 그 포인터를 반환한다.
     */
	sfwTreeNode *getNode (const char *pszNodeName);

private :
	sfwTree				*m_pSfwTree;	///< 생성자의 입력으로 받은 트리 객체
	epicsMessageQueueId	m_queueId;		///< 처리 메시지의 전달에 사용되는 메시지 큐의 식별자

	const epicsBoolean init ();			///< 애트리뷰트의 초기화

	const epicsMessageQueueId createQueue ();	///< 메시지 큐 생성
	void setQueueId (epicsMessageQueueId queueId) { m_queueId = queueId; }

	/**
     * \fn     sfwTreeErr_e openLocalDataFiles (const epicsInt32 iTreeShotNo)
	 * \brief  로컬 데이터 파일 열기
	 * \param  iTreeShotNo		Shot 번호
	 * \return sfwTreeOk(성공), sfwTreeErrInvalid(오류)
	 * \see    processBeginArchiving
	 *
	 * 사용자 애플리케이션으로부터 요청된 데이터는 MDSplus 저장하기 전에 로컬의 임시 파일에 저장된다.
	 * processBeginArchiving() 에 의해 호출되는 함수로서, 
	 * Shot 번호와 로컬 파일의 경로 명을 기반으로 트리 내의 모든 노드를 위한 쓰기용 임시 파일을 연다.
	 * 만일, 로컬 파일의 경로 명과 노드의 파일 명이 지정되지 있지 않으면 로컬 파일은 생성되지 않는다.
	 */
	const sfwTreeErr_e openLocalDataFiles (const epicsInt32 iTreeShotNo);

	/**
     * \fn     sfwTreeErr_e closeLocalDataFiles (const epicsInt32 iTreeShotNo)
	 * \brief  로컬 데이터 파일 닫기
	 * \param  iTreeShotNo		Shot 번호
	 * \return sfwTreeOk(성공), sfwTreeErrInvalid(오류)
	 * \see    processEndArchiving
	 *
	 * processEndArchiving() 에 의해 호출되는 함수로서, openLocalDataFiles() 에 의해 열린 파일을 닫는다.
	 */
	const sfwTreeErr_e closeLocalDataFiles (const epicsInt32 iTreeShotNo);

	/**
     * \fn     char *getLocalHomeDir (const char *pszFilePath, const int iShotNo, char *pszHomeDir)
	 * \brief  로컬 파일의 경로 명과 Shot 번호에 기반한 로컬 파일의 생성 경로 명 얻기
	 * \param  pszFilePath	로컬 파일의 경로 명
	 * \param  iShotNo		Shot 번호
	 * \param  pszHomeDir	로컬 파일의 생성 경로 명
	 * \return 로컬 파일의 생성 경로 명에 대한 포인터
	 *
	 * sfwTreeSetLocalFileHome() 에 의해 설정된 로컬 파일의 경로 명 /a pszFilePath 과 
	 * sfwTreeSetShotInfo() 에 의해 설정된 Shot 번호 /a iShotNo 에 기반하여 
	 * 데이터를 저장할 로컬 파일이 생성될 경로 명 /a pszHomeDir 을 얻는다.
	 */
	const char *getLocalHomeDir (const char *pszFilePath, const int iShotNo, char *pszHomeDir);

	/**
     * \fn     sfwTreeErr_e processReceivedData (sfwTreeMsgHeader *pstHeader)
	 * \brief  데이터 저장에 대한 요청 메시지 처리
	 * \param  pstHeader		메시지의 헤더 정보
 	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    run
	 *
	 * 사용자 애플리케이션으로부터 데이터 저장 요청 메시지를 받았을 때 호출 되는 함수로서
	 * 헤더 정보를 이용하여 저장할 데이터를 메시지 큐로부터 얻고, 
	 * 그 데이터를 로컬 파일에 저정한 이후에 MDSplus 서버로 전송한다.
	 * 참고로, 현재 Segmented 방식의 데이터 저장 유형만을 지원한다.
	 * 만일, 로컬 파일이 쓰기를 위해 열려져 있지 않으면, 로컬 파일로의 저장은 생략된다.
	 */
	const sfwTreeErr_e processReceivedData (sfwTreeMsgHeader *pstHeader);

	/**
     * \fn     sfwTreeErr_e processBeginArchiving (sfwTreeMsgHeader *pstHeader)
	 * \brief  트리에 대한 데이터 저장 준비 작업 수행
	 * \param  pstHeader		메시지의 헤더 정보
 	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    run, sfwTreeBeginArchive, processEndArchiving
	 *
	 * 사용자 애플리케이션으로부터 데이터 저장 준비 요청 메시지를 받았을 때 호출 되는 함수로서
	 * 데이터 저장을 위한 로컬 임시 파일의 생성과 MDSplus 서버와의 연결을 맺는다.
	 * 이벤트 전송을 담당하는 쓰레드가 별도로 실행되지 않는 경우, 이벤트 서버에 대한 연결도 맺는다.
	 * 보다 자세한 내용은, sfwTreeBeginArchive() 를 참조 하도록 한다.
	 */
	const sfwTreeErr_e processBeginArchiving (sfwTreeMsgHeader *pstHeader);

	/**
     * \fn     sfwTreeErr_e processEndArchiving (sfwTreeMsgHeader *pstHeader)
	 * \brief  트리에 대한 데이터 저장 완료 작업 수행
	 * \param  pstHeader		메시지의 헤더 정보
 	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    run, sfwTreeEndArchive, processBeginArchiving
	 *
	 * 사용자 애플리케이션으로부터 데이터 저장 완료 요청 메시지를 받았을 때 호출 되는 함수로서
	 * 이미 생성된 데이터 저장을 위한 로컬 임시 파일을 닫고, MDSplus 서버와의 연결을 끊는다.
	 * 이벤트 전송을 담당하는 쓰레드가 별도로 실행되지 않는 경우, 이벤트 서버에 대한 연결도 끊는다.
	 * 보다 자세한 내용은, sfwTreeEndArchive() 를 참조 하도록 한다.
	 */
	const sfwTreeErr_e processEndArchiving (sfwTreeMsgHeader *pstHeader);

	/**
     * \fn     sfwTreeErr_e processUpdateArchiving (sfwTreeMsgHeader *pstHeader)
	 * \brief  데이터 변경 이벤트 전송 요청 메시지 처리
	 * \param  pstHeader		메시지의 헤더 정보
 	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    run
	 *
	 * 사용자 애플리케이션으로부터 데이터 변경 이벤트 전송 요청 메시지를 받았을 때 호출 되는 함수로서,
	 * 노드가 변경 되었음을 MDS 이벤트 서버에 알린다. 
	 * 보다 자세한 내용은, sfwTreeUpdateArchive() 를 참조 하도록 한다.
	 */
	const sfwTreeErr_e processUpdateArchiving (sfwTreeMsgHeader *pstHeader);
};

#endif // __SFW_TREE_ARCHIVE_THR_H
