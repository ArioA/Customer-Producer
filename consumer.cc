// Consumer
#include <time.h> //Used for counting seconds.
# include "helper.h"

int main (int argc, char *argv[])
{
  if(argc != 2) //If incorrect number of command line arguments.
    {
      perror("Error: must have 1 argument in command line.\n");
      return 2;
    }

  int consID = check_arg(argv[1]); //Consumer's ID.
  int key = SHM_KEY; //Get the shared memory key.

  //Create shared memory, and get it's ID. 
  int shmid(shmget(key, SHM_SIZE, 0644 | IPC_CREAT));

  if(shmid == -1) //If error with creating shared memory.
    {
      perror("Error with shmid in consumer.cc. \n");
      return 1;
    }
  
  QUEUE* jobQ; //Pointer to shared memory.
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0));

  if(shmat(shmid, NULL, 0) == (void*) (-1)) //If error attaching shared memory.
    {
      perror("Error with shmat in consumer.cc.\n");
      shmdt((void*) jobQ);
      shmctl(shmid, IPC_RMID, NULL);
      return 2;
    }

  int semid;
  int sem_key = SEM_KEY;
  
  semid = sem_attach(sem_key); //Attach the three semaphores.

  if(semid == -1)
    {
      perror("Error attatching semaphores in consumer.cc. \n");
      return 1;
    }

  long start_time = time(NULL); //Mark the start time of jobs processing.
  int id;
  int duration;

  sem_wait(semid, 0); //Down on Mutex

  jobQ->activeConsumers++; //New consumer.

  sem_signal(semid, 0); //Up on Mutex
  
  if(jobQ->activeConsumers == 0)
    {
      printf("\n");
    }

  while(true)
    {
      if(sem_timewait(semid, 1, 10) == -1) //If blocked for 10 seconds...
	{
	  printf("Consumer(%d) time  %li: No jobs left. \n", consID,
		 (time(NULL) - start_time));
	  
	  sem_wait(semid, 0); //Down on Mutex

	  jobQ->activeConsumers--; //Lost a consumer.

	  sem_signal(semid, 0); //Up on Mutex
	 
	  if(jobQ->activeConsumers == 0) //If last consumer.
	    {

	      shmdt((void*) jobQ); //Detach shared memory pointer.
	      shmctl(shmid, IPC_RMID, NULL); //Delete shared memory.
	      
	      if(sem_close(semid) == -1) //Get rid of semaphores.
		{
		  perror("Error closing semaphores in consumer.cc.\n");
		  return 1;
		}
	    }
	  else //If not last consumer...
	    sleep(300); //...wait for last consumer to clean up.

	  return 0;
	}
      else //If found a new job to consume.
	{
	  sem_wait(semid, 0); //Down on Mutex

	  duration = jobQ->job[jobQ->front].duration;
	  id = jobQ->job[jobQ->front].id;

	  //Increment front of circular queue.
	  jobQ->front = (jobQ->front + 1) % jobQ->size; 

	  printf("Consumer(%d) time  %li: Job id %d", 
		 consID,(time(NULL) - start_time), id);
	  printf(" executing sleep duration %d \n", duration);

	  sem_signal(semid, 0); //Up on Mutex

	  //Consume job.
	  sleep(duration);

	  printf("Consumer(%d) time  %li: Job id %d", 
		 consID,(time(NULL) - start_time), id);
	  printf(" completed. \n");

	  sem_signal(semid, 2); //Up on empty count.
	}
    }

  shmdt((void*) jobQ);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
