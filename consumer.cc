// Consumer

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

  if(shmid == -1)
    {
      printf("Error with shmid in Consumer. \n");
      return 1;
    }
  
  void* shared_mem(shmat(shmid, NULL, 0));
  
  if(shmat(shmid, NULL, 0) == (void*) (-1))
    {
      perror("Error with shmat in Consumer.\n");
      return 2;
    }

  QUEUE* jobQ;
  jobQ = static_cast<QUEUE*>(shmat(shmid, NULL, 0));


  shmdt((void*) jobQ);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
