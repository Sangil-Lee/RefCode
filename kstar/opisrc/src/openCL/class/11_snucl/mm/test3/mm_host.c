/* mm_host.c */
#include <mpi.h>
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

    int rank, nnode;
    MPI_Request request[2];
    MPI_Status status[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nnode);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    //step 1
    wA = hA = wB = 16 * 64; // square matrix
    hB = wA; wC = wB; hC = hA; 

    sizeA = wA * hA * sizeof(float); // 400 MB
    sizeB = wB * hB * sizeof(float); 
    sizeC = wC * hC * sizeof(float);

    if (rank == 0) {
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
    } else {
        A = (float*) malloc(sizeA / nnode);
        B = (float*) malloc(sizeB);
        C = (float*) malloc(sizeC / nnode);
    }

    // step 2
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    err = clGetDeviceIDs(platform, dev_type, 0, NULL, (unsigned int*) &ndev);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    printf("RANK[%d] NDEV [%d]\n", rank, ndev);

    err = clGetDeviceIDs(platform, dev_type, ndev, device, NULL);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    context = clCreateContext(NULL, ndev, device, NULL, NULL, &err);
    if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);

    for (i = 0; i < ndev; i++) {
        cmq[i] = clCreateCommandQueue(context, device[i], 0, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
    }

    for (i = 0; i < ndev; i++) {
        memA[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeA / nnode / ndev, NULL, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
        memB[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeB, NULL, &err);
        if (err != CL_SUCCESS) printf("[%s:%d] error[%d]\n", __FILE__, __LINE__, err);
        memC[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeC / nnode / ndev, NULL, &err);
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
    if (rank == 0) {
        for (i = 1; i < nnode; i++) {
            MPI_Isend((void*) ((size_t) A + (sizeA / nnode) * i), (int) sizeA / nnode, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[0]);
            MPI_Isend(B, (int) sizeB, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[1]);
        }   
    } else {
        MPI_Irecv(A, (int) sizeA / nnode, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(B, (int) sizeB, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request[1]);
    }
    MPI_Waitall(2, request, status);

    for (i = 0; i < ndev; i++) {
        clEnqueueWriteBuffer(cmq[i], memA[i], CL_FALSE, 0, sizeA / nnode / ndev, (void*) ((size_t) A + (sizeA / nnode / ndev) * i), 0, NULL, NULL);

        clEnqueueWriteBuffer(cmq[i], memB[i], CL_FALSE, 0, sizeB, B, 0, NULL, NULL);
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
    global[1] = hC / nnode / ndev;
    local[0] = local[1] = 16;

    for (i = 0; i < ndev; i++) {
        clEnqueueNDRangeKernel(cmq[i], kernel[i], 2, NULL, global, local, 0, NULL, NULL);
    }

    // step 6
    for (i = 0; i < ndev; i++) {
        clEnqueueReadBuffer(cmq[i], memC[i], CL_TRUE, 0, sizeC / nnode / ndev, (void*) ((size_t) C + (sizeC / nnode / ndev) * i), 0, NULL, NULL);
    }

    if (rank == 0) {
        for (i = 1; i < nnode; i++) {
            MPI_Irecv((void*) ((size_t) C + (sizeC / nnode) * i), (int) sizeC / nnode, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[0]);
        }
    } else {
        MPI_Isend(C, (int) sizeC / nnode, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request[0]);
    }
    MPI_Wait(request, status);

    if (rank == 0) snuclVerifyMM(C, A, B, wA, hA, wB);

    MPI_Finalize();

    return 0;
}

