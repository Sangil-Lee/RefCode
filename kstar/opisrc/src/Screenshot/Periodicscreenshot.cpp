#include <stdexcept>
#include <iostream>

using namespace std;

int main(int const argc, const char ** const argv) 
{
	if (argc != 2)
	{
		printf("Parameter Error\n");
		printf("%s system[TMS,VMS,...]\n",argv[0]);
		return -1;
	};
	// FILE *fp = popen (checkproc.toStdString().c_str(), "r");
	string strsys = string(argv[1]);
	//string command, command1;
	string command;

	if(strcasecmp("VMS", argv[1])==0 )
	{
		command = "import -window root -crop 1280x1024+1280+0 vms.jpg &";
		//command1 = "ncftpput -u kstar -p kstar2007 203.230.119.122 /usr/local/tomcat5/webapps/kstarweb/screenshot /usr/local/opi/screenshot/vms.jpg ";
	}
	else if(strcasecmp("TMS", argv[1])==0 )
	{
		command = "import -window root -crop 1280x1024+0+0 tms.jpg &";
		//command1 = "ncftpput -u kstar -p kstar2007 203.230.119.122 /usr/local/tomcat5/webapps/kstarweb/screenshot /usr/local/opi/screenshot/tms.jpg ";
	}
	//printf("c:%s, c1:%s\n", command.c_str(), command1.c_str());
	printf("c:%s\n", command.c_str());

	string strchk = string("/sbin/pidof ") + strsys;
	FILE *fp = popen (strchk.c_str(), "r");
	char buf[24];
#if 1
	int proc = 0;
	while(true)
	{
		while (fgets (buf, sizeof(buf), fp) ) {};
		proc = atoi(buf);
		printf("%d\n", atoi(buf));
		if(proc != 0)
		{
			printf("c:%s\n", command.c_str());
			system(command.c_str());
			sleep(30);
		}
		else
		{
			sleep(30);
		};
	};
#endif
	pclose(fp);
    return 0;
}
