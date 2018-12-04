#include <sys/mman.h>
#include "connector.h"

#define BUFFER_SIZE 80

static void* buf;
static int* shared;

void conn_create()
{
	//void* buf;//for mmap
	buf = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);//create a new mapping in the virtual adress space(1 - starting address for mapping(kernel chooses the address), 2 - length of the mapping, 3- desired memory protection(may read, may write), 4 - visible to other proccesses mapping the same region or files(MAP_ANON - mapping is not backed by any files); 5,6 -should be like this because of MAP_ANON)

  	if (buf == MAP_FAILED)
  	{
  		printf("Something went wrong with mmap...\n");
  		
 	}
 	shared = buf;
}

int conn_read(int index)
{
	//int* shared = buf;
	return shared[index];

}

void conn_write(int index, int number)
{
	//int* shared = buf;
	shared[index] = number;
	//printf("shared %i\n", shared[index]);
}

void conn_delete()
{ 
	if (munmap(buf, BUFFER_SIZE) == -1) //unmap
	{
		printf("Something went wrong with munmap...\n");
	}
}