#include <stdio.h>

#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define BLK         32

void mat_mul_tiling(float *M_A, float *M_B, float *M_C,
                 size_t ROW_A, size_t COL_A, size_t COL_B) {
  float (*A)[COL_A] = (float (*)[COL_A])M_A;
  float (*B)[COL_B] = (float (*)[COL_B])M_B;
  float (*C)[COL_B] = (float (*)[COL_B])M_C;

  // C = AB
  int i, j, k, jj, kk;
  for (jj = 0; jj < COL_B; jj += BLK) {
    for (kk = 0; kk < COL_A; kk += BLK) {
      for (i = 0; i < ROW_A; i++) {
        for (j = jj; j < MIN(jj + BLK, COL_B); j++) {
          for (k = kk; k < MIN(kk + BLK, COL_A); k++) {
            C[i][j] += A[i][k] * B[k][j];
          }
        }
      }
    }
  }
}
