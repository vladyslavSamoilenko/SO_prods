#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX 20 // pula buforow
#define MAX2 24 // rozmiar pamieci dzielonej

//przydzielanie wartosci komunikatow
#define W1 1 //dla wykluczenie wzajemnego procesów czytających w chwili rozpoczynania i kończenia korzystania z zasobu
#define W2 2 //dla wykluczenie wzajemnego procesów piszących w chwili rozpoczynania i kończenia korzystania z zasobu
#define W3 3 //dla zapewnienia priorytetu pisania nad czytaniem
#define SC 4 //do ochrony wejścia do sekcji krytycznej procesu czytającego
#define SP 5 //dla wykluczenia wzajemnego procesu piszącego względem wszystkich innych procesów

#define P 20 // ilosc procesow

//struktura komunikatu
struct bufor
{
    long mtype;
    int mvalue;
};

int shmID, msgID; //identfikatory

//obsluga handlera
void koniec(int sig)
{
   msgctl(msgID, IPC_RMID, NULL);
   shmctl(shmID, IPC_RMID, NULL);
   printf("MAIN - funkcja koniec sygnal %d: Koniec.\n",sig);
   exit(1);
}


int main()
{
    struct sigaction act;
    act.sa_handler = koniec;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    key_t klucz_kk, klucz_pd; // klucze kolejki i pamieci dzielonej
    struct bufor kom;
    int i;


    if ((klucz_kk = ftok(".", 'A')) == -1) // Stworzenie klucza do kolejki komunikatow
    {
        printf("Blad ftok KK\n");
        exit(1);
    }
    if ((msgID = msgget(klucz_kk, IPC_CREAT | IPC_EXCL | 0666))== -1) // Stworzenie kolejki komunikatow
    {
        printf("blad kolejki komunikatow\n");
        exit(1);
    }


    if ((klucz_pd = ftok(".",'B')) == -1) // Stworzenie klucza do pamieci dzielonej
    {
        printf("Blad ftok PD mainprog\n");
        exit(1);
    }
    if((shmID = shmget(klucz_pd, MAX2 * sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {
        printf("blad pamieci dzielonej\n");
        exit(1);
    }



    // komunikat W1;
    kom.mtype = W1;
    if(msgsnd(msgID, &kom, sizeof(kom.mvalue), 0) == -1)
    {
         printf("blad wyslania kom. pustego\n");
         exit(1);
    }

    // komunikat W2;
    kom.mtype = W2;
    if(msgsnd(msgID, &kom, sizeof(kom.mvalue), 0) == -1)
    {
        printf("blad wyslania kom. pustego\n");
         exit(1);
    }

    // komunikat W3;
    kom.mtype = W3;
    if(msgsnd(msgID, &kom, sizeof(kom.mvalue), 0) == -1)
    {
        printf("blad wyslania kom. pustego\n");
         exit(1);
    }

    // komunikat SC;
    kom.mtype = SC;
    if(msgsnd(msgID, &kom, sizeof(kom.mvalue), 0) == -1)
    {
         printf("blad wyslania kom. pustego\n");
        exit(1);
    }

    // komunikat SP;
    kom.mtype = SP;
    if(msgsnd(msgID, &kom, sizeof(kom.mvalue), 0) == -1)
    {
        printf("blad wyslania kom. pustego\n");
         exit(1);
    }

    for (i = 1; i <= P; i++) // Tworzenie P procesow piszacych
    {
        switch (fork())
        {
        case -1:
            perror("Blad fork (mainprog)");
            exit(2);
        case 0:
            execl("./pisz","pisz", NULL);
        }
    }

    for (i = 1; i <= P; i++) // Tworzenie P procesow czytajacych
    {
        switch (fork())
        {
        case -1:
            printf("Blad fork (mainprog)\n");
            exit(2);
        case 0:
            execl("./czyt","czyt",NULL);
        }
    }

    for (i = 0; i < 2*P; i++){
     wait(NULL);
    }

    msgctl(msgID, IPC_RMID, NULL);
    shmctl(shmID, IPC_RMID, NULL);
    printf("MAIN: Koniec.\n");
}
