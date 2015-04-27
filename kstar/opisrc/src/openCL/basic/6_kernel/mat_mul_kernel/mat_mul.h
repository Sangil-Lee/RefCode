#ifndef __MAT_MUL_H__
#define __MAT_MUL_H__

void mat_mul_seq(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
void mat_mul_openmp(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
void mat_mul_opencl_1d(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
void mat_mul_opencl_2d(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
void mat_mul_opencl_printf(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
void mat_mul_opencl_vector(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B);
#endif //__MAT_MUL_H__
