#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <fcntl.h>

static struct sembuf operacje[5];

void sem_v_podn(int semid, int semnum){
    operacje[semnum].sem_num = semnum;
    operacje[semnum].sem_op = 1;
    operacje[semnum].sem_flg = 0;
    if (semop(semid, &operacje[semnum], 1) == -1){
        perror("Podnoszenie semafora");
        exit(1);
    }
}

void sem_p_wait(int semid, int semnum){
    operacje[semnum].sem_num = semnum;
    operacje[semnum].sem_op = -1;
    operacje[semnum].sem_flg = 0;
    if (semop(semid, &operacje[semnum], 1) == -1){
        perror("Opuszczenie semafora");
        exit(1);
    }
}
