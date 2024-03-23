#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
void main()
{
        int ex, wait_wart, status;
        for(int i=1; i<=3; i++)
        {
                switch(fork())
                {
                case -1:
                        perror("fork error");
                        exit(1);
                        break;
                case 0:
                        ex = execl("/usr/bin/find","find",".","-name","*3.c",NULL);
                        if(ex == -1){
                              perror("execl error");
                                exit(1);
                        }
                        break;
                default:
			sleep(1);
                        break;
                }
        }
        for(int j=1;j<=3;j++){
                wait_wart=wait(&status);

              if(wait_wart == -1){
                   perror("wait error");
                     exit(1);
                }

                printf("Proces potomny %d zakonczyl sie z kodem %d\n",wait_wart, status);
                        }

exit(0);
}
