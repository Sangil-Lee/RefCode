#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <CL/cl.h>

#define PROGRAM_FILE "blank.cl"
#define KERNEL_NAME "blank"

#define CHECK_ERROR(err)                                              \
  if (err != CL_SUCCESS) {                                            \
    fprintf(stderr, "[%s:%d] ERROR: %d\n", __FILE__, __LINE__, err);  \
    exit(EXIT_FAILURE);                                               \
  }

int main() {
   /* Host/device data structures */
   cl_platform_id platform;
   cl_device_id device;
   cl_context context;
   cl_command_queue queue;   
   cl_int err;

   /* Program/kernel data structures */
   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   cl_kernel kernel;

   /* Access the first installed platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   CHECK_ERROR(err);

   /* Access the first GPU/CPU */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
   }
   CHECK_ERROR(err);

   /* Create a context */
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   CHECK_ERROR(err);

   /* Read program file and place content into buffer */
   program_handle = fopen(PROGRAM_FILE, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);   
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size+1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(context, 1, 
      (const char**)&program_buffer, &program_size, &err);
   CHECK_ERROR(err);
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
   if(err < 0) {
      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size+1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
            log_size+1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }
   
   /* Create the kernel */
   kernel = clCreateKernel(program, KERNEL_NAME, &err);
   CHECK_ERROR(err);
   
   /* Create the command queue */
   queue = clCreateCommandQueue(context, device, 0, &err);
   CHECK_ERROR(err);

   /* Enqueue the kernel execution command */
   err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
   CHECK_ERROR(err);
   printf("Successfully queued kernel.\n");
   
   /* Deallocate resources */
   clReleaseCommandQueue(queue);   
   clReleaseKernel(kernel);
   clReleaseProgram(program);
   clReleaseContext(context);   

   return 0;
}
