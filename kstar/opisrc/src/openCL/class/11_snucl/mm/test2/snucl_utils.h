#ifndef SNUCL_UTILS_H
#define SNUCL_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

char* snuclLoadFile(const char* filename, size_t* length_ret);
void snuclVerifyMM(float* C, float* A, float* B, int wA, int hA, int wB);
void snuclTimerStart();
void snuclTimerStop();

#endif

