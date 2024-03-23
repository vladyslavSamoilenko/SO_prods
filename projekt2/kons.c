#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include "operacje.c"
struct bufor{
	long mtype;
	int mvalue;
};
int *pam; // dla przyłączenia pamięci dzielonej
#define MAX2 12
#define MAX 10
#define PELNY 2
#define PUSTY 1
#define zapis pam[MAX+1]
#define odczyt pam[MAX]

int main()
{
   key_t klucz, kluczm, kluczs;
   int msgID, shmID, semID, pid;
   int i;
   struct bufor komunikat;
   sleep(1);
   printf("konsument--------------------------------\n");
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
    };

   kluczm=ftok(".",'B');
   kluczs=ftok(".",'C');
   if (semID=semget(kluczs,1,IPC_CREAT|0666)==-1)
        {perror("błąd semafora ");exit (1);};
   semID=semget(kluczs,2,IPC_CREAT|0666);
   shmID=shmget(kluczm, MAX2*sizeof(int), IPC_CREAT|0666);
   pam=(int*)shmat(shmID,NULL,0);
   
   //odbiór odpowiedniego komunikatu
   if (msgrcv(msgID,&komunikat,sizeof(komunikat.mvalue),PELNY,0)==-1)	
		{
         printf("error!\n");
         exit(0);
	}
   //printf("Odebrano komunikat pelny\n"); 
    
    sleep(getpid()%10);		

   
// w sekcji krytycznej (semafor 0) odczyt z pamięci dzielonej spod indeksu oczyt
   waitSemafor(semID, 1);
   pid = pam[odczyt];
   printf("%d: Odczytano %d z %d\n", getpid(), pid, odczyt);
   
// uaktualnienie wskaźnika do odczytu (mod MAX)  
   odczyt = (odczyt+1)%MAX;
   
   signalSemafor(semID, 1);
//wysłanie odpowiedniego komunkatu
   komunikat.mtype=PUSTY;   
   if( msgsnd(msgID,&komunikat,sizeof(komunikat.mvalue),0)==-1)
		{
			printf("blad wyslania kom. pustego\n");
			exit(1);
		};
	//   printf("wyslany pusty komunikat\n");

}

