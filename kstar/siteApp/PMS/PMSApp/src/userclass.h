#ifndef _USER_SOFTIOC_CLASS_H
#define _USER_SOFTIOC_CLASS_H
#include <cstdarg>
#include <cstring>
#include <errno.h>
#include <string>

using std::string;
//static int NetworkDebug = 0;
//epicsExportAddress(int, NetworkDebug); /*iocsh export variable*/

class IfDriver
{
public:
	IfDriver();
	virtual ~IfDriver();
	int IfGetstats();
	void PrintByte();
	float DiffKByteIn(const int scale=1);
	float DiffKByteOut(const int scale=1);
	int DiffKByteCalc( double &Bytein, double &Byteout,const int scale=1);
	int DiffKByteCalc( double &Eth0Bytein, double &Eth0Byteout,
				double &Eth1Bytein, double &Eth1Byteout, const int scale=1);
	int GetOnce(){return monce;};
	int IsEth1(){return meth1;};
private:
	FILE *mfile;
	unsigned long moldbyteout, moldbytein,meth1oldbyteout,meth1oldbytein;
	unsigned long mbyteout, mbytein, meth1bytein, meth1byteout;
	int monce;
	int meth1;

	void ifdriver_error(char *format, ...) {
		va_list ap;
		fprintf(stderr, "%s: ", "ifdriver");
		va_start(ap, format);
		vfprintf(stderr, format, ap);
		va_end(ap);
		putc('\n', stderr);
	};
	void ifdriver_perror(char *func) {
		ifdriver_error("%s: %s", func, strerror(errno));
	};

	int fileopen(const char* file) {
		if ((mfile = fopen(file, "r")) == NULL) 
		{
			ifdriver_error("can't open %s: %s", file, strerror(errno));
			return -1;
		};
		return 1;
	};
	void fileclose() {
		if(mfile!=NULL) fclose(mfile);
	};

};
#if 0
IfDriver::IfDriver()
{
	printf("moldbytein:%lu, moldbyteout:%lu, mbytein:%lu, mbyteout:%lu\n", 
			moldbytein, moldbyteout, mbytein, mbyteout);
};
IfDriver::~IfDriver()
{
	fileclose();
};
void IfDriver::PrintByte()
{
	printf("moldbytein:%lu, moldbyteout:%lu, mbytein:%lu, mbyteout:%lu\n", 
			moldbytein, moldbyteout, mbytein, mbyteout);
};

float IfDriver::DiffKByteOut(const int scale)
{
	return ((float)(mbyteout - moldbyteout)/(1024*scale));
}
float IfDriver::DiffKByteIn(const int scale)
{
	return ((float)(mbytein - moldbytein)/(1024*scale));
}
int IfDriver::DiffKByteCalc( double &Bytein, double &Byteout, const int scale)
{
	Bytein = (float)(mbytein - moldbytein)/(1024*scale);
	Byteout = (float)(mbyteout - moldbyteout)/(1024*scale);
	return (1);
}
int IfDriver::DiffKByteCalc( double &Eth0Bytein, double &Eth0Byteout,
				double &Eth1Bytein, double &Eth1Byteout, const int scale)
{
	Eth0Bytein = (float)(mbytein - moldbytein)/(1024*scale);
	Eth0Byteout = (float)(mbyteout - moldbyteout)/(1024*scale);
	Eth1Bytein = (float)(meth1bytein - meth1oldbytein)/(1024*scale);
	Eth1Byteout = (float)(meth1byteout - meth1oldbyteout)/(1024*scale);
	return (1);
}

int IfDriver::IfGetstats() 
{
	char buf[1024];
	char *iface, *stats;
	unsigned long bytesin, bytesout;
	char *file = "/proc/net/dev";

	fileopen(file);
	/* check first lines */
	if (fgets(buf, sizeof(buf), mfile) == NULL) goto badproc;
	if (strncmp(buf, "Inter-|", 7))				goto badproc;
	if (fgets(buf, sizeof(buf), mfile) == NULL) goto badproc;

	while (fgets(buf, sizeof(buf), mfile) != NULL) 
	{
		if ((stats = strchr(buf, ':')) == NULL) continue;
		*stats++ = '\0';
		iface = buf;
		while (*iface == ' ') iface++;
		if (*iface == '\0') continue;
		if (sscanf(stats, "%lu %*u %*u %*u %*u %*u %*u %*u %lu %*u", &bytesin, &bytesout) != 2)
			continue;
		if(strcmp(iface, "eth0") == 0 )
		{
			//printf("Iterface:%s, bytesin:%lu, bytesout:%lu\n",iface, bytesin, bytesout);
			if( mbyteout > bytesout || mbytein > bytesin) {
				moldbytein = bytesin;
				moldbyteout= bytesout;
			} else {
				moldbytein = mbytein;
				moldbyteout= mbyteout;
			};
			mbytein = bytesin;
			mbyteout= bytesout;
		}
		else if(strcmp(iface, "eth1") == 0 )
		{
			meth1 = 1;
			if (meth1byteout > bytesout || meth1bytein > bytesin) {
				meth1bytein = bytesin;
				meth1byteout= bytesout;
			} else {
				meth1oldbytein = meth1bytein;
				meth1oldbyteout= meth1byteout;
			};
			meth1bytein = bytesin;
			meth1byteout= bytesout;
		};
	};
	monce = 1;
	fileclose();
	return 1;
badproc:
	fileclose();
	ifdriver_error("%s: unsupported format.", file);
	return 0;
};
#endif
#endif
