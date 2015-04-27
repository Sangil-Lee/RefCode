///////////////////////////////////////////////////////////
//  kupaExportMDS.h
//  Implementation of the Class kupaExportMDS
//  Created on:      23-4-2013 오후 1:41:52
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaExportMDS.h
 * \ingroup kupa
 * \brief   수집된 데이터를 MDSplus에 저장하는 클래스인 kupaExportMDS 정의
 * 
 * 본 파일은, 수집된 데이터를 MDSplus에 저장하는 클래스인 kupaExportMDS 를 정의한다. \n 
 */

#if !defined(EA_581BDE8F_1CE9_4011_A612_166C52981E2E__INCLUDED_)
#define EA_581BDE8F_1CE9_4011_A612_166C52981E2E__INCLUDED_

#include "kupaExportInterface.h"

#define KUPA_EXPORT_MDS_LOCAL	"LOCAL"
#define KUPA_EXPORT_MDS_REMOTE	"REMOTE"

/**
 * \class   kupaExportMDS
 * \brief   수집된 데이터를 MDSplus에 저장하는 클래스
 * 
 * 본 클래스는 수집된 데이터를 MDSplus에 저장하는 클래스로서,
 * 수집된 데이터를 다양한 형식으로 내보내기 위한 공통 인터페이스를 제공하는 kupaExportInterface를 상속하여 그 내용을 구현하였다. \n 
 */

class kupaExportMDS : public kupaExportInterface
{

public:

	/**
     * \fn     kupaExportMDS(kupaCollectInterface * pCollect,
     *			char *treeName, char *mdsAddr, char *eventName, char *eventAddr,
     *			char *opModeName, char* arg6, char* arg7, char* arg8, char* arg9)
     * \brief  생성자
     * \param  pCollect     데이터 수집 객체
     * \param  treeName     MDS 트리명
     * \param  mdsAddr     	MDsplus 서버의 주소
     * \param  eventName	MDS 이벤트명
     * \param  eventAddr   	MDsplus 이벤트 서버의 주소
     * \param  opModeName	운전 모드명
     * \param  arg6     	Reserved.
     * \param  arg7     	Reserved.
     * \param  arg8     	Reserved.
     * \param  arg9     	Reserved.
     * \return 없음
     *
     * 입력으로부터 내부 변수를 설정한다.\n
     */
	kupaExportMDS(kupaCollectInterface * pCollect, 
		char *treeName, char *mdsAddr, char *eventName, char *eventAddr,
		char *opModeName, char* arg6, char* arg7, char* arg8, char* arg9);
		
	/**
     * \fn     virtual ~kupaExportMDS()
     * \brief  소멸자
     */
	virtual ~kupaExportMDS();

	/**
     * \fn     int store()
     * \brief  데이터를 MDSplus에 저장하기
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 수집된 데이터를 MDSplus에 저장한다. \n
     * 단, Segmented archiving 방식을 사용하지는 않는다. \n
     */
	virtual int store();

	/**
     * \fn     kupaCollectInterface * getCollectInterface()
     * \brief  데이터 수집 객체 얻기
     * \return 데이터 수집 객체
     *
     * 버퍼 내에 수집된 데이터를 갖고 있는 데이터 수집 객체를 얻는다.
     */
	kupaCollectInterface * getCollectInterface();

	string getTreeName() { return (m_treeName); }				///< 트리명 얻기
	string getMdsAddr() { return (m_mdsAddr); }					///< MDSplus 서버의 주소 얻기
	string getEventName() { return (m_eventName); }				///< 이벤트명 얻기
	string getEventAddr() { return (m_eventAddr); }				///< MDSplus 이벤트 서버의 주소 얻기
	string getOpModeName() { return (m_opModeName); }			///< 운전 모드명 얻기

	void setTreeName(char *newVal) { m_treeName = newVal; }		///< 트리명 설정
	void setMdsAddr(char *newVal) { m_mdsAddr = newVal; }		///< MDSplus 서버의 주소 설정
	void setEventName(char *newVal) { m_eventName = newVal; }	///< 이벤트명 설정
	void setEventAddr(char *newVal) { m_eventAddr = newVal; }	///< MDSplus 이벤트 서버의 주소 설정
	void setOpModeName(char *newVal) { m_opModeName = newVal; }	///< 운전 모드명 설정

private:
	kupaCollectInterface *	m_kupaCollectInterface;				///< 생성자의 입력. 데이터 수집 객체

	string m_treeName;		///< 생성자의 입력. 트리명
	string m_mdsAddr;		///< 생성자의 입력. MDSplus 서버의 주소
	string m_eventName;		///< 생성자의 입력. 이벤트명
	string m_eventAddr;		///< 생성자의 입력. MDSplus 이벤트 서버의 주소
	string m_opModeName;	///< 생성자의 입력. 운전 모드명
};

#endif // !defined(EA_581BDE8F_1CE9_4011_A612_166C52981E2E__INCLUDED_)
