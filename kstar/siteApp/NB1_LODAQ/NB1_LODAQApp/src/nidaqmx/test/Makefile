SHELL = /bin/sh

LIBS = nidaqmx
LIBFLAGS = -l$(LIBS)
TARGET = ContAcq-IntClk-AnlgStart

OBJS = $(TARGET).o

CDEBUG = -g
LDFLAGS += -g

CFLAGS += $(CDEBUG)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) core

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBFLAGS)
