#include <cadef.h>
#include <dbDefs.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//silee.
using namespace std;

int main(int argc, char *argv[])
{
#if 1
	/* Start up Channel Access */
    int result = ca_context_create(ca_disable_preemptive_callback);
    if (result != ECA_NORMAL) 
	{
        printf("CA error %s occurred while trying to start channel access.\n", ca_message(result));
        return -1;
    };

	enum	Connstatus { NEVER_CONN=0, DISCONN, CONN};

	int     c;
	int     index;
	int     iserr = 0;

	char    *srcfilename = NULL; 

	while ((c = getopt (argc, argv, "c:")) !=  -1) 
	{
		switch (c) {
			case  'c':
				srcfilename = optarg;
				break;
			case  '?':
				printf("USAGE: DBConncheck -c checkfile\n");
				return -1;
		};
	};

	for (index = optind; index < argc; index++) 
	{
		printf ("Non-option argument %s\n", argv[index]);
		iserr++;
	};

	if (iserr||argc<=2) 
	{
		printf("USAGE: DBConncheck -c checkfile\n");
		return -1;
	};

	printf("argc:%d, chkfile:%s\n", argc, srcfilename);
	
	ifstream srcfile(srcfilename);
	if ( srcfile.fail () ) 
	{
		printf("source file not found\n");
		return -1;
	};

	string strToken;
	int line = 0;
	vector<string> checkfile_vec;
	vector<string>::iterator check_vec_iter;
	while (!srcfile.eof ())
	{
		getline (srcfile, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		string pvname = pch;

		checkfile_vec.push_back(pvname);
		line++;
	};
	/* Connect channels */
	enum channel_state state;
	size_t rowcnt = checkfile_vec.size(); 
	chid channelID[rowcnt];
	int res = 0;
	for ( size_t i = 0; i < rowcnt; i++)
	{
		res = ca_create_channel( checkfile_vec.at(i).c_str() , 0, 0, 50, &channelID[i]);
		if (res != ECA_NORMAL) 
		{
			printf("CA error %s occurred while trying to create channel.\n", ca_message(res));
			return -1;
		};
	};
	ca_pend_io (0.5);
	int nconncnt = 0;
	for ( size_t i = 0; i < rowcnt; i++)
	{
		state   = ca_state(channelID[i]);
		//printf("pv_state:%d", state);
		if(state==NEVER_CONN)
		{
			nconncnt++;
			printf("%s: Not found.\n", checkfile_vec.at(i).c_str());
		}
		else if( state== DISCONN )
		{
			nconncnt++;
			printf("%s: Disconnected.\n", checkfile_vec.at(i).c_str());
		}
		else if (state == CONN)
		{
			printf("%s: Connected.\n", checkfile_vec.at(i).c_str());
		};
	};

	/* Shut down Channel Access */
    ca_context_destroy();
#endif
}
