///////////////////////////////////////////////////////////
//  kupaManager.h
//  Implementation of the Class kupaManager
//  Created on:      23-4-2013 ¿H 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaManager.h
 * \ingroup kupa
 * \brief   EPICS PV Archiving 관리자 클래스인 kupaManager 정의
 * 
 * 본 파일은, EPICS PV Archiving 관리자 클래스인 kupaManager 를 정의한다. \n 
 */

#if !defined(EA_FAC80277_F4F9_4b7b_A82D_9E59464FE380__INCLUDED_)
#define EA_FAC80277_F4F9_4b7b_A82D_9E59464FE380__INCLUDED_

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "kutilObj.h"

#include "kupaCollectInterface.h"
#include "kupaExportInterface.h"

using namespace std;

typedef vector<kupaExportInterface *>		kupaExportInterfaceVec;		///< 내보내기 객체들의 벡터
typedef map<string, kupaCollectInterface *>	kupaCollectInterfaceMap;	///< 데이터 수집기 객체들의 맵

#define	KUPA_EXPORT_TYPE_CSV	"CSV"		///< CSV 파일 형식의 내보내기
#define	KUPA_EXPORT_TYPE_MDS	"MDS"		///< MDS DB로의 내보내기
#define	KUPA_EXPORT_TYPE_FILE	"FILE"		///< 파일로의 내보내기

/**
 * \enum   kupaSvcType
 * \brief  PV Archiving 관리자로의 요청 서비스 유형
 */
enum kupaSvcType {
	KUPA_SVC_NONE		= 0x01,		///< 별도로 서비스가 지정되지 않음. 기본 값
	KUPA_SVC_START		= 0x01,		///< 데이터 수집 기능 시작
	KUPA_SVC_UPDATE		= 0x02		///< 현재 값 갱신
};

class kupaPV;

/**
 * \class   kupaMessage
 * \brief   PV Archiving 관리자에 대한 요청 서비스의 메시지 형식
 * 
 * 본 클래스는 PV Archiving 관리자로의 요청 서비스를 전달하기 위한 메시지의 형식을 나타내는 클래스로서,
 * 데이터 수집 객체에서 PV 값을 얻기 위하여 사용된다. \n 
 */

class kupaMessage
{
public :
	kupaMessage (int svc = KUPA_SVC_NONE) { setSvcType (svc); }		///< 생성자. 서비스 유형 설정
	~kupaMessage () {};												///< 소멸자

	int getSvcType () { return (m_svcType); }						///< 서비스 유형 얻기
	void setSvcType (int svc) { m_svcType = svc; }					///< 서비스 유형 설정

private :
	int	m_svcType;													///< 서비스 유형
};

/**
 * \class   kupaManager
 * \brief   EPICS PV Archiving 관리자 클래스
 * 
 * 본 클래스는 EPICS PV Archiving 관리자를 위한 클래스로서,
 * 데이터 수집의 시작 및 지정 형식으로의 내보내기 등을 수행한다. \n 
 */

class kupaManager
{

public:

	/**
     * \fn     virtual ~kupaManager()
     * \brief  소멸자. 내부 자원들의 해제
     */
	virtual ~kupaManager();
	
	/**
     * \fn     kupaManager* getInstance()
     * \brief  kupaManager의 단일 객체 (인스턴스) 생성 및 반환
     */
	static kupaManager* getInstance();

	/**
     * \fn     kupaCollectInterface* getkupaCollectInterface()
     * \brief  데이터 수집기 객체들의 맵 반환 
     */
	kupaCollectInterface* getkupaCollectInterface();

	/**
     * \fn     kupaExportInterfaceVec &getkupaExportInterfaceVec()
     * \brief  내보내기 객체들의 벡터 반환 
     */
	kupaExportInterfaceVec &getkupaExportInterfaceVec();

