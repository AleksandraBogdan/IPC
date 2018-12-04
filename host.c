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
#include <time.h>
#include <math.h>
#include <errno.h>

#include "connector.h"

//#define BUFFER_SIZE 80
#define COUNT_OF_GOATS 7


void sem_wait_timeout(sem_t* p_sem)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 5;
  while ((sem_timedwait(p_sem, &ts)) == -1);
}

int wolfOrGoat(int live)
{
	int A = 1, B = 100;
	if (live == 0)
	{
		B = 50;
		//printf("DEAD\n");
	}
	return A + rand() % (B - A + 1);

}

void childFunction(int i, int seed, sem_t* sem, char* nameSem, sem_t* semParent)
{
	int goat, status;
	while(1)
	{

		srand(seed);
		status = conn_read(i);//get status
		//printf("status %i\n", status);
		if (status == -1)
			break;
		goat =  wolfOrGoat(status);
		conn_write(i, goat);
		//shared[i] = wolfOrGoat(*status);//write to mmap
		printf("Goat's number for %i is %i\n", i, goat);
		sem_wait(sem);
		sem_post(semParent);
	}
	exit(0);
}



void parentFunction(sem_t* semParent, sem_t* sem[], char* nameSem[])
{
	int status[COUNT_OF_GOATS] = {1};// 1- live, 0 -dead
	int count = 0, i, wolf, countOfDeath = 0;
	while(1)
	{
		count = 0;
		for (i = 0; i < COUNT_OF_GOATS; i++)
		{ 
			//printf("sem wait\n");
			conn_write(i,status[i]);
      		sem_post(sem[i]);//to do with time
      		sem_wait_timeout(semParent);
 		}
  		printf("START GAME\n");
  		wolf = wolfOrGoat(1);
  		printf("Wolf's number is %i\n", wolf);
  		for (i = 0; i < COUNT_OF_GOATS; i++)
  		{ 
  			//to do read from mmap
  			if (status[i])
  			{
  				if (abs(conn_read(i) - wolf) > (70 / COUNT_OF_GOATS))
  				{
  					status[i] = 0;
  				}
  			}
  			else
  			{
  				if (abs(conn_read(i) - wolf) < (20 / COUNT_OF_GOATS))
  				{
  					status[i] = 1;
  				}
  			}
  		}
		for (i = 0; i < COUNT_OF_GOATS; i++)
  		{
  			count += status[i];
  			printf("check status %i\n", status[i]);
  		}
  		if (count == 0)
  			++countOfDeath; 
  		else
  			countOfDeath = 0;

  		if (countOfDeath >= 2)
  		{   
  			for (i = 0; i < COUNT_OF_GOATS; i++)
  			{
  				status[i] = -1;
  				conn_write(i,status[i]);
  			}
  			printf("end game\n");
  			break;
  		}
	}
	printf("final\n");
	//exit(0);

}


int main(void)
{
	srand(time(NULL));
	pid_t pid;
  	sem_t* sem[COUNT_OF_GOATS], *semParent;// semaphores for every proccess
  	int i;
  	char* namesSemaphores[COUNT_OF_GOATS] = {"/sem1","/sem2","/sem3","/sem4","/sem5","/sem6","/sem7"};
  	int randSeed[COUNT_OF_GOATS + 1];//seeds for every proccess
  	for (i = 0; i < COUNT_OF_GOATS; i++)
  	{
  		randSeed[i] = rand();
  	}


// work with mmap
  	conn_create();

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
  
	for(i = 0; i < COUNT_OF_GOATS; i++)
	{
		pid = fork();
	  	if(pid)
	  	{
	  		continue;
	  	}
	  	else if (!pid)
	  	{
	  		childFunction(i, randSeed[i], sem[i], namesSemaphores[i], semParent);
	  		exit(0);
	  	}
	  	else
	  	{
	  		printf("Fork failed.\n");
	  	}
	}
	parentFunction(semParent, sem, namesSemaphores);

	conn_delete();
	sem_close(semParent);
	printf("close\n");
	sem_unlink("/semParent");
	for (i = 0; i < COUNT_OF_GOATS; i++)
	{
	 	sem_close(sem[i]);
	 	sem_unlink(namesSemaphores[i]);

	}

	return 0;
}

