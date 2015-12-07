/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

int main (int argc, char **argv)
{

  int key = SHM_KEY;

  int shmid(shmget(key, SHM_SIZE, 0644 | IPC_CREAT));

  void* shared_mem(shmat(shmid, NULL, 0));

  if(shmid == -1)
    {
      printf("Error with shmid in Producer.\n");
      return 1;
    }

  void* shared_mem(shmat(shmid, NULL, 0));

  if(shmat(shmid, NULL, 0) == (void*) (-1))
    {
      perror("Error with shmat in Producer.\n");
      return 2;
    }

  QUEUE* jobQ;
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0));

  //Default settings.
  jobQ->size = 0;
  jobQ->front = 0;
  jobQ->end = 0;


  shmdt((void*) jobQ);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
