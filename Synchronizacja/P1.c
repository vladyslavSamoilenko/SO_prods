#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include "sem.c"

void task(int i, int j){
    printf("Sekcja t%d%d procesu o PID=%d\n", i, j, getpid());
    FILE *fd=fopen("wynik.txt", "a");
    fprintf(fd, "Sekcja t%d%d procesu o PID=%d\n", i, j, getpid());
    fclose(fd);
    sleep(1);
}

    int main(){
    key_t key=ftok(".", 'A');
    if(key==-1){
        perror("ftok error P1\n");
        exit(1);
    }

    int semid=semget(key, 0,0);
        if(semid==-1){
            perror("semget error P1\n");
            exit(2);
        }

    sem_p_wait(semid, 2);
    task(1,1);
    task(1,2);
    sem_v_podn(semid, 3);
    
    return 0;
}
