///////////////////////////////////////////////////////////
//  kupaPV.h
//  Implementation of the Class kupaPV
//  Created on:      23-4-2013 ¿H 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaPV.h
 * \ingroup kupa
 * \brief   EPICS PV 값에 대한 접근 기능을 제공하는 클래스인 kupaPV 정의
 * 
 * 본 파일은, EPICS PV 값에 대한 접근 기능을 제공하는 클래스인 kupaPV 를 정의한다. \n 
 */

#if !defined(EA_09CFE9D9_3A73_4161_96D9_CF2561D39E4C__INCLUDED_)
#define EA_09CFE9D9_3A73_4161_96D9_CF2561D39E4C__INCLUDED_

#include <iostream>
#include <string>
#include <vector>

#include "cadef.h"		// chid

#include "kupaDB.h"
#include "kupaCA.h"

// uses static library for database access
#define KUPA_USE_STATIC_LIB		0						///< Static 라이브러리의 사용 여부. 비 사용

// uses dynamic library for database access
#define	KUPA_USE_DYNAMIC_LIB	!KUPA_USE_STATIC_LIB	///< Dynamic 라이브러리의 사용 여부. 기본 값. Static 라이브러리의 사용 여부와 반대임

// performance issue
#define	KUPA_USE_VALUE_LOCK		0						///< 내부의 현재 값 접근 시의 록 사용 여부

using namespace std;

typedef vector<string>	kupaPVValue;					///< 수집한 PV 값들을 저장하기 위한 버퍼

/**
 * \class   kupaPV
 * \brief   EPICS PV 값에 대한 접근 기능을 제공하는 클래스
 * 
 * 본 클래스는 EPICS PV 값에 대한 접근 기능을 제공하는 클래스로서,
 * 데이터 수집 객체에서 PV 값을 얻기 위하여 사용된다. \n 
 */

class kupaPV
{

public:

	/**
	 * \enum   kupaPvCaType
	 * \brief  EPICS PV로부터 값을 얻기 위한 접근 방법
	 */
	enum kupaPvCaType {
		KUPA_CA_DB		= 0,	///< Static 또는, Dynamic 라이브러리 사용. IOC 내부
		KUPA_CA_GET		= 1,	///< ca_get을 사용하는 Polling 방식
		KUPA_CA_EVENT	= 2		///< 채널을 사용한 이벤트 방식
	};

	/**
     * \fn     kupaPV(string pvName, string nodeName, int caType, string description, string unit)
     * \brief  생성자
     * \param  pvName     	EPICS PV명
     * \param  nodeName     수집한 데이터가 저장될 대상 MDS 노드명
     * \param  caType     	값 취득을 위한 EPICS DB 유형
     * \param  description	PV의 설명
     * \param  unit     	PV 값의 단위
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 값으로 내부 변수를 설정하고, 
     * PV 값에 대한 접근 유형이 로컬 DB로 지정된 경우 DB에 대한 접근을 초기화 한다. \n
     */
	kupaPV(string pvName, string nodeName, int caType, string description, string unit);

	/**
     * \fn     virtual ~kupaPV()
     * \brief  소멸자. 생성자에서 초기화된 로컬 DB 접근 관련 자원을 해제함.
     */
	virtual ~kupaPV();

	/**
     * \fn     int collect()
     * \brief  실제의 PV 값을 얻어 내부의 현재 값을 갱신한 뒤 버퍼에 추가함
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 PV 객체에 지정된 PV의 실제 값을 가져와 내부 버퍼에 추가한다. \n
     * 이 때, 실제의 PV 값을 얻기 위하여 update() 함수를 호출하여 내부의 현재 값을 갱신한 뒤 버퍼에 추가한다. \n
     */
	int collect();

	/**
     * \fn     int update()
     * \brief  실제의 PV 값을 얻어 내부의 현재 값을 갱신함
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 PV 객체에 지정된 PV의 실제 값을 가져와 내부의 현재 값으로 설정한다. \n
     * DB 접근 유형이 이벤트로 지정된 PV의 값은 채널에 의해서 자동으로 갱신이 되므로, 
     * 별도로 update()가 필요하지는 않다.
     */
	int update();

	/**
     * \fn     void reset()
     * \brief  데이터 수집 버퍼 지우기
     * \see    collect
     *
     * collect()에 의해 수집된 모든 데이터를 버퍼로부터 삭제한다. \n
     */
	void reset();

