/****************************************************************************
 * sfwTreeObj.h
 * --------------------------------------------------------------------------
 * MDSplus C++ interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.23  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    sfwTreeObj.h
 * \ingroup sfwtree
 * \brief   Segmented archiving을 위한 C++ 클래스들 정의
 * 
 * 본 파일은, 취득 데이터를 여러 레코드로 나눠 저장하는 MDSplus 세그먼트 방식을 구현한
 * sfwTree 라이브러리의 기능을 제공하는 것으로 C++ 클래스들을 정의한다. \n
 * 동일한 기능을 제공하는 C 함수들은, 여기에서 제공하는 C++ 클래스들을 호출하는 방식으로 구현 되었다.
 */

#ifndef __SFW_TREE_OBJ_H
#define __SFW_TREE_OBJ_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <map>

#include "kutilObj.h"

#include "sfwTree.h"
#include "sfwTreeArchiveThr.h"
#include "sfwTreeEventThr.h"

#define SFW_TREE_USE_EVENT		1		///< 이벤트 전송 기능 지원
#define SFW_TREE_USE_EVENT_THR	0		///< 이벤트 전송 전담 쓰레드 실행 여부. 미 실행 (sfwTreeArchiveThr 가 처리함)

using namespace std;

// forward reference : pointer is only allowed
class sfwTree;
class sfwTreeNode;
class sfwTreeArchiveThr;
class sfwTreeEventThr;
class sfwTreeRecord;

typedef	queue<sfwTreeRecord *>		sfwTreeRecQueue;		///< 데이터 저장용 내부 큐 정의
typedef	map<string,sfwTreeNode*>	sfwTreeNodeMap;			///< 노드 목록을 관리하기 위한 노드명과 노드객체로 구성되는 맵 유형 정의

/**
 * \struct sfwTreeTypeInfo
 * \brief  트리 유형 정보
 */
typedef struct {
	sfwTreeDataType_e	eSfwTreeDataType;		///< 노드 내 데이터 유형
	epicsInt32			iMdsDescType;			///< MDS descriptor 데이터 유형
	epicsInt32			iDataTypeSize;			///< 데이터의 크기 (바이트)
} sfwTreeTypeInfo;

/**
 * \class   sfwTreeType
 * \brief   트리 유형 정보의 접근을 지원하기 위한 클래스
 */
class sfwTreeType
{
public :
	/**
     * \fn     epicsInt32 getMdsType (sfwTreeDataType_e eDataType)
     * \brief  노드의 데이터 유형에 대응하는 MDS descriptor 데이터 유형 얻기
	 * \param  eDataType	노드 내 데이터 유형
     * \return MDS descriptor 데이터 유형
     * \see    sfwTreeTypeInfo
     */
	static epicsInt32 getMdsType (sfwTreeDataType_e eDataType);

	/**
     * \fn     epicsInt32 getTypeSize (sfwTreeDataType_e eDataType)
     * \brief  노드 데이터 유형의 데이터 크기 얻기
	 * \param  eDataType	노드 내 데이터 유형
     * \return 데이터의 크기 (바이트)
     * \see    sfwTreeTypeInfo
     */
	static epicsInt32 getTypeSize (sfwTreeDataType_e eDataType);

private :
	static sfwTreeTypeInfo	m_pstSfwTreeType[];		///< 트리 유형 정보 목록. 정보는 정적으로 사전 설정됨

	/**
     * \fn     epicsInt32 getTypeSize ()
     * \brief  트리 유형 정보 목록 내의 레코드 개수 얻기
     * \return 목록 내의 sfwTreeTypeInfo 레코드 개수
     * \see    sfwTreeTypeInfo
     */
	static epicsInt32 getNumOfTypes ();
};

/**
 * \class   sfwTreeRecord
 * \brief   MDSplus에 저장할 데이터 및 접근을 지원하기 위한 클래스
 * 
 * 본 클래스는 사용자 애플리케이션으로부터 받은 데이터의 내부 관리를 지원하기 위한 클래스로서,
 * sfwTreeNode::putData() 호출 시에 데이터와 데이터 관련 Shot 정보를 기준으로 객체가 생성된다. \n
 * 본 데이터(레코드) 객체는, 해당 노드 내의 sfwTreeRecQueue 에 저장되어 관리된다. \n
 */
class sfwTreeRecord
{
public :
	/**
     * \fn     sfwTreeRecord (const epicsInt32 iTreeShotNo, const epicsInt32 iIndex,
	 *				const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf)
     * \brief  Shot 정보 및 데이터를 이용한 레코드 생성자. 자원 할당 및 입력 정보의 저장
     * \param  iTreeShotNo	MDSplus 트리의 Shot 번호
	 * \param  iIndex		세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime	저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiDataSize	저장할 데이터의 길이 (바이트)
	 * \param  pDataBuf		저장할 노드의 데이터 버퍼의 주소
     * \return 없음
     * \see    set, sfwTreeSetShotInfo, sfwTreePutData
     */
	sfwTreeRecord (const epicsInt32 iTreeShotNo, const epicsInt32 iIndex,
			const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf) {
		set (iTreeShotNo, iIndex, dStartTime, uiDataSize, pDataBuf);
	}

	/**
     * \fn     sfwTreeRecord (const sfwTreeRecord &record)
     * \brief  레코드 복사 생성자
     * \param  record		복사할 레코드 객체
     * \return 없음
     */
	sfwTreeRecord (const sfwTreeRecord &record) {
		set (record.m_iTreeShotNo, record.m_iIndex, record.m_dStartTime, record.m_uiDataSize, record.m_phDataBuf);
	}

	/**
     * \fn     ~sfwTreeRecord ()
     * \brief  소멸자. 자원 해제
     * \return 없음
     */
	~sfwTreeRecord () {
		clear ();
	}

	/**
     * \fn     void operator= (const sfwTreeRecord &record)
     * \brief  대입 연산자 오버로딩
     * \return 없음
     */
	void operator= (const sfwTreeRecord &record) {
		clear ();
		set (record.m_iTreeShotNo, record.m_iIndex, record.m_dStartTime, record.m_uiDataSize, record.m_phDataBuf);
	}

	epicsInt32 getShotNo () { return (m_iTreeShotNo); }		///< 해당 레코드의 Shot 번호 반환
	epicsInt32 getIndex () { return (m_iIndex); }			///< 해당 레코드의 인덱스 번호 반환
	epicsFloat64 getStartTime () { return (m_dStartTime); }	///< 해당 레코드의 데이터 시작 시각(초) 반환
	epicsUInt32 getSize () { return (m_uiDataSize); }		///< 해당 레코드의 데이터 길이 (바이트) 반환
	const void *getDataBuffer () { return (m_phDataBuf); }	///< 해당 레코드의 데이터 버퍼 포인터 반환

private :
	epicsInt32			m_iTreeShotNo;		///< Shot 번호
	epicsInt32			m_iIndex;			///< 인덱스 번호
	epicsUInt32			m_uiDataSize;		///< 데이터의 길이 (바이트)
	epicsFloat64		m_dStartTime;		///< 데이터의 시작 시각 (초)
	void				*m_phDataBuf;		///< 데이터의 버퍼 포인터

	/**
     * \fn     void clear ()
     * \brief  데이터 저장 버퍼 등의 자원 해제
     * \return 없음
     */
	void clear () {
		if (m_phDataBuf) {
			free(m_phDataBuf);
			m_phDataBuf = NULL;
		}
	}

