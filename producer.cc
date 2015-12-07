// Producer

#include<time.h>
# include "helper.h"

int main (int argc, char *argv[])
{
  //Make sure there are correct number of command line arguments.
  if(argc != 3)
    {
      printf("Producer cannot have %d arguments in command line.\n", argc-1);
      printf("Must have 2 arguments.\n");
      
      return 1;
    }

  
  int prodID = check_arg(argv[1]); //Extract Producer's ID.
  int jobs = check_arg(argv[2]); //Extract Producer's number of jobs.
  int key = SHM_KEY;

  //Get the shared memory ID.
  int shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);

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

  //seed the random number generator to time for a bit of variation.

  long start_time = time(NULL);

  srand(start_time);

  for(int k = 0; k < jobs; k++)
    {
      if(k > 0)
	{
	  sleep(rand()%3 + 2); //Preparing next job takes 2-4 seconds.
	}
      JOBTYPE newJob;
      //Job id is one plus the location they occupy in queue.
      newJob.id = (jobQ->end+1); 
      newJob.duration = rand()%6 + 2;

      //DOWN EMPTY COUNT

      //DOWN MUTEX
      jobQ->job[jobQ->end] = newJob;
      //UP MUTEX

      jobQ->size++;
      jobQ->end = (jobQ->end + 1) % MAX_QUEUE_SIZE;

      //UP FILL COUNT

      printf("Producer(%d) time  %li: Job id %d duration %d \n", prodID, 
        (time(NULL) - start_time), newJob.id, newJob.duration);
    }

    printf("Producer(%d) time %li: No more jobs to generate \n", prodID,
        (time(NULL) - start_time));

  shmdt((void*) jobQ);
  shmctl(shmid, IPC_RMID, NULL);
  
  return 0;
}
