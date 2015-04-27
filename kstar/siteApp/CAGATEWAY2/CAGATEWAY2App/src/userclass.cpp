#include "userclass.h"

IfDriver::IfDriver()
{
	printf("moldbytein:%lu, moldbyteout:%lu, mbytein:%lu, mbyteout:%lu\n", 
			moldbytein, moldbyteout, mbytein, mbyteout);
	meth1 = 0;
	meth2 = 0;
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

int IfDriver::DiffKByteCalc( double &Eth0Bytein, double &Eth0Byteout,
				double &Eth1Bytein, double &Eth1Byteout, double &Eth2Bytein, double &Eth2Byteout, const int scale)
{
	Eth0Bytein = (float)(mbytein - moldbytein)/(1024*scale);
	Eth0Byteout = (float)(mbyteout - moldbyteout)/(1024*scale);
	Eth1Bytein = (float)(meth1bytein - meth1oldbytein)/(1024*scale);
	Eth1Byteout = (float)(meth1byteout - meth1oldbyteout)/(1024*scale);
	Eth2Bytein = (float)(meth2bytein - meth2oldbytein)/(1024*scale);
	Eth2Byteout = (float)(meth2byteout - meth2oldbyteout)/(1024*scale);
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
		}
		else if(strcmp(iface, "eth2") == 0 )
		{
			meth2 = 1;
			if (meth2byteout > bytesout || meth2bytein > bytesin) {
				meth2bytein = bytesin;
				meth2byteout= bytesout;
			} else {
				meth2oldbytein = meth2bytein;
				meth2oldbyteout= meth2byteout;
			};
			meth2bytein = bytesin;
			meth2byteout= bytesout;
		}
	};
	monce = 1;
	fileclose();
	return 1;
badproc:
	fileclose();
	ifdriver_error("%s: unsupported format.", file);
	return 0;
};
