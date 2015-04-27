#include "IrtvCommon.h"
#include "irtvGrabFrame.h"
#include "irtvDataService.h"
#include "irtvDataServer.h"

static IrtvSvcUdp			gIrtvSvcUdp;
static IrtvIpAddrMap		gIrtvIpAddrMap;
static IrtvSync				gDataServiceSync;

void threadFunc_ListenThr (void *param)
{
	IrtvSvcData		svcData;

	kLog (K_MON, "[IrtvSvcListener::run] started ...\n");

	if (INVALID_SOCKET == gIrtvSvcUdp.open (IRTV_SVR_PORT_NO)) {
		return;
	}

	kLog (K_MON, "[IrtvSvcListener::run] open ok ..\n");

	// 소켓 초기화 이후 알림
	gDataServiceSync.signal ();

	int		ret;
	char	szIpAddr[40];
    IrtvIpAddrMap::iterator pos;
	epicsUInt32	nCurrEpochTimes;

#if 0
	//TODO
	gIrtvIpAddrMap["172.17.101.187"]	= 1;
	gIrtvIpAddrMap["172.17.101.188"]	= 2;
	gIrtvIpAddrMap["172.17.101.187"]	= 3;
#endif

	while (true) {
		if ( 0 > (ret = gIrtvSvcUdp.read ((char *)&svcData, sizeof(IrtvSvcData), 1000)) ) {
			continue;
		}

		nCurrEpochTimes	= kEpochTimes ();

		// timeout
		if (0 == ret) {
			kLog (K_DEBUG, "[IrtvSvcListener::run] Time out ...\n");
		}
		else {
			strcpy (szIpAddr, gIrtvSvcUdp.getAddrToStr ());

			// 등록 및 수신 시각 갱신
			if (IRTV_SVC_HEARTBEAT == svcData.getSvcType ()) {
				gIrtvIpAddrMap[szIpAddr]	= nCurrEpochTimes;

				kLog (K_DEBUG, "[IrtvSvcListener::run] H/B : ip(%s), time(%d)\n", szIpAddr, gIrtvIpAddrMap[szIpAddr]);
			}
		}

#if 0
		// client의 Heartbeat 만료 여부 검사
		for (pos = gIrtvIpAddrMap.begin(); pos != gIrtvIpAddrMap.end(); ++pos) {
			kLog (K_MON, "[IrtvSvcListener::run] ip(%s), time(%d) ...\n", szIpAddr, gIrtvIpAddrMap[szIpAddr]);

			if (nCurrEpochTimes - 10 > pos->second) {
				kLog (K_MON, "[IrtvSvcListener::run] ip(%s), time(%d) is removed ...\n", szIpAddr, gIrtvIpAddrMap[szIpAddr]);
				gIrtvIpAddrMap.erase (pos);
			}
		}
#endif
	}
}

void testFrameData (GrabFrameData &grabFrameData)
{
	FILE * fp = NULL;
	if (NULL == (fp = fopen (IRTV_LIVE_GRAB_RAW_PATH, "rb"))) {
		printf ("error \n");
		return;
	}

	fseek (fp, 0L, SEEK_END);
	int size = ftell (fp);
	fseek (fp, 0L, SEEK_SET);

	epicsUInt8 *ptr = (epicsUInt8 *)malloc (size);
	if (NULL == ptr) {
		printf ("error \n");
		fclose (fp);
		return;
	}

	fread (ptr, size, 1, fp);
	fclose (fp);

	grabFrameData.setFrameData (640, 512, ptr);

	free (ptr);
	ptr	= NULL;
}

void sendFrameData (GrabFrameData &grabFrameData)
{
	static int	nSvcSeq	= 0;

	IrtvSvcUdpData		udpData;

	int		nSvcSize	= grabFrameData.getTotalSize ();
	const char	*ptr	= (char *)&grabFrameData;

	int		nOffset;
	int		nMsgNum;
	int		nMsgLen;
	int		nLeftSize;
	bool	bLastMsg;
    IrtvIpAddrMap::iterator pos;

	epicsUInt32		nCurrEpochTimes	= kEpochTimes ();

#if 1
	// client의 Heartbeat 만료 여부 검사
	for (pos = gIrtvIpAddrMap.begin(); pos != gIrtvIpAddrMap.end(); ++pos) {
		if (nCurrEpochTimes - 10 > pos->second) {
			kLog (K_MON, "[IrtvSvcListener::run] ip(%s), time(%d) is removed ...\n", pos->first.c_str(), pos->second);
			gIrtvIpAddrMap.erase (pos);
			break;
		}
	}
#endif

	//TODO
	//nSvcSize	= 3000;

	// 서비스 일련번호 증가
	nSvcSeq++;

	nMsgNum		= 0;
	nOffset		= 0;
	nLeftSize	= nSvcSize;

	while (nLeftSize > 0) {
		if (IRTV_MTU < nLeftSize) {
			nMsgLen		= IRTV_MTU;
			bLastMsg	= false;
		}
		else {
			nMsgLen		= nLeftSize;
			bLastMsg	= true;
		}

		// 헤더 작성
		udpData.m_header.set (IRTV_SVC_SEND_DATA, nSvcSeq, nSvcSize, bLastMsg, ++nMsgNum, nMsgLen, nCurrEpochTimes);

		// 데이터 복사
		memcpy (udpData.m_data, ptr + nOffset, nMsgLen);

		// 데이터 전송
		for (pos = gIrtvIpAddrMap.begin(); pos != gIrtvIpAddrMap.end(); ++pos) {
			gIrtvSvcUdp.write ((char *)&udpData, sizeof(IrtvSvcHeader) + nMsgLen, pos->first.c_str(), IRTV_CLNT_PORT_NO);
		}

		nOffset		+= nMsgLen;
		nLeftSize	-= nMsgLen;

		if (0 == (nMsgNum % 20)) {
			epicsThreadSleep (0.002);
		}

		kLog (K_DEL, "[sendFrameData] svc(%d) size(%d) msgNum(%d) msgLen(%d) offset(%d) left(%d) \n",
			nSvcSeq, nSvcSize, nMsgNum, nMsgLen, nOffset, nLeftSize);
	}
}

void threadFunc_DataSendThr (void *param)
{
	gDataServiceSync.wait ();

	GrabFrameData		grabFrameData;
    IrtvIpAddrMap::iterator pos;

	while (true) {
		// 일정 시간 지연
		epicsThreadSleep (IRTV_DATA_SEND_PERIOD / 1000.0);

		kLog (K_INFO, "[threadFunc_DataSendThr] send frame data to clients ...\n");

#if 1
		// 공유메모리로부터 데이터 가져오기
		gGrabFrameShm.read (grabFrameData);
#else
		testFrameData (grabFrameData);
#endif

		// 목록 검색
		for (pos = gIrtvIpAddrMap.begin(); pos != gIrtvIpAddrMap.end(); ++pos) {
			kLog (K_DEBUG, "[threadFunc_DataSendThr] ip(%s) time(%d) \n", pos->first.c_str(), pos->second);
		}

		// 전송
		if (0 < gIrtvIpAddrMap.size()) {
			sendFrameData (grabFrameData);
		}
	}
}

extern "C" int createListenThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.ListenThr", 
			epicsThreadPriorityLow,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_ListenThr,
			(void*)pIRTV);

	return 1;
}

extern "C" int createDataSendThr (void *pIRTV)
{
	epicsThreadCreate ("IRTV.DataSendThr", 
			epicsThreadPriorityLow,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) threadFunc_DataSendThr,
			(void*)pIRTV);

	return 1;
}
