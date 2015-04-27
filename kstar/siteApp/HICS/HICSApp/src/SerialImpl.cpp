///////////////////////////////////////////////////////////
//  SerialImpl.cpp
//  Implementation of the Class SerialImpl
//  Created on:      09-4-2013 ���� 7:24:27
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "SerialImpl.h"

const char	SerialImpl::MODEMDEVICE[]   = "/dev/ttyS0";

SerialImpl::SerialImpl()
{
	m_strDevName	= MODEMDEVICE;
	m_iBaudRate		= BAUDRATE;
	m_fd			= -1;
	m_readFd		= -1;
}

SerialImpl::~SerialImpl()
{
	close ();
}

void SerialImpl::printConfiguration ()
{
	kuDebug (kuINFO, "[SerialImpl::configuration] dev(%s) baud(%d)\n", getDevName(), getBaudRate());
}

void SerialImpl::setConfiguration (const char *pszDevName, const int iBaudRate)
{
	if (NULL != pszDevName) {
		m_strDevName	= pszDevName;
	}

	m_iBaudRate	= iBaudRate;

	printConfiguration ();
}

bool SerialImpl::isOpened()
{
	if ( (0 <= getFd()) || (0 <= getReadFd()) ) {
		return (true);
	}

	return false;
}

int SerialImpl::open()
{
	//m_fd	= ::open (getDevName(), O_RDWR | O_NOCTTY, O_NONBLOCK);		// non-blocking mode
	m_fd	= ::open (getDevName(), O_RDWR | O_NOCTTY);

	if (0 > m_fd) {
		kuDebug (kuERR, "[SerialImpl::open] open failed : dev(%s) baud(%d)\n", getDevName(), getBaudRate());
		return (kuNOK);
	}

	struct termios	oldtio, newtio;

	// stores current setting to vairable
	if (0 > tcgetattr (m_fd, &oldtio)) {
		kuDebug (kuERR, "[SerialImpl::open] dev(%s) tcgetattr failed\n", getDevName());
		//return (kuNOK);
	}

	bzero(&newtio, sizeof(newtio));

	newtio.c_cflag		= CS8;				// 8N1 (8bit, no parity, 1stopbit)
	newtio.c_cflag		|= CLOCAL;			// local connection
	newtio.c_cflag		|= CREAD;			// enable reading
//	newtio.c_cflag		|= CRTSCTS;			// H/W flow control
	newtio.c_cflag		&= ~CRTSCTS;			// disable flow control.   2013. 12. 2 by woong
	newtio.c_cflag		|= getBaudRate();	// baud rate : (ex) B115200
	newtio.c_iflag		= IGNPAR;			// non-parity
	newtio.c_oflag		= 0;
	newtio.c_lflag		= 0;				// input mode (non-canonical, no echo, ...)

	// timeout is 5 seconds
	newtio.c_cc[VTIME]	= 50;				// timeout : TIME * 0.1 sec
	newtio.c_cc[VMIN]	= 0;				// blocking

	if (0 > tcflush (m_fd, TCIFLUSH)) {
		kuDebug (kuERR, "[SerialImpl::open] dev(%s) tcflush failed\n", getDevName());
		//return (kuNOK);
	}

	if (0 > tcsetattr (m_fd, TCSANOW, &newtio)) {
		kuDebug (kuERR, "[SerialImpl::open] dev(%s) tcsetattr failed\n", getDevName());
		//return (kuNOK);
	}

	// duplicates fd to read and write concurrently among threads
	m_readFd = dup(m_fd);

	kuDebug (kuDEBUG, "[SerialImpl::open] dup(%d/%d)\n", m_fd, m_readFd);

	kuDebug (kuINFO, "[SerialImpl::open] dev(%s) was opened\n", getDevName());

	return (kuOK);
}

int SerialImpl::close()
{
	if (isOpened()) {
		if (0 <= getFd()) {
			::close (getFd());
			setFd(-1);
		}

		if (0 <= getReadFd()) {
			::close (getReadFd());
			setReadFd(-1);
		}

		kuDebug (kuINFO, "[SerialImpl::close] dev(%s) was closed\n", getDevName());
	}

	return (kuOK);
}

