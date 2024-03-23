#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

struct bufor
{
    int mtype;
    int mvalue;
};

int *pam;
//dostepna pula buforow
#define MAX 20
#define MAX2 24


#define odczyt pam[MAX]
#define zapis pam[MAX + 1]
#define l_pisz pam[MAX + 2] // lp
#define l_czyt pam[MAX + 3] // lc

//przydzielanie wartosci komunikatow
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
    //kolejka komunikatow
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

    //pamiec dzielona
    klucz_pd = ftok(".", 'B');
    shmID = shmget(klucz_pd, MAX2 * sizeof(int), IPC_CREAT|0666);
    pam = (int*)shmat(shmID, NULL, 0);

    int pid = getpid();
    sleep(getpid()%10);

    msgrcv(msgID, &kom, sizeof(kom.mvalue), W2, 0); // wait(W2)
     l_pisz = l_pisz + 1; // lp = lp + 1
    
     if(l_pisz == 1){
         msgrcv(msgID, &kom, sizeof(kom.mvalue), SC, 0); // wait(SC)
     }
    kom.mtype = W2;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0); // signal(W2)


    msgrcv(msgID, &kom, sizeof(kom.mvalue), SP, 0); // wait(SP)
    pam[zapis] = getpid(); // pisanie

    fprintf(stderr, "Zapisal %d do buforu %d\n", pam[zapis], zapis); // pisanie

    zapis = (zapis + 1) % MAX; // liczenie bufora
    kom.mtype = SP;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0); // signal(SP)
    



    msgrcv(msgID, &kom, sizeof(kom.mvalue), W2, 0); // wait(W2)
    l_pisz = l_pisz - 1; // lp = lp - 1
    
    if (l_pisz == 0){ // if lp = 0 then
        kom.mtype = SC;
        msgsnd(msgID, &kom, sizeof(kom.mvalue), 0); // signal(SC)
    }
    kom.mtype = W2;
    msgsnd(msgID, &kom, sizeof(kom.mvalue), 0); // signal(W2)

}
