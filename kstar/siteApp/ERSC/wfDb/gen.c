#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>


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

#define PRINT_ERR()     printf("file:%s line:%d ERROR!!\n",__FILE__,__LINE__)

#ifndef MIN
#define MIN(x,y)  (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)  (((x) < (y)) ? (y) : (x))
#endif

#ifndef _PI_
#define _PI_	3.141592653589793
#endif


/* variables	**************************************************************/

static	int		isTarget		= 1;

static int _parse_args(int argc, char** argv)
{
	int		errs	= 0;
	int		i;

	printf("USAGE: conv \"file name\" <-r> <-d>\n");
	printf("   -r   PCS target reference file.\n");
	printf("   -d   MMWI raw data file\n");

	for (i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-r") == 0)
		{
			isTarget	= 1;
			continue;
		}

		if (strcmp(argv[i], "-d") == 0)
		{
			isTarget	= 0;
			continue;
		}

		if (isdigit(argv[i][0]) == 0)
		{
			errs	= 1;
			printf("ERROR: invalid argument: %s\n", argv[i]);
			break;
		}

				
		errs	= 1;
		printf("ERROR: invalid argument: %s\n", argv[i]);
		break;
	}

	return(errs);
}


int main(int argc, char **argv)
{
	FILE *fp, *fp1;
	char bufLineScan[512];
	double dTime1, dTime2, vltg1, vltg2;
	unsigned short ch1, ch2;
	char fileName[256];
	char targetName[32];
	int len;
	char c;
	int		errs, i;
	int isFirst = 1;
	short s16Val, temp16;
	int s32Val;
	double f64Val, rad, step;
	float f32Val;



	if( argc < 2 ) {
		printf("Need file name!\n");
		exit(1);
	}
//	sscanf(&argv[1], "%s", fileName);
	sprintf(fileName, argv[1]);


	printf("WF file: '%s' \n", fileName);

//	errs	= _parse_args(argc, argv);


	fp = fopen(fileName, "wb");

	s16Val = -32767;
	s32Val = -32767;

	while( s32Val < 32767 )
	{
//		s16Val = ( s16Val < 0 ) ? 32767 : -32767;
//		temp16 = SWAP16(s16Val);
		temp16 = s16Val;
		fwrite( &temp16, sizeof(short), 1, fp);
		s16Val += 35;
		s32Val += 35;
	}
	
	fclose(fp);

#if 1
/*********************************************/
	fp = fopen("square.dat", "wb");
	fp1 = fopen("square_s.dat", "wb");
	for( i=0; i<500; i++) {
		s16Val = 0x7fff;
		fwrite( &s16Val, sizeof(short), 1, fp);	
		s16Val = SWAP16(s16Val);
		fwrite( &s16Val, sizeof(short), 1, fp1);	
	}

        for( i=0; i<500; i++) {
/*		s16Val = 0x8000; */
		s16Val = 0x0000; /* to make DC component */
		fwrite( &s16Val, sizeof(short), 1, fp);
		s16Val = SWAP16(s16Val);
		fwrite( &s16Val, sizeof(short), 1, fp1);	
      	}
	fclose(fp);
	fclose(fp1);
/*********************************************/
#endif


	/*********************************************/
		fp = fopen("sine.dat", "wb");
		fp1 = fopen("sine_s.dat", "wb");
		step = _PI_ / 400.;
		for( rad=0; rad<(2*_PI_); rad += step ) {
			f64Val = sin(rad);
			s16Val = (short)(32767. * f64Val);
			fwrite( &s16Val, sizeof(short), 1, fp);
			s16Val = SWAP16(s16Val);
			fwrite( &s16Val, sizeof(short), 1, fp1);
		}	
		fclose(fp);
		fclose(fp1);
	/*********************************************/




	return 0;
	
}



