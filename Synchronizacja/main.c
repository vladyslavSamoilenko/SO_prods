#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sem.c"

int main(){
int id1, id2, id3;

key_t key=ftok(".", 'A');
    if(key==-1){
        perror("ftok error\n");
        exit(1);
    }
int semid=semget(key, 5, IPC_CREAT|0666);
    if(semid==-1){
        perror("semget error\n");
        exit(2);
    }

for(int i=0; i<0;i++){
semctl(semid,i,SETVAL,0);
}

id1=fork();
    if(id1==0){
        if(execl("./P1","P1",NULL)==-1){
            perror("exec error P1 \n");
            exit(3);
        }
    exit(EXIT_SUCCESS);
    }
    else if(id1==-1){
        perror("fork error \n");
        exit(4);
    }

id2=fork();
    if(id2==0){
        if(execl("./P2","P2",NULL)==-1){
        perror("exec error P2\n");
        exit(5);
    }
    exit(EXIT_SUCCESS);
    }
    else if(id2==-1){
        perror("fork error \n");
        exit(6);
    }

id3=fork();
    if(id3==0){
        if(execl("./P3","P3",NULL)==-1){
            perror("exec error P3\n");
            exit(7);
        }
        exit(EXIT_SUCCESS);
    }
    else if(id3==-1){
    perror("fork error \n");
    exit(8);
    }

    for(int i=0; i<3; i++){ 
        wait(NULL);
    }
     
    if((semctl(semid, 0, IPC_RMID))==-1){
        perror("semctl error \n");
        exit(9);
    }

    return 0;
}
