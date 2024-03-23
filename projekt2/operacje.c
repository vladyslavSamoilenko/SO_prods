#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/sem.h>

int waitSemafor(int semID, int number)
{
   int result;
   struct sembuf operacje;
   operacje.sem_num = number;
   operacje.sem_op = -1;
   operacje.sem_flg = 0 ;
   
   if ( semop(semID, &operacje, 1) == -1 )
   {
      perror("Blad semop (waitSemafor)");
      return -1;
   }
   
   return 1;
}

void signalSemafor(int semID, int number)
{
   struct sembuf operacje;
   operacje.sem_num = number;
   operacje.sem_op = 1;
   operacje.sem_flg = 0;

   if (semop(semID, &operacje, 1) == -1 )
      perror("Blad semop (postSemafor): ");

   return;
}


