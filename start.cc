/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

int main (int argc, char **argv)
{

  int shmid(shmget(SHM_KEY, SHM_SIZE, 0644 | IPC_CREAT));

  return 0;
}