	/**
     * \fn     int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser)
     * \brief  CA 채널 생성
     * \param  connectionCallback	CA 연결을 처리할 콜백 함수
     * \param  eventCallback 		CA 이벤트를 처리할 콜백 함수
     * \param  pUser 				사용자 정보를 가리키는 포인터. kupManager의 객체
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 객체에 설정된 PV에 대해 CA 채널을 생성한다. \n
     * 이 때, EPICS의 ca_create_channel 을 사용한다. \n
     */
	int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser);

	/**
     * \fn     int addEvent(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser)
     * \brief  PV의 DB 접근 유형이 이벤트인 경우, 생성된 채널에 가입함
     * \param  connectionCallback	채널 연결을 처리할 콜백 함수
     * \param  eventCallback 		PV 값 갱신 이벤트를 처리할 콜백 함수
     * \param  pUser 				사용자 정보를 가리키는 포인터. kupManager의 객체
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 객체에 설정된 PV의 DB 접근 유형이 이벤트로 설정 되어 있는 경우 채널을 생성한다. \n
     * 이 때, EPICS의 ca_create_subscription 을 사용한다. \n
     * 본 채널을 통해 camonitor와 같은 방식으로 변경되는 PV 값을 자동으로 얻을 수 있다. \n
     */
	int addEvent(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser);

	/**
     * \fn     int addEvent()
     * \brief  PV의 DB 접근 유형이 이벤트인 경우, 생성된 채널에 가입함
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 채널의 연결과 이벤트의 처리를 위해, 내부의 기본 콜백 함수를 사용한다. \n
     */
	int addEvent();

	string getPVName();				///< PV명 얻기
	string getPVFieldName();		///< PV의 필드명 얻기. PV명.VAL에 해당함
	string getNodeName();			///< 데이터가 저장될 MDS 노드 또는, 태그명 얻기
	string getDescription();		///< PV의 설명
	string getUnit();				///< PV 값의 단위
	string getValue();									///< 내부의 현재 값 얻기
	string & getValue(string &value);					///< 내부의 현재 값 얻기
	string & getValue(kuUInt32 idx, string &value);		///< 버퍼 내 특정 인덱스의 값 얻기
	int getCAType();									///< DB 접근 유형 얻기. kupaPvCaType
	int getValueSize();									///< 버퍼 내의 PV 값 개수 얻기
	int getValues(kuUInt32 size, kuFloat64 *pValues);	///< 요청한 개수 size 만큼의 값 얻기
	chid getChId();					///< EPICS CA 채널 ID 얻기
	int getDataType();				///< 데이터 유형 얻기
	int isConnected();				///< 연결 여부 얻기
	int isEventType();				///< CA 이벤트 유형 여부 얻기
	int isValidDb();				///< DB 초기화 정상 여부 얻기
	kupaConnectionHandler getConnectHandler();		///< 연결 처리를 위한 핸들러 함수 얻기
	kupaEventHandler getEventHandler();				///< 이벤트 처리를 위한 핸들러 함수 얻기

	void setPVName(string newVal);					///< PV명 설정
	void setNodeName(string newVal);				///< 노드명 설정
	void setDescription(string newVal);				///< 설명 설정
	void setUnit(string newVal);					///< 단위 설정
	void setValue(string newVal);					///< 내부의 현재 값 설정
	void setChId(chid newVal);						///< EPICS 채널 ID 설정
	void setDataType(int dataType);					///< 데이터 유형 설정
	void setCAType(int caType);						///< DB 접근 유형 설정
	void setConnected(int bConnected);				///< 채널에 대한 연결 여부 확인
	void setConnectHandler(kupaConnectionHandler connectionCallback);	///< 연결 처리 핸들러 함수 등록
	void setEventHandler(kupaEventHandler eventCallback);				///< 이벤트 처리 핸들러 함수 등록

private:
	string m_pvName;				///< PV명
	string m_pvFieldName;			///< 필드명. PV명.VAL
	string m_nodeName;				///< 노드명
	string m_description;			///< 설명
	string m_unit;					///< 단위
	int m_caType;					///< DB 접근 유형. kupaPvCaType
	chid m_chid;					///< EPICS 채널 ID
	int m_dataType;					///< 데이터 유형
	int m_bConnected;				///< 채널에 대한 연결 여부

	// access of string requires lock mechanism for integrity. this causes performance burden
	//string m_currValue;
	char m_currValue[128];			///< 현재 값의 문자열

	kupaPVValue m_valueVector;					///< 수집한 값의 배열
	kupaConnectionHandler m_connectCallback;	///< 연결 처리 핸들러 함수
	kupaEventHandler m_eventCallback;			///< 이벤트 처리 핸들러 함수

	void setPVFieldName(string newVal);			///< PV의 값 필드명 설정
	void setValidDb(int nveVal);				///< DB 초기화 정상 여부 설정

	void lockValue();				///< 현재 값 접근 록 설정
	void unlockValue();				///< 현재 값 접근 록 해제

	int m_bValidDb;					///< DB 초기화 정상 여부
	int m_dbStatus;					///< 미 사용
#if KUPA_USE_STATIC_LIB
	kupapDbEntry m_pdbentry;		///< Static library 사용 시의 dbAllocEntry 
#endif
#if KUPA_USE_DYNAMIC_LIB
	kupapDbAddr m_pdbAddr;			///< Dynamic library 사용 시의 DBADDR 주소 공간
#endif

	static kuLock m_valueLock;		///< 현재 값 접근 록
};
#endif // !defined(EA_09CFE9D9_3A73_4161_96D9_CF2561D39E4C__INCLUDED_)
