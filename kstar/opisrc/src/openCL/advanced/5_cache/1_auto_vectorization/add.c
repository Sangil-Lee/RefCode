#ifdef AUTOVEC
void addition_auto(float *A, float *B, float *C, int size)
{
	int i;
	int j;
	for(i = 0; i < size; i++)
		C[i] = A[i] + B[i];
}
#else
void addition_ref(float *A, float *B, float *C, int size)
{
	int i;
	int j;
	for(i = 0; i < size; i++)
		C[i] = A[i] + B[i];
}

typedef float v4sf __attribute__ ((vector_size (16)));
void addition_vec(float *A, float *B, float *C, int size)
{
	int i;
	int j;
	for(i = 0; i+3 < size; i+=4)
	{
		v4sf a = __builtin_ia32_loadups(A + i);
		v4sf b = __builtin_ia32_loadups(B + i);
		v4sf c = __builtin_ia32_addps(a, b);
		__builtin_ia32_storeups(C + i, c);
	}
	for(; i < size; i++)
		C[i] = A[i] + B[i];
}
#endif
