// Producer

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

  //Get the shared memory ID.
  int shmid = shmget(SHM_KEY, SHM_SIZE, 0);

  if(shmid == -1)
    printf("Error \n");

  //QUEUE* queue = (QUEUE*) shmat(shmid, (void*) 0, 0);

  void* front(shmat(shmid, (void*) 0, 0));

  if(front == (void*) (-1))
    {
      perror("Error with shmat.\n");
      return 2;
    }

  //queue = (QUEUE*) front;

  printf("Size of queue: %lu \n", sizeof(queue));
  printf("Size of queue: %lu \n", sizeof(front));
  
  return 0;
}
