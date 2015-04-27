CC = gcc
CFLAGS = -I/usr/local/mdsplus/include -L/usr/local/mdsplus/lib -lMdsLib -lTreeShr

bin: mdsseg mdsex1

mdsseg: mdsseg.c
	$(CC) $(CFLAGS) mdsseg.c -o mdsseg

mdsex1: mdsex1.c
	$(CC) $(CFLAGS) mdsex1.c -o mdsex1

clean:
	rm -f *.o mdsseg mdsex1
