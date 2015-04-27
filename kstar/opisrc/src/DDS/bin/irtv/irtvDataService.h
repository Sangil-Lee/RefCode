#ifndef _IRTV_DATA_SERVICE_H
#define _IRTV_DATA_SERVICE_H

#if !defined(WIN32)
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#else
#include <winsock2.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <epicsTypes.h>

#include <map>
#include <string>

#include "irtvLog.h"
#include "irtvGrabFrameData.h"

#if !defined(WIN32)
#define	SOCKET			int
#define	INVALID_SOCKET	(-1)
#define	closesocket		::close
#endif

using namespace::std;

#define	WINSOCK_LOW_VERSION		2
#define	WINSOCK_HIGH_VERSION	2

// socket information
const int	IRTV_SVR_PORT_NO		= 8001;
const int	IRTV_CLNT_PORT_NO		= 8002;

// service types
const int	IRTV_SVC_HEARTBEAT		= 1;
const int	IRTV_SVC_SEND_DATA		= 11;

const int	IRTV_DATA_SEND_PERIOD	= 500;		// msec

#if defined(WIN32)
const int	IRTV_RCVBUF_MAX			= (65536 * 10);
#else
const int	IRTV_RCVBUF_MAX			= 65536;
#endif

class IrtvSvcHeader
{
public :
	IrtvSvcHeader () { init (); };
	~IrtvSvcHeader () {};

	void set (int nSvcType, int nSvcSeq, int nSvcSize, int bLastMsg, int nMsgSeq, int nMsgLen, int nGrabTime) {
		m_nSvcType	= nSvcType;
		m_nSvcSeq	= nSvcSeq;
		m_nSvcSize	= nSvcSize;
		m_bLastMsg	= bLastMsg;
		m_nMsgSeq	= nMsgSeq;
		m_nMsgLen	= nMsgLen;
		m_nGrabTime	= nGrabTime;
	}

	void init () {
		set (0, 0, 0, 0, 0, 0, 0);
	}

	const int getSvcType () { return (m_nSvcType); }

public :
	unsigned char	m_nSvcType;			// Service type
	unsigned char	m_bLastMsg;			// 1 : Last message
	unsigned short	m_nSvcSeq;			// Sequecne Number for frame data
	unsigned short	m_nMsgSeq;			// 
	unsigned short	m_nMsgLen;			// length of message ( ~ 1500)
	unsigned int	m_nSvcSize;			// size of service
	unsigned int	m_nGrabTime;		// seconds

};

class IrtvSvcData
{
public :
	IrtvSvcData () { init (); };
	~IrtvSvcData () {};

	void set (IrtvSvcHeader &header, GrabFrameData &data) {
		memcpy ((char *)&m_header, (char *)&header, sizeof(IrtvSvcHeader));
		memcpy ((char *)&m_data, (char *)&data, sizeof(GrabFrameData));
	}

	void init () {};

	const int getSvcType () { return (m_header.getSvcType()); }
	const int getWidth () { return (m_data.getWidth()); }
	const int getHeight () { return (m_data.getHeight()); }
	const int getFrameSize () { return (m_data.getFrameSize()); }

public :
	IrtvSvcHeader	m_header;
	GrabFrameData	m_data;

};

const int	IRTV_MTU	= (1472 - sizeof(IrtvSvcHeader));

class IrtvSvcUdpData
{
public :
	IrtvSvcUdpData () { init (); };
	~IrtvSvcUdpData () {};

	void init () {};

	const int getSvcType () { return (m_header.getSvcType()); }

public :
	IrtvSvcHeader	m_header;
	char			m_data[IRTV_MTU];

};

class IrtvSvcUdp
{
public :
	IrtvSvcUdp () {
		init ();
	};

	~IrtvSvcUdp () {};

	void init () {
		m_sockFd	= INVALID_SOCKET;
	}

	// 家南 积己
	const int open (const epicsUInt16 portNo = 0) {
		initSocket ();

		SOCKET	fd	= INVALID_SOCKET;

		if (INVALID_SOCKET == (fd = socket (AF_INET, SOCK_DGRAM, 0))) {
			kLog (K_ERR, "[KsSocketUdp::open] open failed \n", strerror(errno));
			return (INVALID_SOCKET);
		}

		struct sockaddr_in	bindAddr;

		bindAddr.sin_family      = AF_INET;
		bindAddr.sin_addr.s_addr = htonl (INADDR_ANY);
		bindAddr.sin_port        = htons (portNo);

#if !defined(WIN32)
		if (0 != bindAddr.sin_port) {
			if (bind (fd, (struct sockaddr *) &bindAddr, sizeof(bindAddr)) < 0) {
				kLog (K_ERR, "[KsSocketUdp::open] bind failed \n", strerror(errno));
				return (INVALID_SOCKET);
			}
		}
#else
		if (bind (fd, (struct sockaddr *) &bindAddr, sizeof(bindAddr)) < 0) {
			kLog (K_ERR, "[KsSocketUdp::open] bind failed \n", strerror(errno));
			return (INVALID_SOCKET);
		}
#endif

		m_sockFd	= fd;

		setSockOptions ();
		setRecvBufferSize (IRTV_RCVBUF_MAX);

		return (m_sockFd);
	}

	// set the mode of reuse address
	void setSockOptions () {
		int		reuseMode = 1;

		if (setsockopt (m_sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseMode, sizeof(reuseMode)) < 0) {
			kLog (K_ERR, "[IrtvSvcUdp::setSockOptions] reuse address set is failed \n");
		}

#if 0
		unsigned char	loop	= 0;

		if (setsockopt (m_sockFd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop, sizeof(loop)) < 0) {
			kLog (K_ERR, "[IrtvSvcUdp::setSockOptions] loopback set is failedd \n");
		}
#endif
	}

