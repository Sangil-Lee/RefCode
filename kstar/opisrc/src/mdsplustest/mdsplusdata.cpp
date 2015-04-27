#include "mdsplusdata.h"

#define status_ok(status) (((status) & 1) == 1)

MDSPlusData::MDSPlusData()
{
	mdsConnection();
}
MDSPlusData::MDSPlusData(const string nodename, const string treename, const int shot)
					:mnodename(nodename),mtreename(treename),mshot(shot)
{
	mdsConnection();
	msize = getNodeLen();
	mtimebase = new double[msize];
	mshotdata = new double[msize];
#if 0
	if(getShotData()<0)
	{
		delete mtimebase;
		delete mshotdata;
		exit(-1);
	};
#endif
}

void MDSPlusData::setMDSDataInfo(const string nodename, const string treename, const int shot)
{
	mnodename = nodename;
	mtreename = treename;
	mshot = shot;
	msize = getNodeLen();
	if(mtimebase != 0) 
	{
		delete mtimebase;
		mtimebase = 0;
	};
	if(mshotdata != 0) 
	{
		delete mshotdata;
		mshotdata = 0;
	};
	mtimebase = new double[msize];
	mshotdata = new double[msize];
};

int MDSPlusData::mdsConnection(const string stlurlport)
{
	msocket = MdsConnect((char*)stlurlport.c_str());
	if ( msocket == -1 )
	{
		fprintf(stderr,"Error connecting to MDSPlus.\n");
		exit(-1);
	};
	return 0;
}

MDSPlusData::~MDSPlusData()
{
	if(msize != 0)
	{
		delete mtimebase;
		delete mshotdata;
	};
	MdsDisconnect();
}

int MDSPlusData::getNodeLen()
{
	/* local vars */
	int dtype_long = DTYPE_LONG;
	char buf[1024];
	int size;
	int null = 0;
	int idesc = descr(&dtype_long, &size, &null);

	/* init buffer */
	memset(buf,0,sizeof(buf));

	string ftreename = string("\\")+mtreename;
	/* put SIZE() TDI function around tree name */
	snprintf(buf,sizeof(buf)-1,"SIZE(%s)",ftreename.c_str());

	//printf("node:%s, shot:%d\n", mnodename.c_str(), mshot);
	int status = MdsOpen((char*)mnodename.c_str(), &mshot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening data tree for shot %d\n",mshot);
		return -1;
	};

	//printf("buf:%s, shot:%d\n", buf, mshot);
	/* use MdsValue to get the tree length */
	status = MdsValue(buf, &idesc, &null, 0);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Unable to get length of %s.\n",ftreename.c_str());
		return -1;
	};
	/* return tree length */
	MdsClose((char*)mnodename.c_str(), &mshot);
	return size;
}
string MDSPlusData::getShotTime()
{
	/* use get shot time */
	int dtype_str = DTYPE_CSTRING;
	string ftreename = "\\T0_STR";

#if 1
	int status = MdsOpen("rdata", &mshot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening rdata tree for shot %d\n",mshot);
		return "";
	};
#endif
	//int len = 26;
	//char vCstring[26];	//format "0000/00/00 00:00:00.000000";
	int len = 19;
	char vCstring[19];	//format "0000/00/00 00:00:00.000000";
	int null = 0;

	/* create a descriptor for this signal */
	int sigdesc = descr(&dtype_str,vCstring,&null,&len);

	/* retrieve signal */
	status = MdsValue((char*)ftreename.c_str(), &sigdesc, &null, &len );
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal**1\n");
		return "";
	};
	printf("Time:%s\n", vCstring);
	MdsClose((char*)mnodename.c_str(), &mshot);
	return string(vCstring);
}
int MDSPlusData::getShotData()
{
	/* use get_signal_length to get size of signal */
	int dtype_float = DTYPE_DOUBLE;
	string ftreename = string("\\")+mtreename;

	int status = MdsOpen((char*)mnodename.c_str(), &mshot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d\n",mshot);
		return -1;
	};
	int null = 0;
	int len =0;

	/* create a descriptor for this signal */
	int sigdesc = descr(&dtype_float, mshotdata, &msize, &null);
	/* retrieve signal */
	status = MdsValue((char*)ftreename.c_str(), &sigdesc, &null, &len );
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal**1\n");
		return -1;
	}

	/* create a descriptor for the timebase */
	int timedesc = descr(&dtype_float, mtimebase, &msize, &null);

	/* retrieve timebase of signal */
	string strtree_dim= string("DIM_OF(")+ftreename+string(")");
	status = MdsValue((char*)strtree_dim.c_str(), &timedesc, &null, 0);
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving timebase\n");
		return -1;
	};
	// possible function end.
#if 0
	for ( int i = 0 ; i < msize; i++ )
	{
		printf("%i  X:%f  Y:%f\n", i, mtimebase[i], mshotdata[i]);
	};
#endif
	double mxVal = *max_element(&mshotdata[0], &mshotdata[msize]);
	double mnVal = *min_element(&mshotdata[0], &mshotdata[msize]);
	printf("MAX:%f,Min:%f,Diff:%f\n", mxVal, mnVal,mxVal-mnVal);
	MdsClose((char*)mnodename.c_str(), &mshot);
	return 0;
}
int MDSPlusData::getShotData(double *timebase, double *shotdata, int size)
{
	/* use get_signal_length to get size of signal */
	int dtype_float = DTYPE_DOUBLE;
	string ftreename = string("\\")+mtreename;

	int status = MdsOpen((char*)mnodename.c_str(), &mshot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d\n",mshot);
		return -1;
	};

	int null = 0;
	/* create a descriptor for this signal */
	int sigdesc = descr(&dtype_float, shotdata, &size, &null);

	int len =0;
	/* retrieve signal */
	status = MdsValue((char*)ftreename.c_str(), &sigdesc, &null, &len );
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal**2\n");
		return -1;
	}
	/* create a descriptor for the timebase */
	int timedesc = descr(&dtype_float, timebase, &size, &null);

	/* retrieve timebase of signal */
	string strtree_dim= string("DIM_OF(")+ftreename+string(")");
	status = MdsValue((char*)strtree_dim.c_str(), &timedesc, &null, 0);
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving timebase\n");
		return -1;
	};
	MdsClose((char*)mnodename.c_str(), &mshot);
#if 0
	// possible function end.
	for ( int i = 0 ; i < size; i++ )
	{
		printf("%i  X:%f  Y:%f\n", i, timebase[i], shotdata[i]);
	};
#endif
	//double mxVal = *max_element(&mshotdata[0], &mshotdata[msize]);
	//double mnVal = *min_element(&mshotdata[0], &mshotdata[msize]);
	//printf("MAX:%f,Min:%f,Diff:%f\n", mxVal, mnVal,mxVal-mnVal);
	return 0;
}
