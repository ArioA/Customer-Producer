/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

int main (int argc, char **argv)
{

  if(argc != 2)
    {
      perror("Error: must have one argument in command line. \n");
      return 1;
    }

  int shm_key = SHM_KEY;

  int shmid(shmget(shm_key, SHM_SIZE, 0644 | IPC_CREAT));

  if(shmid == -1)
    {
      printf("Error with shmid in start.\n");
      return 1;
    }

  if(shmat(shmid, NULL, 0) == (void*) (-1))
    {
      perror("Error with shmat in start.\n");
      return 2;
    }

  QUEUE* jobQ;
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0));

  //Default settings.
  jobQ->size = check_arg(argv[1]);
  jobQ->front = 0;
  jobQ->end = 0;

  int semid;
  int sem_key = SEM_KEY;

  semid = sem_create(sem_key, 3); //Create three semaphores.

  if(semid == -1)
    {
      perror("Error with semid in start. \n");
    }

  if(sem_init(semid, 0, 1) == -1) //Mutex semaphore.
    perror("Error with mutex semaphore. \n");

  if(sem_init(semid, 1, 0) == -1) //Fill condition semaphore.
    perror("Error with fill count semaphore. \n");

  if(sem_init(semid, 2, jobQ->size) == -1) //Empty condition semaphore.
    perror("Error with empty count semaphore. \n");
  
  return 0;
}
