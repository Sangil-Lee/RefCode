#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <curses.h>



#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>



#include "../driver/ioctl_intLTU.h"

#include "../driver/intLTU_Def.h"

#define LEV4LSB(x) ((x) & 0xf )
#define SFT4MSK4(x) (((x) >> 4) & 0xf )
#define SFT8MSK4(x) (((x) >> 8) & 0xf )
#define SFT12MSK4(x) (((x) >> 12) & 0xf )

#define LEV8LSB(x) ((x) & 0xff )
#define SFT8MSK8(x) (((x) >> 8) & 0xff )
#define SFT16MSK8(x) (((x) >> 16) & 0xff )
#define SFT24MSK8(x) (((x) >> 24) & 0xff )
#define SWAP16(x)	( (LEV8LSB(x) << 8) | SFT8MSK8(x) )
#define SWAP32(x)	(  (SFT24MSK8(x)) | (SFT16MSK8(x)<<8) | (SFT8MSK8(x)<<16) | ( LEV8LSB(x)<<24 ) ) 

#define HSWAP32(x)	( ( ((x) >> 16) & 0xffff) | (((x) << 16) & 0xffff0000 ))


int loop = 1;

char *base0;


#if 1

int  con_getch(void)
{
  int i;
  
  while( ( i = getch() ) == ERR );
  return i;

}


char con_kbhit()
{
  int i, g;  
  nodelay(stdscr, TRUE);
  i = ( ( g = getc(stdin) ) == ERR ? 0 : g );
  nodelay(stdscr, FALSE);

  return i;   
}
#endif


void show_all()
{
	int i;
	unsigned int nval;

	for( i=0; i<100; i++) {
		nval = READ32(base0 + (i*4));
		printf("0x%x:   0x%x\n", (i*4), (nval));
	}

}

void show_reg()
{
	unsigned int nval=0, i;
	nval = READ32(base0 + 0x0);
	printf("0x0:   0x%x\n", (nval));

	nval = READ32(base0 + 0x4);
	printf("0x4:   0x%x\n", (nval));

	nval = READ32(base0 + 0x8);
	printf("0x8:   0x%x\n", (nval));

	nval = READ32(base0 + 0xc);
	printf("0xc:   0x%x\n", (nval));

	nval = READ32(base0 + 0x14);
	printf("0x14:   0x%x\n", (nval));

	nval = READ32(base0 + 0x1c);
	printf("0x1c:   0x%x\n", (nval));

	nval = READ32(base0 + 0x20);
	printf("0x20:   0x%x\n", (nval));

	nval = READ32(base0 + 0x64);
	printf("0x64:	0x%x\n", (nval));

	nval = READ32(base0 + 0x68);
	printf("0x68:   0x%x\n", (nval));

	nval = READ32(base0 + 0x6c);
	printf("0x6c:   0x%x\n", (nval));

}

int main(int argc, char **argv)
{
	int fd;
	char key;
	unsigned int nval=0, i;
	int status;
	
	CLTU2_PORT_CONFIG  cfg;

	printf("hellow world!\n");

	fd = open("/dev/intLTU.0", O_RDWR|O_NDELAY);
	if(fd < 0)
	{
		perror("/dev/intLTU.0 open error!");
		exit(1);
	}
	
	base0 = (char *)mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( base0 == 0x0) {
		perror("mmap() error!");
		exit(1);
	}

	nval = READ32(base0 + 0x0);
	printf("mmap offset 0x0:   0x%x, addr: 0x%x\n", nval, &base0);

/*
	nval=0;
	status = ioctl(fd, IOCTL_CLTU_READ, &nval);
	printf("ioctl offset 0x0:   0x%x\n", nval);


	nval = READ32(base0 + 0x0);
	printf("mmap offset 0x0:   0x%x\n", nval);

	nval = READ32(base0 + 0x4);
	printf("mmap offset 0x4:   0x%x\n", nval);

	nval = READ32(base0 + 0x8);
	printf("mmap offset 0x8:   0x%x\n", nval);
*/
	show_reg();
/*
	nval = 0x12345678;
	printf("write value: 0x%x , offset = 0\n", nval);
	WRITE32(base0 + 0x0, nval);

	printf("read this...\n");
	nval = READ32(base0 + 0x0);
	printf("mmap offset 0x0:   0x%x\n", nval);
*/
#if 0
	nval=123;
	status = ioctl(fd, IOCTL_CLTU_INTERRUPT_ENABLE, &nval);
	printf("ioctl INT enable:   %d\n", nval);
#endif

	while (loop) {
/*		key = toupper(con_getch()) ; */
/*		if( con_kbhit() ) 
		{
*/
/*		printf("key info.\n");
		printf("	's': show info.\n");
		printf("	'c': Int down. (1C)\n");
*/
		key = getc(stdin);
		switch ( key) {
			case 's':
				show_reg();
				break;
			case 'S':
				show_all();
				break;
			case 'q':
			case 'Q': 
				loop = 0;
				break;
			case '1':
				printf("press 1,  0x1\n");  /* Trigger #1 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x1 ) nval &= ~0x1;
				else nval |= 0x1;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case '2':
				printf("press 2,   0x2\n"); /* CLK #1 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x2 ) nval &= ~0x2;
				else nval |= 0x2;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case '3':
				printf("press 3,   0x4\n"); /* Trigger #2 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x4 ) nval &= ~0x4;
				else nval |= 0x4;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case '4':
				printf("press 4,   0x8\n"); /* CLK #2 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x8 ) nval &= ~0x8;
				else nval |= 0x8;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case '5':
				printf("press 5,   0x10\n");  /* Trigger #3 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x10 ) nval &= ~0x10;
				else nval |= 0x10;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case '6':
				printf("press 6,  0x20\n");  /* CLK #3 */
				nval = READ32(base0 + 0x4);
				if( nval & 0x20 ) nval &= ~0x20;
				else nval |= 0x20;
				WRITE32(base0 + 0x4, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			
			case 'c':
			case 'C':
				nval = READ32(base0 + 0x1c);
				if( nval & 0x8 ) nval &= ~0x8;
				else nval |= 0x8;
				nval = 0xffffffff;
				WRITE32(base0 + 0x1c, nval);
				printf("wirte value: 0x%x \n", nval);
				break;
			case 'e':
			case 'E':
/*
				nval = 0xffffffff;
				WRITE32(base0 + 0x68, nval);
				printf("wirte value: 0x%x \n", nval);
*/
#if 1
				nval=123;
				status = ioctl(fd, IOCTL_CLTU_INTERRUPT_ENABLE, &nval);
				printf("ioctl INT enable:   %d\n", nval);
#endif	
				break;
			case 'd':
			case 'D':
/*				nval = 0x0;
				WRITE32(base0 + 0x68, nval);
				printf("wirte value: 0x%x \n", nval);
*/
#if 1
				nval=456;
				status = ioctl(fd, IOCTL_CLTU_INTERRUPT_DISABLE, &nval);
				printf("ioctl INT disable:	 %d\n", nval);
#endif
				break;
			default: break;
                   	}


		usleep(1000);

	}
	
	
	munmap( base0, 0x1000);

#if 0
	nval=456;
	status = ioctl(fd, IOCTL_CLTU_INTERRUPT_DISABLE, &nval);
	printf("ioctl INT disable:   %d\n", nval);
#endif

	close(fd);
	return 0;
}

