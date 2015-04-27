CC = gcc
PROG = vectorize
SRCS = add.c 
OBJS = $(SRCS:%.c=%.o) main.o timer.o
AUTOVEC_OBJS = $(SRCS:%.c=%.v.o)
LDFLAGS = -lm  

LIBPATH =  
INCLUDE =  
CFLAGS = -O3 -fno-tree-vectorize  -msse4.2  
VCFLAGS = -O3 -DAUTOVEC -msse4.2 

all: $(PROG) 
$(PROG): $(OBJS) $(AUTOVEC_OBJS) 
	$(CC) $(OBJS) $(AUTOVEC_OBJS) -o $(PROG) $(LDFLAGS) $(LIBPATH) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)
%.v.o: %.c
	$(CC) $(VCFLAGS) -c $< -o $@ $(INCLUDE)
clean :
	rm *.o  $(PROG) 
