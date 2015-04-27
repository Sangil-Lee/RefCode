#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <numeric>
#include <fstream>
#include "cadef.h"

using namespace std;

static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
	printf("exceptionCallback called\n");
    chid	chid = args.chid;
    //long	stat = args.stat; /* Channel access status code*/
    const char  *channel;
    static char *noname = "unknown";
    channel = (chid ? ca_name(chid) : noname);
    if(chid) printChidInfo(chid,"exceptionCallback");

};
int main(int argc, char *argv[]) 
{
	char    *filename = NULL;
	int		c=0;
	ca_context_create(ca_disable_preemptive_callback);

	int max =0, min = 0, avg=0;
	while ((c = getopt (argc, argv, "f:Mma")) !=  -1) 
	{
		switch (c) {
			case  'f':
				filename = optarg;
				break;
			case  'M':
				max = 1;
				break;
			case  'm':
				min = 1;
				break;
			case  'a':
				avg = 1;
				break;
			case  '?':
				printf("USAGE: caGetMathPV -f pvlistfile -M -m\n");
				return -1;
		};
	};

	vector<string> file_vec;
	vector<string>::iterator file_veciter;
	//string filepath = string("/usr/local/opisrc/src/CAGetMaxPV/")+string(filename);
	//ifstream pvlistfile(filepath.c_str());
	ifstream pvlistfile(filename);
	if ( pvlistfile.fail () ) 
	{
		printf("file not found\n");
		return -1;
	};

	string pvname;
	while (!pvlistfile.eof ())
	{
		getline (pvlistfile, pvname);
		if(pvname[0] == '#' || pvname.empty()==true) continue;

		char str[pvname.size()];
		strcpy (str, pvname.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;

		string pvname = pch;
		//printf("%s\n",pvname.c_str());
		file_vec.push_back(pvname);
		while (pch != NULL)
		{
			if(!(pch = strtok (NULL," \t,"))) continue;
			pvname = pch;
			//printf("%s\n",pvname.c_str());
			file_vec.push_back(pvname);
		};

	};
	pvlistfile.close();

	size_t pvcount = file_vec.size();

	//struct dbr_time_double data[pvcount];
	double data[pvcount];
	chid mychid[pvcount];
	double dvalue[pvcount];

	vector<chid> node_vec;
	vector<chid>::iterator node_veciter;
	for(size_t i = 0; i <pvcount; i++)
	{
		ca_create_channel(file_vec.at(i).c_str(), NULL, NULL, 10, &mychid[i]);
		node_vec.push_back(mychid[i]);
		ca_pend_io(1.0);
	};
	epicsTime  stamp;
	//struct local_tm_nano_sec tm;
	for(size_t i = 0; i < pvcount; i++)
	{
		//ca_get(DBR_TIME_DOUBLE, node_vec.at(i), (void *)&data[i]);
		ca_get(DBR_DOUBLE, node_vec.at(i), (void *)&data[i]);
		ca_pend_io(1.0);
		//dvalue[i] = fabs(data[i].value);
		//dvalue[i] = data[i].value;
		dvalue[i] = data[i];
		//printf("[%s]:%f\n", file_vec.at(i).c_str(),data[i]);
	};
	double resultval;
	if (max==1)
	{
		resultval = *max_element(&dvalue[0], &dvalue[pvcount]);
		size_t index = 0;
		for( index=0; index<pvcount; index++)
			if( dvalue[index] == resultval ) break;

		printf("pv max value:[%s]:%f\n",file_vec.at(index).c_str(),resultval);
	}
	if (min==1)
	{
		resultval = *min_element(&dvalue[0], &dvalue[pvcount]);
		size_t index = 0;
		for( index=0; index<pvcount; index++)
			if( dvalue[index] == resultval ) break;
		printf("pv min value:[%s]:%f\n",file_vec.at(index).c_str(),resultval);
	}
	if (avg==1)
	{
		resultval = accumulate(&dvalue[0], &dvalue[pvcount], 0.0);
		printf("pv average value:%6.12f\n",resultval/pvcount);
	}

	for(size_t i = 0; i < node_vec.size(); i++)
	{
		try {
			ca_clear_channel(node_vec.at(i));
			ca_pend_io(1.0);
		}catch(exception &e)
		{
			printf("error:%s\n", e.what());
		};
	};
	ca_context_destroy();
    return 0;
}
