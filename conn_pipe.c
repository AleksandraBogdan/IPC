

#include "connector.h"

static int filedes[2];

void conn_create()
{
	if (pipe(filedes) == -1)
	{
		printf("Something wrong with creating pipes\n");
	}
  	
}

int conn_read(int index)
{
	close(filedes[1]);
	int value, size;
	size = read(filedes[0], value, sizeof(int));
	if (size < 0)
	{
		printf("erroe with reading\n");
	}
	close(filedes[0]);
	return value;

}

void conn_write(int index, int number)
{
	close(filedes[0]);
	write(filedes[1], number, sizeof(int));
	close(filedes[1]);
}

void conn_delete()
{ 
	close(filedes[0]);
	close(filedes[1]);
}