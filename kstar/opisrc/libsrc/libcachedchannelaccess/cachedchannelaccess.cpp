#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>


#include "cachedchannelaccess.h"

// uses System V shared memory
CachedChannelAccess::CachedChannelAccess ()
{
	init ();
}

CachedChannelAccess::CachedChannelAccess (const char *shmKey, const int shmSize, const int flags, const int mode)
{
	init ();

	try {
		open (shmKey, shmSize, flags, mode);
	}
	catch (...) { }
}

CachedChannelAccess::CachedChannelAccess (const key_t shmKey, const int shmSize, const int flags, const int mode)
{
	init ();

	try {
		open (shmKey, shmSize, flags, mode);
	}
	catch (...) { }
}

CachedChannelAccess::~CachedChannelAccess ()
{
	try {
		close ();
	}
	catch (...) { }

#if 0
	if (NULL != m_lock) {
		delete (m_lock);
	}
#endif
}

void CachedChannelAccess::init()
{
	m_shmKey	= -1;
	m_semKey	= -1;
	m_shmId		= -1;
	m_size		= 0;
	m_shmAddr	= NULL;

	m_mode		= RT_SHM_PERMISSION;
	m_flags		= 0;

	// lock
	m_lockFd			= -1;
	m_lockFileName[0]	= 0x00;

	//m_lock		= NULL;
}

char *CachedChannelAccess::open (const char *shmKey, const int shmSize, const int flags, const int mode)
{
	if ( (NULL == shmKey) || (0 != strncasecmp ("0x", shmKey, 2)) ) 
	{
		printf ("[CachedChannelAccess::remove] shmKey is invalid\n");
	}

	key_t	nShmKey;

	sscanf (shmKey, "%x", &nShmKey);

	return (open (nShmKey, shmSize, flags, mode));
}

char *CachedChannelAccess::open (const key_t shmKey, const int shmSize, const int flags, const int mode)
{
	int		runUserid = -1, runGroupId = -1;

	m_shmKey	= shmKey;
	m_size		= shmSize;
	m_mode		= mode;

	m_flags		= 0;

	//if (flags & RT_SHM_RDONLY)	m_flags |= O_RDONLY;
	if (flags & RT_SHM_RDWR)	m_flags |= O_RDWR;
	if (flags & RT_SHM_CREAT)	m_flags |= O_CREAT;
	if (flags & RT_SHM_EXCL)	m_flags |= O_EXCL;

	printf ("[CachedChannelAccess::open] m_shmKey(0x%x) m_size(%d) flags(0x%x) mode(0%o)\n", m_shmKey, m_size, m_flags, mode);
	if ((m_shmId = shmget (shmKey, 0, 0)) < 0) {
		if ( !(flags & RT_SHM_CREAT) ) {
		}

		if ((m_shmId = shmget (shmKey, shmSize, mode | IPC_CREAT)) < 0) {
			perror ("[CachedChannelAccess::open] shared memory create failed");
		}
	}

	// attach shared memory 
	if ((m_shmAddr = (char *) shmat (m_shmId, 0, 0)) <= (char *)0) {
		perror ("[CachedChannelAccess::open] shmat failed");
	}

	// change owner
	if (flags & RT_SHM_CREAT) {
		setValidUser (m_shmId, runUserid, runGroupId);
	}

#ifdef SYS_DEBUG
	printf ("[CachedChannelAccess::open] m_shmId = %d, m_shmAddr = %p\n", m_shmId, m_shmAddr);
#endif

	// set lock file name
	sprintf (m_lockFileName, "/tmp/.rt_shm_lock_%x", m_shmKey);

	return (m_shmAddr);
}

// read from shared memory
void CachedChannelAccess::read (const void *where, void *readBuf, const unsigned int readSize)
{
	if (NULL != where) {
		copy (where, readBuf, readSize);
	}
	else {
		copy (m_shmAddr, readBuf, readSize);
	}
}

void CachedChannelAccess::read (const int offset, void *readBuf, const unsigned int readSize)
{
	copy (m_shmAddr + offset, readBuf, readSize);
}

void CachedChannelAccess::read (void *readBuf, const unsigned int readSize)
{
	copy (m_shmAddr, readBuf, readSize);
}

// write to shared memory
void CachedChannelAccess::write (void *where, const void *writeBuf, const unsigned int writeSize)
{
	if ( !(RT_SHM_RDWR & m_flags) ) {

#ifdef SYS_DEBUG
		printf ("[CachedChannelAccess::write] readonly : cannot write to shared memory\n");
#endif
	}

	if (NULL != where) {
		copy (writeBuf, where, writeSize);
	}
	else {
		copy (writeBuf, m_shmAddr, writeSize);
	}
}

