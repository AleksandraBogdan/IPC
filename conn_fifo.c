#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "connector.h"
#define N 7
static char* namesFIFO[N] = {"fifo1","fifo2","fifo3","fifo4","fifo5","fifo6","fifo7"};
static int fd;

void conn_create()
{
	int i;
	for(i = 0; i < N;i++)
	{
		if (mkfifo(namesFIFO[i], S_IRWXU ) == -1)
		{
			printf("Something wrong with creating pipes\n");
		}
  	
	}
	
}

int conn_read(int index)
{
	//close(filedes[2 * index + 1]);
	fd = open(namesFIFO[index], O_RDONLY | O_NONBLOCK);
	int value, size;
	size = read(fd, &value, sizeof(int));
	if (size < 0)
	{
		printf("erroe with reading\n");
	}
	//close(filedes[2 * index]);
	close(fd);
	return value;

}

void conn_write(int index, int number)
{
	//close(filedes[2 * index]);
	fd = open(namesFIFO[index], O_WRONLY | O_NONBLOCK);
	write(fd, &number, sizeof(int));
	//close(filedes[2 * index + 1]);
	close(fd);
}

void conn_delete()
{ 
	int i;
	for(i = 0; i < N; i++)
	{
		unlink(namesFIFO[i]);
	}
	
}