#ifndef _IRTV_GRAB_FRAME_SHM_H
#define _IRTV_GRAB_FRAME_SHM_H

#if !defined(WIN32)
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>
#include <winsock2.h>

#include "irtvGrabFrameData.h"

#define	SYS_DEBUG

typedef long long	key_t;

class IrtvGrabFrameShmException
{

public :

	static const int SHM_EXCEPTION_MAX_LEN = 1024;

	char m_exceptionReason[SHM_EXCEPTION_MAX_LEN];

	IrtvGrabFrameShmException () {
		strcpy (m_exceptionReason, "N/A");
	}

	IrtvGrabFrameShmException (char const *fmt, ...) {
		va_list		argv;

		va_start (argv, fmt);
		vsprintf (m_exceptionReason, fmt, argv);
		va_end (argv);
	}

	IrtvGrabFrameShmException (const char *exceptionReason, const char *comment) {
		setReason (exceptionReason, comment);
	}

	const char *getReason () {
		return (m_exceptionReason);
	}

private :

	void setReason (const char *exceptionReason, const char *comment) {

		int maxInputSize = SHM_EXCEPTION_MAX_LEN - 1;
		int	copyOffset = 0;

		if (NULL != exceptionReason) {
			int stringLen = (int)strlen(exceptionReason);

			if (stringLen > maxInputSize) {
				memcpy ((void *)m_exceptionReason, (const void *)exceptionReason, maxInputSize);
				m_exceptionReason[maxInputSize] = 0x00;

				copyOffset += maxInputSize;
			}
			else {
				strcpy (m_exceptionReason, exceptionReason);	

				copyOffset += stringLen;
			}
		}

		if (NULL != comment) {
			int stringLen = (int)strlen(comment);
			int restLen = maxInputSize - copyOffset - 1;

			if (stringLen > restLen) {
				strcat (m_exceptionReason, "\n");	

				memcpy ((void *)(m_exceptionReason + copyOffset + 1), (const void *)comment, restLen);
				m_exceptionReason[maxInputSize] = 0x00;
			}
			else {
				strcat (m_exceptionReason, "\n");	
				strcat (m_exceptionReason, comment);	
			}
		}
	}
};

class IrtvGrabFrameShm
{

public :

	static const key_t IRTV_SHM_KEY	= 0x7000001;

	// Constructor
	IrtvGrabFrameShm ();

	// Destructor
	~IrtvGrabFrameShm ();

	// create or get shared memory
	char *create () throw (IrtvGrabFrameShmException);
	char *open () throw (IrtvGrabFrameShmException);

	// get address of shared memory
	inline char *getShmAddr () { return (m_shmAddr); };

	// read from shared memory
	void read (GrabFrameData &data) throw (IrtvGrabFrameShmException);

	// write to shared memory
	void write (GrabFrameData &data) throw (IrtvGrabFrameShmException);

	// close
	void close () throw (IrtvGrabFrameShmException);

	// remove
	void remove () throw (IrtvGrabFrameShmException);

	// lock
	void lock () throw (IrtvGrabFrameShmException);

	// unlock
	void unlock () throw (IrtvGrabFrameShmException);

	// return size of shared memory
	const int getShmSize () { return (m_shmSize); }

private :

#if defined(WIN32)
	HANDLE	m_shmObj;
#endif

	key_t	m_shmKey;		// key of shared memory
	int		m_flags;		// open flags
	int		m_mode;			// permission of shared memory : RDONLY or RDWR
	int		m_shmId;		// id of shared memory
	int		m_shmSize;		// size of shared memory
	char	*m_shmAddr;		// address of shared memory

	void init ();

	void isOK ();

	void copy (const void *srcAddr, void *destAddr, const unsigned int copyLen) throw (IrtvGrabFrameShmException);
	char *open (const int flags) throw (IrtvGrabFrameShmException);

};

inline IrtvGrabFrameShm::IrtvGrabFrameShm ()
{
	init ();
}

inline IrtvGrabFrameShm::~IrtvGrabFrameShm ()
{
	try {
		close ();
	}
	catch (IrtvGrabFrameShmException &shmErr) {
		printf ("IrtvGrabFrameShm destructor : %s\n", shmErr.getReason());
	}
}

inline void IrtvGrabFrameShm::init()
{
#if defined(WIN32)
	m_shmObj	= NULL;
#endif

	m_shmKey	= IRTV_SHM_KEY;
	m_shmId		= -1;
	m_shmSize	= sizeof(GrabFrameData);
	m_shmAddr	= NULL;
	m_mode		= 0644;
	m_flags		= 0;
}

inline char *IrtvGrabFrameShm::create () throw (IrtvGrabFrameShmException)
{
	return (open (O_RDWR | O_CREAT));
}

inline char *IrtvGrabFrameShm::open () throw (IrtvGrabFrameShmException)
{
	return (open (O_RDWR));
}

