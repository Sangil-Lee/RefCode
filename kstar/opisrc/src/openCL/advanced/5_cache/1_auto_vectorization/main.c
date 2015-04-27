#include <stdio.h>
#include <unistd.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"


#define EQUAL(x, y) ((x - y) < 0.001 && (y - x) < 0.001)
void addition_ref(float * A, float * B, float * C, int size);

void addition_vec(float *A, float *B, float *C, int size);
float get_pi_ref(float *A, float*B, int size);
float get_pi_vec(float *A, float*B, int size);

static void rand_init(float * buf, int size, int max)
{
	int i;
	for(i = 0; i < size; i++)
		buf[i] = max * ((float)rand()/(float)RAND_MAX);
}

int main(int argc, char * argv[]){

	int i;
	int size = 1048576; 
	int p1size = 65536;
	int p2size = 1048576;
	int dim = 1024;
	int option = 0;

	srand(time(NULL));

	char c;

	timer t;
	timer_init(&t);
	float * A = (float *) valloc (sizeof(float) * size);
	float * B = (float *) valloc (sizeof(float) * size);
	float * C_ref = (float *) valloc (sizeof(float) * size);
	float * C = (float *) valloc (sizeof(float) * size);

	rand_init(A, size, 1);
	rand_init(B, size, 1);
	memset(C_ref, 0, size);
	memset(C, 0, size);

	addition_ref(A,B,C_ref,p1size); //filling the buffer
	
	timer_start(&t);
	for(i = 0; i<10000; i++)
	addition_ref(A,B,C_ref,p1size);
	timer_stop(&t);
	timer_print(&t, "sequential addition");

	addition_ref(A,B,C,p1size); //filling the buffer

	timer_start(&t);
	for(i = 0; i<10000; i++)
	addition_auto(A,B,C,p1size);
	timer_stop(&t);
	timer_print(&t, "auto-vectorized addition");

	int error = -1;
	for(i = 0; i < p1size; i++)
	{
		if(!EQUAL(C[i], C_ref[i])) 
		{
			error = i;
			break;
		}
	}

	if(error == -1)
		printf("vectroized addition passed\n");
	else
		printf("vectorized addition failed, C[%d] = %f != %f %d\n", error, C[error], C_ref[error], C[error] != C_ref[error]);

	printf("\n");



	free(A);
	free(B);
	free(C);
	return 0;				
}