	/**
     * \fn     void set (const epicsInt32 iTreeShotNo, const epicsInt32 iIndex,
	 *				const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf)
     * \brief  입력 받은 Shot 정보 및 데이터의 저장
     * \param  iTreeShotNo	MDSplus 트리의 Shot 번호
	 * \param  iIndex		세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime	저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiDataSize	저장할 데이터의 길이 (바이트)
	 * \param  pDataBuf		저장할 노드의 데이터 버퍼의 주소
     * \return 없음
     * \see    sfwTreeRecord, sfwTreeSetShotInfo, sfwTreePutData
     */
	void set (const epicsInt32 iTreeShotNo, const epicsInt32 iIndex,
			const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf) {
		m_iTreeShotNo	= iTreeShotNo;
		m_iIndex		= iIndex;
		m_dStartTime	= dStartTime;
		m_uiDataSize	= uiDataSize;

		if (NULL != (m_phDataBuf = malloc(uiDataSize))) {
			memcpy (m_phDataBuf, pDataBuf, uiDataSize);
		}
	}
};

/**
 * \class   sfwTreeLock
 * \brief   트리 또는 노드 접근 제어를 위한 록 클래스
 * 
 * 본 클래스는 sfwTree와 sfwTreeNode 클래스에 의해 상속되어 트리 또는, 노드의 배타적인 접근을 위해 사용된다. \n
 */
class sfwTreeLock
{
public :
	sfwTreeLock () {
		m_epicsMutexId	= epicsMutexCreate ();
	}	///< 생성자. 록을 위한 EPICS 뮤텍스 생성

	~sfwTreeLock () {
		epicsMutexDestroy (m_epicsMutexId);
	}	///< 소멸자. sfwTreeLock()에서 생성된 록을 위한 EPICS 뮤텍스 소멸

	epicsMutexLockStatus lock () {
		return (epicsMutexLock (m_epicsMutexId));
	}	///< 록 설정

	void unlock () {
		epicsMutexUnlock (m_epicsMutexId);
	}	///< 록 해제

private :
	epicsMutexId		m_epicsMutexId;		///< EPICS 뮤텍스 록
};

/**
 * \class   sfwTreeNode
 * \brief   MDSplus 노드로의 데이터 저장을 위한 클래스
 * 
 * 본 클래스는 MDSplus 노드에 실험 데이터를 저장하기 위해 필요한 노드 정보들과 요청 데이터를 관리한다. \n
 */
class sfwTreeNode : public sfwTreeLock
{
public :
	/**
     * \fn     sfwTreeNode (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
	 *				sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate=0)
     * \brief  생성자. 노드 및 노드의 데이터 관련 정보 설정
     * \param  pSfwTree			sfwTree 객체
	 * \param  pszNodeName		노드 명
	 * \param  pszFileName		데이터 저장을 위한 임시 로컬 파일의 이름
	 * \param  eNodeType		노드의 유형. value or waveform
	 * \param  eNodeDataType	노드의 데이터 유형
	 * \param  uiSamplingRate	데이터의 샘플 주기 (Hz)
     * \return 없음
     * \see    setNodeInfo, sfwTreeAddNode, sfwTreeSetLocalFileHome, sfwTreeSetPutType, sfwTreeSetSamplingRate, sfwTreeSetNodeSamplingRate
     */
	sfwTreeNode (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
			sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate=0);

	/**
     * \fn     sfwTreeNode (const sfwTreeNode &node)
     * \brief  노드 객체를 이용한 복사
     * \param  node		복사할 노드 객체
     * \return 없음
     */
	sfwTreeNode (const sfwTreeNode &node);

	/**
     * \fn     ~sfwTreeNode ()
     * \brief  소멸자. 열린 파일 닫기 등 자원 해제
     */
	~sfwTreeNode ();

	/**
     * \fn     epicsInt32 getShotNo ()
     * \brief  sfwTree 객체 내의 설정 Shot 번호 얻기
     * \return Shot 번호
     */
	const epicsInt32 getShotNo ();

	/**
     * \fn     void setNodeInfo (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
	 *				sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate=0)
     * \brief  노드 및 노드의 데이터 관련 정보 설정
     * \param  pSfwTree			sfwTree 객체
	 * \param  pszNodeName		노드 명
	 * \param  pszFileName		데이터 저장을 위한 임시 로컬 파일의 이름
	 * \param  eNodeType		노드의 유형. value or waveform
	 * \param  eNodeDataType	노드의 데이터 유형
	 * \param  uiSamplingRate	데이터의 샘플 주기 (Hz)
     * \return 없음
     * \see    sfwTreeNode, sfwTreeAddNode, sfwTreeSetLocalFileHome, sfwTreeSetPutType, sfwTreeSetSamplingRate, sfwTreeSetNodeSamplingRate
     */
	void setNodeInfo (sfwTree *pSfwTree, const char *pszNodeName, const char *pszFileName,
			sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, epicsUInt32 uiSamplingRate=0);

	/**
     * \fn     epicsUInt32 getSamplingRate ()
     * \brief  데이터의 샘플 주기 얻기
     * \return 데이터의 샘플 주기 (Hz)
     */
	const epicsUInt32 getSamplingRate () { return (m_uiSamplingRate); }

	/**
     * \fn     sfwTreeErr_e setSamplingRate (const epicsUInt32 uiSamplingRate)
     * \brief  데이터의 샘플 주기 설정
     * \param  uiSamplingRate	데이터의 샘플 주기 (Hz)
	 * \return sfwTreeOk(성공)
     */
	const sfwTreeErr_e setSamplingRate (const epicsUInt32 uiSamplingRate);

	/**
     * \fn     char *getName ()
     * \brief  노드의 이름 얻기
     * \return 노드의 이름에 대한 포인터
     */
	const char *getName () { return (m_strNodeName.c_str()); }

	/**
     * \fn     void setName (const char *pszNodeName)
     * \brief  노드의 이름 설정
     * \param  pszNodeName	설정할 노드의 이름
     */
	void setName (const char *pszNodeName) { m_strNodeName	= pszNodeName ? pszNodeName : ""; }

	/**
     * \fn     char *getFileName ()
     * \brief  데이터 저장을 위한 임시 로컬 파일의 이름 얻기
     * \return 데이터 저장을 위한 임시 로컬 파일의 이름에 대한 포인터
     */	
    const char *getFileName () { return (m_strFileName.c_str()); }

	/**
     * \fn     string & getFileNameStr ()
     * \brief  노드의 이름을 갖는 string 객체 얻기
     * \return 노드의 이름을 갖는 string 객체
     */
    string & getFileNameStr () { return (m_strFileName); }

	/**
     * \fn     void setFileName (const char *pszFileName)
     * \brief  데이터 저장을 위한 임시 로컬 파일의 이름 설정
     * \param  pszFileName	설정할 데이터 저장을 위한 임시 로컬 파일의 이름
     */
	void setFileName (const char *pszFileName) { m_strFileName	= pszFileName ? pszFileName : ""; }

	/**
     * \fn     char *getFilePath ()
     * \brief  로컬 파일의 경로 명 얻기. 로컬 파일의 생성 디렉터리와 파일 명으로 구성됨
     * \return 로컬 파일의 경로 명에 대한 포인터
     */	
	const char *getFilePath () { return (m_strFilePath.c_str()); }

	/**
     * \fn     sfwTree *getTree ()
     * \brief  트리 객체 반환
     * \return sfwTree 객체의 포인터
     */	
	sfwTree *getTree () { return (m_pSfwTree); }

	/**
     * \fn     void setTree (sfwTree *pSfwTree)
     * \brief  트리 객체 설정
     * \param  pSfwTree		설정할 트리 객체의 포인터
     */
	void setTree (sfwTree *pSfwTree) { m_pSfwTree = pSfwTree; }

	/**
     * \fn     sfwTreeErr_e putData (const epicsFloat64 dValue)
     * \brief  하나의 실수 값 저장 요청
     * \param  dValue	저장할 실수 값
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    sfwTreePutFloat64
     */
	const sfwTreeErr_e putData (const epicsFloat64 dValue);

	/**
     * \fn     sfwTreeErr_e putData (const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf)
     * \brief  데이터 저장 요청
     * \param  iIndex		세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
     * \param  dStartTime	저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
     * \param  uiSamples	저장할 데이터의 샘플 개수
     * \param  pDataBuf		저장할 노드의 데이터 버퍼의 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    sfwTreePutData
     */
	const sfwTreeErr_e putData (const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf);

