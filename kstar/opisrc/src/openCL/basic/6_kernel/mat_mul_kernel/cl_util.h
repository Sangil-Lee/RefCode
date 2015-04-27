#ifndef __CL_UTIL_H__
#define __CL_UTIL_H__

#include <CL/cl.h>

/* OpenCL utility functions */
void print_device_name(cl_device_id dev);
char *get_source_code(const char *file_name, size_t *len);
void print_build_log(cl_program program, cl_device_id dev);

#define CHECK_ERROR(err)                                              \
  if (err != CL_SUCCESS) {                                            \
    fprintf(stderr, "[%s:%d] ERROR: %d\n", __FILE__, __LINE__, err);  \
    exit(EXIT_FAILURE);                                               \
  }

#endif //__CL_UTIL_H__
