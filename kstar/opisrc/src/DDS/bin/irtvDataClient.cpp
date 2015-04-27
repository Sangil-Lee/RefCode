#include "irtvCommon.h"
#include "irtvGrabFrameData.h"
#include "irtvDataService.h"
#include "irtvDataClient.h"

static IrtvSvcUdp			gIrtvSvcUdp;
static IrtvSvcDataAssembler	gDataAssembler;

// display on/off
extern bool				gbDisplayOnOff;

void irtvSendHeartbeat ()
{
	static int	nSvcSeq	= 0;

	IrtvSvcUdpData	udpData;

	epicsUInt32		nEpochTimes	= kEpochTimes ();

	// 헤더 작성
	udpData.m_header.set (IRTV_SVC_HEARTBEAT, ++nSvcSeq, 0, 1, 1, 0, nEpochTimes);

	// 헤더만 전송
	gIrtvSvcUdp.write ((char *)&udpData.m_header, sizeof(IrtvSvcHeader), IRTV_SVR_IP_ADDR, IRTV_SVR_PORT_NO);

	kLog (K_INFO, "[irtvSendHeartbeat] send heartbeat message \n");
}

void processRecvData (IrtvSvcDataAssembler &gDataAssembler, IrtvSvcUdpData &udpData)
{
	// 수신한 이미지 Frame Data를 화면 표시용 버퍼에 기록
	gDataAssembler.addData (udpData);
}

void irtvDataRecvThr (void *arg)
{
	IrtvSvcUdpData			udpData;

	kLog (K_MON, "[irtvDataRecvThr::run] started ...\n");

	int		nTimeout	= 0;	// 5000
	int		ret;
	char	szIpAddr[40];

	while (true) {
		if ( 0 > (ret = gIrtvSvcUdp.read ((char *)&udpData, sizeof(IrtvSvcUdpData), nTimeout)) ) {
			continue;
		}

		strcpy (szIpAddr, gIrtvSvcUdp.getAddrToStr ());

		kLog (K_DEL, "[irtvDataRecvThr::run] received data : ip(%s) svc(%d) msgNum(%d) ...\n", 
			szIpAddr, udpData.getSvcType (), udpData.m_header.m_nMsgSeq);

		// 등록 및 수신 시각 갱신
		if (IRTV_SVC_SEND_DATA == udpData.getSvcType ()) {
			processRecvData (gDataAssembler, udpData);
		}
	}
}

void irtvDataSendThr (void *arg)
{
	kLog (K_MON, "[irtvDataSendThr::run] started ...\n");

	while (true) {
		if (true == gbDisplayOnOff) {
			// Heaertbeat 전송
			irtvSendHeartbeat ();

			kLog (K_INFO, "[irtvDataSendThr::run] send heartbeat ...\n");
		}

		epicsThreadSleep (5);
	}
}

int createDataRecvThr (void *arg)
{
	kLog (K_MON, "[createDataRecvThr] epicsThreadCreate() ... \n");
	
	// Target 설정
	gDataAssembler.setTarget ((GrabFrameData *)arg);

	// 소켓 열기
	if (INVALID_SOCKET == gIrtvSvcUdp.open (IRTV_CLNT_PORT_NO)) {
		return 0;
	}

	kLog (K_MON, "[irtvDataRecvThr::run] open ok ..\n");

	epicsThreadCreate ("IRTV.DataRecvThr", 
			epicsThreadPriorityLow,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) irtvDataRecvThr,
			(void*)arg);

	epicsThreadCreate ("IRTV.DataSendThr", 
			epicsThreadPriorityLow,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) irtvDataSendThr,
			(void*)arg);

	return 1;
}
