#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#define		PI 			3.14159265358979332384626433832795

//period is 60 sec( 1 minute)
#define 	PERIOD 		60

using namespace std;

int
main(int argc, char **argv) 
{
	double x;
	string mod = "sin";
	int radian = 0;

	while ( true )
	{
		try 
		{
			x = (radian + (36.0 / PERIOD) ) * PI / 180;
			string const serverUrl("http://172.17.100.116:8081/RPC2");
			string methodName = string("sample.") + mod;

			xmlrpc_c::clientSimple myClient;
			xmlrpc_c::value rpcresult;

			myClient.call(serverUrl, methodName, "d", &rpcresult, x);

			//int const sum = xmlrpc_c::value_int(rpcresult);
			double const result = xmlrpc_c::value_double(rpcresult);

			// Assume the method returned an integer; throws error if not
			cout << "Result of RPC: " << result << endl;
		} 
		catch (girerr::error const error) 
		{
			cerr << "Client threw error: " << error.what() << endl;
		} 
		catch (...) 
		{
			cerr << "Client threw unexpected error." << endl;
		};
		radian += (360 / PERIOD);
		radian %= 360;
		sleep(1);
	};

    return 0;
}
