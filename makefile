	
all:
	gcc -o host_mmap host.c conn_mmap.c -lpthread -lrt
	#gcc -o host_pipe host.c conn_pipe.c -lpthread -lrt
