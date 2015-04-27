/****************************************************************************
 * kupaMDS.h
 * --------------------------------------------------------------------------
 * MDSplus interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.05.13  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kupaMDS.h
 * \ingroup kupa
 * \brief   MDSplus에 대한 접근을 지원하는 클래스인 kupaMDS 정의
 * 
 * 본 파일은, MDSplus에 대한 접근을 지원하는 클래스인 kupaMDS 를 정의한다. \n 
 */

#ifndef _KUPA_MDS_H
#define _KUPA_MDS_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <mdslib.h>

using namespace std;

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuDebug.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

/**
 * \class   kupaMDS
 * \brief   MDSplus에 대한 접근을 지원하는 클래스
 * 
 * 본 클래스는 MDSplus에 대한 접근을 지원하는 클래스로서, 수집된 데이터를 MDSplus로 저장할 때 필요로 하는 기능을 제공한다. \n 
 */

class kupaMDS
{
public :
	kupaMDS ();		///< 생성자. init() 호출
	~kupaMDS ();	///< 소멸자. MDSplus 서버와의 연결 해제

	/**
     * \fn     kuBoolean init()
     * \brief  내부 변수 초기화
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean init ();

	/**
     * \fn     kuBoolean set (string treeName, string mdsAddr, string eventName, string eventAddr)
     * \brief  입력 값으로 내부 변수 설정
     * \param  treeName    	MDS 트리명
     * \param  mdsAddr     	MDSplus 서버의 주소
     * \param  eventName   	MDSpuls 이벤트명
     * \param  eventAddr   	MDSplus 이벤트 서버의 주소
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean set (string treeName, string mdsAddr, string eventName, string eventAddr);

	/**
     * \fn     kuBoolean connect()
     * \brief  MDSplus 서버에 대한 연결 맺기
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean connect ();

	/**
     * \fn     kuBoolean disConnect()
     * \brief  MDSplus 서버에 대한 연결 해제
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean disConnect ();

	/**
     * \fn     kuBoolean createTree (const int nShotNo)
     * \brief  트리 생성
     * \param  nShotNo    	생성할 트리의 번호
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean createTree (const int nShotNo);

	/**
     * \fn     kuBoolean open (const int nShotNo)
     * \brief  트리 열기
     * \param  nShotNo    	트리의 번호
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean open (const int nShotNo);

	/**
     * \fn     kuBoolean close ()
     * \brief  트리 닫기
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean close ();

	/**
     * \fn     kuBoolean write (const char *szTagName, const char *szUnit, const void *databuf,
     * 					int nDataType, int nSamples, kuFloat64 fStartTime, kuFloat64 fTimeGap)
     * \brief  데이터를 MDSplus 서버에 저장
     * \param  szTagName    	MDS 태그명 또는 노드명
     * \param  szUnit    		데이터의 단위
     * \param  databuf    		데이터 버퍼의 주소
     * \param  nDataType    	데이터의 유형
     * \param  nSamples    		데이터의 개수
     * \param  fStartTime    	데이터의 시작 시각 (초)
     * \param  fTimeGap    		데이터 사이의 시간 간격 (초)
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean write (const char *szTagName, const char *szUnit, const void *databuf,
					int nDataType, int nSamples, kuFloat64 fStartTime, kuFloat64 fTimeGap);

	/**
     * \fn     kuBoolean write (const char *szTagName, const void *databuf)
     * \brief  하나의 값을 MDSplus 서버에 저장
     * \param  szTagName    	MDS 태그명 또는 노드명
     * \param  databuf    		float 타입의 값
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean write (const char *szTagName, const void *databuf);

	/**
     * \fn     kuBoolean sendEvent ()
     * \brief  데이터의 변경을 알리기 위하여, MDSplus 서버에 이벤트를 전송함
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean sendEvent ();

	/**
     * \fn     char *getStatusMsg ()
     * \brief  MDS API 호출 결과에 대한 문자열 얻기. 미 구현
     * \return MdsGetMsg()의 결과. 실제로는 항상 NULL을 반환함
     *
     * MdsGetMsg()를 사용하는 경우, 컴파일 오류가 발생한다.\n
     * 이에 따라, 본 함수에서는 항상 NULL을 반환한다.\n
     * MDS API 호출의 성공 여부를 판단하기 위해서는, isOK()함수를 사용하도록 한다.
     */
	//const char *getStatusMsg () { return (MdsGetMsg (m_status)); }
	const char *getStatusMsg () { return (NULL); }