	/**
	 * \fn     sfwTreeErr_e waitForSync (epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0)
	 * \brief  노드의 데이터 버퍼 검사
	 * \param  dTimeout			점검의 최대 만료 시간 (초). 0 인 경우, SFW_TIME_SYNC_TIMEOUT 을 사용함
	 * \param  dCheckPeriod		점검 주기 (초). 0 인 경우, SFW_TIME_SYNC_PERIOD 를 사용함
	 * \return sfwTreeOk(버퍼 내 대기 데이터 없음), sfwTreeErr(대기 데이터가 존재함)
	 * \see    sfwTreeSyncNode
	 *
	 * 본 함수는, putData() 를 통해 저장 요청된 노드의 데이터들이 
	 * 내부 버퍼에 쌓여 있는지 아니면, MDSplus 서버로의 저장을 위해 버퍼가 모두 비워졌는지를 확인한다. \n
	 * 본 함수는, 사용자 애플리케이션에게 데이터 저장을 위한 내부 작업이 완료 되었는지를 제공하기 위해 작성 되었다. \n
	 * 참고로, 버퍼 내에 저장 요청 데이터가 없다고 해서 모든 데이터가 MDSplus 저장 완료 되었다는 것을 보장하지는 않는다.
	 */
	const sfwTreeErr_e waitForSync (epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0);

	/**
	 * \fn     sfwTreeErr_e openFile (const char *pszHomeDir)
	 * \brief  데이터를 저장할 로컬 임시 파일 열기
	 * \param  pszHomeDir		로컬 파일이 생성될 홈 디렉터리의 경로 (경로 + Shot 번호)
	 *
	 * 입력으로 받은 홈 디렉터리 경로명과 사전 설정된 노드의 파일명을 이용하여 
	 * 다음 예와 같이 로컬 파일이 생성된다. \n	 
	 * 예) /data/mdsdata/S123456789/NODE1
	 */
	const sfwTreeErr_e openFile (const char *pszHomeDir);

	/**
	 * \fn     sfwTreeErr_e closeFile ()
	 * \brief  openFile을 통해 열린 로컬 임시 파일 닫기
	 * \see    openFile
	 */
	const sfwTreeErr_e closeFile ();

	/**
	 * \fn     sfwTreeErr_e push (sfwTreeRecord *pRecord)
	 * \brief  하나의 데이터 레코드를 내부 큐에 추가함
	 * \see    putData 메서드의 호출 시, 하나의 데이터 레코드에 대한 객체를 생성하여 내부 큐에 추가한다.
	 */
	sfwTreeErr_e push (sfwTreeRecord *pRecord);

	/**
	 * \fn     sfwTreeRecord *pop ()
	 * \brief  내부 큐로부터 데이터 레코드를 얻고 큐로부터 지우기
	 * \return 정상 시, 데이터 레코드 객체의 주소를 반환함. NULL(오류)
	 * \see    push
	 */
	sfwTreeRecord *pop ();

	/**
	 * \fn     sfwTreeRecord *get ()
	 * \brief  내부 큐로부터 데이터 레코드 얻기. pop 과 달리 해당 레코드를 지우지는 않음
	 * \return 정상 시, 데이터 레코드 객체의 주소를 반환함. 오류 시, NULL을 반환함
	 * \see    pop
	 */
	sfwTreeRecord *get () { return (m_qRecQueue.front()); }

	/**
	 * \fn     epicsInt32 size ()
	 * \brief  내부 큐 내의 데이터 레코드 개수 얻기
	 * \return 데이터 레코드의 개수
	 */
	const epicsInt32 size () { return (m_qRecQueue.size()); }

	/**
	 * \fn     FILE *getFilePointer ()
	 * \brief  임시 파일의 포인터 얻기
	 * \return 정상 시, 임시 파일의 포인터를 반환함. 오류 시, NULL을 반환함
	 */
	FILE *getFilePointer () { return (m_fp); }

	/**
	 * \fn     sfwTreeNodeType_e getType ()
	 * \brief  노드의 유형 얻기
	 * \return 노드의 유형
	 */
	sfwTreeNodeType_e getType () { return (m_eNodeType); }

	/**
	 * \fn     void setType (sfwTreeNodeType_e eNodeType)
	 * \brief  노드의 유형 설정
	 * \param  eNodeType	노드의 유형
	 */
	void setType (sfwTreeNodeType_e eNodeType) { m_eNodeType = eNodeType; }

	/**
	 * \fn     sfwTreeDataType_e getDataType ()
	 * \brief  노드의 데이터 유형 얻기
	 * \return 노드의 데이터 유형
	 */
	sfwTreeDataType_e getDataType () { return (m_eDataType); }

	/**
	 * \fn     void setDataType (sfwTreeDataType_e eNodeDataType)
	 * \brief  노드의 데이터 유형 설정. 이 때, 데이터 유형에 따라 그 크기를 같이 설정함
	 * \param  eNodeDataType	노드의 데이터 유형
	 */
	void setDataType (sfwTreeDataType_e eNodeDataType) {
		m_eDataType	= eNodeDataType;

		// update size of data type
		setDataTypeSize ();
	}

	/**
	 * \fn     epicsUInt32 getDataTypeSize ()
	 * \brief  노드의 데이터 유형 크기 얻기
	 * \return 노드의 데이터 유형 크기 (바이트)
	 */
	const epicsUInt32 getDataTypeSize () { return (m_uiDataTypeSize); }

	/**
	 * \fn     void setDataTypeSize ()
	 * \brief  노드의 데이터 유형 크기 설정. setDataType 에 의해 호출됨
	 * \see    setDataType
	 */
	void setDataTypeSize () { m_uiDataTypeSize = sfwTreeType::getTypeSize (getDataType()); }

	/**
     * \fn     void operator= (const sfwTreeNode &node)
     * \brief  대입 연산자 오버로딩. 입력 노드의 정보로 자신의 정보를 설정함 
     * \return 없음
     */
	void operator= (const sfwTreeNode &node) {
		clear ();
		setNodeInfo (node.m_pSfwTree, node.m_strNodeName.c_str(), node.m_strFileName.c_str(), node.m_eNodeType, node.m_eDataType);
	}

private :
	sfwTree				*m_pSfwTree;		///< sfwTree 객체
	string				m_strNodeName;		///< 노드 명
	string				m_strFileName;		///< 데이터 저장을 위한 임시 로컬 파일의 이름
	sfwTreeNodeType_e	m_eNodeType;		///< 노드의 유형. value or waveform
	sfwTreeDataType_e	m_eDataType;		///< 노드 데이터의 유형
	epicsUInt32			m_uiDataTypeSize;	///< 노드 데이터의 크기 (바이트)
	epicsUInt32			m_uiSamplingRate;	///< 노드 데이터의 샘플 주기(Hz)

	sfwTreeRecQueue		m_qRecQueue;		///< 데이터 (레코드) 저장을 위한 내부 큐
	FILE				*m_fp;				///< 로컬 파일의 포인터
	string				m_strFilePath;		///< 로컬 파일의 경로 명

	char *makeFilePath (const char *pszHomeDir, char *pszFilePath);
	void clear ();

};

/**
 * \class   sfwTree
 * \brief   MDSplus 트리에 대한 접근을 지원하기 위한 클래스
 */
class sfwTree : public sfwTreeLock
{
public :
	/**
     * \fn     sfwTree (const char *pszHandleName)
     * \brief  생성자
     * \param  pszHandleName	sfwTree 객체의 이름
     * \return 없음
     *
     * 내부 애트리뷰트들을 초기 값으로 설정하고, 데이터 저장을 담당하는 쓰레드 sfwTreeArchiveThr 를 생성한다. \n
     * 만일, 이벤트 전송 기능이 허용되고 별도의 이벤트 처리 쓰레드를 생성 하도록 컴파일 될 경우는 이벤트 담당 쓰레드인 sfwTreeEventThr 를 생성한다. \n
     * 현재 이벤트 전송 기능은 허용되나 별도의 이벤트 처리 쓰레드를 생성하지 않는다. \n
     * 이에 따라, 이벤트 전송 기능은 sfwTreeArchiveThr 가 수행 한다.
     */
	sfwTree (const char *pszHandleName);

