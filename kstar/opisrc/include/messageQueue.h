// Filename : messageQueue.h
// don't use implict create the message queue.
// so open flags were added.
//
// Revision 1 
// Author: leesi
// Initial revision
//
// ***************************************************************************

#ifndef _MESSAGEQUEUE_H
#define _MESSAGEQUEUE_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

#include <sys/ipc.h>
#include <sys/msg.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

class MessageQueue
{

public :

	static const unsigned int RT_DYNAMIC_MSGTYPE	= 0;
	static const unsigned int RT_ALL_MSGTYPE		= 0;
	static const int RT_MSG_PERMISSION		= 0644;
	static const int RT_MSG_BUFFER_MAX = 8192;

	enum OpenMode_e {
		RT_MSG_OPEN		= 0,
		RT_MSG_CREAT	= IPC_CREAT,
		RT_MSG_EXCL		= IPC_EXCL
	};

	enum FlushMode_e {
		RT_MSG_FLUSH,
		RT_MSG_NOFLUSH
	};

	enum ReadMode_e {
		RT_MSG_WAIT = 0,
		RT_MSG_NOWAIT = IPC_NOWAIT
	};

	typedef struct {
		long	type;
		char	text[RT_MSG_BUFFER_MAX];
	} MessageQueueBuffer_s;

	// Constructor
	MessageQueue (const key_t msgKey = 0);

	// Destructor
	~MessageQueue ();

	// create or get message queue
	void open (const int flags, const int msgType = RT_DYNAMIC_MSGTYPE, 
			const FlushMode_e flushMode = RT_MSG_FLUSH);

	// get id of message queue
	inline const int getMsgId () { return (m_msgId); };

	// get id of message queue
	inline const int getMsgType () { return (m_msgType); };

	// read message from message queue
	const int read (void *readBuf, const unsigned int readBufferSize);

	const int read (MessageQueueBuffer_s *msgQueueBuffer,
			const int readMsgType, const ReadMode_e readMode);

	// write message to message queue
	void write (const void *writeBuf, const unsigned int writeSize, const int destMsgType);

	void write (const MessageQueueBuffer_s *msgQueueBuffer, const unsigned int writeSize);

	// flush messages
	void flush (const int msgType);

	// close
	void close ();

	// remove
	void remove ();

	// get used rate (%d)
	const int getUsedRate (int msgId);
	void printUsedRate ();

	inline MessageQueueBuffer_s *getMsgQueueBufAddr() { return (&m_rwMsgQueueBuffer); };

private :

	key_t	m_msgKey;	// key of message queue
	int	m_msgType;	// msgtype of message queue
	unsigned int	m_sizeOfMsgQueueBuffer;

	int		m_msgId;				// id of message queue
	int		*m_msgIdList;			// id list of message queue
	unsigned int	m_maxNumOfMsgQueue;		// maximum number of message queue
	int		m_flags;				// open flags

	MessageQueueBuffer_s	m_rwMsgQueueBuffer;

	void isOK ();
	const int getDestMsgId (const int msgType);	// get destnation message queue id

	const int read (MessageQueueBuffer_s *msgQueueBuffer, const int msgId,
			const int readMsgType, const ReadMode_e readMode);

	const int setValidUser (const int &uid, const int &gid);
};

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// _MESSAGEQUEUE_H

