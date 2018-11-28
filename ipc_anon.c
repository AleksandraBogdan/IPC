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
#include <sys/sysinfo.h>

#define BUFFER_SIZE 80
struct sysinfo info;

int randomTime()
{
  int time = clock();
  return time;
}

int rndmgnrt()
{
  //рандом
  int randTime, randLib, bufferRam, procs;
  randLib = rand();
  //uptime = (int)info.uptime;
  procs = (int)info.procs;
  bufferRam = (int)info.bufferram;
  randTime = (int)randomTime();
  return ((randLib ^ randTime) * (bufferRam ^ procs));
}

int wolfOrGoat(int live)
{
  int A = 1, B = 100;
  if (!live)
  {
    B = 50;
  }
  return A + rand() % (B - A + 1);

}


int main(void)
{
  sysinfo(&info);
  srand(time(NULL));
  int pid;
  void* buf;
  sem_t* sem;
  int wolf, countOfGoats = 1, i;
  int status[1] = {1};// 1- live, 0 -dead

  buf = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);//create a new mapping in the virtual adress space(1 - starting address for mapping(kernel chooses the address), 2 - length of the mapping, 3- desired memory protection(may read, may write), 4 - visible to other proccesses mapping the same region or files(MAP_ANON - mapping is not backed by any files); 5,6 -should be like this because of MAP_ANON)
  printf("Mapped at %p\n", buf);
  if (buf == MAP_FAILED)
  {
    printf("Something went wrong with mmap...\n");
  }
  sem = sem_open("/semsem", O_CREAT | O_EXCL, S_IRWXU, 0);//create or open semaphore(0 - initial value for semaphore)
  if(sem == SEM_FAILED)
  {
    printf("Something went wrong...\n");
  }

  
  int *shared = buf;
  pid = fork(); //create child proccess
  if(pid != 0)//parent proccess
  {
    sem_wait(sem);// block semaphore
    wolf = rndmgnrt();
    printf("wolf = %d\n", wolf);
    printf("goal = %d\n", shared[0]);
    for (i = 0; i < countOfGoats; i++)
    {
      if (status[i])
      {
        if (abs(shared[i] - wolf) < (70 / countOfGoats))
        {
          continue;
        }
        else
        {
          status[i] = 0;
        }
      }
      else
      {
        if (abs(shared[i] - wolf) < (20 / countOfGoats))
        {
          status[i] = 1;
        }
      }
    }
    if (munmap(buf, BUFFER_SIZE) == -1) //unmap
    {
      printf("Something went wrong with munmap...\n");
    }
    sem_close(sem);//close semaphore
    sem_unlink("/semsem");//remove semaphore
  }
  else if(pid == 0)//child proccess
  {
    sem = sem_open("/semsem", 0);
    if(sem == SEM_FAILED)
    {
      printf("Something went wrong...\n");
    }

    //strcpy(buf, "hello\n");
    shared[0] = rndmgnrt();
    sem_post(sem);//unblock semaphore
    sem_close(sem);//close semaphore
  }
  else
  {
    printf("Fork failed!\n");
  }

  return 0;
}