inline char *IrtvGrabFrameShm::open (const int flags) throw (IrtvGrabFrameShmException)
{
	m_flags		= flags;

#ifdef SYS_DEBUG
	printf ("[IrtvGrabFrameShm::open] m_shmKey(0x%x) m_shmSize(%d) flags(0x%x)\n", (int)m_shmKey, m_shmSize, m_flags);
#endif

#if !defined(WIN32)
	if ((m_shmId = shmget (m_shmKey, 0, 0)) < 0) {
		if ( !(flags & O_CREAT) ) {
#ifdef DEBUG
			perror ("[IrtvGrabFrameShm::open] shared memory get failed");
#endif
			throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory get failed", strerror(errno));
		}

		if ((m_shmId = shmget (m_shmKey, m_shmSize, m_mode | IPC_CREAT)) < 0) {
#ifdef DEBUG
			perror ("[IrtvGrabFrameShm::open] shared memory create failed");
#endif
			throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory create failed", strerror(errno));
		}
	}

	// attach shared memory 
	if ((m_shmAddr = (char *) shmat (m_shmId, 0, 0)) <= (char *)0) {
#ifdef DEBUG
		perror ("[IrtvGrabFrameShm::open] shmat failed");
#endif

		throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory attach failed", strerror(errno));
	}
#else	// WIN32
	char	keyName[64];
	sprintf (keyName, "IRTV-SHM-%x", m_shmKey);

	if (NULL == (m_shmObj = OpenFileMappingA (FILE_MAP_ALL_ACCESS, NULL, keyName))) {
		if ( !(flags & O_CREAT) ) {
#ifdef DEBUG
			perror ("[IrtvGrabFrameShm::open] shared memory get failed");
#endif
			throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory get failed", strerror(errno));
		}

		if (NULL == (m_shmObj = CreateFileMappingA (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_shmSize, keyName))) {
#ifdef DEBUG
			perror ("[IrtvGrabFrameShm::open] shared memory create failed");
#endif
			throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory create failed", strerror(errno));
		}
	}

	// attach shared memory 
	if (NULL == (m_shmAddr = (char *) MapViewOfFile (m_shmObj, FILE_MAP_ALL_ACCESS, 0, 0, m_shmSize))) {
#ifdef DEBUG
		perror ("[IrtvGrabFrameShm::open] shmat failed");
#endif

		throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::open] shared memory attach failed", strerror(errno));
	}
#endif

#ifdef SYS_DEBUG
	printf ("[IrtvGrabFrameShm::open] m_shmId = %d, m_shmAddr = %p\n", m_shmId, m_shmAddr);
#endif

	return (m_shmAddr);
}

inline void IrtvGrabFrameShm::read (GrabFrameData &data) throw (IrtvGrabFrameShmException)
{
	copy (m_shmAddr, &data, sizeof(GrabFrameData));
}

inline void IrtvGrabFrameShm::write (GrabFrameData &data) throw (IrtvGrabFrameShmException)
{
	copy (&data, m_shmAddr, sizeof(GrabFrameData));
}

inline void IrtvGrabFrameShm::close () throw (IrtvGrabFrameShmException)
{
	isOK ();

#if !defined(WIN32)
	if (shmdt (m_shmAddr) < 0) {
		throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::close] shared memory detach failed", strerror(errno));
	}
#else
	UnmapViewOfFile (m_shmAddr);

	CloseHandle (m_shmObj);

	m_shmObj = NULL;
#endif

	m_shmAddr = NULL;
}

// remove
inline void IrtvGrabFrameShm::remove () throw (IrtvGrabFrameShmException)
{
	isOK ();
	
	close ();

#if !defined(WIN32)
	if (0 > shmctl (m_shmId, IPC_RMID, (struct shmid_ds *)0)) {
		setErrNum (errno);
		printf ("[IrtvGrabFrameShm::remove] shared memory remove failed %s\n", strerror(errno));
		throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::remove] shared memory remove failed", strerror(errno));
	}
#endif
}

inline void IrtvGrabFrameShm::lock () throw (IrtvGrabFrameShmException)
{
	isOK ();
}

inline void IrtvGrabFrameShm::unlock () throw (IrtvGrabFrameShmException)
{
	isOK ();
}

inline void IrtvGrabFrameShm::copy (const void *srcAddr, void *destAddr, const unsigned int copyLen) throw (IrtvGrabFrameShmException)
{
	isOK ();

	memcpy (destAddr, srcAddr, copyLen);
}

inline void IrtvGrabFrameShm::isOK () throw (IrtvGrabFrameShmException)
{
	if (NULL == m_shmAddr) {
		printf ("[IrtvGrabFrameShm::isOK] shared memory is not initialized\n");
		throw IrtvGrabFrameShmException ("[IrtvGrabFrameShm::isOK] shared memory is invalid");
	}
}

extern 	IrtvGrabFrameShm	gGrabFrameShm;

#endif _IRTV_GRAB_FRAME_SHM_H
