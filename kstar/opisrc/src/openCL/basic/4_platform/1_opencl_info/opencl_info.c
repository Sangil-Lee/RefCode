#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CL/cl.h>

const char *clu_ErrorString(cl_int err) {
	static const char *errorStrs[] = {
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"CL_INVALID_VALUE",
		"CL_INVALID_DEVICE_TYPE",
		"CL_INVALID_PLATFORM",
		"CL_INVALID_DEVICE",
		"CL_INVALID_CONTEXT",
		"CL_INVALID_QUEUE_PROPERTIES",
		"CL_INVALID_COMMAND_QUEUE",
		"CL_INVALID_HOST_PTR",
		"CL_INVALID_MEM_OBJECT",
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
		"CL_INVALID_IMAGE_SIZE",
		"CL_INVALID_SAMPLER",
		"CL_INVALID_BINARY",
		"CL_INVALID_BUILD_OPTIONS",
		"CL_INVALID_PROGRAM",
		"CL_INVALID_PROGRAM_EXECUTABLE",
		"CL_INVALID_KERNEL_NAME",
		"CL_INVALID_KERNEL_DEFINITION",
		"CL_INVALID_KERNEL",
		"CL_INVALID_ARG_INDEX",
		"CL_INVALID_ARG_VALUE",
		"CL_INVALID_ARG_SIZE",
		"CL_INVALID_KERNEL_ARGS",
		"CL_INVALID_WORK_DIMENSION",
		"CL_INVALID_WORK_GROUP_SIZE",
		"CL_INVALID_WORK_ITEM_SIZE",
		"CL_INVALID_GLOBAL_OFFSET",
		"CL_INVALID_EVENT_WAIT_LIST",
		"CL_INVALID_EVENT",
		"CL_INVALID_OPERATION",
		"CL_INVALID_GL_OBJECT",
		"CL_INVALID_BUFFER_SIZE",
		"CL_INVALID_MIP_LEVEL",
		"CL_INVALID_GLOBAL_WORK_SIZE",
	};

	int index = -err;
	int errorCount = sizeof(errorStrs) / sizeof(errorStrs[0]);
	return (index >= 0 && index < errorCount) ? errorStrs[index] : "Unknown Error";
}


static inline void clu_CheckError(cl_int err, const char *msg) {
	if (err != CL_SUCCESS) {
		fprintf(stderr, "ERROR: %s (%d:%s)\n", msg, err, clu_ErrorString(err));
		exit(EXIT_FAILURE);
	}
}


void opencl_info() {
	cl_int           err_code;

	cl_platform_id  *platforms;
	cl_device_type   device_type;
	cl_uint          num_devices;
	cl_device_id    *devices;

	// Get OpenCL platforms
	// - Get the number of available platforms
	cl_uint num_platforms;
	err_code = clGetPlatformIDs(0, NULL, &num_platforms);
	clu_CheckError(err_code, "clGetPlatformIDs() for num_platforms");
	if (num_platforms == 0) {
		fprintf(stderr, "No OpenCL platform!\n");
		exit(EXIT_FAILURE);
	}
	// - Get platform IDs
	platforms = (cl_platform_id *)malloc(num_platforms*sizeof(cl_platform_id));
	err_code = clGetPlatformIDs(num_platforms, platforms, NULL);
	clu_CheckError(err_code, "clGetPlatformIDs()");

	// Get platform informations
	printf("\nNumber of platforms: %u\n\n", num_platforms);
	char tmp_buf[1024];

	//TODO:
	//Write your code
	for (cl_uint i = 0; i < num_platforms; i++) 
	{
		printf("platform: %u\n", i);

		err_code = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, &tmp_buf, NULL);
		clu_CheckError(err_code, "clGetPlatformInfo() for CL_PLAATFORM_NAME");
		printf(" - CL_PLATFORM_NAME : %s\n", tmp_buf);


	}

	free(platforms);
}


int main(int argc, char **argv) {
	opencl_info();
	fflush(stdout);
	fflush(stderr);

	return EXIT_SUCCESS;
}

