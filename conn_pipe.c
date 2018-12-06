
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>           

#include "connector.h"
#define N 7

static int filedes[2 * N];

void conn_create()
{
	int i;
	for(i = 0; i < N;i++)
	{
		if (pipe(&filedes[2  * i]) == -1)
		{
			printf("Something wrong with creating pipes\n");
		}
  	
	}
	
}

int conn_read(int index)
{
	//close(filedes[2 * index + 1]);
	int value, size;
	size = read(filedes[2 * index], &value, sizeof(int));
	if (size < 0)
	{
		printf("erroe with reading\n");
	}
	//close(filedes[2 * index]);
	return value;

}

void conn_write(int index, int number)
{
	//close(filedes[2 * index]);
	write(filedes[2 * index + 1], &number, sizeof(int));
	//close(filedes[2 * index + 1]);
}

void conn_delete()
{ 
	int i;
	for(i = 0; i < N; i++)
	{
		close(filedes[2 * i]);
		close(filedes[2 * i + 1]);
	}
	
}