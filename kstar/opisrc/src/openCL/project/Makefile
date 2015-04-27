CC=gcc
INCLUDES=
CFLAGS=-O2 -Wall -std=c99
LDFLAGS=-lm -ljpeg -lOpenCL
TARGET=raytracer
OBJS=raytracer.o timers.o cl_util.o

all: $(TARGET)

$(TARGET): $(OBJS)
						$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)