	/**
     * \fn     ~sfwTree ()
     * \brief  소멸자
     * \return 없음
     */
	~sfwTree ();

	/**
     * \fn     epicsBoolean isEnable ()
     * \brief  sfwTree 기능의 수행 여부 얻기
     * \return epicsTrue(기능 수행 허용), epicsFalse(기능 수행 비허용)
	 * \see    sfwTreeEnable, sfwTreeEnabled
     */
	static const epicsBoolean isEnable ();

	/**
	 * \fn     void printInfo ()
	 * \brief  트리 설정 정보 표시
	 * \return 없음
	 * \see    setShotInfo
	 *
	 * 본 함수는, setShotInfo()에 의해 설정된 트리 및 MDSplus 서버에 대한 정보를 화면에 표시한다.
	 */
	void printInfo ();

	/**
	 * \fn     sfwTreeErr_e setShotInfo (const epicsInt32 iShotNo, const char *pszTreeName, 
	 				const char *pszMdsAddr, const char *pszEventName, const char *pszEventAddr)
	 * \brief  데이터를 저장할 트리와 연결 정보 설정
	 * \param  iShotNo			MDSplus 트리의 Shot 번호
	 * \param  pszTreeName		트리 명
	 * \param  pszMdsAddr		MDSplus 서버의 주소 (IP + 포트)
	 * \param  pszEventName		MDS 이벤트 명. NULL이면, 이벤트를 보내지 않음
	 * \param  pszEventAddr		MDSplus 이벤트 서버의 주소 (IP + 포트). NULL이면, MDSplus 서버의 주소와 동일한 것으로 간주함
	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    sfwTreeSetShotInfo
	 *
	 * 데이터를 저장할 트리와 MDS 서버와의 연결 정보를 설정한다.
	 */
	const sfwTreeErr_e setShotInfo (const epicsInt32 iShotNo, const char *pszTreeName,
			const char *pszMdsAddr, const char *pszEventName, const char *pszEventAddr);

	/**
	 * \fn     sfwTreeErr_e setShotNum (const epicsInt32 iShotNo)
	 * \brief  Shot 번호 설정
	 * \param  iShotNo			MDSplus 트리의 Shot 번호
	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    sfwTreeSetShotNum 
	 *
	 * setShotInfo()와 달리, Shot 번호만을 설정한다.
	 */
	const sfwTreeErr_e setShotNum (const epicsInt32 iShotNo);

	/**
	 * \fn     char *getLocalFileHome ()
	 * \brief  노드에 저장할 데이터의 임시 저장 경로명 얻기
	 * \return 노드에 저장할 데이터의 임시 저장 경로명
	 * \see    setLocalFileHome, sfwTreeSetLocalFileHome
	 */
	const char *getLocalFileHome () { return (m_strLocalFileHome.c_str()); }

	/**
	 * \fn     sfwTreeErr_e setLocalFileHome (const char *pszHomeDirName)
	 * \brief  노드에 저장할 데이터의 임시 저장 경로명 설정
	 * \param  pszHomeDirName	임시 저장할 파일의 생성 경로명
	 * \return sfwTreeOk(성공), sfwTreeErrInvalid(입력 디렉터리 존재하지 않음)
	 * \see    sfwTreeSetLocalFileHome
	 *
	 * putData() 함수를 통해 MDSplus에 데이터를 저장하는 경우, 요청 받은 데이터는 저장 경로 \a pszHomeDirName 내의 로컬 파일에 임시 저장된다.
	 * MDSplus로의 저장이 실패하는 경우, 로컬 파일에 저장된 데이터를 이용하여 별도의 방법으로 복구가 가능하다.
	 * 단, 로컬 파일에 저장된 데이터를 이용한  복구 방법은 구현되어 있지 않다.
	 * 이미 존재하는 저장 경로명 \a pszHomeDirName 을 입력으로 사용하여야 한다. (예, /data/segdata)
	 * 만일, 저장 경로명이 NULL인 경우는 요청 받은 데이터를 위한 임시 파일들을 생성하지 않는다.
	 */
	const sfwTreeErr_e setLocalFileHome (const char *pszHomeDirName);

	/**
	 * \fn     sfwTreePutType_e getDataPutType ()
	 * \brief  노드 데이터의 저장 유형 얻기
	 * \return 노드 데이터의 저장 유형
	 * \see    setDataPutType, sfwTreeSetPutType
	 */
	const sfwTreePutType_e getDataPutType () { return (m_eDataPutType); }
	
	/**
	 * \fn     sfwTreeErr_e setDataPutType (const sfwTreePutType_e eDataPutType)
	 * \brief  노드 데이터의 저장 유형 설정
	 * \param  eDataPutType		데이터의 저장 유형
	 * \return sfwTreeOk(성공), sfwTreeErrInvalid(입력값 오류)
	 * \see    sfwTreeSetPutType
	 *
	 * 취득된 데이터는 하나의 데이터 또는, 여러 레코드로 나뉘어 저장될 수 있다.
	 * 즉, Bulk 또는, Segmented archving의 저장 방식이 가능하다. \n
	 * Bulk 방식이 모든 데이터를 취득한 뒤에 저장하는 것에 반해, Segmented archving은 데이터를 취득하는 도중에 저장할 수 있다. \n
	 * 따라서,  Segmented archving 방식을 사용하는 경우 데이터 취득 중에도 데이터의 확인이 가능하다.
	 */
	const sfwTreeErr_e setDataPutType (const sfwTreePutType_e eDataPutType);

	/**
     * \fn     epicsUInt32 getSamplingRate (sfwTreeNode *pSfwTreeNode=NULL)
     * \brief  데이터의 샘플 주기 얻기
     * \return 데이터의 샘플 주기 (Hz)
	 * \see    setSamplingRate, sfwTreeGetSamplingRate, sfwTreeGetNodeSamplingRate
     *
     * 만일, 입력 노드 객체가 NULL 이거나 노드의 샘플 주기가 설정 되어 있지 않으면, 트리 객체에 설정된 대표 값을 반환한다. \n
     */
	const epicsUInt32 getSamplingRate (sfwTreeNode *pSfwTreeNode=NULL);

	/**
	 * \fn     sfwTreeErr_e setSamplingRate (const epicsUInt32 uiSamplingRate)
	 * \brief  저장할 데이터의 샘플 주기 설정
	 * \param  uiSamplingRate	데이터의 샘플 주기
	 * \return sfwTreeOk(성공), sfwTreeErrInvalid(실패)
	 * \see    sfwTreeSetSamplingRate
	 *
	 * 저장할 데이터의 샘플 주기 /a uiSamplingRate 를 설정한다. \n
	 * 본 샘플 주기는, 저장할 데이터들의 시각을 나타내기 위한 SLOPE를 생성하는데 사용된다. \n
	 * 본 샘플 주기는, 트리 내의 모든 노드에 대해 적용되는 기본 값이다.
	 */
	const sfwTreeErr_e setSamplingRate (const epicsUInt32 uiSamplingRate);

	/**
	 * \fn     sfwTreeNode *addNode (const char *pszNodeName, 
	 *				sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, 
	 *				const char *pszFileName, epicsUInt32 uiSamplingRate=0)
	 * \brief  트리 객체에 노드를 추가함
	 * \param  pszNodeName		노드 명
	 * \param  eNodeType		시그널 또는, 단일 값 여부
	 * \param  eNodeDataType	데이터의 유형
	 * \param  pszFileName		노드 데이터의 임시 저장을 위한 로컬 파일 명
	 * \param  uiSamplingRate	데이터의 샘플 주기 (Hz)
	 * \return 생성된 노드 객체에 대한 포인터
	 * \see    sfwTreeAddNode
	 *
	 * 노드 객체를 생성한 뒤, 트리 객체 내에 노드를 추가한다.
	 */
	sfwTreeNode *addNode (const char *pszNodeName, sfwTreeNodeType_e eNodeType, sfwTreeDataType_e eNodeDataType, const char *pszFileName, epicsUInt32 uiSamplingRate=0);

