CC=gcc
CFLAGS=-Wall -O3
LLIB=-lpthread

all: ipc_anon

ipc_anon: ipc_anon.o

.c.o:
#%.o: %.c
	$(CC) $(CFLAGS) -c $<

.o:
	$(CC) $< -o $@ $(LLIB)
	rm *.o

.PHONY: clean

clean:
	rm -f `find . -maxdepth 1 -executable -type f`

# $^ $@ -- right and left side of : respectively

