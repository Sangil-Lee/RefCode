///////////////////////////////////////////////////////////
//  UdpImpl.h
//  Implementation of the Class UdpImpl
//  Created on:      09-4-2013 ���� 7:40:26
//  Original author: ysw
///////////////////////////////////////////////////////////

#if !defined(EA_0BD67333_9542_4f02_B38A_B42C1A4A9680__INCLUDED_)
#define EA_0BD67333_9542_4f02_B38A_B42C1A4A9680__INCLUDED_

#include <string>

#include "ControllerInterface.h"

using namespace std;

class UdpImpl : public ControllerInterface
{

public:
	UdpImpl();
	virtual ~UdpImpl();

	int getFd();
	string getIpAddr();
	short getPortNo();
	void setFd(int newVal);
	void setIpAddr(string newVal);
	void setPortNo(short newVal);
	virtual int close();
	virtual bool isOpened();
	virtual int open();
	virtual int read(void * buf, const int size);
	virtual int write(void * buf, int size);
	void setConfiguration (const char *pszIpAddr, const short sPortNo);
	void printConfiguration ();

private:
	int m_fd;
	short m_sPortNo;
	string m_szIpAddr;

};
#endif // !defined(EA_0BD67333_9542_4f02_B38A_B42C1A4A9680__INCLUDED_)
