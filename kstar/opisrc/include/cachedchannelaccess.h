#ifndef _CACHEDCHANNELACCESS_H
#define _CACHEDCHANNELACCESS_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

typedef struct CachedData
{
	unsigned int chindex;
	int         dbrequest;
	short		prec;
	short		severity;
	short		connstatus;
	short		padding;
	double		pvvalue;
	char		strvalue[32];
	char		pvname[32];
};

class CachedChannelAccess 
{
public :

	static const int RT_SHM_PAGE_SIZE	= 4096;
	static const int RT_SHM_PERMISSION	= 0644;

	enum OpenMode_e {
		RT_SHM_RDONLY	= O_RDONLY,
		RT_SHM_RDWR		= O_RDWR,
		RT_SHM_CREAT	= O_CREAT,
		RT_SHM_EXCL		= O_EXCL
	};

	// Constructor
	CachedChannelAccess ();

	CachedChannelAccess (const char *shmKey, const int shmSize, const int flags, const int mode = RT_SHM_PERMISSION);
	CachedChannelAccess (const key_t shmKey, const int shmSize, const int flags, const int mode = RT_SHM_PERMISSION);

	// Destructor
	~CachedChannelAccess ();

	// create or get shared memory
	char *open (const char *shmKey, const int shmSize, const int flags, const int mode = RT_SHM_PERMISSION);
	char *open (const key_t shmKey, const int shmSize, const int flags, const int mode = RT_SHM_PERMISSION);

	// get address of shared memory
	inline char *getShmAddr () { return (m_shmAddr); };

	// read from shared memory
	void read (const void *where, void *readBuf, const unsigned int readSize);
	void read (const int offset, void *readBuf, const unsigned int readSize);
	void read (void *readBuf, const unsigned int readSize);

	// write to shared memory
	void write (void *where, const void *writeBuf, const unsigned int writeSize);
	void write (int offset, const void *writeBuf, const unsigned int writeSize); 
	void write (const void *writeBuf, const unsigned int writeSize); 

	// close
	void close (); 

	// remove
	void remove ();

	static void remove (const char *shmKey); 
	static void removeByKey (const key_t shmKey);
	static void remove (const int shmId);

#if 0
	// lock
	void lock (const boolean fBlocking = TRUE) 
	// unlock
	void unlock () 
#endif

	// return the id of opened shared memory that is same with file descriptor
	int getShmId () { return (m_shmId); };

private :

	key_t	m_shmKey;		// key of shared memory
	key_t	m_semKey;		// key of semaphore for lock/unlock

	int		m_flags;		// open flags
	int		m_mode;			// permission of shared memory : RDONLY or RDWR
	int		m_shmId;		// id of shared memory
	int		m_size;			// size of shared memory
	char	*m_shmAddr;		// address of shared memory

	int		m_lockFd;		// lock file descriptor
	char	m_lockFileName[64];
	//RtLock	*m_lock;

	void init ();

	void isOK ();

	void copy (const void *srcAddr, void *destAddr, const unsigned int copyLen);

	const int setValidUser (const int &fd, const int &uid, const int &gid);
	const int openLockFile ();
	const int closeLockFile ();
};

#endif

