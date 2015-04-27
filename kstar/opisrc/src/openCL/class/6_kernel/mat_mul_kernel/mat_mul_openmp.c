#include <stdio.h>
#include <omp.h>

void mat_mul_openmp(float *M_A, float *M_B, float *M_C,
                    size_t ROW_A, size_t COL_A, size_t COL_B) {
  float (*A)[COL_A] = (float (*)[COL_A])M_A;
  float (*B)[COL_B] = (float (*)[COL_B])M_B;
  float (*C)[COL_B] = (float (*)[COL_B])M_C;

  // C = AB
  int i, j, k;

  #pragma omp parallel default(shared) private(i,j,k)
  {
    #pragma omp master
    printf("# of threads: %d\n", omp_get_num_threads());

    #pragma omp for
    for (i = 0; i < ROW_A; i++) {
      for (j = 0; j < COL_B; j++) {
        for (k = 0; k < COL_A; k++) {
          C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
  }
}

