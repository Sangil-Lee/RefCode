#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <CL/cl.h>
#include "cl_util.h"
#include "timers.h"


void mat_mul_opencl_1d(float *M_A, float *M_B, float *M_C,
                       size_t ROW_A, size_t COL_A, size_t COL_B) {
  cl_platform_id   *platform;
  cl_device_type   dev_type = CL_DEVICE_TYPE_DEFAULT;
  cl_device_id     dev;
  cl_context       context;
  cl_command_queue cmd_queue;
  cl_program       program;
  cl_kernel        kernel;
  cl_mem           mem_A, mem_B, mem_C;
  cl_int           err;
  cl_uint          num_platforms;
  cl_uint          num_dev = 0;

  int i;

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
    err = clGetDeviceIDs(platform[i], dev_type, 1, &dev, &num_dev);
    if (err != CL_DEVICE_NOT_FOUND) CHECK_ERROR(err);
    if (num_dev == 1) break;
  }
  if (num_dev < 1) {
    fprintf(stderr, "[%s:%d] ERROR: No device\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }
  print_device_name(dev);

  // Context
  context = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
  CHECK_ERROR(err);

  // Command queue
  cmd_queue = clCreateCommandQueue(context, dev, 0, &err);
  CHECK_ERROR(err);

  // Create a program.
  size_t source_len;
  char *source_code = get_source_code("./kernel_1d.cl", &source_len);
  program = clCreateProgramWithSource(context,
                                      1,
                                      (const char **)&source_code,
                                      &source_len,
                                      &err);
  free(source_code);
  CHECK_ERROR(err);

  // Build the program.
  char build_opts[200];
  sprintf(build_opts, "-DROW_A=%lu -DCOL_A=%lu -DCOL_B=%lu",
          ROW_A, COL_A, COL_B);
  err = clBuildProgram(program, 1, &dev, build_opts, NULL, NULL);
  if (err != CL_SUCCESS) {
    print_build_log(program, dev);
    CHECK_ERROR(err);
  }

  // Kernel
  kernel = clCreateKernel(program, "mat_mul", &err);
  CHECK_ERROR(err);

  // Buffers
  mem_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                         sizeof(float) * ROW_A * COL_A,
                         M_A, &err);
  CHECK_ERROR(err);

  mem_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                         sizeof(float) * COL_A * COL_B,
                         M_B, &err);
  CHECK_ERROR(err);

  mem_C = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                         sizeof(float) * ROW_A * COL_B,
                         M_C, &err);
  CHECK_ERROR(err);

  // Set the arguments.
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_A);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_B);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_C);
  CHECK_ERROR(err);

  // Enqueue the kernel.
  size_t lws[1] = {256};
  size_t gws[1];
  gws[0] = (size_t)ceil((double)ROW_A / lws[0]) * lws[0];
  timer_start(1);
  err = clEnqueueNDRangeKernel(cmd_queue,
                               kernel,
                               1, NULL,
                               gws, lws,
                               0, NULL, NULL);
  CHECK_ERROR(err);

  // Read the result.
  err = clEnqueueReadBuffer(cmd_queue,
                            mem_C,
                            CL_TRUE, 0,
                            sizeof(float) * ROW_A * COL_B, 
                            M_C,
                            0, NULL, NULL);
  CHECK_ERROR(err);
  timer_stop(1);
  printf("Kernel time : %f sec\n", timer_read(1));

  // Release
  clReleaseMemObject(mem_A);
  clReleaseMemObject(mem_B);
  clReleaseMemObject(mem_C);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(cmd_queue);
  clReleaseContext(context);
  free(platform);
}


