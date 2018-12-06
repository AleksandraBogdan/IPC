CC=gcc 
CFLAGS=-Wall -Werror 
LIBS=-lpthread
TARGET=host.c 
EXECUTABLES=$(ALL_TARGETS:./conn_%.c=host_%)
ALL_TARGETS=$(shell find . -name "conn_*.c")


all: $(EXECUTABLES)
	
host_%: conn_%.o $(TARGET:.c=.o)
	$(CC) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean

clean:
	rm -rf `find . -maxdepth 1 -executable -type f`