	/**
	 * \fn     void printNodes ()
	 * \brief  트리 내의 노드 목록 표시
	 * \return 없음
	 * \see    sfwTreePrintNodes
	 *
	 * 본 함수는, addNode 에 의해 생성된 모든 노드의 목록을 표시한다.
	 */
	void printNodes ();

	/**
	 * \fn     sfwTreeNode *getNode (const char *pszNodeName)
	 * \brief  입력된 이름을 갖는 트리 내의 노드 얻기
	 * \return 존재하는 경우, 노드의 객체에 대한 포인터를 반환함. 존재하지 않으면, NULL을 반환함
	 * \see    addNode, printNodes
	 */
	sfwTreeNode *getNode (const char *pszNodeName);

	/**
	 * \fn     sfwTreeErr_e beginArchive ()
	 * \brief  트리에 대한 데이터 저장 준비 작업 수행
	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    setShotInfo, endArchive, sfwTreeBeginArchive
	 *
	 * MDSplus 데이터는, Shot 별로 저장이 이루어진다.
	 * 이에 따라, 매 Shot 마다 트리에 데이터를 저장하기 위한 사전 작업을 수행한다.
	 * 즉, setShotInfo()에 의해 설정된 트리 및 서버에의 연결 정보를 이용하여,
	 * MDSplus 서버와 이벤트 서버에 대한 연결을 맺고, 데이터를 저장할 트리를 연다.
	 * 그리고, 데이터용 로컬 파일의 저장 경로 및 파일 명이 정상인 경우, 임시 파일을 생성한다.
	 */
	const sfwTreeErr_e beginArchive ();

	/**
	 * \fn     sfwTreeErr_e updateArchive ()
	 * \brief  데이터 변경 이벤트 전송
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    setShotInfo, sfwTreeUpdateArchive
	 *
	 * MDSplus로의 데이터 저장 이후에 노드가 변경 되었음을 알리기 위하여 MDS 이벤트를 전송한다.
	 * 이 때, setShotInfo()를 통해 설정한 MDS 이벤트 서버의 정보를 활용한다.
	 * 본 MDS 이벤트는 jScope 등에서 노드 데이터의 변경에 따른 자동 갱신에 사용될 수 있다.
	 */
	const sfwTreeErr_e updateArchive ();

	/**
	 * \fn     sfwTreeErr_e endArchive (sfwTreeEndAction_e eAction)
	 * \brief  데이터 저장 완료 작업 수행
	 * \param  eAction		저장 종료 시의 처리 유형. 미 구현
	 * \return sfwTreeOk(성공), sfwTreeErr(오류)
	 * \see    beginArchive, sfwTreeEndArchive
	 *
	 * 본 함수는, 모든 데이터에 대한 저장 요청을 완료한 뒤 더 이상 사용하지 않는 자원을 해제하기 위한 것으로,
	 * beginArchive()에서 수행한 MDSplus 서버와 이벤트 서버로의 연결을 해제하고, 열려 있는 임시 데이터 파일을 닫는다.
	 */
	const sfwTreeErr_e endArchive (sfwTreeEndAction_e eAction);

	/**
	 * \fn     sfwTreeErr_e waitForSync (epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0)
	 * \brief  모든 노드의 데이터 버퍼 검사
	 * \param  dTimeout			점검의 최대 만료 시간 (초)
	 * \param  dCheckPeriod		점검 주기 (초)
	 * \return sfwTreeOk(버퍼 내 대기 데이터 없음), sfwTreeErr(대기 데이터가 존재함)
	 * \see    sfwTreeInit
	 *
	 * 본 함수는, putData() 를 통해 저장 요청된 모든 노드의 데이터들이 
	 * 내부 버퍼에 쌓여 있는지 아니면, MDSplus 서버로의 저장을 위해 버퍼가 모두 비워졌는지를 확인한다. \n
	 * 본 함수는, 사용자 애플리케이션에게 데이터 저장을 위한 내부 작업이 완료 되었는지를 제공하기 위해 작성 되었다. \n
	 * 참고로, 버퍼 내에 저장 요청 데이터가 없다고 해서 모든 데이터가 MDSplus 저장 완료 되었다는 것을 보장하지는 않는다.
	 */
	const sfwTreeErr_e waitForSync (epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0);

	/**
	 * \fn     sfwTreeErr_e waitForSync (sfwTreeNode *pSfwTreeNode, 
	 *				epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0)
	 * \brief  입력 노드의 데이터 버퍼 검사
	 * \param  pSfwTreeNode		addNode()에 의해 생성된 노드 객체
	 * \param  dTimeout			점검의 최대 만료 시간 (초)
	 * \param  dCheckPeriod		점검 주기 (초)
	 * \return sfwTreeOk(버퍼 내 대기 데이터 없음), sfwTreeErr(대기 데이터가 존재함)
	 * \see    sfwTreeInit
	 *
	 * 본 함수는, putData() 를 통해 저장 요청된 해당 노드의 데이터들이 
	 * 내부 버퍼에 쌓여 있는지 아니면, MDSplus 서버로의 저장을 위해 버퍼가 모두 비워졌는지를 확인한다. \n
	 * 본 함수는, 사용자 애플리케이션에게 데이터 저장을 위한 내부 작업이 완료 되었는지를 제공하기 위해 작성 되었다. \n
	 * 참고로, 버퍼 내에 저장 요청 데이터가 없다고 해서 모든 데이터가 MDSplus 저장 완료 되었다는 것을 보장하지는 않는다.
	 */
	const sfwTreeErr_e waitForSync (sfwTreeNode *pSfwTreeNode, epicsFloat64 dTimeout=0.0, epicsFloat64 dCheckPeriod=0.0);

	/**
	 * \fn     sfwTreeErr_e putData (sfwTreeNode *pSfwTreeNode, 
	 *				const epicsInt32 iIndex, const epicsFloat64 dStartTime, 
	 *				const epicsUInt32 uiSamples, const void *pDataBuf)
	 * \brief  노드 데이터의 저장 요청
	 * \param  pSfwTreeNode		addNode()에 의해 생성된 노드 객체
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime		저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiSamples		저장할 데이터의 샘플 개수
	 * \param  pDataBuf			저장할 노드의 데이터 버퍼 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 내부적으로 트리 객체와 함께 생성된 데이터 저장용 쓰레드인 Archive Thread에게 데이터의 저장을 요청한다. \n
	 * 저장이 요청된 데이터는 로컬 파일에 저장된 뒤, MDS 서버로 전달된다. \n
	 * 본 함수는, 저장 요청 데이터가 MDS 서버에 저장된 것을 보장하지는 않는다. \n
	 * 만일, 저장 요청한 모든 데이터가 MDS 서버로 전달이 되었는지를 확인하기 위해서는, 
	 * waitForSync() 를 참조 하도록 한다.
	 */
	const sfwTreeErr_e putData (sfwTreeNode *pSfwTreeNode, const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf);

