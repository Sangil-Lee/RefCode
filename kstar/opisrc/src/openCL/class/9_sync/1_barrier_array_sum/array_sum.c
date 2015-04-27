#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include <math.h>

#include "cl_util.h"

void array_sum(int* M_A, size_t arr_size)
{
  /* TODO */
  cl_platform_id   *platform;
  cl_device_type   dev_type = CL_DEVICE_TYPE_GPU;
  cl_device_id     dev;
  cl_context       context;
  cl_command_queue cmd_queue;
  cl_program       program;
  cl_kernel        kernel;
  cl_mem           mem_A;
  cl_int           err;
  cl_uint          num_platforms;
  cl_uint          num_dev = 0;

  int i; 

  char *dtype = getenv("CL_DEV_TYPE");
  if (dtype) {
    if (strcasecmp(dtype, "cpu") == 0) {
      dev_type = CL_DEVICE_TYPE_CPU;
    } else if (strcasecmp(dtype, "gpu") == 0) {
      dev_type = CL_DEVICE_TYPE_GPU;
    }
  }

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
  size_t source_len;
  char *source_code = get_source_code("./array_sum.cl", &source_len);
  program = clCreateProgramWithSource(context, 1,
                                      (const char **)&source_code,
                                      &source_len, &err);
  free(source_code);
  CHECK_ERROR(err);
  
  // Build the program.
  char build_opts[200];
  sprintf(build_opts, "-DARR_SIZE=%lu ", arr_size);
  err = clBuildProgram(program, 1, &dev, build_opts, NULL, NULL);

  if (err != CL_SUCCESS) {
    print_build_log(program, dev);
    CHECK_ERROR(err);
  }

  // Kernel
  kernel = clCreateKernel(program, "array_sum", &err);
  CHECK_ERROR(err);

  // Buffers
  mem_A = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                         sizeof(int) * arr_size, M_A, &err);
  CHECK_ERROR(err);

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_A);
  CHECK_ERROR(err);

  size_t lws[1] = {4};
  size_t gws[1] = {4};

  err = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, NULL, gws, lws, 0, NULL, NULL);
  CHECK_ERROR(err);

  err = clEnqueueReadBuffer(cmd_queue, mem_A, CL_TRUE, 0, sizeof(int) * arr_size, M_A, 
		  0, NULL, NULL);
  CHECK_ERROR(err);
}

int main()
{
	const int ARR_SIZE=16;
	int M_A[ARR_SIZE];
	int i;
	int sum = 0;

	for( i = 0; i < ARR_SIZE; i++ )
	{
		M_A[i] = i+1;
		sum += i+1;
	}

	array_sum(M_A, ARR_SIZE);

	printf("SUM: %d\n", sum);
	// verification
	int verified = 1;
	if( M_A[0] != sum )
	{
		printf("M_A[0] = %d\n", M_A[0]);
		verified = 0;
	}

	printf("Sum (1~%d) = %d\n", ARR_SIZE, M_A[0]);

	if( verified )
		printf("VERIFICATION = TRUE.\n");
	else
		printf("VERIFICATION = FALSE.\n");

	return 0;
}
