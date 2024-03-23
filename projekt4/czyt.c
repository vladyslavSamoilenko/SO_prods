#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

struct bufor
{
    int mtype;
    int mvalue;
};

int *pam;

#define MAX 20
#define MAX2 24

#define odczyt pam[MAX]
#define zapis pam[MAX + 1]
#define l_pisz pam[MAX + 2] // lp
#define l_czyt pam[MAX + 3] // lc

#define W1 1 //dla wykluczenie wzajemnego procesów czytających w chwili rozpoczynania i kończenia korzystania z zasobu
#define W2 2 //dla wykluczenie wzajemnego procesów piszących w chwili rozpoczynania i kończenia korzystania z zasobu
#define W3 3 //dla zapewnienia priorytetu pisania nad czytaniem
#define SC 4 //do ochrony wejścia do sekcji krytycznej procesu czytającego
#define SP 5 //dla wykluczenia wzajemnego procesu piszącego względem wszystkich innych procesów


int main()
{
   key_t klucz_kk, klucz_pd;
   int msgID, shmID;
   int i;
   struct bufor kom;


   if ((klucz_kk = ftok(".", 'A')) == -1)
   {
      printf("Blad ftok (A)\n");
      exit(2);
   }
   msgID = msgget(klucz_kk, IPC_CREAT | 0666);
   if (msgID == -1)
   {
      printf("blad klejki komunikatow\n");
      exit(1);
   }



   klucz_pd = ftok(".",'B');
   shmID = shmget(klucz_pd, MAX2*sizeof(int), IPC_CREAT|0666);
   pam = (int*) shmat(shmID, NULL, 0);

    int pid = getpid();
    sleep(pid%10);

    msgrcv(msgID, &kom, sizeof(kom.mvalue), W3, 0);// wait(W3)
    
    msgrcv(msgID, &kom, sizeof(kom.mvalue), SC, 0); // wait(SC)
    
    if(msgrcv(msgID, &kom, sizeof(kom.mvalue), W1, 0) == -1){} // wait(W1)
      l_czyt = l_czyt + 1; // lc = lc + 1
      if(l_czyt == 1){
         msgrcv(msgID, &kom, sizeof(kom.mvalue), SP, 0);// wait(SP)
      } 
    kom.mtype = W1;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0); // signal(W1)
    kom.mtype = SC;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0);// signal(SC)
    kom.mtype = W3;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0);// signal(W3)
    fprintf(stderr, " Odczytal %d z bufora %d\n",pam[odczyt], odczyt);// czytanie
    odczyt = (odczyt + 1) % MAX; // liczenie bufora
    msgrcv(msgID, &kom, sizeof(kom.mvalue), W1, 0); // wait(W1)
    
    l_czyt = l_czyt - 1;  // lc = lc - 1
    if (l_czyt == 0){  // if lc = 0 then
      kom.mtype = SP;
      msgsnd(msgID, &kom, sizeof(kom.mvalue), 0);// signal(SP)
    }            
    kom.mtype = W1;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0);// signal(W1)
   
}
