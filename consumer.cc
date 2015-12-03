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

  int shmid(shmget(SHM_KEY, SHM_SIZE, 0644 | IPC_CREAT));
  
  void* data(shmat(shmid, (void*) 0, 0));

  return 0;
}
