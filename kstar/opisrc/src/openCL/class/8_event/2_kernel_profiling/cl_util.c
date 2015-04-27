#include <stdio.h>
#include <stdlib.h>
#include "cl_util.h"


void print_device_name(cl_device_id dev) {
  cl_int err;
  size_t name_size;

  clGetDeviceInfo(dev, CL_DEVICE_NAME, 0, NULL, &name_size);

  char *dev_name = (char *)malloc(name_size + 1);
  err = clGetDeviceInfo(dev, CL_DEVICE_NAME,
                        name_size, dev_name, NULL);
  CHECK_ERROR(err);

  printf("Device: %s\n", dev_name);

  free(dev_name);
}


char *get_source_code(const char *file_name, size_t *len) {
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    fprintf(stderr, "[%s:%d] ERROR: Failed to open %s\n",
            __FILE__, __LINE__, file_name);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  size_t length = (size_t)ftell(file);
  rewind(file);

  char *source_code = (char *)malloc(length + 1);
  if (fread(source_code, length, 1, file) != 1) {
    fclose(file);
    free(source_code);

    fprintf(stderr, "[%s:%d] ERROR: Failed to read %s\n",
            __FILE__, __LINE__, file_name);
    exit(EXIT_FAILURE);
  }
  fclose(file);

  *len = length;
  source_code[length] = '\0';
  return source_code;
}


void print_build_log(cl_program program, cl_device_id dev) {
  size_t log_size;
  clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                        0, NULL, &log_size);
  char *log = (char *)malloc(log_size + 1);
  clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                        log_size, log, NULL);
  fprintf(stderr, "\n---------- BUILD LOG ----------\n%s\n", log);
  fprintf(stderr, "---------------------------------------\n");
  free(log);
}

