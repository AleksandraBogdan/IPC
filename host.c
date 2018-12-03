#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 80
#define COUNT_OF_GOATS 7

int wolfOrGoat(int live)
{
	int A = 1, B = 100;
	if (!live)
	{
		B = 50;
		printf("DEAD\n");
	}
	return A + rand() % (B - A + 1);

}

void childFunction(int i, int* shared, int* status, int seed, sem_t* sem, char* nameSem, sem_t* semParent)
{
	while(1)
	{
		printf("child func work %i\n", i);
		srand(seed);
		shared[i] = wolfOrGoat(*status);
		printf("Goat's number for %i is %i\n", i, shared[i]);
		sem_wait(sem);
		sem_post(semParent);
		printf("end child\n");
	}
	exit(0);
}



void parentFunction(sem_t* semParent, sem_t* sem[], char* nameSem[], int* shared, int* status)
{
	printf("parent func work\n");
	int count = 0, i, wolf, countOfDeath = 0;
	while(1)
	{

		for (i = 0; i < COUNT_OF_GOATS; i++)
		{ 
			//printf("sem wait\n");
      		sem_post(sem[i]);//to do with time
      		sem_wait(semParent);
 		}
  		printf("START GAME\n");
  		wolf = wolfOrGoat(1);
  		printf("Wolf's number is %i\n", wolf);
  		for (i = 0; i < COUNT_OF_GOATS; i++)
  		{ 
  			if (status[i])
  			{
  				if (abs(shared[i] - wolf) > (70 / COUNT_OF_GOATS))
  				{
  					printf("abs > 70:dead\n");
  					status[i] = 0;
  				}
  			}
  			else
  			{
  				if (abs(shared[i] - wolf) < (20 / COUNT_OF_GOATS))
  				{
  					printf("abs < 20:alive\n");
  					status[i] = 1;
  				}
  			}
  		}
		for (i = 0; i < COUNT_OF_GOATS; i++)
  		{
  			printf("check: status is %i\n", status[i]);
  			count += status[i];
  		}
  		if (count == COUNT_OF_GOATS)
  			++countOfDeath; 
  		else
  			countOfDeath = 0;

  		if (countOfDeath >= 2)
  		{ 
  			printf("end game");
  			break;
  		}
	}
	printf("final\n");
	exit(0);

}


int main(void)
{
	srand(time(NULL));
	pid_t pid;
  	void* buf;//for mmap
  	sem_t* sem[COUNT_OF_GOATS], *semParent;// semaphores for every proccess
  	int i;
  	char* namesSemaphores[COUNT_OF_GOATS] = {"/sem1","/sem2","/sem3","/sem4","/sem5","/sem6","/sem7"};
  	int status[COUNT_OF_GOATS] = {1};// 1- live, 0 -dead
  	int randSeed[COUNT_OF_GOATS + 1];//seeds for every proccess
  	for (i = 0; i < COUNT_OF_GOATS + 1; i++)
  	{
  		randSeed[i] = rand();
  	}


// work with mmap
  	buf = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);//create a new mapping in the virtual adress space(1 - starting address for mapping(kernel chooses the address), 2 - length of the mapping, 3- desired memory protection(may read, may write), 4 - visible to other proccesses mapping the same region or files(MAP_ANON - mapping is not backed by any files); 5,6 -should be like this because of MAP_ANON)

  	if (buf == MAP_FAILED)
  	{
  		printf("Something went wrong with mmap...\n");
 	}

//work with semaphores
  	semParent = sem_open("/semParent", O_CREAT, S_IRWXU, 0);//create or open semaphore(0 - initial value for semaphore)
  	if(semParent == SEM_FAILED)
	{
		printf("Something went wrong...\n");
	}
	for (i = 0; i < COUNT_OF_GOATS; i++)
	{
		sem[i] = sem_open(namesSemaphores[i], O_CREAT, S_IRWXU, 0);
	  	if(sem[i] == SEM_FAILED)
	  	{
	  		printf("Something went wrong...%i semaphores\n", i);
	  	}
	}
  

	int *shared = buf;
	for(i = 0; i < COUNT_OF_GOATS; i++)
	{
		pid = fork();
	  	if(pid)
	  	{
	  		continue;
	  	}
	  	else if (!pid)
	  	{
	  		childFunction(i, shared, &status[i], randSeed[i], sem[i], namesSemaphores[i], semParent);
	  		exit(0);
	  	}
	  	else
	  	{
	  		printf("Fork failed.\n");
	  	}
	}
	parentFunction(semParent, sem, namesSemaphores, shared, status);

	if (munmap(buf, BUFFER_SIZE) == -1) //unmap
	{
		printf("Something went wrong with munmap...\n");
	}
	sem_close(semParent);
	sem_unlink("/semParent");
	for (i = 0; i < COUNT_OF_GOATS; i++)
	{
	 	sem_close(sem[i]);
	 	sem_unlink(namesSemaphores[i]);

	}

	return 0;
}

