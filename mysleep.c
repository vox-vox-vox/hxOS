#include <unistd.h>
#include <signal.h>
#include <stdio.h>

/*
struct sigaction{

	void 		(*sa_handler)(int);							// signal handler 地址 or SIG_IGN(忽略信号) or SIG_DFL(执行系统默认)
	sigset_t 	sa_mask;									// additional signals to block
	int			sa_flags;									// signal options
	void		(*sa_sigaction)(int,siginfo_t *,void *);	//alternate handler

}
*/

void sig_alrm(int signo){
	//nothing to do
}

unsigned int mysleep(unsigned int nsecs){
	struct sigaction newact, oldact;
	unsigned int unslept;

	newact.sa_handler = sig_alrm;// 设置newact的自定义处理函数为sig_alrm
	sigemptyset(&newact.sa_mask);//清空newact的阻塞信号掩码
	newact.sa_flags = 0;
	
}
