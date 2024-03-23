#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
  int mainpid = getpid();
  char cmd[20];
  sprintf(cmd, "pstree -ph %d", mainpid);
  printf("Proces macierzysty 1 - PID: %d, PPID: %d\n\n", getpid(), getppid());

  for (int i = 1; i <= 3; i++)
  {
       
        switch (fork())
        {
        case -1:
          perror("Fork failed");
          exit(1);
          break;
        case 0:
         printf("Proces potomny %d - PID: %d , PPID: %d \n\n", i, getpid(), getppid());
         break;
        
        default:
          printf("##Proces macierzysty %d\n", i);
        }
        system(cmd);
        printf("\n");
        sleep(1); 
  }
 

  return 0;
}
