/****************************************************************************
 * sfwTree.h
 * --------------------------------------------------------------------------
 * MDSplus interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.23  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    sfwTree.h
 * \ingroup sfwtree
 * \brief   Segmented archiving을 위한 C 함수 정의
 * 
 * 본 파일은, 취득 데이터를 여러 레코드로 나눠 저장하는 MDSplus 세그먼트 방식을 구현한
 * sfwTree 라이브러리의 기능을 제공하는 것으로 C 함수를 정의한다. \n
 * 여기에서 제공하는 C 함수들은, sfwTree 라이브러리의 C++ 클래스들을 호출하는 방식으로 구현 되었다. \n
 */

#ifndef __SFW_TREE_H
#define __SFW_TREE_H

#include "kutil.h"

#include <mdslib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	SFW_TREE_NAME_LEN		64		///< 트리 또는 노드 이름의 최대 길이
#define	SFW_TREE_ADDR_LEN		24		///< 트리 주소의 최대 길이 (IP 주소와 포트 포함)
#define	SFW_TREE_PATH_LEN		256		///< 경로명의 최대 길이
#define	SFW_TREE_NODE_MAX		256		///< 하나의 트리 객체 내 최대 노드 개수

#define	SFW_TREE_SYNC_TIMEOUT	10.0	///< 데이터 저장 완료 여부의 최대 점검 시간 (초)
#define	SFW_TREE_SYNC_PERIOD	1.0		///< 데이터 저장 완료 여부의 점검 주기 (초)

/**
 * \enum   sfwTreeArchMode_e
 * \brief  Segmented archiving의 시각 지정 유형
 * 
 * MDSplus가 제공하는 레코드 방식의 저장은, 데이터의 시각을 표시하는 방법에 따라 두 가지 유형으로 나뉜다. \n
 * 즉, 상대 시각으로 표현하는 Normal 유형과 절대 시각으로 표현하는 Timestamp 유형을 사용할 수 있다. \n
 * 여기에서는 Segmented archiving 수행 시 적용할 시각 표현 유형 별로 상수를 정의한다. \n
 * 참고로, 현재까지는 Normal 방식 만을 제공한다.
 */
typedef enum {
	sfwTreeArchNormal		= 1,	///< 상대 시각 (또는, 옵셋) 방식으로 기본 값임
	sfwTreeArchTimestamp	= 2		///< 절대 시각으로 표현하는 Timestamp 유형 (미 구현)
} sfwTreeArchMode_e;

/**
 * \enum   sfwTreeSampleOrder_e
 * \brief  저장을 위해 입력되는 샘플들의 정렬 방법
 */
typedef enum {
	sfwTreeSampleByTime		= 1,	///< 시간 순서의 데이터 입력 샘플
	sfwTreeSampleByNode		= 2		///< 노드 별 데이터 입력 샘플
} sfwTreeSampleOrder_e;

/**
 * \enum   sfwTreePutType_e
 * \brief  데이터 저장 유형
 */
typedef enum {
	sfwTreePutBulk			= 1,	///< 한 번에 모든 데이터 저장 (Bulk archiving)
	sfwTreePutSegment		= 2		///< 여러 레코드로 나눠 저장 (Segmented archiving)
} sfwTreePutType_e;

/**
 * \enum   sfwTreeNodeType_e
 * \brief  MDSplus 노드의 유형
 */
typedef enum {
	sfwTreeNodeWaveform		= 1,	///< 시그널. waveform을 저장함
	sfwTreeNodeValue		= 2		///< 값 : 숫자 또는, 문자열
} sfwTreeNodeType_e;

/**
 * \enum   sfwTreeDataType_e
 * \brief  노드에 저장될 숫자 데이터의 유형
 */
typedef enum {
	sfwTreeNodeInt16		= 1,	///< 16비트 정수형
	sfwTreeNodeInt32		= 2,	///< 32비트 정수형
	sfwTreeNodeFloat32		= 3,	///< 32비트 실수형
	sfwTreeNodeFloat64		= 4		///< 64비트 실수형
} sfwTreeDataType_e;

/**
 * \enum   sfwTreeErr_e
 * \brief  오류 정의
 */
typedef enum {
	sfwTreeOk				= 0,	///< 정상
	sfwTreeErr				= 1,	///< 임의의 오류 발생
	sfwTreeErrInvalid		= 2,	///< 입력 오류
	sfwTreeErrConn			= 3,	///< 서버와의 연결 오류
	sfwTreeErrPut			= 4		///< 데이터 저장 오류 
} sfwTreeErr_e;

