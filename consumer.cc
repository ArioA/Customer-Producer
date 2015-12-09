// Consumer
#include <time.h>
# include "helper.h"

int main (int argc, char *argv[])
{
  if(argc != 2)
    {
      printf("Producer cannot have %d arguments in command line.\n", argc);
      printf("Must have 1 arguments.\n");
    }

  int consID = check_arg(argv[1]);
  int key = SHM_KEY;

  int shmid(shmget(key, SHM_SIZE, 0644 | IPC_CREAT));
  bool last_consumer = false;

  if(shmid == -1)
    {
      perror("Error with shmid in Consumer. \n");
      return 1;
    }
  
  QUEUE* jobQ;
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0));

  if(shmat(shmid, NULL, 0) == (void*) (-1))
    {
      perror("Error with shmat in Consumer.\n");
      shmdt((void*) jobQ);
      shmctl(shmid, IPC_RMID, NULL);
      return 2;
    }

  int semid;
  int sem_key = SEM_KEY;
  
  semid = sem_attach(sem_key); //Attach the three semaphores.

  long start_time = time(NULL); //Mark the start time of jobs processing.

  int the_front(0);
  
  while(true)
    {
      int timewait = sem_timewait(semid, 1, 10);

      if(timewait == -1)
	{
	  printf("Consumer(%d) time  %li: No jobs left. \n", consID,
		 (time(NULL) - start_time));
	  
	  if(last_consumer)
	    {
	      if(sem_close(semid) == -1) //Get rid of semaphores.
		perror("Error closing semaphores in consumer.\n");
	      
	      shmdt((void*) jobQ);
	      shmctl(shmid, IPC_RMID, NULL);
	    }

	  sleep(10);

	  return 0;
	}
      else
	{
	  last_consumer = false;

	  sem_wait(semid, 0); //Down on Mutex

	  the_front = jobQ->front;

	  jobQ->front = (jobQ->front + 1) % jobQ->size;

	  if(jobQ->front == jobQ->end)
	    {
	      last_consumer = true;
	    }

	  sem_signal(semid, 0); //Up on Mutex

	  printf("Consumer(%d) time  %li: Job id %d", 
		 consID,(time(NULL) - start_time), jobQ->job[the_front].id);
	  printf(" executing sleep duration %d \n", 
		 jobQ->job[the_front].duration);

	  sleep(jobQ->job[the_front].duration);

	  printf("Consumer(%d) time  %li: Job id %d", 
		 consID,(time(NULL) - start_time), jobQ->job[the_front].id);
	  printf(" completed. \n");

	  sem_signal(semid, 2); //Up on empty count.
	}
    }

  shmdt((void*) jobQ);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
