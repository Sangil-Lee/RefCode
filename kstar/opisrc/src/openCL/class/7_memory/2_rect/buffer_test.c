#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <CL/cl.h>

#define CHECK_ERROR(err)                                              \
  if (err != CL_SUCCESS) {                                            \
    fprintf(stderr, "[%s:%d] ERROR: %d\n", __FILE__, __LINE__, err);  \
    exit(EXIT_FAILURE);                                               \
  }

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {
   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   CHECK_ERROR(err);

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if (err != CL_SUCCESS) {
     err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
     CHECK_ERROR(err);
     printf("Device: CPU\n");
   } else
     printf("Device: GPU\n");

   return dev;
}


int main() {
   /* OpenCL data structures */
   cl_device_id device;
   cl_context context;
   cl_command_queue queue;
   cl_int i, j, err;

   /* Data and buffers */
   float full_matrix[80], zero_matrix[80];
   const size_t buffer_origin[3] = {5*sizeof(float), 3, 0};
   const size_t host_origin[3] = {1*sizeof(float), 1, 0};
   const size_t region[3] = {4*sizeof(float), 4, 1};
   cl_mem matrix_buffer;

   /* Initialize data */
   for(i=0; i<80; i++) {
      full_matrix[i] = i*1.0f;
      zero_matrix[i] = 0.0;
   }   
   
   /* Create a device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   CHECK_ERROR(err);

   /* Create a buffer to hold 80 floats */
   matrix_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
       sizeof(full_matrix), NULL, &err);
   CHECK_ERROR(err);

   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 0, &err);
   CHECK_ERROR(err);
   
   /* Enqueue command to write to buffer */
   err = clEnqueueWriteBuffer(queue, matrix_buffer, CL_TRUE, 0,
         sizeof(full_matrix), full_matrix, 0, NULL, NULL); 
   CHECK_ERROR(err);

   /* Enqueue command to read rectangle of data */
   err = clEnqueueReadBufferRect(queue, matrix_buffer, CL_TRUE, 
         buffer_origin, host_origin, region, 10*sizeof(float), 0, 
         10*sizeof(float), 0, zero_matrix, 0, NULL, NULL);
   CHECK_ERROR(err);

   /* Display matrices */
   printf("\nFull Matrix:\n");
   for(i=0; i<8; i++) {
      for(j=0; j<10; j++) {
         printf("%6.1f", full_matrix[j+i*10]);
      }
      printf("\n");
   }

   printf("\nZero Matrix:\n");
   for(i=0; i<8; i++) {
      for(j=0; j<10; j++) {
         printf("%6.1f", zero_matrix[j+i*10]);
      }
      printf("\n");
   }

   /* Deallocate resources */
   clReleaseMemObject(matrix_buffer);
   clReleaseCommandQueue(queue);
   clReleaseContext(context);

   return 0;
}

