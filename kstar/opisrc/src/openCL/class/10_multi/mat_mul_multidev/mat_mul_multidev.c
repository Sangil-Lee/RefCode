#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include <math.h>

#include "cl_util.h"

void mat_mul(float *M_A, float *M_B, float *M_C, int COL_A, int ROW_A,
    int COL_B, int ROW_B, int COL_C, int ROW_C)
{
  cl_platform_id    *platform;
  cl_device_type    dev_type = CL_DEVICE_TYPE_GPU;
  cl_device_id      *devs = NULL;
  cl_context        context;
  cl_command_queue  *cmd_queues;
  cl_program        program;
  cl_kernel         *kernels;
  cl_mem            *mem_A;
  cl_mem            *mem_B;
  cl_mem            *mem_C;
  cl_uint           num_platforms;
  cl_uint           num_devs = 0;
  cl_int            err;
  cl_event          *ev_kernels;

  size_t size_A, size_B, size_C;
  int    i;

  size_A = COL_A * ROW_A * sizeof(float);
  size_B = COL_B * ROW_B * sizeof(float);
  size_C = COL_C * ROW_C * sizeof(float);

  // Get the device type to use from the environmental variable.
  char *dtype = getenv("CL_DEV_TYPE");
  if (dtype) {
    if (strcasecmp(dtype, "cpu") == 0) {
      dev_type = CL_DEVICE_TYPE_CPU;
    } else if (strcasecmp(dtype, "gpu") == 0) {
      dev_type = CL_DEVICE_TYPE_GPU;
    }
  }

  // Platform
  err = clGetPlatformIDs(0, NULL, &num_platforms);
  CHECK_ERROR(err);
  if (num_platforms == 0) {
    fprintf(stderr, "[%s:%d] ERROR: No OpenCL platform\n", __FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }
  printf("Number of platforms: %u\n", num_platforms);
  platform = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
  err = clGetPlatformIDs(num_platforms, platform, NULL);
  CHECK_ERROR(err);

  // Device
  for (i = 0; i < num_platforms; i++) {
    err = clGetDeviceIDs(platform[i], dev_type, 0, NULL, &num_devs);
    if (err != CL_DEVICE_NOT_FOUND) CHECK_ERROR(err);
    if (num_devs >= 1)
    {
      devs = (cl_device_id*)malloc(sizeof(cl_device_id) * num_devs);

      err = clGetDeviceIDs(platform[i], dev_type, num_devs, devs, NULL);
      break;
    }
  }
  if ( devs == NULL || num_devs < 1) {
    fprintf(stderr, "[%s:%d] ERROR: No device\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  for( i = 0; i < num_devs; ++i )
  {
    printf("dev[%d] : ", i);
    print_device_name(devs[i]);
  }

  // Context
  context = clCreateContext(NULL, num_devs, devs, NULL, NULL, &err);
  CHECK_ERROR(err);

  // Command queue
  cmd_queues = (cl_command_queue*)malloc(sizeof(cl_command_queue)*num_devs);
  for( i = 0; i < num_devs; ++i )
  {
    cmd_queues[i] = clCreateCommandQueue(context, devs[i], 0, &err);
    CHECK_ERROR(err);
  }

  // Create a program.
  size_t source_len;
  char *source_code = get_source_code("./mat_mul.cl", &source_len);
  program = clCreateProgramWithSource(context,
                                      1,
                                      (const char **)&source_code,
                                      &source_len,
                                      &err);
  free(source_code);
  CHECK_ERROR(err);

  // Build the program.
  char build_opts[200];
  sprintf(build_opts, "-DCOL_A=%d -DCOL_B=%d", COL_A, COL_B);
  err = clBuildProgram(program, num_devs, devs, build_opts, NULL, NULL);
  if (err != CL_SUCCESS) {
    print_build_log(program, devs[0]);
    CHECK_ERROR(err);
  }

  // Kernel
  kernels = (cl_kernel*)malloc(sizeof(cl_kernel)*num_devs);
  for (i = 0; i < num_devs; i++) {
    kernels[i] = clCreateKernel(program, "mat_mul", NULL);
  }

  // Buffers
  mem_A = (cl_mem*)malloc(sizeof(cl_mem)*num_devs);
  mem_B = (cl_mem*)malloc(sizeof(cl_mem)*num_devs);
  mem_C = (cl_mem*)malloc(sizeof(cl_mem)*num_devs);
  for (i = 0; i < num_devs; i++) {
    mem_A[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_A / num_devs, NULL, NULL);
    mem_B[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_B, NULL, NULL);
    mem_C[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_C / num_devs, NULL, NULL);
  }

  // Write to Buffers 
  for (i = 0; i < num_devs; i++) {
    clEnqueueWriteBuffer(cmd_queues[i], mem_A[i], CL_FALSE, 0, size_A / num_devs, 
        (void*) (((char*) M_A) + (size_A / num_devs) * i), 0, NULL, NULL);
    clEnqueueWriteBuffer(cmd_queues[i], mem_B[i], CL_FALSE, 0, size_B, M_B, 0, NULL, NULL);
  }

  // Set the arguments.
  for (i = 0; i < num_devs; i++) {
    clSetKernelArg(kernels[i], 0, sizeof(cl_mem), (void*) &mem_C[i]);
    clSetKernelArg(kernels[i], 1, sizeof(cl_mem), (void*) &mem_A[i]);
    clSetKernelArg(kernels[i], 2, sizeof(cl_mem), (void*) &mem_B[i]);
  }

  // Enqueue the kernel.
  size_t lws[2] = {16, 16};
  size_t gws[2] = { COL_C, ROW_C/num_devs };

  ev_kernels = (cl_event*)malloc(sizeof(cl_event)*num_devs);
  for (i = 0; i < num_devs; i++) {
    err = clEnqueueNDRangeKernel(cmd_queues[i],
                                 kernels[i],
                                 2, NULL,
                                 gws, lws,
                                 0, NULL, &ev_kernels[i]);
    CHECK_ERROR(err);
  }

  // Read the result.
  for (i = 0; i < num_devs; i++) {
    clEnqueueReadBuffer(cmd_queues[i], mem_C[i], CL_TRUE, 0, size_C / num_devs, 
        (void*) ( ((char*)M_C) + (size_C / num_devs) * i), 1, &ev_kernels[i], NULL);
  }

  // Release
  for( i = 0; i < num_devs; ++i )
  {
    clReleaseMemObject(mem_A[i]);
    clReleaseMemObject(mem_B[i]);
    clReleaseMemObject(mem_C[i]);
    clReleaseKernel(kernels[i]);
    clReleaseCommandQueue(cmd_queues[i]);
    clReleaseEvent(ev_kernels[i]);
  }
  clReleaseProgram(program);
  clReleaseContext(context);
  free(platform);
  free(mem_A);
  free(mem_B);
  free(mem_C);
  free(cmd_queues);
  free(kernels);
  free(devs);
  free(ev_kernels);
}

int main()
{
  float *M_A;
  float *M_B;
  float *M_C;
  float *M_CV;
  int COL_A, ROW_A, COL_B, ROW_B, COL_C, ROW_C;
  size_t size_A, size_B, size_C;
  int i, j, k;

  // Step 1
  COL_A = ROW_A = COL_B = 16 * 64;
  ROW_B = COL_A; COL_C = COL_B; ROW_C = ROW_A;

  size_A = COL_A * ROW_A * sizeof(float);
  size_B = COL_B * ROW_B * sizeof(float);
  size_C = COL_C * ROW_C * sizeof(float);

  M_A = (float*) malloc(size_A);
  M_B = (float*) malloc(size_B);
  M_C = (float*) malloc(size_C);
  M_CV = (float*) malloc(size_C);

  for (i = 0; i < ROW_A; i++) {
    for (j = 0; j < COL_A; j++) {
      M_A[i * COL_A + j] = (float) i;
    }
  }

  for (i = 0; i < ROW_B; i++) {
    for (j = 0; j < COL_B; j++) {
      M_B[i * COL_B + j] = (float) j;
    }
  }

  mat_mul(M_A, M_B, M_C, COL_A, ROW_A, COL_B, ROW_B, COL_C, ROW_C);

  // verification
  int verified = 1;
  for (i = 0; i < COL_B; i++) {
    for (j = 0; j < ROW_A; j++) {
      float acc = 0.0;
      for (k = 0; k < COL_A; k++) {
        acc += M_A[i*COL_A+k] * M_B[k*COL_B+j];
      }
      M_CV[j*COL_C+i] = acc;
    }
  }

  for( i = 0; i < ROW_C; ++i ) {
    for( j = 0; j < COL_C; ++j ) {
      if( M_CV[i*COL_C+j] != M_C[i*COL_C+j] ) {
        printf("M_C[%d*COL_C+%d] = %f. should be %f\n", i, j, M_C[i*COL_C+j], M_CV[i*COL_C+j]);
        verified = 0;
      }
    }
  }

  if( verified )
    printf("VERIFICATION = TRUE.\n");
  else
    printf("VERIFICATION = FALSE.\n");

  free(M_A);
  free(M_B);
  free(M_C);
  free(M_CV);

  return 0;
}
