#include <mysql++.h>
#include <sys/time.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <kstardb.h>
#include <kstardbconn.h>

using namespace std;
using namespace mysqlpp;

void MicroTimePrint ( long *sec, double *msec )
{
	struct timeval tp;

	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;
		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		printf("sec: %d, msec:%f\n", *sec, *msec);
	};
};


int main(int argc, char *argv[])
{

#if 1
	int     c;
	int     index;
	int     flist = 0;
	int     rlist = 0;
	int     iserr = 0;

	char    *srcfilename = NULL; 
	char    *orgfilename = NULL;   
	char    *mgrfilename = NULL;   

	while ((c = getopt (argc, argv, "s:o:frm:")) !=  -1) 
	{
		switch (c) {
			case  's':
				srcfilename = optarg;
				break;
			case  'o':
				orgfilename = optarg;
				break;
			case  'f':
				flist = 1;
				break;
			case  'r':
				rlist = 1;
				break;
			case  'm':
				mgrfilename = optarg;
				break;
			case  '?':
				printf("USAGE: DBMerge -s source_file -o origin_file [-f(foward) -r(reverse) -m mergefile]\n");
				return -1;
		};
	};

	for (index = optind; index < argc; index++) 
	{
		printf ("Non-option argument %s\n", argv[index]);
		iserr++;
	};

	if (iserr||argc<=4) 
	{
		printf("USAGE: DBMerge -s source_file -o origin_file [-f(foward) -r(reverse) -m mergefile]\n");
		return -1;
	};

	printf("argc:%d, src:%s, org:%s, flist:%d, rlist:%d, mgr:%s\n", argc, srcfilename, orgfilename, flist, rlist, mgrfilename);
#endif

#if 1
	//set
	set<string> srcfile_set, orgfile_set, mgrfile_set;
	set<string>::iterator str_set_iter;
#else
	//vector
	vector<string> srcfile_vec, orgfile_vec, mgrfile_vec;
	vector<string>::iterator str_vec_iter;
#endif

	ifstream srcfile(srcfilename);
	ifstream orgfile(orgfilename);

	if ( srcfile.fail () ) 
	{
		printf("source file not found\n");
		return -1;
	};
	if ( orgfile.fail () ) 
	{
		printf("origin file not found\n");
		return -1;
	};

	string strToken;
	int line = 0;

	while (!srcfile.eof ())
	{
		getline (srcfile, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		string pvname = pch;
#if 1
		//set
		str_set_iter = find(srcfile_set.begin(), srcfile_set.end(), pvname);
		if ( str_set_iter != srcfile_set.end() ) 
		{
			printf("Already existed PVName:%s in source file:%s\n", pvname.c_str(), srcfilename);
			continue;
		};
		srcfile_set.insert(pvname);
#else
		//vector
		str_vec_iter = find(srcfile_vec.begin(), srcfile_vec.end(), pvname);
		if ( str_vec_iter != srcfile_vec.end() ) 
		{
			printf("Already existed PVName:%s in source file:%s\n", pvname.c_str(), srcfilename);
			continue;
		};
		srcfile_vec.push_back(pvname);
#endif
		line++;
	};
	srcfile.close();

	while (!orgfile.eof ())
	{
		getline (orgfile, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str," \t,"))) continue;
		string pvname = pch;
#if 1
		//set
		str_set_iter = find(orgfile_set.begin(), orgfile_set.end(), pvname);
		if ( str_set_iter != orgfile_set.end() ) 
		{
			printf("Already existed PVName:%s in origin file:%s\n", pvname.c_str(), orgfilename);
			continue;
		};
		orgfile_set.insert(pvname);
#else
		//vector
		str_vec_iter = find(orgfile_vec.begin(), orgfile_vec.end(), pvname);
		if ( str_vec_iter != orgfile_vec.end() ) 
		{
			printf("Already existed PVName:%s in origin file:%s\n", pvname.c_str(), orgfilename);
			continue;
		};
		orgfile_vec.push_back(pvname);
#endif
		line++;
	};
	orgfile.close();

#if 1
	//set
	if( srcfile_set == orgfile_set) printf("%s and %s is same file\n", srcfilename, orgfilename);
	else printf("%s and %s is different file\n", srcfilename, orgfilename);
#else
	//vector
	if( srcfile_vec == orgfile_vec) printf("%s and %s is same file\n", srcfilename, orgfilename);
	else printf("%s and %s is different file\n", srcfilename, orgfilename);
#endif

	set<string>::iterator diff_set_iter;
	if (flist == 1)
	{
		printf("Compare srcfile to orgfile list:\n");
		for( str_set_iter = srcfile_set.begin(); str_set_iter != srcfile_set.end(); ++str_set_iter )
		{
			diff_set_iter = find( orgfile_set.begin(), orgfile_set.end(), *str_set_iter);
			if(diff_set_iter != orgfile_set.end()) continue;
			printf("\tPVName: %s\n", (*str_set_iter).c_str() );
		};
	};
	if (rlist == 1)
	{
		printf("Compare orgfile to srcfile list:\n");
		for( str_set_iter = orgfile_set.begin(); str_set_iter != orgfile_set.end(); ++str_set_iter )
		{
			diff_set_iter = find( srcfile_set.begin(), srcfile_set.end(), *str_set_iter);
			if(diff_set_iter != srcfile_set.end()) continue;
			printf("\tPVName: %s\n", (*str_set_iter).c_str() );
		};
	};

	//fstream mgrfile(mgrfilename, ios::out);
	fstream mgrfile(mgrfilename, ios::in);
	printf("unionsize:%d, srcsize:%d, orgsize:%d\n",srcfile_set.size()+orgfile_set.size(), srcfile_set.size(), orgfile_set.size());
	size_t union_size = srcfile_set.size()+orgfile_set.size();
	vector<string> src_org_set(union_size);
	vector<string>::iterator src_org_set_iter;
	if ( mgrfilename != 0 )
	{
		set_union(srcfile_set.begin(), srcfile_set.end(), orgfile_set.begin(), orgfile_set.end(), src_org_set.begin());
		if (!mgrfile)
		{
			mgrfile.open(mgrfilename, ios_base::out); 
		}
		else //ok, file exists; close and reopen in write mode
		{
			mgrfile.close();
			mgrfile.open(mgrfilename, ios_base::out);
		};

		for ( src_org_set_iter = src_org_set.begin(); src_org_set_iter != src_org_set.end(); ++src_org_set_iter)
		{
			if(src_org_set_iter->empty() == true ) continue;
			mgrfile << *src_org_set_iter << endl;
		};
		mgrfile.close();
	};

	return 0;
}