	/**
	 * \fn     sfwTreeErr_e putData (const char *pszTreeNodeName, 
	 *				const epicsInt32 iIndex, const epicsFloat64 dStartTime, 
	 *				const epicsUInt32 uiDataSize, const void *pDataBuf)
	 * \brief  노드 데이터의 저장 요청
	 * \param  pszTreeNodeName	addNode()에 의해 생성된 노드 객체의 이름
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime		저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiDataSize		저장할 데이터의 크기 (바이트)
	 * \param  pDataBuf			저장할 노드의 데이터 버퍼 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 내부적으로 트리 객체와 함께 생성된 데이터 저장용 쓰레드인 Archive Thread에게 데이터의 저장을 요청한다. \n
	 * 저장이 요청된 데이터는 로컬 파일에 저장된 뒤, MDS 서버로 전달된다. \n
	 * 본 함수는, 저장 요청 데이터가 MDS 서버에 저장된 것을 보장하지는 않는다. \n
	 * 만일, 저장 요청한 모든 데이터가 MDS 서버로 전달이 되었는지를 확인하기 위해서는, 
	 * waitForSync() 를 참조 하도록 한다.
	 */
	const sfwTreeErr_e putData (const char *pszTreeNodeName, const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiDataSize, const void *pDataBuf);

	/**
	 * \fn     sfwTreeErr_e open ()
	 * \brief  MDSplus 트리 열기
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 이미 연결이 이루어진 MDSplus 트리에 대해 데이터를 저장할 수 있도록 트리를 연다. \n
	 */
	const sfwTreeErr_e open ();

	/**
	 * \fn     sfwTreeErr_e close ()
	 * \brief  MDSplus 트리 닫기
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    open
	 *
	 * 본 함수는, 이미 열려 있는 트리를 닫는다. \n
	 */
	const sfwTreeErr_e close ();

	/**
	 * \fn     sfwTreeErr_e writeSegment (sfwTreeNode *pTreeNode, const epicsInt32 iIndex, 
	 *				epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDataBuf)
	 * \brief  세그먼트 방식의 데이터 저장
	 * \param  pTreeNode	addNode()에 의해 생성된 노드 객체
	 * \param  iIndex		세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime	저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiSamples		저장할 데이터의 샘플 개수
	 * \param  pDataBuf		저장할 노드의 데이터 버퍼 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 세그먼트 방식의 데이터를 저장하기 위한 descriptor 생성 등의 사전 작업을 수행한 뒤,
	 * writeSegmentNoPut 또는, writeSegmentWithPut 함수를 호출하여 데이터를 저장한다. \n
	 * 현재 보다 효율적인 데이터 저장을 위하여 PutSegment()를 사용하지 않는 writeSegmentNoPut 을 호출한다.
	 */
	const sfwTreeErr_e writeSegment (sfwTreeNode *pTreeNode, const epicsInt32 iIndex,
			epicsFloat64 dStartTime, const epicsUInt32 uiSamples, const void *pDatabuf);

	/**
	 * \fn     sfwTreeErr_e writeSegmentNoPut (const char *pszTagName, const epicsInt32 iIndex, 
	 *				int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
	 * \brief  세그먼트 방식의 데이터 저장
	 * \param  pszTagName		노드 또는, 태그 명
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  startTimeDesc	저장할 데이터의 시작 시각에 대한 descriptor
	 * \param  endTimeDesc		저장할 데이터의 종료 시각에 대한 descriptor
	 * \param  endIdxDesc		저장할 데이터의 마지막 인텍스에 대한 descriptor. 샘플 개수 - 1
	 * \param  timeRateDesc		데이터 간의 시간 간격에 대한 descriptor	 
	 * \param  valueDesc		저장할 노드의 데이터에 대한 descriptor
	 * \param  statDesc			처리 결과에 대한 descriptor
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 보다 효율적인 세그먼트 방식의 데이터를 저장하기 위하여
	 * PutSegment() 없이 BeginSegment() 만을 호출한다.
	 */
	const sfwTreeErr_e writeSegmentNoPut (const char *pszTagName, const epicsInt32 iIndex,
			int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc);

	/**
	 * \fn     sfwTreeErr_e writeSegmentWithPut (const char *pszTagName, const epicsInt32 iIndex, 
	 *				int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
	 * \brief  세그먼트 방식의 데이터 저장 (미 사용)
	 * \param  pszTagName		노드 또는, 태그 명
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  startTimeDesc	저장할 데이터의 시작 시각에 대한 descriptor
	 * \param  endTimeDesc		저장할 데이터의 종료 시각에 대한 descriptor
	 * \param  endIdxDesc		저장할 데이터의 마지막 인텍스에 대한 descriptor. 샘플 개수 - 1
	 * \param  timeRateDesc		데이터 간의 시간 간격에 대한 descriptor	 
	 * \param  valueDesc		저장할 노드의 데이터에 대한 descriptor
	 * \param  statDesc			처리 결과에 대한 descriptor
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, BeginSegment() 와 PutSegment() 모두를 사용하여 세그먼트 방식의 데이터를 저장한다. \n
	 * 현재 사용하지 않는다.
	 */
	const sfwTreeErr_e writeSegmentWithPut (const char *pszTagName, const epicsInt32 iIndex,
			int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc);

	/**
	 * \fn     sfwTreeErr_e write (const char *szTagName, const void *pDatabuf, 
	 *				epicsUInt32 uiSamplingRate, epicsUInt32 uiSamples, epicsFloat64 dStartTime)
	 * \brief  Bulk 방식의 데이터 저장
	 * \param  szTagName		노드 (또는, 태그) 명
	 * \param  pDatabuf			저장할 노드 데이터의 버퍼 주소
	 * \param  uiSamplingRate	샘플 주기 (Hz)
	 * \param  uiSamples		저장할 데이터의 샘플 개수
	 * \param  dStartTime		저장할 데이터의 시작 시각 (초)
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, Bulk 형태의 데이터 저장을 지원하기 위하여 작성 되었다. \n
	 * 이는 Segmented archiving 이전의 데이터 저장 방식으로서 모든 데이터를 취득한 뒤 한 번에 저장한다. \n
	 * Bulk 형태의 데이터 저장은, 현재 정식으로 지원되지 않으므로 필요 시 추가적인 검증 작업이 요구된다.
	 */
	const sfwTreeErr_e write (const char *szTagName, const void *pDatabuf, epicsUInt32 uiSamplingRate, epicsUInt32 uiSamples, epicsFloat64 dStartTime);

	/**
	 * \fn     sfwTreeErr_e write (const char *szTagName, char *pszValue)
	 * \brief  노드에 문자열 형태의 데이터 저장
	 * \param  szTagName	노드 (또는, 태그) 명
	 * \param  pszValue		저장할 문자열의 버퍼 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 해당 노드에 문자열 값을 저장한다.
	 */
	const sfwTreeErr_e write (const char *szTagName, char *pszValue);

	/**
	 * \fn     sfwTreeErr_e write (const char *szTagName, const epicsFloat64 dValue)
	 * \brief  노드에 하나의 실수 값 저장
	 * \param  szTagName	노드 (또는, 태그) 명
	 * \param  dValue		저장할 실수 값
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, 실수 값을 입력으로 받아 문자열로 변환한 뒤 해당 노드에 저장한다.
	 */
	const sfwTreeErr_e write (const char *szTagName, const epicsFloat64 dValue);

	/**
	 * \fn     sfwTreeErr_e sendEvent ()
	 * \brief  MDS 이벤트 전송
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    updateArchive
	 *
	 * 본 함수는, 노드의 데이터가 변경 되었음을 알리기 위하여 MDS 이벤트 서버에게 이벤트를 전송한다. \n
	 * MDS 이벤트 서버에 대한 연결은, beginArchive 호출 시점에 이루어진다.
	 */
	const sfwTreeErr_e sendEvent ();

	/**
	 * \fn     sfwTreeErr_e sendEventWithConnection ()
	 * \brief  MDS 이벤트 전송
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    sendEvent
	 *
	 * 본 함수는, 노드의 데이터가 변경 되었음을 알리기 위하여 MDS 이벤트 서버에게 이벤트를 전송한다. \n
	 * sendEvent 가 MDS 이벤트 서버에 대한 연결을 미리 갖고 있는 것과는 달리, 
	 * 본 함수는 매 요청 시 마다 연결을 맺어 이벤트를 전송한 뒤 연결을 해제한다.
	 */
	const sfwTreeErr_e sendEventWithConnection ();

