// Filename : messageQueue.cpp
//
// **** REVISION HISTORY *****************************************************
// don't use implict create the message queue.
// so open flags were added.
//
// ***************************************************************************

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <errno.h>

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "messageQueue.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

const int	MSG_SEND_RETRY_CNT		= 3;
const int	MSG_SEND_RETRY_SLEEP	= 10;	// msec 

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Import Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Export Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Module Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function Definitions 
// ---------------------------------------------------------------------------

MessageQueue::MessageQueue (const key_t msgKey)
{
	m_msgId		= -1;
	m_msgIdList	= NULL;
	m_msgType	= -1;

	// uses user's message queue key
	m_msgKey			= msgKey;
	m_maxNumOfMsgQueue	= 1;
	m_sizeOfMsgQueueBuffer	= RT_MSG_BUFFER_MAX;
}

MessageQueue::~MessageQueue ()
{
	try {
		close ();
	}
	catch (...) { }
}

void MessageQueue::open (const int flags, const int msgType, const FlushMode_e flushMode) throw (MessageQueueException)
{
	int		runUserid = -1, runGroupId = -1;

	// check owner
	if (flags & RT_MSG_CREAT) {
		if (OK != RtObject::getValidUser (runUserid, runGroupId)) {
			exit (1);
		}
	}

	gDebug.println (Debug::SLOG, "[MessageQueue::open] msgKey(0x%x) msgType(%d) perm(0%o) flush(%d)", 
						m_msgKey, msgType, RT_MSG_PERMISSION, flushMode);

	if (0x00 == m_msgKey) {
		setErrNum (RT_EMQKEYINVALID);

		gDebug.perror (Debug::SCRI, "[MessageQueue::open] message queue key is invalid. please check table is loaded.");
		throw MessageQueueException ("[MessageQueue::open] message queue key is invalid.");
	}

	// initialize message queue list
	m_msgIdList = (int *)malloc (sizeof(int) * m_maxNumOfMsgQueue);

	if (NULL == m_msgIdList) {
		setErrNum (RT_EMQKEYALLOC);

		gDebug.perror (Debug::SCRI, "[MessageQueue::open] malloc failed");
		throw MessageQueueException ("[MessageQueue::open] malloc failed", strerror(errno));
	}

	for (int i = 0; i < m_maxNumOfMsgQueue; i++) {
		if ((m_msgIdList[i] = msgget (m_msgKey + i, RT_MSG_PERMISSION | flags)) < 0) {
			free (m_msgIdList);
			setErrNum (RT_EMQCREATE);

			gDebug.perror (Debug::SCRI, "[MessageQueue::open] message queue create failed : key(0x%x)", m_msgKey + i);
			throw MessageQueueException ("[MessageQueue::open] message queue create failed", strerror(errno));
		}
	}

	// change owner
	if (flags & RT_MSG_CREAT) {
		setValidUser (runUserid, runGroupId);
	}

	m_msgType	= msgType;
	m_msgId		= getDestMsgId (msgType);
	m_flags		= flags;
	
	// flush messages of this msgtype in messag queue
	if (RT_MSG_FLUSH == flushMode) {
		flush (m_msgType);
	}

	gDebug.println (Debug::SLOG, "[MessageQueue::open] m_msgId(%d) m_msgType(%d)", m_msgId, m_msgType);
}

// read from message queue
const int MessageQueue::read (void *readBuf, const unsigned int readBufferSize) throw (MessageQueueException)
{
	if ((0 == readBufferSize) || (NULL == readBuf)) {
		return (0);
	}

	int32	readLen, retLen;
   
	readLen = read (&m_rwMsgQueueBuffer, m_msgType, RT_MSG_WAIT);
   
	if (readBufferSize < (unsigned int)readLen) {
		retLen = readBufferSize;
	}
	else {
		retLen = readLen;
	}

	memcpy (readBuf, m_rwMsgQueueBuffer.text, retLen);

	gDebug.println (Debug::SLOG, "[MessageQueue::read] readLen(%d) retLen(%d)", readLen, retLen);

	return (retLen);
}

const int MessageQueue::read (MessageQueueBuffer_s *msgQueueBuffer, 
		const int readMsgType, const ReadMode_e readMode) throw (MessageQueueException)
{
	return (read (msgQueueBuffer, getDestMsgId (readMsgType), readMsgType, readMode));
}

