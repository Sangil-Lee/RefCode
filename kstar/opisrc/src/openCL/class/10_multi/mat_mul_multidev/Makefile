TARGET = mat_mul_multidev 
OBJS   = mat_mul_multidev.o cl_util.o

CC       = gcc
INCLUDES = 
CFLAGS   = -O2 -Wall $(INCLUDES)
LDFLAGS  = -lm -lOpenCL


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
