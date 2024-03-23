#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "operacje.c"
#include <time.h>
#include <sys/sem.h>

struct bufor{
	long mtype;
	int mvalue;
};

int *pam;

#define MAX 10
#define MAX2 12
#define PELNY 2
#define PUSTY 1
#define odczyt pam[MAX]
#define zapis pam[MAX+1]
int main()
{
   key_t klucz, kluczm,kluczs;
   int msgID, pid;
   int shmID, semID;
   int i;
   time_t czas;   
   struct bufor komunikat;

   if ( (klucz = ftok(".", 'A')) == -1 )
   {
      printf("Blad ftok (A)\n");
      exit(2);
   };

   msgID=msgget(klucz, IPC_CREAT | 0666);
   if (msgID==-1)
   { 
      printf("blad klejki komunikatow\n");
      exit(1);
   }
   kluczm= ftok(".", 'B');
   kluczs= ftok(".", 'C');
   shmID=shmget(kluczm, MAX2*sizeof(int),IPC_CREAT|0666);
   pam=(int*)shmat(shmID, NULL, 0);
   semID=semget(kluczs,2,IPC_CREAT|0666);
   
   // odbi�r komunikatu
   if (msgrcv(msgID, &komunikat, sizeof(komunikat.mvalue),PUSTY,0)==-1)	
		{
         printf("error!\n");
         exit(0);
	}
   



   pid = getpid();
   sleep(pid%10);
  // w sekcji krytycznej  zapis PID-u (getpid()) do puli bufor�w pod indeks zapis
   waitSemafor(semID, 0);
   pam[zapis] = pid;
  //wypisanie informacji na ekranie
  printf("%d: Zapisano %d do %d\n", getpid(), pid, zapis);
  //modyfikacja indeksu do zapisu


  zapis = (zapis+1)%MAX;
  //wys�anie odpowiedniego komunikatu
  signalSemafor(semID, 0);
  komunikat.mtype=PELNY;
  if( msgsnd(msgID,&komunikat,sizeof(komunikat.mvalue),0)==-1)
		{
			printf("blad wyslania kom. pelnego\n");
			exit(1);
		};
  
   
 }