/**
 * \enum   sfwTreeEndAction_e
 * \brief  저장 종료 시의 처리 유형
 */
typedef enum {
	sfwTreeActionFlush		= 0,	///< 버퍼 내의 데이터 저장 수행
	sfwTreeActionDrop		= 1		///< 버퍼 내의 데이터 폐기
} sfwTreeEndAction_e;

/**
 * \enum   sfwTreeOperCode_e
 * \brief  노드 저장 관련 요청 메시지의 유형
 *
 * sfwTree 라이브러리의 사용 시, 그 기능을 수행하기 위하여 Archive 쓰레드에게 해당 요청 메시지를 전송한다.
 * 여기에서는, sfwTree 내부의 Archive 쓰레드에 전달되는 요청 메시지들의 유형을 정의한다.
 */
typedef enum {
	sfwTreeOperBegin		= 0,	///< 데이터 저장의 시작 요청
	sfwTreeOperPut			= 1,	///< 데이터 저장 요청
	sfwTreeOperEnd			= 2,	///< 데이터 저장의 종료 요청
	sfwTreeOperEvent		= 3		///< MDS 이벤트 전송 요청
} sfwTreeOperCode_e;

/**
 * \struct sfwTreeNodeInfo
 * \brief  노드 목록
 */
typedef struct {
	ELLNODE				ellNode;							///< 노드 목록을 위한 링크
	char				szTreeNodeName[SFW_TREE_NAME_LEN];	///< 노드 명
} sfwTreeNodeInfo;

/**
 * \struct sfwTreeInfo
 * \brief  트리 정보
 */
typedef struct {
	epicsUInt8			bRemote;							///< 원격 MDSplus 서버 여부. 1(원격 연결 필요)
	epicsInt32			iTreeShotNo;						///< Shot 번호
	char				szTreeName[SFW_TREE_NAME_LEN];		///< 트리 명
	char				szTreeMdsAddr[SFW_TREE_PATH_LEN];	///< MDSplus 서버의 주소 (IP 주소 + 포트 번호)
	char				szTreeEventName[SFW_TREE_NAME_LEN];	///< 이벤트 명
	char				szTreeEventAddr[SFW_TREE_PATH_LEN];	///< MDS 이벤트 서버의 주소 (IP 주소 + 포트 번호)
} sfwTreeInfo;

/**
 * \struct sfwTreeMsgHeader
 * \brief  노드 저장 관련 요청 메시지의 헤더
 */
typedef struct {
	epicsUInt32			uiOperCode;							///< 요청 메시지의 유형. \see sfwTreeOperCode_e
	epicsUInt32			uiMsgLen;							///< 요청 메시지 내의 사용자 데이터 크기 (바이트)
	epicsInt32			iTreeShotNo;						///< Shot 번호
	epicsInt32			iIndex;								///< 메시지의 인덱스
	epicsFloat64		dStartTime;							///< 데이터의 시작 시각
	char				szNodeName[SFW_TREE_NAME_LEN];		///< 노드 명
} sfwTreeMsgHeader;

/**
 * \struct sfwTreeFileHeader
 * \brief  취득 데이터를 저장하는 로컬 파일 내의 헤더로서 트리와 노드들의 정보를 가짐
 */
typedef struct {
	sfwTreeInfo			recTreeInfo;						///< 트리 정보

	epicsUInt32			uiTreeNodeNum;						///< 노드의 개수
	sfwTreeNodeInfo		recTreeNodeList[SFW_TREE_NODE_MAX];	///< 노드들의 정보 목록
} sfwTreeFileHeader;

typedef void *			sfwTreePtr;							///< 트리 객체를 나타내는 포인터
typedef void *			sfwTreeNodePtr;						///< 노드 객체를 나타내는 포인터
typedef void *			sfwTreeDataPtr;						///< 데이터를 가리키는 포인터

#define	SFW_TREE_MQ_CAPACITY	100							///< 요청 메시지의 최대 대기 개수
#define	SFW_TREE_MQ_MAX_SIZE	sizeof(sfwTreeMsgHeader)	///< 요청 메시지의 헤더 크기 (바이트)

