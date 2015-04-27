#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

using namespace std;
using namespace xmlrpc_c;

int
main(int argc, char **argv) 
{
	double a;
	double b;
	string mod;
#if 1
    if (argc == 4) 
	{
		a = atof(argv[2]);
		b = atof(argv[3]);
		mod = argv[1];
    }
	else
	{
		a = 2.3;
		b = 2.4; 
		mod = "add";
	};
#endif

    try 
	{
        string const serverUrl("http://172.17.100.116:8081/RPC2");
        string methodName = string("localCmd.") + mod;

        clientSimple myClient;
        value rpcresult;
        
        myClient.call(serverUrl, methodName, "dd", &rpcresult, a, b);

        //int const sum = value_int(rpcresult);
        //double const result = value_double(rpcresult);

		string result = value_string(rpcresult);

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
    }

    return 0;
}
