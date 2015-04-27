#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include <math.h>

#include "cl_util.h"

#define NUM_ITERATIONS 1000

void printUsage(const char* prog_name)
{
	printf("Usage: %s <arr_size>\n", prog_name);
}

void buffers(float* M_A, float* M_B, float* M_C, size_t arr_size)
{
  cl_platform_id   *platform;
  cl_device_type   dev_type = CL_DEVICE_TYPE_GPU;
  cl_device_id     dev;
  cl_context       context;
  cl_command_queue cmd_queue;
  cl_program       program;
  cl_kernel        kernel;
  cl_mem           mem_A, mem_B, mem_C;
  cl_int           err;
  cl_uint          num_platforms;
  cl_uint          num_dev = 0;

  cl_event         ev_kernel;
  cl_event         ev_read_buffer;

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
  cmd_queue = clCreateCommandQueue(context, dev, CL_QUEUE_PROFILING_ENABLE, &err);
  CHECK_ERROR(err);

  // Create a program.
  size_t source_len;
  char *source_code = get_source_code("./buffers.cl", &source_len);
  program = clCreateProgramWithSource(context,
                                      1,
                                      (const char **)&source_code,
                                      &source_len,
                                      &err);
  free(source_code);
  CHECK_ERROR(err);

  // Build the program.
  char build_opts[200];
  sprintf(build_opts, "-DARR_SIZE=%lu", arr_size);
  err = clBuildProgram(program, 1, &dev, build_opts, NULL, NULL);
  if (err != CL_SUCCESS) {
    print_build_log(program, dev);
    CHECK_ERROR(err);
  }

  // Kernel
  kernel = clCreateKernel(program, "buffers", &err);
  CHECK_ERROR(err);

  // Buffers
  mem_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                         sizeof(float) * arr_size,
                         M_A, &err);
  CHECK_ERROR(err);

  mem_B = clCreateBuffer(context, CL_MEM_READ_ONLY,
                         sizeof(float) * arr_size,
                         NULL, &err);
  CHECK_ERROR(err);

  mem_C = clCreateBuffer(context, CL_MEM_READ_WRITE,
                         sizeof(float) * arr_size,
                         NULL, &err);
  CHECK_ERROR(err);

  // Write to Buffers
  err = clEnqueueWriteBuffer(cmd_queue,
                             mem_B, 
                             CL_TRUE, 0,
                             sizeof(float) * arr_size,
                             M_B,
                             0, NULL, NULL);
  CHECK_ERROR(err);

  // Set the arguments.
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_A);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_B);
  CHECK_ERROR(err);
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &mem_C);
  CHECK_ERROR(err);

  // Enqueue the kernel.
  size_t lws[1] = {16};
  size_t gws[1];
  gws[0] = (size_t)ceil((double)arr_size / lws[0]) * lws[0];
  err = clEnqueueNDRangeKernel(cmd_queue,
                               kernel,
                               1, NULL,
                               gws, lws,
                               0, NULL, &ev_kernel);
  CHECK_ERROR(err);

  // Read the result.
  cl_ulong total_time, start_time, end_time;
  total_time = 0;

  // read memory using ReadBuffer
  for( i = 0; i < NUM_ITERATIONS; ++i )
  {
    err = clEnqueueReadBuffer(cmd_queue,
        mem_C,
        CL_TRUE, 0,
        sizeof(float) * arr_size,
        M_C,
        1, &ev_kernel, &ev_read_buffer);
    CHECK_ERROR(err);

    err = clGetEventProfilingInfo( ev_read_buffer,
        CL_PROFILING_COMMAND_START,
        sizeof(start_time), &start_time, NULL);
    CHECK_ERROR(err);

    err = clGetEventProfilingInfo( ev_read_buffer,
        CL_PROFILING_COMMAND_END,
        sizeof(end_time), &end_time, NULL);
    CHECK_ERROR(err);

    total_time += end_time - start_time;
  }

  printf("Average read time (ns): %lf\n", (double)total_time/NUM_ITERATIONS);

  // read memory using MapBuffer
  void* mapped_memory;
  total_time = 0;

  for( i = 0; i < NUM_ITERATIONS; ++i )
  {
    mapped_memory = clEnqueueMapBuffer(cmd_queue,
        mem_C, CL_TRUE, CL_MAP_READ, 0,
        sizeof(float) * arr_size,
        0, NULL, &ev_read_buffer, &err);
    CHECK_ERROR(err);

    err = clEnqueueUnmapMemObject(cmd_queue,
        mem_C, mapped_memory, 0, NULL, NULL);
    CHECK_ERROR(err);

    err = clGetEventProfilingInfo( ev_read_buffer,
        CL_PROFILING_COMMAND_START,
        sizeof(start_time), &start_time, NULL);
    CHECK_ERROR(err);

    err = clGetEventProfilingInfo( ev_read_buffer,
        CL_PROFILING_COMMAND_END,
        sizeof(end_time), &end_time, NULL);
    CHECK_ERROR(err);

    total_time += end_time - start_time;
  }

  printf("Average map time (ns): %lf\n", (double)total_time/NUM_ITERATIONS);

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

int main(int argc, char* argv[] )
{
  int i;
	int arr_size;
  float* M_A;
  float* M_B;
  float* M_C;

	if( argc != 2 )
	{
		printUsage(argv[0]);
		return 1;
	}

	arr_size = atoi(argv[1]);

	printf("arr_size = %d\n", arr_size);

	M_A = malloc(arr_size*sizeof(float));
	M_B = malloc(arr_size*sizeof(float));
	M_C = malloc(arr_size*sizeof(float));

  for( i = 0; i < arr_size; i++ )
  {
    M_A[i] = i;
    M_B[i] = 2*i;
  }

  buffers(M_A, M_B, M_C, arr_size);

  // verification
  int verified = 1;
  for( i = 0; i < arr_size; i++ )
  {
    if( M_C[i] != 3*i )
    {
      verified = 0;
    }
  }

  if( verified )
    printf("VERIFICATION = TRUE.\n");
  else
    printf("VERIFICATION = FALSE.\n");

  return 0;
}