	/**
     * \fn     kuBoolean isConnected ()
     * \brief  MDSplus 서버와의 연결 여부 얻기
     * \return 연결(kuTRUE), 비 연결(kuFALSE)
     */
	const kuBoolean isConnected () { return (m_bConnected); }

	/**
     * \fn     void setConnected (const kuBoolean bConnected)
     * \brief  MDSplus 서버와의 연결 여부 설정
     * \param  bConnected  		연결 여부
     */
	void setConnected (const kuBoolean bConnected) { m_bConnected	= bConnected; }

	/**
     * \fn     kuBoolean isOpened ()
     * \brief  트리가 데이터 저장을 위해 열려 있는지의 여부 얻기
     * \return 열려 있음(kuTRUE), 열려 있지 않음(kuFALSE)
     */
	const kuBoolean isOpened () { return (m_bOpened); }

	/**
     * \fn     void setOpened (const kuBoolean bOpened)
     * \brief  트리가 열려 있는지의 여부 설정
     * \param  bOpened  		트리의 열림 여부
     */
	void setOpened (const kuBoolean bOpened) { m_bOpened	= bOpened; }

	/**
     * \fn     void printInfo (const int nPrintLevel, const char *name)
     * \brief  내부 변수의 설정 값 표시
     */
	void printInfo (const int nPrintLevel, const char *name);

	/**
     * \fn     kuInt32 getSize (const char *szTagName)
     * \brief  MDSplus 서버로부터 노드의 데이터 크기 얻기
     * \param  szTagName    	MDS 태그명 또는 노드명
     * \return 성공(SIZE의 결과), 실패(kuNOK)
     */
	const kuInt32 getSize (const char *szTagName);

	/**
     * \fn     kuBoolean getData (const char *szTagName, double *timebase, double *shotdata, int size)
     * \brief  MDSplus 서버로부터 노드의 데이터와 시각 정보 얻기
     * \param  szTagName    MDS 태그명 또는 노드명
     * \param  timebase    	MDSplus 서버로부터 얻은 시간 정보. DIM_OF()의 결과
     * \param  shotdata    	MDSplus 서버로부터 얻은 데이터. DTYPE_DOUBLE
     * \param  size	    	얻고자 하는 데이터의 크기 (개수)
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean getData (const char *szTagName, double *timebase, double *shotdata, int size);

	/**
     * \fn     kuBoolean getAverage (const char *szTagName, int(*filter)(double), double &dAverage)
     * \brief  MDSplus 서버로부터 노드 데이터의 평균 값 얻기
     * \param  szTagName    MDS 태그명 또는 노드명
     * \param  filter    	노드의 평균 값을 계산하는데 사용될 데이터의 선별을 위한 사용자 필터 함수
     * \param  dAverage    	MDSplus 서버로부터 얻은 데이터의 평균 값
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
    const kuBoolean getAverage (const char *szTagName, int(*filter)(double), double &dAverage);

	/**
     * \fn     kuBoolean getIntegral (const char *szTagName, double &dIntegral)
     * \brief  MDSplus 서버로부터 노드 데이터의 적분 값 얻기
     * \param  szTagName    MDS 태그명 또는 노드명
     * \param  dIntegral   	MDSplus 서버로부터 얻은 데이터의 적분 값
     * \return 성공(kuTRUE), 실패(kuFALSE)
     */
	const kuBoolean getIntegral (const char *szTagName, double &dIntegral);

protected :
	const int getShotNo () { return (m_nShotNo); }					///< Shot 번호 얻기
	void setShotNo (const int nShotNo) { m_nShotNo	= nShotNo; }	///< Shot 번호 설정

	/**
     * \fn     kuBoolean isOK (const int status)
     * \brief  MDSplus API의 정상 호출 여부 얻기
     * \param  status    MDSplus API의 호출 결과를 나타내는 상태 값
     * \return 정상(kuTRUE), 오류(kuFALSE)
     */
	const kuBoolean isOK (const int status) {
		m_status	= status;
		return ( (status & 1) ? kuTRUE : kuFALSE );
	}

private :
	int			m_socket;		///< MDSplus 서버에 대한 소켓 연결
	kuBoolean	m_bConnected;	///< MDSplus 서버에 대한 연결 여부
	kuBoolean	m_bOpened;		///< MDS 트리가 열려 있는지의 여부
	int			m_status;		///< MDSplus API의 호출 상태 값
	int			m_nShotNo;		///< Shot 번호

	string		m_treeName;		///< 트리명
	string		m_mdsAddr;		///< MDSplus 서버의 주소
	string		m_eventName;	///< MDS 이벤트명
	string		m_eventAddr;	///< MDSplus 이벤트 서버의 주소

};

#endif // _KUPA_MDS_H
