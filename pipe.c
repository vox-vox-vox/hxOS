#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define MAXLINE 80
int main(void){
	int n;
	int fd[2];
	pid_t pid;
	char line[MAXLINE];
	
	if(pipe(fd)<0){ //调用pipe(fd),得到fd[0]为读端，fd[1]为写端
		perror("pipe");
		exit(1);
	}
	if((pid=fork()) < 0){
		perror("fork");
		exit(1);	
	}
	if(pid > 0){/*parent*/
		close(fd[0]);// 父进程关闭读端
		write(fd[1],"hello world\n",12);// 父进程向写端写入
		wait(NULL);
	}else{/*child*/
		close(fd[1]);// 子进程关闭写端
		n = read(fd[0],line,MAXLINE);// 子进程向读端读入
		write(STDOUT_FILENO,line,n);
	}
	return 0;


}
