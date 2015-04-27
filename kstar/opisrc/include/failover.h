// Filename : icmpsocket.h
//
// ***************************************************************************
//
// Copyright(C) 2009, National Fusion Research Institute, All rights reserved.
// Proprietary information, National Fusion Research Institute.
//
// ***************************************************************************
//
// ***************************************************************************
#ifndef _FAILOVER_H
#define _FAILOVER_H
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <iostream>
#include <ostream>
#include <iterator>
#include "icmpsocket.h"

using namespace std;

class FailOver
{
public:
	//Default constaructor.
	FailOver();

	//Destructor.
	~FailOver();

private:
	ICMPSocket *micmpsocket;
	int checkSlave(const string ipaddr);
	int checkMaster(const string ipaddr);
};
#endif
