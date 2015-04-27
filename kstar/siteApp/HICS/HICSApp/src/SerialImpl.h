///////////////////////////////////////////////////////////
//  SerialImpl.h
//  Implementation of the Class SerialImpl
//  Created on:      09-4-2013 ���� 7:24:27
//  Original author: ysw
///////////////////////////////////////////////////////////

#if !defined(EA_3B47E355_CB7D_49e1_8FB5_E07544D57C8B__INCLUDED_)
#define EA_3B47E355_CB7D_49e1_8FB5_E07544D57C8B__INCLUDED_

#include <string>
#include <termios.h>

#include "kutilObj.h"

#include "ControllerInterface.h"

using namespace std;

class SerialImpl : public ControllerInterface
{
public:
	static const char	MODEMDEVICE[];
	static const int	BAUDRATE	= B115200;

#if 0
	static const int	ENQ_CHR		= 0x05;		// Enquire : start code of frame
	static const int	EOT_CHR		= 0x04;		// End of Text : end code of frame
#else
	static const int	ENQ_CHR		= 0x0A;		// Enquire : start code of frame
	static const int	EOT_CHR		= 0x0D;		// End of Text : end code of frame
#endif
	static const int	START_CHR	= ENQ_CHR;	// start code of frame
	static const int	STOP_CHR	= EOT_CHR;	// end code of frame

	SerialImpl();
	virtual ~SerialImpl();

	virtual int close();
	virtual bool isOpened();
	virtual int open();
	virtual int read(void * buf, int size);
	virtual int write(void * buf, int size);
	int getFd();
	void setFd(int newVal);
	int getReadFd();
	void setReadFd(int newVal);
	void setConfiguration (const char *pszDevName, const int iBaudRate);
	const char *getDevName ();
	const int getBaudRate ();
	void printConfiguration ();

private:
	int m_fd;
	int m_readFd;	// dup(m_fd)
	string m_strDevName;
	int m_iBaudRate;

	const int readn (const int fd, char *buf, const int nbytes);
	const int readWithControl(const int fd, char *buf, const int size);
	const int writeWithControl(int fd, char * buf, int size);

};
#endif // !defined(EA_3B47E355_CB7D_49e1_8FB5_E07544D57C8B__INCLUDED_)
