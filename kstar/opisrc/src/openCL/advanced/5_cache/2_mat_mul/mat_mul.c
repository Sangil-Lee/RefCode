#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include "timers.h"

#define NDIM    1024

double a[NDIM][NDIM];
double b[NDIM][NDIM];
double c[NDIM][NDIM];

int print_matrix = 0;
int validation = 0;

int ijk = 1;
int kij = 0;
int jki = 0;

void mat_mul_ijk( double c[NDIM][NDIM], double a[NDIM][NDIM], double b[NDIM][NDIM] )
{
	int i, j, k;
	double sum;
	
	// C = AB
	for( i = 0; i < NDIM; i++ )
	{
		for( j = 0; j < NDIM; j++ )
		{
			sum = 0;
			for( k = 0; k < NDIM; k++ )
			{
				sum += a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}

void mat_mul_kij( double c[NDIM][NDIM], double a[NDIM][NDIM], double b[NDIM][NDIM] )
{
	// TODO:
}

void mat_mul_jki( double c[NDIM][NDIM], double a[NDIM][NDIM], double b[NDIM][NDIM] )
{
	// TODO:
}

void check_mat_mul( double c[NDIM][NDIM], double a[NDIM][NDIM], double b[NDIM][NDIM] )
{
	int i, j, k;
	double sum;
	int validated = 1;

	printf("Validating the result..\n");
	
	// C = AB
	for( i = 0; i < NDIM; i++ )
	{
		for( j = 0; j < NDIM; j++ )
		{
			sum = 0;
			for( k = 0; k < NDIM; k++ )
			{
				sum += a[i][k] * b[k][j];
			}

			if( c[i][j] != sum )
			{
				printf("c[%d][%d] is differ(value=%lf correct_value=%lf)!!\n", i, j, c[i][j], sum );
				validated = 0;
			}
		}
	}

	printf("Validation : ");
	if( validated )
		printf("SUCCESSFUL.\n");
	else
		printf("FAILED.\n");
}

void print_mat( double mat[NDIM][NDIM] )
{
	int i, j;

	for( i = 0; i < NDIM; i++ )
	{
		for( j = 0; j < NDIM; j++ )
		{
			printf("%8.2lf ", mat[i][j]);
		}
		printf("\n");
	}
}

void print_help(const char* prog_name)
{
	printf("Usage: %s [-pvh]\n", prog_name );
	printf("\n");
	printf("OPTIONS\n");
	printf("  -p : print matrix data.\n");
	printf("  -v : validate matrix multiplication.\n");
	printf("  -h : print this page.\n");
	printf("  -i : matrix multiplication (ikj) (default).\n");
	printf("  -k : matrix multiplication (kij).\n");
	printf("  -j : matrix multiplication (jki).\n");
}

void parse_opt(int argc, char** argv)
{
	int opt;

	while( (opt = getopt(argc, argv, "pvhikjs:")) != -1 )
	{
		switch(opt)
		{
		case 'p':
			// print matrix data.
			print_matrix = 1;
			break;

		case 'v':
			// validation
			validation = 1;
			break;

		case 'i':
			ijk = 1;
			kij = 0;
			jki = 0;
			break;

		case 'k':
			kij = 1;
			ijk = 0;
			jki = 0;
			break;
	
		case 'j':
			jki = 1;
			ijk = 0;
			kij = 0;
			break;

		case 'h':
		default:
			print_help(argv[0]);
			exit(0);
			break;
		}
	}
}

int main(int argc, char** argv)
{
	int i, j, k = 1;

	parse_opt( argc, argv );

	for( i = 0; i < NDIM; i++ )
	{
		for( j = 0; j < NDIM; j++ )
		{
			a[i][j] = k;
			b[i][j] = k;
			k++;
		}
	}

	timer_start(1);
	if(ijk == 1) {
		printf("Matirx multiply ijk ... \n");
		mat_mul_ijk( c, a, b );
	}
	else if(kij == 1) {
		printf("Matirx multiply kij ... \n");
		mat_mul_kij( c, a, b );	
	}
	else {
		printf("Matirx multiply jki ... \n");
		mat_mul_jki( c, a, b );	
	}
	timer_stop(1);

	printf("Time elapsed : %lf sec\n", timer_read(1));


	if( validation )
		check_mat_mul( c, a, b );

	if( print_matrix )
	{
		printf("MATRIX A: \n");
		print_mat(a);

		printf("MATRIX B: \n");
		print_mat(b);

		printf("MATRIX C: \n");
		print_mat(c);
	}

	return 0;
}
