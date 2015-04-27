CC=gcc
INCLUDES=
CFLAGS=-O2 -Wall -std=c99
LDFLAGS=-lm -ljpeg
TARGET=raytracer
OBJS=raytracer.o timers.o

all: $(TARGET)

$(TARGET): $(OBJS)
						$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)
