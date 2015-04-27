__kernel void mat_mul(__global float *M_A,
                      __global float *M_B,
                      __global float *M_C) {
  __global float (*A)[COL_A] = (__global float (*)[COL_A])M_A;
  __global float (*B)[COL_B] = (__global float (*)[COL_B])M_B;
  __global float (*C)[COL_B] = (__global float (*)[COL_B])M_C;

  int i, j, k;
  i = get_global_id(1);
  j = get_global_id(0);
  if (i >= ROW_A || j >= COL_B) return;

  for (k = 0; k < COL_A; k += 4) {
    float4 v_A = (float4)(A[i][k], A[i][k+1], A[i][k+2], A[i][k+3]);
    float4 v_B = (float4)(B[k][j], B[k+1][j], B[k+2][j], B[k+3][j]);
    C[i][j] += dot(v_A, v_B);
  }
}

