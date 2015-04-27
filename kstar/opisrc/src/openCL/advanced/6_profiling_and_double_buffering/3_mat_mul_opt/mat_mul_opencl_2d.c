#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
#include "cl_util.h"


void mat_mul_opencl_2d(float *M_A, float *M_B, float *M_C,
                       size_t ROW_A, size_t COL_A, size_t COL_B) {
  cl_platform_id   *platform;
  cl_device_type   dev_type;
  cl_device_id     dev;
  cl_context       context;
  cl_command_queue cmd_queue;
  cl_program       program;
  cl_kernel        kernel;
  cl_mem           mem_A, mem_B, mem_C;
  cl_event         ev_kernel;
  cl_int           err;
  cl_uint          num_platforms;
  cl_uint          num_dev = 0;
  int i;

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
  dev_type = get_device_type();
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
  free(platform);

  // Context
  context = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
  CHECK_ERROR(err);

  // Command queue
  cmd_queue = clCreateCommandQueue(context, dev,
                                   CL_QUEUE_PROFILING_ENABLE,
                                   &err);
  CHECK_ERROR(err);

  // Create a program.
  char *source_code = get_source_code("./kernel_2d.cl");
  program = clCreateProgramWithSource(context,
                                      1, (const char **)&source_code,
                                      NULL, &err);
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

  mem_B = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                         sizeof(float) * COL_A * COL_B,
                         NULL, &err);
  CHECK_ERROR(err);
  err = clEnqueueWriteBuffer(cmd_queue,
                             mem_B,
                             CL_FALSE, 0,
                             sizeof(float) * COL_A * COL_B,
                             M_B,
                             0, NULL, NULL);
  CHECK_ERROR(err)

  mem_C = clCreateBuffer(context, CL_MEM_READ_WRITE, 
                         sizeof(float) * ROW_A * COL_B,
                         NULL, &err);
  CHECK_ERROR(err);

  // Set the arguments.
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_A);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_B);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_C);
  CHECK_ERROR(err);

  // Enqueue the kernel.
  size_t lws[2] = {16, 16};
  size_t gws[2];
  gws[1] = (size_t)ceil((double)ROW_A / lws[1]) * lws[1];
  gws[0] = (size_t)ceil((double)COL_B / lws[0]) * lws[0];
  err = clEnqueueNDRangeKernel(cmd_queue,
                               kernel,
                               2, NULL,
                               gws, lws,
                               0, NULL,
                               &ev_kernel);
  CHECK_ERROR(err);

  // Read the result.
  err = clEnqueueReadBuffer(cmd_queue,
                            mem_C,
                            CL_TRUE, 0,
                            sizeof(float) * ROW_A * COL_B, 
                            M_C,
                            0, NULL, NULL);
  CHECK_ERROR(err);

  // Read the profiling info.
  cl_ulong start_time, end_time;
  err = clGetEventProfilingInfo(ev_kernel, CL_PROFILING_COMMAND_START, 
                                sizeof(cl_ulong), &start_time, NULL);
  CHECK_ERROR(err);
  err = clGetEventProfilingInfo(ev_kernel, CL_PROFILING_COMMAND_END, 
                                sizeof(cl_ulong), &end_time, NULL);
  CHECK_ERROR(err);
  printf("Kernel time : %lf sec\n", (double)(end_time - start_time) / 10e9);

  // Release
  clReleaseEvent(ev_kernel);
  clReleaseMemObject(mem_A);
  clReleaseMemObject(mem_B);
  clReleaseMemObject(mem_C);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(cmd_queue);
  clReleaseContext(context);
}


