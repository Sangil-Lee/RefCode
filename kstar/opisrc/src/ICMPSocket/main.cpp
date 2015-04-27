#include <stdio.h>
#include "icmpsocket.h"

string IntToStr ( const int value, const char *format="%d")
{
	char tmpVal[8];
	snprintf    (tmpVal, sizeof(tmpVal), format, value );
	return (value > 32767 ? "IntOverflow": value < -32768 ? "IntUnderflow": tmpVal);
};

string  Epoch2Datetimestr ( time_t epochtime = 0 )
{
	struct tm *ptime;

	int year = 0;

	int month = 0, day = 0, hour = 0, minute = 0, second = 0;

	//ptime = ( epochtime == 0 ) ? localtime ((const time_t*)time(NULL)) : localtime ( &epochtime );
	if ( epochtime == 0 ) epochtime = time (NULL);

	ptime =	localtime ( &epochtime );

	year	= ptime -> tm_year + 1900;
	month	= ptime -> tm_mon + 1;
	day		= ptime -> tm_mday;
	hour	= ptime -> tm_hour;
	minute  = ptime -> tm_min;
	second  = ptime -> tm_sec;

	return ( IntToStr (year) + "/" + IntToStr (month,"%02d") + "/" + IntToStr (day,"%02d")+ " "+ IntToStr (hour,"%02d") + ":" + IntToStr (minute,"%02d") + ":" + IntToStr(second,"%02d") );
};
int main (int argc, char *argv[])
{
	fstream logfile("icmplog.txt");
#if 1
	if(!logfile) 
	{
		printf("icmplog file open failed\n");
		return -1;
	};
#endif
	ICMPSocket pingSocket;
	pingSocket.open();
	set<string> setrequestAddr;
	set<string> setreplyAddr;
	set<string> resultAddr;
#if 0
	setrequestAddr.insert("172.17.22.100"); setrequestAddr.insert("172.17.22.100");
	setrequestAddr.insert("172.17.22.100"); setrequestAddr.insert("172.17.21.100");
	setrequestAddr.insert("172.17.23.100");
#endif
	//setrequestAddr.insert("172.18.54.135");
	setrequestAddr.insert("172.17.100.101"); setrequestAddr.insert("172.17.100.102");
	setrequestAddr.insert("172.17.100.103"); setrequestAddr.insert("172.17.100.104");
	setrequestAddr.insert("172.17.100.105"); setrequestAddr.insert("172.17.100.106");
	setrequestAddr.insert("172.17.100.107"); setrequestAddr.insert("172.17.100.108");
	setrequestAddr.insert("172.17.100.109"); setrequestAddr.insert("172.17.100.110"); 
	setrequestAddr.insert("172.17.100.111"); setrequestAddr.insert("172.17.100.112"); 
	setrequestAddr.insert("172.17.100.113"); setrequestAddr.insert("172.17.100.114"); 
	setrequestAddr.insert("172.17.100.115"); setrequestAddr.insert("172.17.100.116"); 
	setrequestAddr.insert("172.17.100.117"); setrequestAddr.insert("172.17.100.118"); 
	setrequestAddr.insert("172.17.100.119"); setrequestAddr.insert("172.17.100.120"); 
	setrequestAddr.insert("172.17.100.198"); setrequestAddr.insert("172.17.100.199"); 
	setrequestAddr.insert("172.17.100.200"); setrequestAddr.insert("172.17.100.201"); 
	setrequestAddr.insert("172.17.100.202"); setrequestAddr.insert("172.17.100.203");
	while(1)
	{
		try {
			//multiple host
			int time = pingSocket.ping( &setrequestAddr, &setreplyAddr, resultAddr);
			printf("timed: %d\n", time);
		} catch (Exception &sockErr) {
			printf("Exception %s\n", sockErr.getReason());
			pingSocket.close();
		}
#if 0
		set<string>::iterator setRIter;
		set<string>::iterator IpFaultIter;

		size_t i = 0;
		for( setRIter = setreplyAddr.begin(); setRIter != setreplyAddr.end(); ++setRIter, i++)
		{
			printf("count(%2d):ipAddr(%s)\n", i, setRIter->c_str());
		};
		for( setRIter = setrequestAddr.begin(); setRIter != setrequestAddr.end(); ++setRIter)
		{
			IpFaultIter = find (setreplyAddr.begin(), setreplyAddr.end(), setRIter->c_str());
			if(IpFaultIter == setreplyAddr.end())
				printf("*********Network Connection Failed:%s, time(%s)*************\n", setRIter->c_str(),Epoch2Datetimestr().c_str());
		};
#else
#if 0
		// move to ping method.
		set<string> setDiff;
		//insert_iterator<set <string, less<string>, allocator<string> > >  OR
		insert_iterator<set <string> > 
			setDiffIns(setDiff, setDiff.begin());
		//set_difference(setrequestAddr.begin(), setrequestAddr.end(), setreplyAddr.begin(), setreplyAddr.end(),
				//ostream_iterator<string>(cout, " " ));
		//cout<<endl;
		set_difference(setrequestAddr.begin(), setrequestAddr.end(), setreplyAddr.begin(), setreplyAddr.end(),
				setDiffIns);
#endif
		set<string>::iterator setRIter;
		size_t i = 0;
		for( setRIter = setreplyAddr.begin(); setRIter != setreplyAddr.end(); ++setRIter, i++)
		{
			printf("count(%2d):ipAddr(%s)\n", i, setRIter->c_str());
		};
		set<string>::iterator setIterFail;
		for( setIterFail= resultAddr.begin();setIterFail != resultAddr.end();++setIterFail)
		{
			printf("Nework Fail IP Addr:(%s), time(%s)\n", 
					setIterFail->c_str(), Epoch2Datetimestr().c_str());
			logfile << "Network Fail IP Addr:(" <<setIterFail->c_str()<<"), time("<<Epoch2Datetimestr()<<")"<<endl;
		}
#endif
		setreplyAddr.clear();
		resultAddr.clear();
		sleep(2);
	};
	pingSocket.close();
	logfile.close();
	return 0;
#if 0
	try {
		//single host
		pingSocket.ping("172.17.20.101");
		pingSocket.ping("172.17.100.101");
	}catch(Exception &sockErr) {
		printf("Exception %s\n", sockErr.getReason());
	}
	//setrequestAddr.insert("172.17.100.1");
#else
#endif
}