// read from message queue
const int MessageQueue::read (MessageQueueBuffer_s *msgQueueBuffer, const int msgId,
		const int readMsgType, const ReadMode_e readMode) throw (MessageQueueException)
{
	gDebug.println (Debug::SLOG, "[MessageQueue::read] msgid(%d) msgtype(%d) mode(0x%x)", msgId, readMsgType, readMode);

	int32	readLen;

	while (((readLen = msgrcv (msgId, msgQueueBuffer, m_sizeOfMsgQueueBuffer, readMsgType, readMode)) < 0) &&
				(EINTR == errno)) {
		continue;	// ignore interrupt
	}

	if (0 > readLen) {
		if ( (RT_MSG_NOWAIT == readMode) && (ENOMSG == errno) ) {
			// no data
			return (0);
		}

		setErrNum (RT_EMQREAD);

		gDebug.perror (Debug::SLOG,  "[MessageQueue::read] msgrcv failed");
		throw MessageQueueException ("[MessageQueue::read] msgrcv failed", strerror(errno));
	}

	gDebug.println (Debug::SLOG, "[MessageQueue::read] readLen(%d)", readLen);

	return (readLen);
}

// write to message queue
void MessageQueue::write (const void *writeBuf, const unsigned int writeSize, const int destMsgType) throw (MessageQueueException)
{
	if ((0 == writeSize) || (NULL == writeBuf)) {
		return;
	}

	if (writeSize > m_sizeOfMsgQueueBuffer) {
		setErrNum (RT_EMQBIGSIZE);

		gDebug.println (Debug::SERR, "[MessageQueue::write] write size(%d) is too big", writeSize);
		throw MessageQueueException ("[MessageQueue::write] write size is too big");
	}

	m_rwMsgQueueBuffer.type = destMsgType;
	memcpy (m_rwMsgQueueBuffer.text, writeBuf, writeSize);

	write (&m_rwMsgQueueBuffer, writeSize);
}

void MessageQueue::write (const MessageQueueBuffer_s *msgQueueBuffer, const unsigned int writeSize) throw (MessageQueueException)
{
	int		msgId = getDestMsgId (msgQueueBuffer->type);

	gDebug.println (Debug::SLOG, "[MessageQueue::write] msgId(%d) msgtype(%d) size(%d)", msgId, msgQueueBuffer->type, writeSize);

#if 0
	if (msgsnd (msgId, msgQueueBuffer, writeSize, RT_MSG_NOWAIT) < 0) {
		setErrNum (RT_EMQWRITE);

		gDebug.perror (Debug::SERR, "[MessageQueue::write] msgsnd failed : msgid(%d) cid(%d) size(%d)", 
										msgId, msgQueueBuffer->type, writeSize);
		throw MessageQueueException ("[MessageQueue::write] msgsnd failed", strerror(errno));
	}
#else
	int		i;

	for (i = 0; i < MSG_SEND_RETRY_CNT; i++) {
		if (msgsnd (msgId, msgQueueBuffer, writeSize, RT_MSG_NOWAIT) < 0) {
			// when message queue is full, retry 3 times after sleep
			if (EAGAIN != errno) {
				break;
			}
		}
		else {
			// OK
			return;
		}

		// wait and retry
		if ( i < (MSG_SEND_RETRY_CNT - 1) ) {
			Timer::sleep(MSG_SEND_RETRY_SLEEP);
		}
	}

	setErrNum (RT_EMQWRITE);

	gDebug.perror (Debug::SERR, "[MessageQueue::write] msgsnd failed : msgid(%d) cid(%d) size(%d)", 
									msgId, msgQueueBuffer->type, writeSize);
	throw MessageQueueException ("[MessageQueue::write] msgsnd failed", strerror(errno));
#endif
}

