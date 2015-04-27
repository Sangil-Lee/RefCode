#include "snucl_utils.h"

struct timeval t0, t1; 

char* snuclLoadFile(const char* filename, size_t* length_ret) {
    FILE *fp;
    size_t length;
    fp = fopen(filename, "rb");
    if (fp == 0) return NULL;

    fseek(fp, 0, SEEK_END); 
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET); 

    char* ret = (char*) malloc(length + 1); 
    if (fread(ret, length, 1, fp) != 1) {
        fclose(fp);
        free(ret);
        return NULL;
    }   

    fclose(fp);
    if (length_ret) *length_ret = length;
    ret[length] = '\0';

    return ret;
}
void snuclVerifyMM(float* C, float* A, float*B, int wA, int hA, int wB) {
    int i, j, k;
    int unit = hA * wB / 10; 
    int index = 0;
    printf("[SnuCL] VerifyMM : ");
    for (j = 0; j < hA; j++) {
        for (i = 0; i < wB; i++) {
            float acc = 0.0f;
            for (k = 0; k < wA; k++) {
                acc += A[j * wA + k] * B[k * wB + i]; 
            }   
            if (C[j * wB + i] != acc) {
                printf(" FAIL at [%d,%d] element CL[%f] vs CORRECT[%f]\n", i, j, C[j * wB + i], acc);
                return;
            }
            if (index++ % unit == (unit - 1)) {
                printf(".");
                fflush(stdout);
            }   
        }   
        fflush(stdout);
    }
    printf(" PASS\n"); 
}
void snuclTimerStart() {
    gettimeofday(&t0, NULL);
}

void snuclTimerStop() {
    gettimeofday(&t1, NULL);
    double ms = ((t1.tv_usec - t0.tv_usec) * 0.001) + ((t1.tv_sec - t0.tv_sec) * 1000.0);
    printf("[SnuCL] Elapsed Time : %lf secs\n", ms / 1000);
}