	kupaManager* getkupaManager();	///< 내부의 kupaManager 객체 반환
	string getManagerName();		///< kupaManager 객체에 지정된 이름 얻기
	string getShotNumPvName();		///< Shot 번호 관련 PV 명 얻기
	string getArmPvName();			///< SYS_ARMING 관련 PV 명 얻기
	string getRunPvName();			///< SYS_RUN 관련 PV 명 얻기
	string getOpModePvName();		///< SYS_OP_MODE 관련 PV 명 얻기
	string getPostActionCmd();		///< 데이터 수집 및 내보내기 이후에 수행될 명령의 문자열 얻기
	int getOpMode();				///< Operation 모드 얻기
	long getShotNum();				///< Shot 번호 얻기
	int getSamplingRate();			///< 샘플 주기 얻기
	double getStartTime();			///< 데이터의 시작 시각 얻기
	double getDuration();			///< 데이터의 수집 시간 얻기
	double getDelaySec();			///< 데이터 수집의 지연 시간 얻기
	epicsEventId getStartEventId();			///< 데이터 수집 시작을 알리기 위한 이벤트 식별자 얻기. 메시지 큐로 대체됨
	epicsMessageQueueId getMsgQueueId();	///< kupaManager에게 서비스를 전달하기 위한 메시지 큐 ID 얻기
	int getSampleCnt();						///< 수집해야 할 총 데이터의 개수 얻기

	void setkupaCollectInterface(kupaCollectInterface* newVal);			///< 데이터 수집기 객체들의 맵 설정
	void setkupaExportInterfaceVec(kupaExportInterfaceVec &newVal);		///< 내보내기 객체들의 벡터 설정
	void setkupaManager(kupaManager *newVal);	///< 내부의 kupaManager 객체 설정
	void setManagerName(string newVal);			///< kupaManager 객체의 이름 설정
	void setShotNumPvName(string newVal);		///< Shot 번호 관련 PV 명 설정
	void setArmPvName(string newVal);			///< SYS_ARMING 관련 PV 명 설정
	void setRunPvName(string newVal);			///< SYS_RUN 관련 PV 명 설정
	void setOpModePvName(string newVal);		///< SYS_OP_MODE 관련 PV 명 설정
	void setPostActionCmd(string newVal);		///< 데이터 수집 및 내보내기 이후에 수행될 명령의 문자열 설정

	void setOpMode(int newVal);					///< Operation 모드 설정
	void setShotNum(long newVal);				///< Shot 번호 설정
	void setSamplingRate(int newVal);			///< 샘플 주기 설정
	void setStartTime(double newVal);			///< 데이터의 시작 시각 설정
	void setDuration(double newVal);			///< 데이터의 수집 시간 설정
	void setDelaySec(double newVal);			///< 데이터 수집의 지연 시간 설정

	/**
     * \fn     int initManager(char* name, char *shotNumPvName, char* opModePvName, char* runPvName,
     *				int samplingRate, double startTime, double duration, double delaySec)
     * \brief  내부 변수 초기화 및 PV 수집 객체 생성
     * \param  name				kupaManager의 고유 이름
     * \param  shotNumPvName	Shot 번호 관련 PV 명
     * \param  opModePvName		SYS_OP_MODE 관련 PV 명
     * \param  runPvName		SYS_RUN 관련 PV 명
     * \param  samplingRate		샘플 주기
     * \param  startTime		데이터의 시작 시각
     * \param  duration			데이터의 수집 시간
     * \param  delaySec			데이터 수집의 지연 시간
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 데이터 수집 및 내보내기를 위한 각종 설정 값을 입력으로 받아 내부 변수에 설정한다. \n
     * 그리고, 데이터를 수집할 PV 목록을 관리할 kupaCollectPV 객체를 생성한다. \n
     */
	int initManager(char* name, char *shotNumPvName, char* opModePvName, char* runPvName,
			int samplingRate, double startTime, double duration, double delaySec);

	/**
     * \fn     int addExport(char *type, char* name, char *path, 
     *				char* arg3, char* arg4, char* arg5, char* arg6, char* arg7, char* arg8, char* arg9)
     * \brief  내보내기 객체 생성 및 벡터에 추가
     * \param  type		내보내기 형식. KUPA_EXPORT_TYPE_CSV, KUPA_EXPORT_TYPE_MDS, KUPA_EXPORT_TYPE_FILE
     * \param  name		Export 객체의 고유 이름
     * \param  path		내보내기 파일의 생성 경로 (CSV 유형) 또는, MDSplus 서버의 주소 (MDS 유형)
     * \param  arg3		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg4		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg5		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg6		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg7		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg8		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \param  arg9		매개변수. Export 유형에 따라 사용 여부 및 그 의미가 상이함
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 내보내기 유형에 따라 해당 객체를 생성하고, 그 객체를 내보내기 인터페이스 벡터에 등록한다. \n
     */
	int addExport(char *type, char* name, char *path, 
			char* arg3, char* arg4, char* arg5, char* arg6, char* arg7, char* arg8, char* arg9);

