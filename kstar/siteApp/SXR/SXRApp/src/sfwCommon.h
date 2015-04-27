#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H


#ifdef __cplusplus
extern "C" {
#endif


#define LEV8LSB(x) ((x) & 0xff )
#define SFT8MSB(x) (((x) >> 8) & 0xff )
#define SFT16MSB(x) (((x) >> 16) & 0xff )
#define SFT24MSB(x) (((x) >> 24) & 0xff )
#define SWAP16(x)	( (LEV8LSB(x) << 8) | SFT8MSB(x) )
#define SWAP32(x)	(  (SFT24MSB(x)) | (SFT16MSB(x)<<8) | (SFT8MSB(x)<<16) | ( LEV8LSB(x)<<24 ) ) 


int wf_print_date();
void printlog(const char *fmt, ... );

unsigned long long int wf_getCurrentUsec(void);
double                 wf_intervalUSec(unsigned long long int start, unsigned long long int stop);





#ifdef __cplusplus
}
#endif

#endif

