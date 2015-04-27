TARGET = mat_mul
OBJS   = mat_mul.o timers.o cl_util.o \
				 mat_mul_seq.o \
				 mat_mul_openmp.o \
				 mat_mul_opencl_1d.o \
				 mat_mul_opencl_2d.o \
				 mat_mul_opencl_printf.o \
				 mat_mul_opencl_vector.o

CC       = gcc
INCLUDES = 
CFLAGS   = -O3 -Wall $(INCLUDES)
LDFLAGS  = -lm -fopenmp -lOpenCL


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

mat_mul_openmp.o: mat_mul_openmp.c
	$(CC) $(CFLAGS) -fopenmp -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
