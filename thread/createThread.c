#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* 下面是一个运用pthread库创建线程的demo  暂时会在16行报错*/

pthread_t ntid;

void printids (const char *s){
	pid_t		pid;// process id
	pthread_t	tid;// thread id
	pid = getpid();
	tid= pthread_self();
	//printf("%s pid %u tid %u (0x%x)\n",s,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid);
	printf("%s pid %u  \n",s,(unsigned int)pid);
}

void *thr_fn(void *arg){
	printids(arg);
	return NULL;
}

int main (void){
	int err;
	err = pthread_create( &ntid , NULL , thr_fn , "new thread: " ); // 函数原型：int pthread_create(A,B,C,D) A:thread id B:暂时未知 C:thread 执行的函数对应的指针 D:输入thread的参数
	if (err!=0){
		fprintf(stderr,"can not create thread : %s\n", strerror(err));
		exit(1);
	}
	printids("main thread: ");
	sleep(1);
	return 0;
}


