#ifndef _IRTV_SYNC_H
#define _IRTV_SYNC_H

// Frame Data 동기화 수단
class IrtvSync
{
public :
	IrtvSync () {
		m_lockId	= epicsMutexMustCreate ();
	};

	~IrtvSync () {};

	const int lock () { return (epicsMutexLock (m_lockId)); }
	void unlock () { epicsMutexUnlock (m_lockId); }

	void wait () { m_event.wait(); }
	void signal () { m_event.signal(); }

private :

	epicsMutexId		m_lockId;		// mutex lock id
	epicsEvent			m_event;		// event
};

// Live Data
extern IrtvSync		gLiveFrameDataSync;

// Command
extern IrtvSync		gIrtvCommandSync;

#endif // _IRTV_SYNC_H
