#include <string>
#include <cmath>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "cadef.h"
#include "valueToStr.h"

using namespace std;
using namespace xmlrpc_c;

int main() 
{
	string mod;
    try 
	{
#if 0
        string const serverUrl("http://172.17.100.116:8081/RPC2");
        string methodName = string("localCmd.") + string("GETPV");
        clientSimple myClient;
		string pvname = "TMS_CSBUS_SPT_SAD001";
		while(true)
		{
			value rpcresult;
			myClient.call(serverUrl, methodName, "s", &rpcresult, pvname.c_str());
			double const result = value_double(rpcresult);
			// Assume the method returned an integer; throws error if not
			cout << "Result of RPC: " << result << endl;
			sleep(1);
		};
#else
        string const serverUrl("http://172.17.100.116:8081/RPC2");
        string const serverUrl2("http://172.17.100.102:8081/RPC2");
        string methodName = string("localCmd.") + string("SENDPV");
		clientSimple myClient;
		clientSimple myClient2;
		string pvname[3] = {"TMS_CSBUS_SPT_SAD001", "TMS_CSBUS_SPT_SAD002","TMS_CSBUS_SPT_SAD003"};

		chid pv_chid[3];
		for ( int i = 0; i < 3; i++)
		{
			ca_create_channel(pvname[i].c_str(), 0, 0, 0, &pv_chid[i]);
		};
		ca_pend_io(0.1);
		struct dbr_time_double data[3];
		int count = 0;
		while ( true )
		{
			for ( int i = 0; i < 3; i++ )
			{
				ca_get(DBR_TIME_DOUBLE, pv_chid[i], (void *)&data[i]);
			};
			ca_pend_io(0.2);
			printf("[Count:%d]->PV1:%f, PV2:%f, PV3:%f\n", count, data[0].value, data[1].value, data[2].value);
			long sec; double msc, msc1;
			GetMicroTime( &sec, &msc );
			printf("[Before->Sec:%ld] [mSec]:%f\n", sec, msc);
			value rpcresult, rpcresult2;
			myClient.call(serverUrl, methodName, "iddd", &rpcresult, count, data[0].value,data[1].value,data[2].value);
			myClient2.call(serverUrl2, methodName, "iddd", &rpcresult2, count, data[0].value,data[1].value,data[2].value);
			string msg = value_string(rpcresult);
			string msg2 = value_string(rpcresult2);
			GetMicroTime( &sec, &msc1 );
			printf("[After->Sec:%ld] [mSec]:%f, [Offset]:%f\n", sec, msc, fabs(msc1-msc));
			cout << "OPI16_MethodCall:" << msg<<" OPI02_MethodCall:"<<msg2<< endl;
			count++;
			//usleep(500000);
			sleep(1);
		};
#endif
    } 
	catch (girerr::error const error) 
	{
        cerr << "Client threw error: " << error.what() << endl;
    } 
	catch (...) 
	{
        cerr << "Client threw unexpected error." << endl;
    }

    return 0;
}