	/**
     * \fn     int addPV(char* pvName, char* nodeName, int caType, char *description, char *unit)
     * \brief  데이터를 수집할 PV 객체 생성
     * \param  pvName			EPICS PV명
     * \param  nodeName			수집한 데이터가 저장될 대상 MDS 노드명
     * \param  caType			값 취득을 위한 EPICS DB 유형
     * \param  description		PV의 설명
     * \param  unit				PV 값의 단위
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 데이터를 수집할 PV 객체를 생성한다. \n
     */
	int addPV(char* pvName, char* nodeName, int caType, char *description, char *unit);

	/**
     * \fn     int addPostAction(char* cmd)
     * \brief  데이터 수집 및 내보내기 이후에 수행될 명령문 설정
     * \param  cmd			데이터 수집 및 내보내기 이후에 수행될 명령문
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 데이터 수집 및 내보내기 이후에 수행될 명령문의 문자열을 내부 변수에 설정한다. \n
     */
	int addPostAction(char* cmd);

	/**
     * \fn     int runManager()
     * \brief  데이터 수집 및 내보내기를 수행하는 쓰레드의 생성
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 데이터 수집 및 내보내기를 수행하는 쓰레드의 생성한다. \n
     */
	int runManager();

	/**
     * \fn     int run()
     * \brief  데이터 수집 및 내보내기를 수행하는 쓰레드의 처리 내용
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 본 함수는 데이터 수집 및 내보내기를 수행하는 쓰레드의 실제 처리 내용으로서
     * kupaMessage의 형태로 수신되는 서비스 요청을 수신 받아 그 서비스를 처리한다. \n
     * 처리되는 서비스로는, 데이터 archiving과 현재 값의 갱신이 있다.
     */
	int run();

	/**
     * \fn     int sendStartEvent()
     * \brief  처리 쓰레드에게 데이터 수집 및 내보내기 기능의 시작 요청 메시지를 전송함
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int sendStartEvent ();

	/**
     * \fn     int sendUpdateEvent()
     * \brief  처리 쓰레드에게 현재 값 갱신 요청 메시지를 전송함
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int sendUpdateEvent ();

	/**
     * \fn     int updatePV(char* pvName, char* data)
     * \brief  해당 PV의 현재 값 설정
     * \param  pvName		PV명
     * \param  data 		PV의 설정 값
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 받은 PV \a pvName 에 해당하는 객체 내부의 현재 값을 입력 값 \a data 로 설정한다.
     */
	int updatePV(char* pvName, char* data);

	/**
     * \fn     string & getValue (char* pvName, string &value)
     * \brief  해당 PV의 현재 값 얻기
     * \param  pvName		PV명
     * \param  value 		PV의 현재 값
     * \return PV의 현재 값을 갖는 string 객체
     *
     * 입력 받은 PV \a pvName 에 해당하는 객체 내부의 현재 값을 반환한다. \n
     * 내부적으로는, 등록된 각 PV 객체의 getValue 함수를 호출한다. \n
     */
	string & getValue (char* pvName, string &value);

	/**
     * \fn     int processTrigger(char* pvName, double value)
     * \brief  시퀀스 연계 PV들의 변경 이벤트 처리
     * \param  pvName		발생 이벤트의 PV명
     * \param  value 		해당 PV의 현재 값
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 운전 시퀀스에 연동되는 PV들 (SHOT_NO, SYS_OP_MODE, SYS_RUN)의 값 변경 시 호출되는 함수로서,
     * 해당 PV명에 따라 Shot 번호 설정, 저장 기능의 시작, 운전 모드 설정 등의 처리를 수행한다. \n
     */
	int processTrigger(char* pvName, double value);

