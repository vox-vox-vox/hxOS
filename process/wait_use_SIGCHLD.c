#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void childHandler(int sig){
	int stat_val;
	wait(&stat_val);
	printf("child exited with code %d\n",stat_val);
	return;
}

int main(void)
{
	struct sigaction newact,oldact;
	newact.sa_handler=childHandler;
	sigemptyset(&newact.sa_mask);//清空newact的阻塞信号掩码
	newact.sa_flags=0;
	sigaction(SIGCHLD,&newact,&oldact);
	
    pid_t pid;
    pid = fork();
    if (pid<0){
        perror("fork failed");
        exit(1);
    }
    if (pid==0){
        int i;
        for(i=3;i>0;i--){
            printf("This is the child\n");
            sleep(1);
        }
        exit(2);
    }else{
		while(1){
			printf("parent is doing something\n");
			sleep(1);
		}
		sigaction(SIGCHLD,&oldact,NULL);// 恢复SIGALRM原先的sigaction
	}
    return 0;
}
