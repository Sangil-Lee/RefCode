///////////////////////////////////////////////////////////
//  kupaCollectPV.h
//  Implementation of the Class kupaCollectPV
//  Created on:      23-4-2013 오후 1:41:58
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaCollectPV.h
 * \ingroup kupa
 * \brief   EPICS PV의 값을 수집하는 클래스인 kupaCollectPV 정의
 * 
 * 본 파일은, EPICS PV의 값을 수집하는 클래스인 kupaCollectPV 를 정의한다. \n 
 */

#if !defined(EA_382E0D98_05D0_4ceb_9139_B399FE463A45__INCLUDED_)
#define EA_382E0D98_05D0_4ceb_9139_B399FE463A45__INCLUDED_

#include <map>

#include "kupaPV.h"
#include "kupaCollectInterface.h"

using namespace std;

typedef map<string, kupaPV*> kupaCollectPVMap;		///< 등록된 PV 객체를 관리하는 Map 타입 정의

/**
 * \class   kupaCollectPV
 * \brief   EPICS PV의 값을 수집하는 클래스
 * 
 * 본 클래스는 EPICS PV의 값을 수집하는 클래스로서,
 * 데이터 수집을 위한 공통 인터페이스를 제공하는 클래스인 kupaCollectInterface를 상속하여 그 내용을 구현하였다. \n 
 */

class kupaCollectPV : public kupaCollectInterface
{

public:

	/**
     * \fn     kupaCollectPV()
     * \brief  생성자
     */
	kupaCollectPV();

	/**
     * \fn     virtual ~kupaCollectPV()
     * \brief  소멸자. 등록된 PV들에 대한 객체를 제거한다.
     */
	virtual ~kupaCollectPV();

	/**
     * \fn     int collect()
     * \brief  데이터 수집
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 등록된 모든 PV들의 값을 수집한다. \n
     * 내부적으로는, 등록된 각 PV 객체의 collect 함수를 호출한다. \n
     */
	virtual int collect();

	/**
     * \fn     int update()
     * \brief  데이터 갱신
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 등록된 모든 PV들의 객체 내부 현재 값을 EPICS DB 내의 값으로 갱신한다. \n
     * 내부적으로는, 등록된 각 PV 객체의 update 함수를 호출한다. \n
     */
	virtual int update();

	/**
     * \fn     void reset()
     * \brief  데이터 수집 버퍼 지우기
     * \see    collect
     *
     * collect()에 의해 수집된 모든 데이터를 버퍼로부터 삭제한다. \n
     * 이는, 새로운 Shot의 데이터를 수집하기 위해 이전의 데이터를 삭제하고 버퍼를 초기화 하기 위하여 사용된다. \n
     * 내부적으로는, 등록된 각 PV 객체의 reset 함수를 호출한다. \n
     */
	virtual void reset();

	/**
     * \fn     int addPV(string pvName, string nodeName, int caType, string description, string unit)
     * \brief  데이터를 수집할 하나의 PV 추가
     * \param  pvName     	EPICS PV명
     * \param  nodeName     수집한 데이터가 저장될 대상 MDS 노드명
     * \param  caType     	값 취득을 위한 EPICS DB 유형
     * \param  description	PV의 설명
     * \param  unit     	PV 값의 단위
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 데이터를 수집할 하나의 PV 객체를 생성한 뒤, 등록된 PV들의 목록을 갖는 맵에 추가한다. \n
     * 보다 자세한 내용은, kupaCollectInterface::addPV() 함수를 참조하도록 한다.
     */
    virtual int addPV(string pvName, string nodeName, int caType, string description, string unit);

	/**
     * \fn     int updatePV(string pvName, string value)
     * \brief  해당 PV의 값 설정
     * \param  pvName		PV명
     * \param  value 		PV의 설정 값
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 받은 PV \a pvName 에 해당하는 객체 내부의 현재 값을 입력 값 \a value 로 설정한다.
     */
	virtual int updatePV(string pvName, string value);

	/**
     * \fn     string & getValue(string pvName, string &value)
     * \brief  해당 PV의 값 얻기
     * \param  pvName		PV명
     * \param  value 		PV의 현재 값
     * \return PV의 현재 값을 갖는 string 객체
     * \see	   kupaStart, updatePV
     *
     * 입력 받은 PV \a pvName 에 해당하는 객체 내부의 현재 값을 반환한다. \n
     * 내부적으로는, 등록된 각 PV 객체의 getValue 함수를 호출한다. \n
     */
	virtual string & getValue(string pvName, string &value);

	/**
     * \fn     int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser)
     * \brief  PV들의 채널 생성
     * \param  connectionCallback	채널 연결을 처리할 콜백 함수
     * \param  eventCallback 		PV 값 갱신 이벤트를 처리할 콜백 함수
     * \param  pUser 				사용자 정보를 가리키는 포인터. kupManager의 객체
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * caType이 이벤트로 설정된 모든 등록 PV들에 대해 채널을 생성한다. \n
     * 내부적으로는, 등록된 각 PV 객체의 createChannel 함수를 호출한다. \n
     */
	virtual int createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser);

	/**
     * \fn     kupaCollectPVMap & getkupaCollectPVMap()
     * \brief  등록된 PV들의 목록을 갖는 맵의 객체 얻기
     * \return PV 목록을 갖는 맵의 객체
     * \see	   addPV
     *
     * 입력 받은 PV \a pvName 에 해당하는 객체 내부의 현재 값을 반환한다. \n
     * PV의 현재 값은, kupaStart() 또는, updatePV()를 통해서 갱신될 수 있다.
     */
	kupaCollectPVMap & getkupaCollectPVMap();

private:
	kupaCollectPVMap	m_kupaCollectPVMap;		///< 등록된 PV들의 목록을 갖는 맵 객체

};
#endif // !defined(EA_382E0D98_05D0_4ceb_9139_B399FE463A45__INCLUDED_)
