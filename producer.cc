// Author: Ario Aliabadi
// Last Edited: 11/12/2015
// CID: 01110168

//This program models the behaviour of a job producer in a consumer-producer
//scenario, using semaphores.

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

if(check_arg(argv[1]) == -1)
    {
      perror("Invalid command line argument for producer ID in producer.cc\n");
      return 1;
    }

 if(check_arg(argv[2]) == -1)
    {
      perror("Invalid command line argument");
      perror(" for producer jobs in producer.cc\n");
      return 1;
    }
  
  int prodID = check_arg(argv[1]); //Extract Producer's ID.
  int jobs = check_arg(argv[2]); //Extract Producer's number of jobs.
  int key = SHM_KEY; //Extract the shared memory key from helper.h

  //Get the shared memory ID.
  int shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);

  if(shmid == -1) //Check that memory was succesfully created.
    {
      printf("Error with shmid in Producer.\n");
      return 1;
    }

  //Check that memory was successfully attached.
  if(shmat(shmid, NULL, 0) == (void*) (-1)) 
    {
      perror("Error with shmat in Producer.\n");
      return 2;
    }

  QUEUE* jobQ; //Pointer to QUEUE object in shared memory.
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0)); 

  int semid;
  int sem_key = SEM_KEY;
  
  //Attach the three semaphores, extract semaphore ID.
  semid = sem_attach(sem_key);

  //seed the random number generator to time for a bit of variation.

  sleep(1); //Wait for newline from consumers (purely aethetic).

  long start_time = time(NULL); //Take the current time.

  srand(start_time); //Seed random number generator for a bit of variation.

  for(int k = 0; k < jobs; k++) //For each job...
    {
      if(k > 0)
	{
	  sleep(rand()%3 + 2); //Preparing next job takes 2-4 seconds.
	}

      sem_wait(semid, 2);//Down on empty count.

      sem_wait(semid, 0); //Down on mutex.

      JOBTYPE newJob; //Create new job.

      //Job id is one plus the location they occupy in queue.
      newJob.id = (jobQ->end+1); 

      //Job duration can be between 2 and 7 seconds.
      newJob.duration = rand()%6 + 2;

      jobQ->job[jobQ->end] = newJob; //Add newJob to end of the queue.

      //Increment end position of cirular queue.
      jobQ->end = (jobQ->end + 1) % jobQ->size;

      printf("Producer(%d) time  %li: Job id %d duration %d \n", prodID, 
        (time(NULL) - start_time), newJob.id, newJob.duration);

      sem_signal(semid, 0); //Up on mutex 

      sem_signal(semid, 1); //Up on fill count.
    }

    printf("Producer(%d) time  %li: No more jobs to generate \n", prodID,
        (time(NULL) - start_time));

    sleep(600); //Wait for consumers to finish consuming.
          //If there are no consumers, naturally this will result in deadlock.
          //Shared memory and semaphores are deleted in consumer.cc.
    
  return 0;
}
