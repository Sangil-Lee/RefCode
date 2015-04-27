__kernel void mat_mul(__global float* C, __global float* A, __global float* B)
{
  int i = get_global_id(0);
  int j = get_global_id(1);
  int k;
  float acc = 0.0;

  for ( k = 0; k < COL_A; k++) {
    acc += A[j * COL_A + k] * B[k * COL_B + i];
  }
  C[j * COL_B + i] = acc;
}

