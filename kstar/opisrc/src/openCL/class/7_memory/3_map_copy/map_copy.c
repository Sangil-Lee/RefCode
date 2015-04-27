#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
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
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
      printf("Device: CPU\n");
   } else printf("Device: GPU\n");
   CHECK_ERROR(err);

   return dev;
}

int main() {
   /* OpenCL data structures */
   cl_device_id device;
   cl_context context;
   cl_command_queue queue;
   cl_int i, j, err;

   /* Data and buffers */
   float data_one[100], data_two[100], result_array[100];
   cl_mem buffer_one, buffer_two;
   void* mapped_memory;

   /* Initialize arrays */
   for(i=0; i<100; i++) {
      data_one[i] = 1.0f*i;
      data_two[i] = -1.0f*i;
      result_array[i] = 0.0f;
   }

   /* Create a device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   CHECK_ERROR(err);

   /* Create buffers */
   buffer_one = clCreateBuffer(context, CL_MEM_READ_WRITE | 
         CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
   CHECK_ERROR(err);
   buffer_two = clCreateBuffer(context, CL_MEM_READ_WRITE | 
         CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);
   CHECK_ERROR(err);

   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 0, &err);
   CHECK_ERROR(err);

   /* Enqueue command to copy buffer one to buffer two */
   err = clEnqueueCopyBuffer(queue, buffer_one, buffer_two, 0, 0,
         sizeof(data_one), 0, NULL, NULL); 
   CHECK_ERROR(err);

   /* Enqueue command to map buffer two to host memory */
   mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE,
         CL_MAP_READ, 0, sizeof(data_two), 0, NULL, NULL, &err);
   CHECK_ERROR(err);

   /* Transfer memory and unmap the buffer */
   memcpy(result_array, mapped_memory, sizeof(data_two));
   err = clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory,
         0, NULL, NULL);
   CHECK_ERROR(err);

   /* Display updated buffer */
   printf("\nBuffer 1:\n");
   for(i=0; i<10; i++) {
      for(j=0; j<10; j++) {
         printf("%6.1f", data_one[j+i*10]);
      }
      printf("\n");
   }

   printf("\nResult array:\n");
   for(i=0; i<10; i++) {
      for(j=0; j<10; j++) {
         printf("%6.1f", result_array[j+i*10]);
      }
      printf("\n");
   }

   /* Deallocate resources */
   clReleaseMemObject(buffer_one);
   clReleaseMemObject(buffer_two);
   clReleaseCommandQueue(queue);
   clReleaseContext(context);

   return 0;
}
