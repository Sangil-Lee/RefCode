/* mm_host.c */
#include <CL/cl.h>
#include <snucl_utils.h>

int main(int argc, char** argv) {
    cl_platform_id platform;
    cl_device_type dev_type = CL_DEVICE_TYPE_GPU;
    cl_device_id device;
    cl_context context;
    cl_command_queue cmq;
    cl_program program;
    cl_kernel kernel;
    cl_mem memA, memB, memC;
    cl_int err;

    float *A, *B, *C;
    int wA, hA, wB, hB, wC, hC;
    size_t sizeA, sizeB, sizeC;
    size_t global[2], local[2];
    int i, j;

    //step 1
    wA = hA = wB = 16 * 16; // square matrix
    hB = wA; wC = wB; hC = hA; 

    sizeA = wA * hA * sizeof(float); // 400 MB
    sizeB = wB * hB * sizeof(float); 
    sizeC = wC * hC * sizeof(float);

    A = (float*) malloc(sizeA);
    B = (float*) malloc(sizeB);
    C = (float*) malloc(sizeC);

    for (i = 0; i < hA; i++) {
        for (j = 0; j < wA; j++) {
            A[i * wA + j] = (float) i;
        }
    }

    for (i = 0; i < hB; i++) {
        for (j = 0; j < wB; j++) {
            B[i * wB + j] = (float) j;
        }
    }

    // step 2
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    err = clGetDeviceIDs(platform, dev_type, 1, &device, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    cmq = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    memA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeA, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    memB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeB, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    memC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeC, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    // step 3
    size_t program_src_len;
    char* program_src = snuclLoadFile("/home/jungwon/edu/mm/mm_kernel.cl", &program_src_len);
    program = clCreateProgramWithSource(context, 1, (const char**) &program_src, &program_src_len, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    kernel = clCreateKernel(program, "mm", &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    // step 4
    err = clEnqueueWriteBuffer(cmq, memA, CL_FALSE, 0, sizeA, A, 0, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    err = clEnqueueWriteBuffer(cmq, memB, CL_FALSE, 0, sizeB, B, 0, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    // step 5
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memC);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memA);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memB);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    err = clSetKernelArg(kernel, 3, sizeof(cl_int), (void*) &wA);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    err = clSetKernelArg(kernel, 4, sizeof(cl_int), (void*) &wB);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    global[0] = wC;
    global[1] = hC;
    local[0] = local[1] = 16;

    err = clEnqueueNDRangeKernel(cmq, kernel, 2, NULL, global, local, 0, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    // step 6
    err = clEnqueueReadBuffer(cmq, memC, CL_TRUE, 0, sizeC, C, 0, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    snuclVerifyMM(C, A, B, wA, hA, wB);

    return 0;
}


