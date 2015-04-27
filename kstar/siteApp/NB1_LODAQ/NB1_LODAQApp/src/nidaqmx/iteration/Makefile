LIBS = -lrt
LIBFLAGS = $(LIBS) -lnidaqmx
TARGET = kstar-daqtest-iteration
OBJS = $(TARGET).o

CDEBUG = -g
LDFLAGS += -g
CFLAGS += $(CDEBUG)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) core

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBFLAGS)