	//const char *getStatusMsg () { return (MdsGetMsg (m_iStatus)); }
	/**
	 * \fn     char *getStatusMsg ()
	 * \brief  MDSplus 함수 호출 오류 메시지 얻기. 미 구현
	 * \return N/A
	 *
	 * 본 함수는, MDSplus 함수 호출 이후의 오류 메시지를 얻기 위하여 작성되었다. \n
	 * 그러나, MdsGetMsg 함수를 이용할 경우 컴파일 오류가 발생하여 실제 사용하고 있지 않다. \n
	 * 이에 따라, 현재는 그 결과에 상관 없이 N/A 문자열을 반환한다.
	 */
	const char *getStatusMsg () { return ("N/A"); }

	/**
	 * \fn     sfwTreeErr_e connect ()
	 * \brief  MDSplus 서버에 대한 연결
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    connectEvent
	 *
	 * 본 함수는, MDSplus 서버에 대한 연결을 맺는다. \n
	 * MdsConnect 함수의 경우 내부적으로 전역변수를 사용하는 관계로 여러 개의 연결을 관리할 수 없다. \n
	 * 이에 따라, MdsConnect 대신 ConnectToMds 를 사용하여 MDSplus 서버와 이벤트 서버에 대한 연결을 동시에 가질 수 있도록 하였다.
	 */
	const sfwTreeErr_e connect ();
	
	/**
	 * \fn     sfwTreeErr_e disConnect ()
	 * \brief  MDSplus 서버에 대한 연결 해제
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    connect
	 *
	 * connect 를 통해 구축된 MDSplus 서버에 대한 연결을 해제한다.
	 */
	const sfwTreeErr_e disConnect ();

	/**
	 * \fn     epicsBoolean isConnected ()
	 * \brief  MDSplus 서버에 대한 연결 여부 확인
	 * \return epicsTrue(연결됨), epicsFalse(연결 안 됨)
	 * \see    connect, disConnect
	 *
	 * MDSplus 이벤트 서버에 대한 연결 여부를 확인한다.
	 */
	const epicsBoolean isConnected () { return (INVALID_SOCKET != m_iSocket ? epicsTrue : epicsFalse); }

	/**
	 * \fn     epicsBoolean isOpened ()
	 * \brief  데이터를 쓰기 위한 트리가 열려 있는지 확인
	 * \return epicsTrue(열려 있음), epicsFalse(열려 있지 않음)
	 * \see    beginArchive
	 */
	const epicsBoolean isOpened () { return (m_bOpened); }
	void setOpened (const epicsBoolean bOpened) { m_bOpened	= bOpened; }

	/**
	 * \fn     epicsMessageQueueId getQueueId ()
	 * \brief  sfwArchiveThr 에서 생성된 데이터용 메시지 큐의 ID 얻기
	 * \return 메시지 큐 ID(정상), NULL(오류)
	 */
	const epicsMessageQueueId getQueueId ();

	/**
	 * \fn     sfwTreeNodeMap *getNodeMap ()
	 * \brief  노드들의 목록을 관리하는 맵의 레퍼런스 얻기
	 * \return 노드 목록 맵의 레퍼런스
	 */
	sfwTreeNodeMap *getNodeMap () { return (&m_mapSfwTreeNode); }

	/**
	 * \fn     epicsInt32 getShotNo ()
	 * \brief  setShotInfo() 함수에 의해 설정된 Shot 번호 얻기
	 * \return Shot 번호 얻기
	 */
	const epicsInt32 getShotNo () { return (m_iShotNo); }
	void setShotNo (const epicsInt32 iShotNo) { m_iShotNo = iShotNo; }

	// --------------------------
	// for Event
	// --------------------------
	/**
	 * \fn     epicsMessageQueueId getEventQueueId ()
	 * \brief  sfwTreeEventThr 에서 생성된 이벤트용 메시지 큐의 ID 얻기
	 * \return 메시지 큐 ID(정상), NULL(오류)
	 */
	const epicsMessageQueueId getEventQueueId ();

	/**
	 * \fn     sfwTreeErr_e connectEvent ()
	 * \brief  MDSplus 이벤트 서버에 대한 연결
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    connect
	 *
	 * 본 함수는, MDSplus 이벤트 서버에 대한 연결을 맺는다. \n
	 * MdsConnect 함수의 경우 내부적으로 전역변수를 사용하는 관계로 여러 개의 연결을 관리할 수 없다. \n
	 * 이에 따라, MdsConnect 대신 ConnectToMds 를 사용하여 MDSplus 서버와 이벤트 서버에 대한 연결을 동시에 가질 수 있도록 하였다.
	 */
	const sfwTreeErr_e connectEvent ();

	/**
	 * \fn     sfwTreeErr_e disConnectEvent ()
	 * \brief  MDSplus 이벤트 서버에 대한 연결 해제
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    connectEvent
	 *
	 * connectEvent 를 통해 구축된 MDSplus 이벤트 서버에 대한 연결을 해제한다.
	 */
	const sfwTreeErr_e disConnectEvent ();

	/**
	 * \fn     epicsBoolean isEventConnected ()
	 * \brief  MDSplus 이벤트 서버에 대한 연결 여부 확인
	 * \return epicsTrue(연결됨), epicsFalse(연결 안 됨)
	 * \see    connectEvent, disConnectEvent
	 *
	 * MDSplus 이벤트 서버에 대한 연결 여부를 확인한다.
	 */
	const epicsBoolean isEventConnected () { return (INVALID_SOCKET != m_iEventSocket ? epicsTrue : epicsFalse); }

	// for data and event connection

	/**
	 * \fn     void useDataSocket ()
	 * \brief  MDSplus 서버에 대한 연결을 내부적으로 사용함
	 * \return 없음
	 * \see    connect, setSocket
	 *
	 * mdslib.h에서 제공하는 MDS 함수들은 소켓 연결 시 전역 변수인 mdsSocket을 사용한다. \n
	 * 본 라이브러리에서는 데이터 저장을 위한 MDSplus 서버와의 연결과 이벤트 전송을 위한 MDS 이벤트 서버와의 연결이 생성된다. \n
	 * 따라서, 본 함수는 데이터 저장을 위한 MDSplus 서버에 대한 연결을 기본으로 사용하기 위하여 mdsSocket의 값으로 설정한다.
	 */
	void useDataSocket ();

	/**
	 * \fn     void useEventSocket ()
	 * \brief  MDSplus 이벤트 서버에 대한 연결을 내부적으로 사용함
	 * \return 없음
	 * \see    connectEvent, setSocket
	 *
	 * mdslib.h에서 제공하는 MDS 함수들은 소켓 연결 시 전역 변수인 mdsSocket을 사용한다. \n
	 * 본 라이브러리에서는 데이터 저장을 위한 MDSplus 서버와의 연결과 이벤트 전송을 위한 MDS 이벤트 서버와의 연결이 생성된다. \n
	 * 따라서, 본 함수는 이벤트 전송을 위한 MDSplus 이벤트 서버에 대한 연결을 기본으로 사용하기 위하여 mdsSocket의 값으로 설정한다.
	 */
	void useEventSocket ();

	/**
	 * \fn     void setSocket (epicsInt32 iSocket)
	 * \brief  입력 받은 소켓 연결을 MDSplus 내부 소켓으로 설정함
	 * \return 없음
	 * \see    useDataSocket, useEventSocket
	 *
	 * mdslib.h에서 제공하는 MdsSetSocket() 함수를 이용하여, 입력 받은 소켓 연결 \a iSocket 값으로 내부 소켓 값을 설정한다.
	 */
	void setSocket (epicsInt32 iSocket);

	/**
	 * \fn     epicsInt32 getSocket ()
	 * \brief  MDSplus 내부 소켓 얻기
	 * \return MDSplus 내부 소켓
	 * \see    connect, connectEvent, setSocket
	 */
	const epicsInt32 getSocket () {
		extern SOCKET mdsSocket;
		return (mdsSocket);
	}

protected :
	/**
	 * \fn     sfwTreeErr_e init ()
	 * \brief  멤버 변수 초기화
	 * \return sfwTreeOk(성공), 그 외(오류)
	 */
	const sfwTreeErr_e init ();