/**
 * \fn     epicsBoolean sfwTreeEnabled ()
 * \brief  sfwTree 기능의 수행 여부 얻기
 * \return epicsTrue(기능 수행 허용), epicsFalse(기능 수행 비허용)
 * \see    sfwTreeEnable, sfwTree::isEnable()
 *
 * iocShell의 변수 <b>sfwTreeEnable</b> 을 통해 온라인 상에서 sfwTree 기능의 수행 여부를 제어할 수 있다.
 */
epicsShareFunc epicsBoolean epicsShareAPI sfwTreeEnabled ();

/**
 * \fn     sfwTreePtr sfwTreeInit (const char *pszHandleName)
 * \brief  sfwTree 객체 생성
 * \param  pszHandleName	생성될 트리 객체의 이름
 * \return 생성된 트리 객체의 포인터
 * \see    new sfwTree (pszHandleName)
 *
 * MDSplus에 데이터를 저장하기 위하여 사용되는 sfwTree 클래스의 인스턴스 (또는, 트리 객체)를 생성한다.
 * \a sfwTreePtr 유형의 반환 값은, sfwTree 객체의 주소를 갖는다.
 */
epicsShareFunc sfwTreePtr epicsShareAPI sfwTreeInit (
	const char			*pszHandleName		// name of handle (thread)
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetLocalFileHome (sfwTreePtr pSfwTree, const char *pszHomeDirName)
 * \brief  노드에 저장할 데이터의 임시 저장 경로명 설정
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  pszHomeDirName	임시 저장할 파일의 생성 경로명
 * \return sfwTreeOk(성공), sfwTreeErrInvalid(입력 디렉터리 존재하지 않음)
 * \see    sfwTreeInit, sfwTreePutData
 *
 * sfwTreePutData() 함수를 통해 MDSplus에 데이터를 저장하는 경우, 요청 받은 데이터는 저장 경로 \a pszHomeDirName 내의 로컬 파일에 임시 저장된다.
 * MDSplus로의 저장이 실패하는 경우, 로컬 파일에 저장된 데이터를 이용하여 별도의 방법으로 복구가 가능하다.
 * 단, 로컬 파일에 저장된 데이터를 이용한  복구 방법은 구현되어 있지 않다.
 * 이미 존재하는 저장 경로명 \a pszHomeDirName 을 입력으로 사용하여야 한다. (예, /data/segdata)
 * 만일, 저장 경로명이 NULL인 경우는 요청 받은 데이터를 위한 임시 파일들을 생성하지 않는다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetLocalFileHome (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	const char			*pszHomeDirName		// path name of home directory for local data file
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetPutType (sfwTreePtr pSfwTree, sfwTreePutType_e eDataPutType)
 * \brief  노드 데이터의 저장 유형 설정
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  eDataPutType		데이터의 저장 유형
 * \return sfwTreeOk(성공), sfwTreeErrInvalid(입력값 오류)
 * \see    sfwTreeInit
 *
 * 취득된 데이터는 하나의 데이터 또는, 여러 레코드로 나뉘어 저장될 수 있다.
 * 즉, Bulk 또는, Segmented archving의 저장 방식이 가능하다. \n
 * Bulk 방식이 모든 데이터를 취득한 뒤에 저장하는 것에 반해, Segmented archving은 데이터를 취득하는 도중에 저장할 수 있다. \n
 * 따라서,  Segmented archving 방식을 사용하는 경우 데이터 취득 중에도 데이터의 확인이 가능하다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetPutType (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	sfwTreePutType_e	eDataPutType		// bulk or segment
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetSamplingRate (sfwTreePtr pSfwTree, epicsUInt32 uiSamplingRate)
 * \brief  저장할 데이터의 샘플 주기 설정
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  uiSamplingRate	데이터의 샘플 주기
 * \return sfwTreeOk(성공), sfwTreeErrInvalid(실패)
 * \see    sfwTreeInit, sfwTreeGetSamplingRate, sfwTreeSetNodeSamplingRate
 *
 * 저장할 데이터의 샘플 주기 /a uiSamplingRate 를 설정한다. \n
 * 본 샘플 주기는, 저장할 데이터들의 시각을 나타내기 위한 SLOPE를 생성하는데 사용된다. \n
 * 본 샘플 주기는, 트리 내의 모든 노드에 대해 적용되는 기본 값이다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetSamplingRate (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	epicsUInt32			uiSamplingRate		// sampling rate (Hz)
);

/**
 * \fn     epicsUInt32 sfwTreeGetSamplingRate (sfwTreePtr pSfwTree)
 * \brief  샘플 주기 설정값 얻기
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return 데이터의 샘플 주기
 * \see    sfwTreeInit, sfwTreeSetSamplingRate
 */
epicsShareFunc epicsUInt32 epicsShareAPI sfwTreeGetSamplingRate (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetNodeSamplingRate (sfwTreeNodePtr pSfwTreeNode, epicsUInt32 uiSamplingRate)
 * \brief  특정 노드에 저장할 데이터의 샘플 주기 설정
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \param  uiSamplingRate	데이터의 샘플 주기
 * \return sfwTreeOk(성공), sfwTreeErrInvalid(실패)
 * \see    sfwTreeAddNode, sfwTreeGetNodeSamplingRate, sfwTreeSetSamplingRate
 *
 * 입력 노드 \a pSfwTreeNode 에 저장할 데이터의 샘플 주기 /a uiSamplingRate 를 설정한다. \n
 * 본 샘플 주기는, 노드에 저장할 데이터들의 시각을 나타내기 위한 SLOPE를 생성하는데 사용된다. \n
 * 본 샘플 주기는, 트리 내의 모든 노드에 대해 적용될 샘플 주기를 설정하는 sfwTreeSetSamplingRate()와 달리
 * 입력으로 받은 특정 노드에 적용될 데이터의 샘플 주기를 설정한다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetNodeSamplingRate (
	sfwTreeNodePtr		pSfwTreeNode,		// Tree node object created by sfwTreeAddNode()
	epicsUInt32			uiSamplingRate		// sampling rate (Hz)
);

/**
 * \fn     epicsUInt32 sfwTreeGetNodeSamplingRate (sfwTreeNodePtr pSfwTreeNode)
 * \brief  샘플 주기 설정값 얻기
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \return 해당 노드의 데이터 샘플 주기
 * \see    sfwTreeAddNode, sfwTreeSetNodeSamplingRate
 */
epicsShareFunc epicsUInt32 epicsShareAPI sfwTreeGetNodeSamplingRate (
	sfwTreeNodePtr		pSfwTreeNode		// Tree node object created by sfwTreeAddNode()
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetShotInfo (sfwTreePtr pSfwTree, epicsInt32	iTreeShotNo, 
 				const char *pszTreeName, const char *pszTreeMdsAddr, 
 				const char *pszTreeEventName, const char *pszTreeEventAddr)
 * \brief  데이터를 저장할 트리와 연결 정보 설정
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  iTreeShotNo		MDSplus 트리의 Shot 번호
 * \param  pszTreeName		트리 명
 * \param  pszTreeMdsAddr	MDSplus 서버의 주소 (IP + 포트)
 * \param  pszTreeEventName	MDS 이벤트 명. NULL이면, 이벤트를 보내지 않음
 * \param  pszTreeEventAddr	MDSplus 이벤트 서버의 주소 (IP + 포트). NULL이면, MDSplus 서버의 주소와 동일한 것으로 간주함
 * \return sfwTreeOk(성공), sfwTreeErr(트리 객체 또는, 트리명 오류)
 * \see    sfwTreeInit
 *
 * 데이터를 저장할 트리와 MDS 서버와의 연결 정보를 설정한다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetShotInfo (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	epicsInt32			iTreeShotNo,		// number of shot
	const char			*pszTreeName,		// name of tree. NULL is not allowed
	const char			*pszTreeMdsAddr,	// IP address and port # for mdsip. NULL supposes tree in local
	const char			*pszTreeEventName,	// name of event. NULL(event is not required)
	const char			*pszTreeEventAddr	// IP address and port # for event. NULL supposes tree in local
);

/**
 * \fn     sfwTreeErr_e sfwTreeSetShotNum (sfwTreePtr pSfwTree, epicsInt32	iTreeShotNo)
 * \brief  Shot 번호 설정
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  iTreeShotNo		MDSplus 트리의 Shot 번호
 * \return sfwTreeOk(성공), sfwTreeErr(트리 객체 또는, 트리명 오류)
 * \see    sfwTreeInit, sfwTreeSetShotInfo
 *
 * sfwTreeSetShotInfo()와 달리, Shot 번호만을 설정한다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSetShotNum (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	epicsInt32			iTreeShotNo			// number of shot
);

/**
 * \fn     sfwTreeNodePtr sfwTreeAddNode (sfwTreePtr pSfwTree, 
 					const char *pszNodeName, sfwTreeNodeType_e	eNodeType,
 					sfwTreeDataType_e eNodeDataType, const char *pszFileName)
 * \brief  트리 객체에 노드를 추가함
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  pszNodeName		노드 명
 * \param  eNodeType		시그널 또는, 단일 값 여부
 * \param  eNodeDataType	데이터의 유형
 * \param  pszFileName		노드 데이터의 임시 저장을 위한 로컬 파일 명
 * \return 생성된 노드 객체에 대한 포인터
 * \see    sfwTreeInit
 *
 * 노드 객체를 생성한 뒤, sfwTreeInit()으로 생성한 트리 객체 내에 노드를 추가한다.
 */
epicsShareFunc sfwTreeNodePtr epicsShareAPI sfwTreeAddNode (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	const char			*pszNodeName,		// name of node
	sfwTreeNodeType_e	eNodeType,			// type of node
	sfwTreeDataType_e	eNodeDataType,		// data type of node
	const char			*pszFileName		// name of file
);

/**
 * \fn     sfwTreeNodePtr sfwTreeAddNodeWithSamplingRate (sfwTreePtr pSfwTree, 
 					const char *pszNodeName, sfwTreeNodeType_e	eNodeType,
 					sfwTreeDataType_e eNodeDataType, const char *pszFileName,
 					epicsUInt32 uiSamplingRate)
 * \brief  트리 객체에 노드를 추가함
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  pszNodeName		노드 명
 * \param  eNodeType		시그널 또는, 단일 값 여부
 * \param  eNodeDataType	데이터의 유형
 * \param  pszFileName		노드 데이터의 임시 저장을 위한 로컬 파일 명
 * \param  uiSamplingRate	노드 고유의 데이터 샘플 주기
 * \return 생성된 노드 객체에 대한 포인터
 * \see    sfwTreeInit, sfwTreeAddNode
 *
 * 노드 객체를 생성한 뒤, sfwTreeInit()으로 생성한 트리 객체 내에 노드를 추가한다.
 * sfwTreeAddNode()와 그 기능은 동일하나, sfwTreeAddNodeWithSamplingRate()은, 노드 고유의 데이터 샘플 주기를 별도로 설정할 수 있다.
 */
epicsShareFunc sfwTreeNodePtr epicsShareAPI sfwTreeAddNodeWithSamplingRate (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	const char			*pszNodeName,		// name of node
	sfwTreeNodeType_e	eNodeType,			// type of node
	sfwTreeDataType_e	eNodeDataType,		// data type of node
	const char			*pszFileName,		// name of file
	epicsUInt32			uiSamplingRate		// sampling rate (Hz)
);

/**
 * \fn     sfwTreeErr_e sfwTreeBeginArchive (sfwTreePtr pSfwTree)
 * \brief  트리에 대한 데이터 저장 준비 작업 수행
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return sfwTreeOk(성공), sfwTreeErr(오류)
 * \see    sfwTreeInit, sfwTreeEndArchive
 *
 * MDSplus 데이터는, Shot 별로 저장이 이루어진다.
 * 이에 따라, 매 Shot 마다 트리에 데이터를 저장하기 위한 사전 작업을 수행한다.
 * 즉, sfwTreeSetShotInfo()에 의해 설정된 트리 및 서버에의 연결 정보를 이용하여,
 * MDSplus 서버와 이벤트 서버에 대한 연결을 맺고, 데이터를 저장할 트리를 연다.
 * 그리고, 데이터용 로컬 파일의 저장 경로 및 파일 명이 정상인 경우, 임시 파일을 생성한다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeBeginArchive (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     sfwTreeErr_e sfwTreePutData (sfwTreeNodePtr pSfwTreeNode, epicsInt32 iIndex,
 				epicsFloat64 dStartTime, epicsUInt32 uiSamples, sfwTreeDataPtr pDataPtr)
 * \brief  노드 데이터의 저장 요청
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \param  iIndex			세그먼트의 인덱스 번호. Segmented archiving 시 사용되며, 0부터 시작함.
 * \param  dStartTime		저장할 데이터의 시작 시각 (초). 절대 또는, 상대 시각
 * \param  uiSamples		저장할 데이터의 샘플 개수
 * \param  pDataPtr			저장할 노드의 데이터 버퍼의 주소
 * \return sfwTreeOk(성공), 그 외(오류)
 * \see    sfwTreeInit
 *
 * 본 함수는, 내부적으로 트리 객체와 함께 생성된 데이터 저장용 쓰레드인 Archive Thread에게 데이터의 저장을 요청한다. \n
 * 저장 요청된 데이터는 로컬 파일에 저장된 뒤, MDS 서버로 전달된다. \n
 * 본 함수는, 저장 요청 데이터가 MDS 서버에 저장된 것을 보장하지는 않는다. \n
 * 만일, 저장 요청한 모든 데이터가 MDS 서버로 전달이 되었는지를 확인하기 위해서는, 
 * sfwTreeSyncNode() 또는, sfwTreeSyncArchive()을 참조 하도록 한다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreePutData (
	sfwTreeNodePtr		pSfwTreeNode,		// Tree node object created by sfwTreeAddNode()
	epicsInt32			iIndex,				// index of record : 0 ~ N-1
	epicsFloat64		dStartTime,			// start time (s)
	epicsUInt32			uiSamples,			// number of samples
	sfwTreeDataPtr		pDataPtr			// pointer for data
);

/**
 * \fn     sfwTreeErr_e sfwTreePutFloat64 (sfwTreeNodePtr pSfwTreeNode, epicsFloat64 value)
 * \brief  노드의 실수 값 하나에 대한 저장 요청
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \param  value			저장할 실수 값
 * \return sfwTreeOk(성공), 그 외(오류)
 * \see    sfwTreeInit, sfwTreePutData
 *
 * 본 함수는, 한 개의 실수 값을 노드에 저장하기 위하여 사용된다.
 * 그 이외는 sfwTreePutData()의 처리와 동일하다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreePutFloat64 (
	sfwTreeNodePtr		pSfwTreeNode,		// Tree node object created by sfwTreeAddNode()
	epicsFloat64		value				// value
);

/**
 * \fn     sfwTreeErr_e sfwTreeUpdateArchive (sfwTreePtr pSfwTree)
 * \brief  데이터 변경 이벤트 전송
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return sfwTreeOk(성공), 그 외(오류)
 * \see    sfwTreeInit
 *
 * MDSplus로의 데이터 저장 이후에 노드가 변경 되었음을 알리기 위하여 MDS 이벤트를 전송한다.
 * 이 때, sfwTreeSetShotInfo()를 통해 설정한 MDS 이벤트 서버의 정보를 활용한다.
 * 본 MDS 이벤트는 jScope 등에서 노드 데이터의 변경에 따른 자동 갱신에 사용될 수 있다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeUpdateArchive (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     sfwTreeErr_e sfwTreeSyncNode (sfwTreeNodePtr pSfwTreeNode,
 				epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
 * \brief  입력 노드의 데이터 버퍼 검사
 * \param  pSfwTreeNode		sfwTreeAddNode()에 의해 생성된 노드 객체
 * \param  dTimeout			점검의 최대 만료 시간 (초)
 * \param  dCheckPeriod		점검 주기 (초)
 * \return sfwTreeOk(버퍼 내 대기 데이터 없음), sfwTreeErr(대기 데이터가 존재함)
 * \see    sfwTreeInit
 *
 * 본 함수는, sfwTreePutData() 또는, sfwTreePutFloat64() 를 통해 저장 요청된 해당 노드의 데이터들이 
 * 내부 버퍼에 쌓여 있는지 아니면, MDSplus 서버로의 저장을 위해 버퍼가 모두 비워졌는지를 확인한다. \n
 * 본 함수는, 사용자 애플리케이션에게 데이터 저장을 위한 내부 작업이 완료 되었는지를 제공하기 위해 작성 되었다. \n
 * 참고로, 버퍼 내에 저장 요청 데이터가 없다고 해서 모든 데이터가 MDSplus 저장 완료 되었다는 것을 보장하지는 않는다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSyncNode (
	sfwTreeNodePtr		pSfwTreeNode,		// Tree node object created by sfwTreeAddNode()
	epicsFloat64		dTimeout,			// timeout (seconds). default is 10s
	epicsFloat64		dCheckPeriod		// checking period (seconds). default is 1s
);

/**
 * \fn     sfwTreeErr_e sfwTreeSyncArchive (sfwTreePtr pSfwTree,
 				epicsFloat64 dTimeout, epicsFloat64 dCheckPeriod)
 * \brief  모든 노드의 데이터 버퍼 검사
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  dTimeout			점검의 최대 만료 시간 (초)
 * \param  dCheckPeriod		점검 주기 (초)
 * \return sfwTreeOk(버퍼 내 대기 데이터 없음), sfwTreeErr(대기 데이터가 존재함)
 * \see    sfwTreeInit
 *
 * 본 함수는, sfwTreePutData() 또는, sfwTreePutFloat64() 를 통해 저장 요청된 모든 노드의 데이터들이 
 * 내부 버퍼에 쌓여 있는지 아니면, MDSplus 서버로의 저장을 위해 버퍼가 모두 비워졌는지를 확인한다. \n
 * 본 함수는, 사용자 애플리케이션에게 데이터 저장을 위한 내부 작업이 완료 되었는지를 제공하기 위해 작성 되었다. \n
 * 참고로, 버퍼 내에 저장 요청 데이터가 없다고 해서 모든 데이터가 MDSplus 저장 완료 되었다는 것을 보장하지는 않는다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeSyncArchive (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	epicsFloat64		dTimeout,			// timeout (seconds). default is 10s
	epicsFloat64		dCheckPeriod		// checking period (seconds). default is 1s
);

/**
 * \fn     sfwTreeErr_e sfwTreeEndArchive (sfwTreePtr pSfwTree, sfwTreeEndAction_e eAction)
 * \brief  데이터 저장 완료 작업 수행
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \param  eAction			저장 종료 시의 처리 유형. 미 구현
 * \return sfwTreeOk(성공), sfwTreeErr(오류)
 * \see    sfwTreeInit, sfwTreeBeginArchive
 *
 * 본 함수는, 모든 데이터에 대한 저장 요청을 완료한 뒤 더 이상 사용하지 않는 자원을 해제하기 위한 것으로,
 * sfwTreeBeginArchive()에서 수행한 MDSplus 서버와 이벤트 서버로의 연결을 해제하고, 열려 있는 임시 데이터 파일을 닫는다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeEndArchive (
	sfwTreePtr			pSfwTree,			// Tree object created by sfwTreeInit()
	sfwTreeEndAction_e	eAction				// action for remained data in buffer
);

/**
 * \fn     sfwTreeErr_e sfwTreeDelete (sfwTreePtr pSfwTree)
 * \brief  트리 삭제
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return sfwTreeOk(성공), sfwTreeErr(오류)
 * \see    sfwTreeInit
 *
 * 본 함수는, sfwTreeInit()에 의해 생성된 트리 객체를 제거한다.
 * 트리 객체의 생성 및 제거는 프로그램 기동 및 종료 시에 한 번만 수행해 주면 된다.
 */
epicsShareFunc sfwTreeErr_e epicsShareAPI sfwTreeDelete (
	sfwTreePtr			*pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     void sfwTreePrintInfo (sfwTreePtr pSfwTree)
 * \brief  트리 설정 정보 표시
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return 없음
 * \see    sfwTreeInit, sfwTreeSetShotInfo
 *
 * 본 함수는, sfwTreeSetShotInfo()에 의해 설정된 트리 및 MDSplus 서버에 대한 정보를 화면에 표시한다.
 */
epicsShareFunc void epicsShareAPI sfwTreePrintInfo (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     void sfwTreePrintNodes (sfwTreePtr pSfwTree)
 * \brief  트리 내의 노드 목록 표시
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return 없음
 * \see    sfwTreeInit, sfwTreeAddNode, sfwTreeAddNodeWithSamplingRate
 *
 * 본 함수는, sfwTreeAddNode() 또는, sfwTreeAddNodeWithSamplingRate()에 의해 생성된 모든 노드의 목록을 표시한다.
 */
epicsShareFunc void epicsShareAPI sfwTreePrintNodes (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

/**
 * \fn     epicsBoolean sfwTreeConnected (sfwTreePtr pSfwTree)
 * \brief  MDSplus 서버와의 연결 여부 반환
 * \param  pSfwTree			sfwTreeInit()에 의해 생성된 트리 객체
 * \return epicsTrue(연결되어 있음), epicsFalse(비연결)
 * \see    sfwTreeInit, sfwTreeBeginArchive
 */
epicsShareFunc epicsBoolean epicsShareAPI sfwTreeConnected (
	sfwTreePtr			pSfwTree			// Tree object created by sfwTreeInit()
);

#ifdef __cplusplus
}
#endif

#endif	// __SFW_TREE_H