const int SerialImpl::readn (const int fd, char *buf, const int nbytes)
{
	int		nleft, nread, noffset;
	char	ch;

	nleft	= nbytes;
	noffset	= 0;

	memset (buf, 0x00, sizeof(buf));

	while (nleft > 0) {
		if ((nread = ::read (fd, &ch, 1)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			else {					// Error
				return (-1);
			}
		}
		else if (0 == nread) {		// EOF
			buf[nread] = '\0';
			break;
		}

		if ( (buf[0] != START_CHR) && (ch != START_CHR) ) {
			kuDebug (kuMON, "[SerialImpl::readn] nbytes(%d) noffset(%d) : ch(0x%02x) skip ...\n", nbytes, noffset, ch);
			continue;
		}

		buf[noffset++]	= ch;
		nleft			-= nread;

		kuDebug (kuDATA, "[SerialImpl::readn] nbytes(%d) noffset(%d) : ch(0x%02x)\n", nbytes, noffset, ch);
	}

	// checks start and stop byte
	if ( (buf[0] != START_CHR) || (buf[nbytes-1] != STOP_CHR) ) {
		return (kuNOK);
	}

	// check checksum : none

	return (nbytes - nleft);
}

const int SerialImpl::readWithControl(const int fd, char *buf, const int size)
{
	char	tmp[256];
	int		iTotSize	= size + 2;		// start(byte) + data(size) + end(byte)

	if (iTotSize != readn (fd, tmp, iTotSize)) {
		return (kuNOK);
	}

	// copies received data to user buffer
	memcpy (buf, tmp + 1, size);

	return (size);
}

int SerialImpl::read(void * buf, int size)
{
	if ( (NULL == buf) || (0 >= size) ) {
		kuDebug (kuERR, "[SerialImpl::read] input is invalid \n");
		return (kuNOK);
	}

	int	iReadBytes;

	if (size != (iReadBytes = readWithControl (getReadFd(), (char *)buf, size))) {
		kuDebug (kuERR, "[SerialImpl::read] size(%d) reads(%d)\n", size, iReadBytes);
	}

	if (kuTRUE == kuCanPrint (kuDATA)) {
		kuDebug (kuDATA, "[SerialImpl::read ] dev(%s) ----------------------\n", m_strDevName.c_str());
		kuStd::dump (iReadBytes, (char *)buf);
	}

	kuDebug (kuTRACE, "[SerialImpl::read] dev(%s) : reads(%d) \n", m_strDevName.c_str(), iReadBytes);

	return (iReadBytes);
}

const int SerialImpl::writeWithControl(int fd, char * buf, int size)
{
	char	tmp[256];
	int		iTotSize	= size + 2;		// start(byte) + data(size) + end(byte)

	tmp[0]		= START_CHR;
	tmp[size+1]	= STOP_CHR;
	memcpy (tmp + 1, buf, size);

	if (iTotSize != kuStd::writen (fd, tmp, iTotSize)) {
		return (kuNOK);
	}

	return (size);
}

int SerialImpl::write(void * buf, int size)
{
	if ( (NULL == buf) || (0 >= size) ) {
		kuDebug (kuERR, "[SerialImpl::read] input is invalid \n");
		return (kuNOK);
	}

	if (kuTRUE == kuCanPrint (kuDATA)) {
		kuDebug (kuMON, "[SerialImpl::write] dev(%s) ----------------------\n", m_strDevName.c_str());
		kuStd::dump (size, (char *)buf);
	}

	if (size != writeWithControl (getFd(), (char *)buf, size)) {
		return (kuNOK);
	}

	kuDebug (kuTRACE, "[SerialImpl::write] dev(%s) : writes(%d)\n", getDevName(), size);

	return (kuOK);
}

int SerialImpl::getFd()
{
	return m_fd;
}

void SerialImpl::setFd(int newVal)
{
	m_fd = newVal;
}

int SerialImpl::getReadFd()
{
	return m_readFd;
}

void SerialImpl::setReadFd(int newVal)
{
	m_readFd = newVal;
}

const char * SerialImpl::getDevName()
{
	return m_strDevName.c_str();
}

const int SerialImpl::getBaudRate()
{
	return m_iBaudRate;
}

