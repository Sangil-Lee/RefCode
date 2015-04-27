/* mm_host.c */
#include <CL/cl.h>
#include <snucl_utils.h>

#define MAX_DEV 32

int main(int argc, char** argv) {
    cl_platform_id platform;
    cl_device_type dev_type = CL_DEVICE_TYPE_GPU;
    cl_device_id device[MAX_DEV];
    cl_context context;
    cl_command_queue cmq[MAX_DEV];
    cl_program program;
    cl_kernel kernel[MAX_DEV];
    cl_mem memA[MAX_DEV], memB[MAX_DEV], memC[MAX_DEV];
    cl_int err;
    int ndev;

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

    err = clGetDeviceIDs(platform, dev_type, 0, NULL, (unsigned int*) &ndev);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    printf("NDEV [%d]\n", ndev);
    err = clGetDeviceIDs(platform, dev_type, ndev, device, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    context = clCreateContext(NULL, ndev, device, NULL, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    for (i = 0; i < ndev; i++) {
        cmq[i] = clCreateCommandQueue(context, device[i], 0, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    }

    for (i = 0; i < ndev; i++) {
        memA[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeA / ndev, NULL, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
        memB[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeB, NULL, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
        memC[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeC / ndev, NULL, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    }

    // step 3
    size_t program_src_len;
    char* program_src = snuclLoadFile("/home/jungwon/edu/mm/mm_kernel.cl", &program_src_len);
    program = clCreateProgramWithSource(context, 1, (const char**) &program_src, &program_src_len, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    for (i = 0; i < ndev; i++) {
        kernel[i] = clCreateKernel(program, "mm", &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    }

    // step 4
    for (i = 0; i < ndev; i++) {
        err = clEnqueueWriteBuffer(cmq[i], memA[i], CL_FALSE, 0, sizeA / ndev, (void*) ((size_t) A + (sizeA / ndev) * i), 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

        err = clEnqueueWriteBuffer(cmq[i], memB[i], CL_FALSE, 0, sizeB, B, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    }

    // step 5
    for (i = 0; i < ndev; i++) {
        clSetKernelArg(kernel[i], 0, sizeof(cl_mem), (void*) &memC[i]);
        clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void*) &memA[i]);
        clSetKernelArg(kernel[i], 2, sizeof(cl_mem), (void*) &memB[i]);
        clSetKernelArg(kernel[i], 3, sizeof(cl_int), (void*) &wA);
        clSetKernelArg(kernel[i], 4, sizeof(cl_int), (void*) &wB);
    }

    global[0] = wC;
    global[1] = hC / ndev;
    local[0] = local[1] = 16;

    for (i = 0; i < ndev; i++) {
        clEnqueueNDRangeKernel(cmq[i], kernel[i], 2, NULL, global, local, 0, NULL, NULL);
    }

    // step 6
    for (i = 0; i < ndev; i++) {
          clEnqueueReadBuffer(cmq[i], memC[i], CL_TRUE, 0, sizeC / ndev, (void*) ((size_t) C + (sizeC / ndev) * i), 0, NULL, NULL);
    }

    snuclVerifyMM(C, A, B, wA, hA, wB);

    return 0;
}

