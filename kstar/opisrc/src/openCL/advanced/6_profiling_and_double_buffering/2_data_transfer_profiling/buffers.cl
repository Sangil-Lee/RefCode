__kernel void buffers(__global float* A, __global float* B, __global float* C)
{
  int i;
  i = get_global_id(0);

  C[i] = A[i] + B[i];
}
