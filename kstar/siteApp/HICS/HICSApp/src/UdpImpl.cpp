///////////////////////////////////////////////////////////
//  UdpImpl.cpp
//  Implementation of the Class UdpImpl
//  Created on:      09-4-2013 ���� 7:40:26
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "UdpImpl.h"
#include "kutil.h"
#include "kutilObj.h"


UdpImpl::UdpImpl(){
	m_fd		= -1;
	m_sPortNo	= 0;
}



UdpImpl::~UdpImpl(){

}





int UdpImpl::getFd(){

	return m_fd;
}


string UdpImpl::getIpAddr(){

	return m_szIpAddr;
}


short UdpImpl::getPortNo(){

	return m_sPortNo;
}


void UdpImpl::setFd(int newVal){

	m_fd = newVal;
}


void UdpImpl::setIpAddr(string newVal){

	m_szIpAddr = newVal;
}


void UdpImpl::setPortNo(short newVal){

	m_sPortNo = newVal;
}


bool UdpImpl::isOpened(){

	return false;
}


int UdpImpl::open(){

	return (kuOK);
}


int UdpImpl::read(void * buf, const int size){

	return (kuOK);
}


int UdpImpl::write(void * buf, int size){

	return (kuOK);
}

int UdpImpl::close(){

	return (kuOK);
}

void UdpImpl::setConfiguration (const char *pszIpAddr, const short sPortNo)
{
}

void UdpImpl::printConfiguration()
{
}

