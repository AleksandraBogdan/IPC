	
all:
	gcc -o host host.c conn_mmap.c -lpthread -lrt