	/**
	 * \fn     epicsBoolean isMdsOK ()
	 * \brief  MDSplus 함수의 정상 여부 확인
	 * \return epicsTrue(정상 호출), epicsFalse(오류)
	 */
	const epicsBoolean isMdsOK () { return isMdsOK (m_iStatus); }

	/**
	 * \fn     epicsBoolean isMdsOK (const epicsInt32 iStatus)
	 * \brief  입력으로 받은 MDSplus 함수의 결과가 정상인지 확인
	 * \return epicsTrue(정상 호출), epicsFalse(오류)
	 */
	const epicsBoolean isMdsOK (const epicsInt32 iStatus) {
		m_iStatus	= iStatus;
		return ( (m_iStatus & 1) ? epicsTrue : epicsFalse );
	}

	/**
	 * \fn     sfwTreeErr_e createTree (const epicsInt32 iShotNo)
	 * \brief  입력 Shot 번호에 해당하는 트리 생성
	 * \return sfwTreeOk(성공), 그 외(오류)
	 */
	const sfwTreeErr_e createTree (const epicsInt32 iShotNo);

	/**
	 * \fn     sfwTreeErr_e sendMessage (const sfwTreeOperCode_e operCode, const char *pszTreeNodeName,
	 *							const epicsInt32 iIndex, const epicsFloat64 dStartTime, 
	 *							const epicsUInt32 uiSize, const void *pDataBuf)
	 * \brief  저장의 시작과 끝, 이벤트 전송 등의 메시지 전송
	 * \param  operCode			메시지의 유형
	 * \param  pszTreeNodeName	노드 명
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  dStartTime		저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
	 * \param  uiSize			저장할 데이터의 크기 (바이트)
	 * \param  pDataBuf			저장할 노드의 데이터 버퍼 주소
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는 사용자 애플리케이션에서 내부 쓰레드인 sfwTreeArchiveThr 와 sfwTreeEventThr 에게 하나의 메시지를 전송한다. \n
	 * 내부적으로는 sfwTreeMsgHeader 구조체에 입력 정보를 저장한 뒤, 각 쓰레드에게 전송한다. \n
	 * 현재 sfwTreeEventThr 는 생성되지 않으므로, 실제로는 sfwTreeArchiveThr 에게만 전송 된다.
	 */
	const sfwTreeErr_e sendMessage (const sfwTreeOperCode_e operCode, const char *pszTreeNodeName,
			const epicsInt32 iIndex, const epicsFloat64 dStartTime, const epicsUInt32 uiSize, const void *pDataBuf);

private :
	epicsInt32			m_iSocket;			///< MDSplus 서버에 대한 연결 소켓
	epicsInt32			m_iEventSocket;		///< MDSplus 이벤트 서버에 대한 연결 소켓
	epicsBoolean		m_bEventServer;		///< MDSplus 서버와 이벤트 서버의 분리 여부
	epicsBoolean		m_bOpened;			///< 트리가 열려 있는지의 여부
	epicsInt32			m_iStatus;			///< MDS 함수의 호출 결과
	epicsInt32			m_iShotNo;			///< Shot 번호

	string				m_strTreeName;		///< 트리 명
	string				m_strMdsAddr;		///< MDSplus 서버의 주소 (IP + Port)
	string				m_strEventName;		///< MDSplus 이벤트 명
	string				m_strEventAddr;		///< MDSplus 이벤트 서버의 주소 (IP + Port)
	string				m_strLocalFileHome;	///< 데이터 저장용 로컬 파일의 홈 디렉터리

	sfwTreePutType_e	m_eDataPutType;		///< 데이터 저장 유형. bulk or segment
	epicsUInt32			m_uiSamplingRate;	///< 샘플 주기 (Hz)

	sfwTreeNodeMap		m_mapSfwTreeNode;	///< 노드 목록을 관리하는 맵
	sfwTreeArchiveThr	*m_pArchiveThr;		///< 데이터 저장을 담당하는 쓰레드
	sfwTreeEventThr		*m_pEventThr;		///< 이벤트 전송을 담당하는 쓰레드

	/**
	 * \fn     sfwTreeErr_e createArchiveThr (const char *pszThrName)
	 * \brief  sfwTreeArchiveThr 객체를 이용한 입력 이름의 데이터 저장용 쓰레드 생성
	 * \return sfwTreeOk(성공), 그 외(오류)
	 */
	const sfwTreeErr_e createArchiveThr (const char *pszThrName);

	/**
	 * \fn     sfwTreeErr_e createEventThr (const char *pszThrName)
	 * \brief  sfwTreeEventThr 객체를 이용한 입력 이름의 이벤트 전송용 쓰레드 생성
	 * \return sfwTreeOk(성공), 그 외(오류)
	 */
	const sfwTreeErr_e createEventThr (const char *pszThrName);

	/**
	 * \fn     epicsBoolean isEventServer
	 * \brief  MDSplus 서버와 이벤트 서버의 분리 여부 확인
	 * \return epicsTrue(상이한 호스트), epicsFalse(동일 호스트)
	 */
	const epicsBoolean isEventServer () { return (m_strMdsAddr != m_strEventAddr ? epicsTrue : epicsFalse); }

	/**
	 * \fn     sfwTreeErr_e beginSegment (const char *pszTagName, const epicsInt32 iIndex, 
	 *				int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
	 * \brief  MDSplus에 세그먼트 데이터 저장
	 * \param  pszTagName		노드 또는, 태그 명
	 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
	 * \param  startTimeDesc	저장할 데이터의 시작 시각에 대한 descriptor
	 * \param  endTimeDesc		저장할 데이터의 종료 시각에 대한 descriptor
	 * \param  endIdxDesc		저장할 데이터의 마지막 인텍스에 대한 descriptor. 샘플 개수 - 1
	 * \param  timeRateDesc		데이터 간의 시간 간격에 대한 descriptor	 
	 * \param  valueDesc		저장할 노드의 데이터에 대한 descriptor
	 * \param  statDesc			처리 결과에 대한 descriptor
	 * \return sfwTreeOk(성공), 그 외(오류)
	 *
	 * 본 함수는, Long pulse의 데이터를 여러 세그먼트의 레코드 형태로 노드에 추가 저장할 수 있도록 
	 * MDSplus에서 <A HREF="http://mdsplus.org/index.php?title=Documentation:Users:LongPulseExtensions&open=10334500169904705241151&page=Documentation%2FUsers+Guide%2FLong+Pulse+Extensions">MDSplus Long Pulse Extensions</A> 으로 제공하는 함수를 사용한다. \n
	 * Mdsplus의 제공 함수 중, Normal Segment 방식인 BeginSegment 함수를 사용하였다.\n
	 */
	const sfwTreeErr_e beginSegment (const char *pszTagName, const epicsInt32 iIndex,
			int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc);

	/**
	 * \fn     sfwTreeErr_e putSegment (const char *pszTagName, const epicsInt32 iIndex, 
	 *				int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc)
	 * \brief  MDSplus의 PutSegment 함수를 이용하여 세그먼트에 데이터를 저장함. 미 사용
	 */
	const sfwTreeErr_e putSegment (const char *pszTagName, const epicsInt32 iIndex,
			int startTimeDesc, int endTimeDesc, int endIdxDesc, int timeRateDesc, int valueDesc, int statDesc);

	/**
	 * \fn     sfwTreeErr_e disConnectSocket (epicsInt32 iSocket)
	 * \brief  입력으로 받은 소켓 닫기
	 * \return sfwTreeOk(성공), 그 외(오류)
	 * \see    disConnect, disConnectEvent
	 */
	const sfwTreeErr_e disConnectSocket (epicsInt32 iSocket);
};

#endif // __SFW_TREE_OBJ_H
