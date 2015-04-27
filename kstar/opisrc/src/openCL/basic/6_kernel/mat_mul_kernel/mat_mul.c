#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mat_mul.h"
#include "timers.h"

/////////////////////////////////////////////////////////////////////////////
// Matrices
/////////////////////////////////////////////////////////////////////////////
static float *M_A = NULL;
static float *M_B = NULL;
static float *M_C = NULL;

static size_t ROW_A = 1000;
static size_t COL_A = 1000;
static size_t ROW_B = 1000;
static size_t COL_B = 1000;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
static void initialize();
static void release();
static void verify();
static void print_options();
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Options
// NOTE: Keep the sequence between 'option_enum' and 'option_name'.
/////////////////////////////////////////////////////////////////////////////
enum option_enum {
  OPT_SEQ = 0,
  OPT_OMP,
  OPT_OCL_1D,
  OPT_OCL_2D,
  OPT_OCL_PRINTF,
  OPT_OCL_VECTOR,
  OPT_LAST
};

const char *option_desc[] = {
  "Sequential",
  "OpenMP",
  "OpenCL - 1D kernel",
  "OpenCL - 2D kernel",
  "OpenCL - printf",
  "OpenCL - vectorization",
};
/////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {
  // Check the sanity of options.
  if (sizeof(option_desc) / sizeof(char *) != OPT_LAST) {
    fprintf(stderr, "ERROR: check the option list! (%lu != %d)\n",
            sizeof(option_desc) / sizeof(char), OPT_LAST);
    exit(EXIT_FAILURE);
  }

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <option> [-v]\n\n", argv[0]);
    print_options();
    exit(EXIT_FAILURE);
  }

  int option = atoi(argv[1]);

  initialize();

  printf("= Matrix Multiplication =\n");
  printf("C[%lu][%lu] = A[%lu][%lu] x B[%lu][%lu]\n",
      ROW_A, COL_B, ROW_A, COL_A, ROW_B, COL_B);

  if (option < 0 || option >= OPT_LAST) {
    fprintf(stderr, "%d: unknown option\n", option);
  } else {
    printf("%s\n", option_desc[option]);
  }

  timer_start(0);
  switch (option) {
    case OPT_SEQ: 
      mat_mul_seq(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    case OPT_OMP: 
      mat_mul_openmp(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    case OPT_OCL_1D: 
      mat_mul_opencl_1d(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    case OPT_OCL_2D: 
      mat_mul_opencl_2d(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    case OPT_OCL_PRINTF: 
      mat_mul_opencl_printf(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    case OPT_OCL_VECTOR: 
      mat_mul_opencl_vector(M_A, M_B, M_C, ROW_A, COL_A, COL_B);
      break;

    default: break;
  }
  timer_stop(0);
  printf("Elapsed time: %f sec\n", timer_read(0));

  if (argc >= 3 && strcmp(argv[2], "-v") == 0) {
    verify();
  }

  release();

  return EXIT_SUCCESS;
}


void initialize() {
  int i, j;

  // The matrix sizes are read from the file.
  const char *file_name = "matrix.txt";
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    fprintf(stderr, "%s does not exist. The default size (1000 x 1000) is used.\n", file_name);
  } else {
    char buf[128];

    if (fgets(buf, sizeof(buf), file) != NULL) {
      ROW_A = (size_t)atol(buf);
    }
    if (fgets(buf, sizeof(buf), file) != NULL) {
      ROW_B = COL_A = (size_t)atol(buf);
    }
    if (fgets(buf, sizeof(buf), file) != NULL) {
      COL_B = (size_t)atol(buf);
    }

    fclose(file);
  }

  M_A = (float *)calloc(ROW_A * COL_A, sizeof(float));
  M_B = (float *)calloc(ROW_B * COL_B, sizeof(float));
  M_C = (float *)calloc(ROW_A * COL_B, sizeof(float));

  srand(0);

  float (*A)[COL_A] = (float (*)[COL_A])M_A;
  for (i = 0; i < ROW_A; i++) {
    for (j = 0; j < COL_A; j++) {
      A[i][j] = (float)(rand() % 1000) / 100.0f;
    }
  }

  float (*B)[COL_B] = (float (*)[COL_B])M_B;
  for (i = 0; i < ROW_B; i++) {
    for (j = 0; j < COL_B; j++) {
      B[i][j] = (float)(rand() % 1000) / 100.0f;
    }
  }
}


void release() {
  if (M_A) free(M_A);
  if (M_B) free(M_B);
  if (M_C) free(M_C);
}


void verify() {
  int i, j, k;
  float sum;
  float (*A)[COL_A] = (float (*)[COL_A])M_A;
  float (*B)[COL_B] = (float (*)[COL_B])M_B;
  float (*C)[COL_B] = (float (*)[COL_B])M_C;

  for (i = 0; i < ROW_A; i++) {
    for (j = 0; j < COL_B; j++) {
      sum = 0.0;
      for (k = 0; k < COL_A; k++) {
        sum += A[i][k] * B[k][j];
      }
      if (fabsf(C[i][j] - sum) > 0.1) {
        fprintf(stderr, "Verification: FAIL (C[%d][%d]: %f vs. %f)\n",
                i, j, C[i][j], sum);
        return;
      }
    }
  }

  printf("Verification: SUCCESS\n");
}


void print_options() {
  int i;

  printf("== Option List ==\n");
  for (i = 0; i < OPT_LAST; i++) {
    printf("%2d: %s\n", i, option_desc[i]);
  }
}