void CachedChannelAccess::write (int offset, const void *writeBuf, const unsigned int writeSize) 
{
	if ( !(RT_SHM_RDWR & m_flags) ) {

#ifdef SYS_DEBUG
		printf ("[CachedChannelAccess::write] readonly : cannot write to shared memory\n");
#endif
	}

	copy (writeBuf, m_shmAddr + offset, writeSize);
}

void CachedChannelAccess::write (const void *writeBuf, const unsigned int writeSize)
{
	if ( !(RT_SHM_RDWR & m_flags) ) {

#ifdef SYS_DEBUG
		printf ("[CachedChannelAccess::write] readonly : cannot write to shared memory\n");
#endif
	}

	copy (writeBuf, m_shmAddr, writeSize);
}

// close
void CachedChannelAccess::close ()
{
	isOK ();

	if (shmdt (m_shmAddr) < 0) {
		printf("[CachedChannelAccess::close] shared memory detach failed(%s)", strerror(errno));
	}

	m_shmAddr = NULL;
}

// remove
void CachedChannelAccess::remove ()
{
	isOK ();
	
	close ();

	remove (m_shmId);
}

void CachedChannelAccess::remove (const char *shmKey)
{
	if (NULL == shmKey) {
		printf ("[CachedChannelAccess::remove] shmKey is invalid\n");
	}

	key_t	nShmKey;

	sscanf (shmKey, "%x", &nShmKey);

	removeByKey (nShmKey);
}

void CachedChannelAccess::removeByKey (const key_t shmKey)
{
	int		shmId;

	if ((shmId = shmget (shmKey, 0, 0)) < 0) {
		printf ("[CachedChannelAccess::remove] shmKey is invalid\n");
	}

	remove (shmId);
}

void CachedChannelAccess::remove (const int shmId)
{
	if (0 > shmctl (shmId, IPC_RMID, (struct shmid_ds *)0)) {
		printf ("[CachedChannelAccess::remove] shared memory remove failed %s\n", strerror(errno));
	}
}

#if 0
void CachedChannelAccess::lock (const boolean fBlocking) 
{
	isOK ();
	
	if (NULL == m_lock) {
		char shmKey[40];
		sprintf (shmKey, "0x%x", m_shmKey);

		if (NULL == (m_lock = new RtLock (RtLock::RT_LOCK_INIT_VALUE, shmKey))) {
			perror ("[CachedChannelAccess::lock] cannot create RtLock");
		}
	}

	if (NULL != m_lock) {
		m_lock->lock ();
	}
}
// unlock
void CachedChannelAccess::unlock ()
{
	isOK ();
	if (NULL != m_lock) {
		m_lock->unlock ();
	}
}

#endif

const int CachedChannelAccess::openLockFile ()
{
	if (0 > m_lockFd) {
		if (0 > (m_lockFd = ::open (m_lockFileName, O_CREAT | O_RDWR, 0666))) {
			printf ("[CachedChannelAccess::openLockFile] key(0x%x) : lock file (%s) open failed : %s", m_shmKey, m_lockFileName, strerror(errno));
			return (-1);
		}
	}

	return (1);
}

const int CachedChannelAccess::closeLockFile ()
{
	if (0 <= m_lockFd) {
		if (0 > ::close (m_lockFd)) {
			printf ("[CachedChannelAccess::closeLockFile] key(0x%x) : lock file (%s) open failed : %s", m_shmKey, m_lockFileName, strerror(errno));
			return (-1);
		}

		// initialize
		m_lockFd	= -1;
	}

	return (1);
}

void CachedChannelAccess::copy (const void *srcAddr, void *destAddr, const unsigned int copyLen)
{
	isOK ();

	memcpy (destAddr, srcAddr, copyLen);
}

void CachedChannelAccess::isOK ()
{
	if (NULL == m_shmAddr) 
	{
#ifdef SYS_DEBUG
		printf ("[CachedChannelAccess::isOK] shared memory is not initialized\n");
#endif
	}
}

const int CachedChannelAccess::setValidUser (const key_t &shmId, const int &uid, const int &gid)
{
	if ( (0 != getuid()) && (0 != geteuid()) ) {
		return (1);
	}

	struct shmid_ds	buf;

	if (0 > shmctl (shmId, IPC_STAT, &buf)) {
		perror ("[CachedChannelAccess::setValidUser] shmctl failed : cannot get stat");
		return (-1);
	}

	buf.shm_perm.uid	= uid;
	buf.shm_perm.gid	= gid;
	buf.shm_perm.cuid	= uid;
	buf.shm_perm.cgid	= gid;

	if (0 > shmctl (shmId, IPC_SET, &buf)) {
		perror ("[CachedChannelAccess::setValidUser] shmctl failed : cannot set stat");
		return (-1);
	}

	return (1);
}
