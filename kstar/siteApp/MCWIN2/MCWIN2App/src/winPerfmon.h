#ifndef		CA_WINPERFMON_H
#define		CA_WINPERFMON_H

#pragma once

#include <map>
#include <vector>
#include <string>
#include <Windows.h>

using std::map;
using std::vector;
using std::string;

class CAWinPerfmon
{
public:
	CAWinPerfmon();
	virtual ~CAWinPerfmon();

	enum TrafficType 
	{
		AllTraffic		= 388,
		IncomingTraffic	= 264,
		OutGoingTraffic	= 506
	};
	int	GetInterfaceTotalTraffic(int index);
	double GetTraffic(int interfaceNumber);
	void SetTrafficType(int trafficType);
	string GetInterfaceName(const int index);
	string GetSystemName();
	string GetUserName();


private:
	double lasttraffic;
	int CurrentInterface;
	int CurrentTrafficType;
	BOOL GetInterfaces();
	//CStringList Interfaces;
	//map < DWORD, DWORD &>		Bandwidths;
	//map < DWORD, DWORD &>		TotalTraffics;
	vector <string>				Interfaces;
	map < DWORD, DWORD >		Bandwidths;
	map < DWORD, DWORD >		TotalTraffics;
};
#endif