// flush
void MessageQueue::flush (const int flushMsgType) throw (MessageQueueException)
{
	gDebug.println (Debug::SLOG, "[MessageQueue::flush] flush msgtype(%d)", flushMsgType);

	for (int i = 0; i < m_maxNumOfMsgQueue; i++) {
		while (1) {
			try {
				if (0 >= read (&m_rwMsgQueueBuffer, m_msgIdList[i], flushMsgType, RT_MSG_NOWAIT)) {
					gDebug.println (Debug::SLOG, "[MessageQueue::flush] msgid(%d) msgtype(%d) : flush ok", m_msgIdList[i], flushMsgType);
					break;
				}
			}
			catch (MessageQueueException &msgQeueuErr) {
				gDebug.println (Debug::SERR, "[MessageQueue::flush] msgid(%d) msgtype(%d) : failed", m_msgIdList[i], flushMsgType);
				break;
			}
		}
	}
}

void MessageQueue::close () throw (MessageQueueException)
{
	m_msgType = -1;

	if (NULL != m_msgIdList) {
		free (m_msgIdList);
		m_msgIdList = NULL;
	}
}

void MessageQueue::remove () throw (MessageQueueException)
{
	isOK ();
	
	for (int i = 0; i < m_maxNumOfMsgQueue; i++) {
		if (msgctl (m_msgIdList[i], IPC_RMID, 0) < 0) {
			setErrNum (RT_EMQREMOVE);

			gDebug.perror (Debug::SERR, "[MessageQueue::remove] message queue remove failed");
			throw MessageQueueException ("[MessageQueue::remove] message queue remove failed", strerror(errno));
		}
	}
}

void MessageQueue::isOK ()
{
	if (-1 == m_msgId || NULL == m_msgIdList || 0 > m_msgType) {
		setErrNum (RT_EMQINIT);

		gDebug.println (Debug::SCRI, "[MessageQueue::isOK] message queue was not initialized");
		throw MessageQueueException ("[MessageQueue::isOK] message queue was not initialized");
	}
}

void MessageQueue::printUsedRate () throw (MessageQueueException)
{
	int				used;
	struct msqid_ds	msgrec;

	for (int i = 0; i < m_maxNumOfMsgQueue; i++) {
		if (msgctl (m_msgIdList[i], IPC_STAT, &msgrec) < 0) {
			printf ("   M/Q%2d : key(0x%8x) id(%5d) : msgctl failed ...\n", i + 1, m_msgKey + i, m_msgIdList[i]);
			continue;
		}

#ifdef __alpha
		used = (msgrec.msg_cbytes   * 100) / msgrec.msg_qbytes;
#else
		used = (msgrec.__msg_cbytes * 100) / msgrec.msg_qbytes;
#endif

		printf ("   M/Q%2d : key(0x%8x) id(%5d) max(%7ld) cur(%7ld) used(%3d%%) num(%3ld) spid(%6d) rpid(%6d)\n", 
				i + 1, m_msgKey + i, m_msgIdList[i], msgrec.msg_qbytes, msgrec.msg_cbytes, used,
				msgrec.msg_qnum, msgrec.msg_lspid, msgrec.msg_lrpid);
	}
}

const int MessageQueue::getUsedRate (int msgId) throw (MessageQueueException)
{
	isOK ();
	
	struct msqid_ds		msgrec;

	if (msgctl (msgId, IPC_STAT, &msgrec) < 0) {
		return (-1);
	}

#ifdef __alpha
	int	used = (msgrec.msg_cbytes   * 100) / msgrec.msg_qbytes;
#else
	int	used = (msgrec.__msg_cbytes * 100) / msgrec.msg_qbytes;
#endif

	return (used);
}

const int MessageQueue::getDestMsgId (const int msgType)
{
	return (m_msgIdList[msgType % m_maxNumOfMsgQueue]);
}

const int MessageQueue::setValidUser (const int &uid, const int &gid)
{
	if ( (0 != getuid()) || (0 != geteuid()) ) {
		return (NOK);
	}

	struct msqid_ds	buf;

	for (int i = 0; i < m_maxNumOfMsgQueue; i++) {
		// get data structure of message queue
		if (0 > msgctl (m_msgIdList[i], IPC_STAT, &buf)) {
			perror ("msgctl (IPC_STAT) was failed");
			return (NOK);
		}

		// change user and group id
		buf.msg_perm.uid = uid;
		buf.msg_perm.gid = gid;

		if (0 > msgctl (m_msgIdList[i], IPC_SET, &buf)) {
			perror ("msgctl (IPC_SET) was failed");
			return (NOK);
		}
	}

	return (OK);
}

// End of File
