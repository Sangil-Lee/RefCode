#ifdef cl_khr_printf
#pragma OPENCL EXTENSION cl_khr_printf: enable
#else
#ifdef cl_amd_printf
#pragma OPENCL EXTENSION cl_amd_printf: enable
#endif
#endif

__kernel void mat_mul(__global float *M_A,
                      __global float *M_B,
                      __global float *M_C) {
  __global float (*A)[COL_A] = (__global float (*)[COL_A])M_A;
  __global float (*B)[COL_B] = (__global float (*)[COL_B])M_B;
  __global float (*C)[COL_B] = (__global float (*)[COL_B])M_C;

  int i, j, k;
  i = get_global_id(1);
  j = get_global_id(0);

  if (i == 0 && j == 0) {
    printf("------------------------------\n");
    printf("work_dim     : %u\n", get_work_dim());
    printf("global_size  : (%lu, %lu)\n", get_global_size(0), get_global_size(1));
    printf("global_id    : (%lu, %lu)\n", get_global_id(0), get_global_id(1));
    printf("local_size   : (%lu, %lu)\n", get_local_size(0), get_local_size(1));
    printf("local_id     : (%lu, %lu)\n", get_local_id(0), get_local_id(1));
    printf("num_groups   : (%lu, %lu)\n", get_num_groups(0), get_num_groups(1));
    printf("group_id     : (%lu, %lu)\n", get_group_id(0), get_group_id(1));
    printf("global_offset: (%lu, %lu)\n", get_global_offset(0), get_global_offset(1));
    printf("------------------------------\n");
  }

  if (i >= ROW_A || j >= COL_B) return;

  for (k = 0; k < COL_A; k++) {
    C[i][j] += A[i][k] * B[k][j];
  }
}

