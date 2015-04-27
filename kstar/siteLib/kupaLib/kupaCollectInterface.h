///////////////////////////////////////////////////////////
//  kupaCollectInterface.h
//  Implementation of the Interface kupaCollectInterface
//  Created on:      23-4-2013 오후 1:41:56
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaCollectInterface.h
 * \ingroup kupa
 * \brief   데이터 수집을 위한 공통 인터페이스를 제공하는 클래스인 kupaCollectInterface 정의
 * 
 * 본 파일은, 임의의 소스로부터 데이터를 수집하기 위한 공통 인터페이스를 제공하는 클래스 kupaCollectInterface 를 정의한다. \n 
 */

#if !defined(EA_E88D65E0_1734_4016_9CA6_6A9668EAD7EA__INCLUDED_)
#define EA_E88D65E0_1734_4016_9CA6_6A9668EAD7EA__INCLUDED_

#include <string>

#include "kupaCA.h"

using namespace std;

/**
 * \class   kupaCollectInterface
 * \brief   임의의 소스로부터 데이터를 수집하기 위한 공통 인터페이스 제공 클래스
 * 
 * 본 클래스는 임의의 소스로부터 데이터를 수집하기 위한 공통 인터페이스를 제공하는 클래스로서,
 * 내부 구현이 없이 인터페이스만을 제공하기 위한 순수 가상함수들로 구성되어 있다. \n 
 * 데이터 소스로는 EPICS PV, 소켓 및 시리얼 등의 통신, 임의 형식의 파일 등이 될 수 있다. \n
 * 참고로, 현재 EPICS PV에 대해서만 구현되어 있다. \n
 */

class kupaCollectInterface
{

public:

	/**
     * \fn     virtual ~kupaCollectInterface()
     * \brief  소멸자
     */
	virtual ~kupaCollectInterface() {};

	/**
     * \fn     int collect()
     * \brief  데이터 수집
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 해당 소스로부터 데이터를 수집하여 버퍼에 추가한다. \n
     */
	virtual int collect() =0;

	/**
     * \fn     int update()
     * \brief  데이터 갱신
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 해당 소스로부터 최신의 데이터를 취득하여 내부의 현재 값으로 갱신한다. \n
     */
	virtual int update() =0;

	/**
     * \fn     void reset()
     * \brief  데이터 수집 버퍼 지우기
     * \see    collect
     *
     * collect()에 의해 수집된 모든 데이터를 버퍼로부터 삭제한다. \n
     * 이는, 새로운 Shot의 데이터를 수집하기 위해 이전의 데이터를 삭제하고 버퍼를 초기화 하기 위하여 사용된다.
     */
	virtual void reset() =0;

	/**
     * \fn     int addPV(string pvName, string nodeName, int caType, string description, string unit)
     * \brief  데이터 수집 객체 내에 하나의 PV 명과 정보 추가
     * \param  pvName     	EPICS PV명
     * \param  nodeName     수집한 데이터가 저장될 대상 MDS 노드명
     * \param  caType     	값 취득을 위한 Channel Access 유형
     * \param  description	PV의 설명
     * \param  unit     	PV 값의 단위
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 데이터 수집을 담당하는 객체 내에 하나의 PV를 추가한다. \n
     * 입력으로 받은 \a description 과 \a unit 은, CSV 파일에 기록된다. \n
     * \n
     * PV의 값을 취득하기 위한 CA 유형 \a caType 의 종류는 다음과 같다. \n
     *   - 0 : Static 또는, Dynamic library를 이용한 PV 값 읽기. KUPA_USE_STATIC_LIB or KUPA_USE_DYNAMIC_LIB
     *   - 1 : ca_get 를 이용한 PV 값 읽기. caget
     *   - 2 : 이벤트 방식을 이용한 PV 값 읽기. camonitor
     */
	virtual int addPV(string pvName, string nodeName, int caType, string description, string unit) =0;

	/**
     * \fn     int updatePV(string pvName, string value)
     * \brief  특정 PV의 값 설정
     * \param  pvName		PV명
     * \param  value 		PV의 설정 값
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 받은 PV \a pvName 의 내부 현재 값을 입력 값 \a value 로 설정한다.
     */
	virtual int updatePV(string pvName, string value) =0;

	/**
     * \fn     string & getValue(string pvName, string &value)
     * \brief  현재 값 얻기
     * \param  pvName		PV명
     * \param  value 		PV의 현재 값
     * \return PV의 현재 값을 갖는 string 객체
     * \see	   kupaStart, updatePV
     *
     * 입력 받은 PV \a pvName 의 데이터 수집기 내부 현재 값을 반환한다. \n
     * 데이터 수집기 내부의 값은, kupaStart() 또는, updatePV()를 통해서 갱신될 수 있다.
     */
	virtual string & getValue(string pvName, string &value)=0;

	/**
     * \fn     int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser)
     * \brief  PV들의 채널 생성
     * \param  connectionCallback	채널 연결을 처리할 콜백 함수
     * \param  eventCallback 		PV 값 갱신 이벤트를 처리할 콜백 함수
     * \param  pUser 				사용자 정보를 가리키는 포인터. kupManager의 객체
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 데이터 수집기 내에 등록된 PV들 중, caType이 이벤트로 설정된 모든 PV들에 대한 채널을 생성한다.
     */
	virtual int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser) =0;

};
#endif // !defined(EA_E88D65E0_1734_4016_9CA6_6A9668EAD7EA__INCLUDED_)