	void setRecvBufferSize (const int size) {
		int			orgBufSize	= 0;
		socklen_t	len	= sizeof(orgBufSize);

		if (getsockopt (m_sockFd, SOL_SOCKET, SO_RCVBUF, (char *)&orgBufSize, &len) == 0) {
			if (orgBufSize >= size) {
				kLog (K_MON, "[IrtvSvcUdp::setRecvBufferSize] old(%d) new(%d) : skip \n", orgBufSize, size);
				return;
			}
		}

		kLog (K_MON, "[IrtvSvcUdp::setRecvBufferSize] old(%d) -> new(%d) \n", orgBufSize, size);

		int		bufSize = size;

		// set recv buffer size
		if (setsockopt (m_sockFd, SOL_SOCKET, SO_RCVBUF, (char *)&bufSize, sizeof(bufSize)) < 0) {
			kLog (K_ERR, "[IrtvSvcUdp::setRecvBufferSize] SO_RCVBUF failed (%d) \n", bufSize);
		}
	}

	void write (const char *writeBuf, const epicsUInt32 writeSize, const char *destIpAddr, const epicsUInt16 portNo) {
		struct sockaddr_in	destAddr;

		destAddr.sin_family      = AF_INET;
		destAddr.sin_port        = htons (portNo);
		destAddr.sin_addr.s_addr = inet_addr(destIpAddr);

		write (writeBuf, writeSize, &destAddr);
	}

	void write (const char *writeBuf, const epicsUInt32 writeSize, const sockaddr_in *destAddr) {
		if (sendto (m_sockFd, writeBuf, writeSize, 0, (struct sockaddr *)destAddr, sizeof(sockaddr_in)) != (int)writeSize) {
			kLog (K_ERR, "[KsSocketUdp::write] sendto failed \n");
		}
	}

	const int read (char *readBuf, const epicsUInt32 readBufferSize, const epicsUInt32 readTimeOut) {
		kLog (K_DEBUG, "[KsSocketUdp::read] size(%d) timeout(%d) \n", readBufferSize, readTimeOut);

		struct timeval	timeout;
		struct timeval	*pTimeout = NULL;

		if (0 != readTimeOut) {
			timeout.tv_sec  = readTimeOut / 1000;
			timeout.tv_usec = (readTimeOut % 1000) * 1000;

			pTimeout = &timeout;
		}

		int		nReadLen;
		int		addrLen = sizeof(struct sockaddr_in);
		fd_set	readfds;

		// clear timeout flag
		m_isTimeOut	= false;

		while (1) {
			FD_ZERO ((fd_set *)&readfds);
			FD_SET  (m_sockFd, (fd_set *)&readfds);

			switch (select (m_sockFd + 1, (fd_set *)&readfds, (fd_set *)NULL, (fd_set *)NULL, pTimeout)) {
				case 0 :	// timeout
					// set timeout
					m_isTimeOut = true;
					return (0);

				case -1 :	// fail or EINTR
					if (EINTR == errno) {
						continue;
					}

					kLog (K_ERR, "[KsSocketUdp::read] recvfrom failed \n");
					return (NOK);
			}

			break;
		}

		kLog (K_DEBUG, "[KsSocketUdp::read] recvfrom ... \n");

		while (1) {
			nReadLen = recvfrom (m_sockFd, readBuf, readBufferSize, 0, (struct sockaddr *) &m_srcAddr, (socklen_t *)&addrLen);

			if (nReadLen < 0) {
				if (EINTR == errno) {
					continue;

				}
#if defined(WIN32)
				if (WSAECONNRESET == WSAGetLastError()) {
					continue;
				}
#endif
			}
			break;
		}

		if (nReadLen < 0) {
			kLog (K_ERR, "[KsSocketUdp::read] recvfrom failed \n");
			return (NOK);
		}

		return (nReadLen);
	}

	void close () {
		if (m_sockFd != INVALID_SOCKET) {
			closesocket (m_sockFd);
			m_sockFd = INVALID_SOCKET;
		}
	}

	const unsigned int getSrcInetAddr () {
		return (m_srcAddr.sin_addr.s_addr); 
	}

	const char *getAddrToStr () {
		return (getAddrToStr (m_srcAddr.sin_addr.s_addr));
	}

	const char *getAddrToStr (const epicsUInt32 addr) {
		static char buf[30];
		struct in_addr	inAddr;

		memcpy ((char *)&inAddr, (char *)&addr, sizeof(struct in_addr));
		sprintf (buf, "%s", inet_ntoa (inAddr));

		return (buf);
	}

protected :

	const int initSocket () {
#if defined(WIN32)
		WORD	wVersionRequested	= MAKEWORD (WINSOCK_LOW_VERSION, WINSOCK_HIGH_VERSION);
		WSADATA	wsaData;

		if (0 != WSAStartup (wVersionRequested, &wsaData)) {
			kLog (K_ERR, "[IrtvSvcUdp::initSocket] WSAStartup failed \n", strerror(errno));
			return (NOK);
		}

		if (LOBYTE(wsaData.wVersion) != WINSOCK_LOW_VERSION || HIBYTE(wsaData.wVersion) != WINSOCK_HIGH_VERSION) {
			// Tell the user that we could not find a usable WinSock DLL
			WSACleanup();
			kLog (K_ERR, "[IrtvSvcUdp::initSocket] WSAStartup : Could not find a usable version of Winsock.dll \n");
			return (NOK);
		}
#endif

		return (OK);
	}

private :

	SOCKET			m_sockFd;
	bool			m_isTimeOut;
	sockaddr_in		m_srcAddr;			// address of source

};

#endif // _IRTV_DATA_SERVICE_H