	/**
     * \fn     int lock()
     * \brief  MDS API 접근 시 록 설정
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int lock ();

	/**
     * \fn     int unlock()
     * \brief  MDS API 접근 시 설정한 록 해제
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int unlock ();

	/**
     * \fn     epicsMutexId getLockId()
     * \brief  MDS 접근 록 ID 얻기
     * \return 성공(kuOK), 실패(kuNOK)
     */
	epicsMutexId getLockId ();

	/**
     * \fn     void setLockId (epicsMutexId newVal)
     * \param  newVal		MDS 접근 록 ID
     * \brief  MDS 접근 록 ID 설정
     */
	void setLockId (epicsMutexId newVal);

protected:

	/**
     * \fn     kupaManager()
     * \brief  생성자. 내부 변수 초기화 및 MDS 접근 록 객체 생성
     */
	kupaManager();

	/**
     * \fn     kupaManager(const kupaManager &obj)
     * \param  obj		복사할 객체
     * \brief  생성자. 입력 받은 객체로 자신의 내부 변수 설정
     */
	kupaManager(const kupaManager &obj);	// copy constructor

private:
	static kupaManager* m_kupaManagerInstance;			///< kupaManager 객체

	kupaCollectInterface* m_kupaCollectInterface;		///< 데이터 수집기 객체들의 맵
	kupaExportInterfaceVec m_kupaExportInterfaceVec;	///< 내보내기 객체들의 벡터
	string m_managerName;		///< kupaManager 객체의 이름
	string m_shotNumPvName;		///< Shot 번호 관련 PV 명
	string m_armPvName;			///< SYS_ARMING 관련 PV 명. 미 사용
	string m_runPvName;			///< SYS_RUN 관련 PV 명
	string m_opModePvName;		///< SYS_OP_MODE 관련 PV 명
	string m_postActionCmd;		///< 데이터 수집 및 내보내기 이후에 수행될 명령의 문자열
	int m_opMode;				///< Operation 모드. 1 : Remote Mode
	int m_shotNum;				///< Shot 번호
	int m_sampingRate;			///< 샘플 주기
	double m_startTime;			///< 데이터의 시작 시각
	double m_duration;			///< 데이터의 수집 시간
	double m_delaySec;			///< 데이터 수집의 지연 시간
	epicsEventId m_startEventId;		///< 데이터 수집 시작을 알리기 위한 이벤트 식별자. 메시지 큐로 대체됨
	epicsMessageQueueId m_msgQueueId;	///< kupaManager에게 서비스를 전달하기 위한 메시지 큐 ID
	kupaMessage m_svcMessage;			///< kupaManager에게 전달되는 서비스 메시지
	kuLock *m_mdsLock;					///< MDSplus 접근을 위한 록

	kupaPV *m_shotNumPV;		///< Shot 번호 취득을 위한 kupaPV 객체
	kupaPV *m_armPV;			///< SYS_ARMING 상태 취득을 위한 kupaPV 객체
	kupaPV *m_runPV;			///< SYS_RUN 상태  취득을 위한 kupaPV 객체
	kupaPV *m_opModePV;			///< SYS_OP_MODE 상태 취득을 위한 kupaPV 객체

	int performCollect();		///< 데이터 수집 수행. kupaCollectInterface의 collect 호출
	int performExport();		///< 데이터 내보내기 및 post action 수행. kupaCollectInterface의 update 호출
	int performReset();			///< PV들의 수집 데이터 삭제. kupaCollectInterface의 reset 호출
	int performUpdate();		///< PV들의 현재 값 갱신. kupaCollectInterface의 update 호출
	int initChannelAccess();	///< 등록된 모든 PV들의 EPICS 채널 설정. SHOT_NO, SYS_OP_MODE, SYS_RUN 포함

	void doStartDelay();		///< 설정 시간 만큼 지연 수행
	void doSamplingDelay();		///< 샘플 주기에 따른 지연 수행. 즉, 다음 샘플 주기까지 대기함

};
#endif // !defined(EA_FAC80277_F4F9_4b7b_A82D_9E59464FE380__INCLUDED_)
