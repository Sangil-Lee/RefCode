#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "../driver/ioctl_cltu.h"

#include "../src/cltu_Def.h"


#define READ16(port)  (*((volatile unsigned short int*)(port)))
#define READ32(port)  (*((volatile unsigned int*)(port)))
#define WRITE16( port, val )  (*((volatile unsigned short int*)(port))=((unsigned short int)(val)))
#define WRITE32( port, val )  (*((volatile unsigned int*)(port))=((unsigned int)(val)))


int main(int argc, char **argv)
{
	int fd;
	unsigned int nval=0, i;
	int status;
	char *base0;
	CLTU2_PORT_CONFIG  cfg;

	printf("hellow world!\n");

	fd = open("/dev/tss/cltu.0", O_RDWR|O_NDELAY);
	if(fd < 0)
	{
		perror("/dev/tss/cltu.0 open error!");
		exit(1);
	}

	base0 = (char *)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( base0 == 0x0) {
		perror("mmap() error!");
		exit(1);
	}

	nval = READ32(base0 + 0x0);
	printf("read mmap offset 0x0:   0x%x, addr: 0x%x\n", nval, &base0);

	nval=0;
	status = ioctl(fd, IOCTL_XTU_READ, &nval);
	if (status == -1)	printf("ioctl, IOCTL_XTU_READ, error\n");
	printf("ioctl status: %d,  first read  offset 0x0:   0x%x\n", status, nval);
#if 1
	nval=5;
	status = ioctl(fd, IOCTL_XTU_SET_ENDIAN, &nval);
	if (status == -1) printf("ioctl, IOCTL_XTU_SET_ENDIAN, error\n");
	printf("ioctl status: %d,  set endian\n", status);
#endif

	nval=0xabcd1234;
	status = ioctl(fd, IOCTL_XTU_WRITE, &nval);
	if (status == -1) printf("ioctl, IOCTL_XTU_WRITE, error\n");
	printf("ioctl status: %d,  user write value:   0x%x\n", status, nval);

	nval=0;
	status = ioctl(fd, IOCTL_XTU_READ, &nval);
	if (status == -1) printf("ioctl, IOCTL_XTU_READ, error\n");
	printf("ioctl status: %d,  second read  offset 0x0:   0x%x\n", status, nval);

	
	munmap( base0, 0x1000);

	close(fd);
	return 0;
}

