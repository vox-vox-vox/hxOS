#include <unistd.h>
#include <signal.h>
#include <stdio.h>

/*
struct sigaction{

    void        (*sa_handler)(int);                         // signal handler 地址 or SIG_IGN(忽略信号) or SIG_DFL(执行系统默认)
    sigset_t    sa_mask;                                    // additional signals to block
    int         sa_flags;                                   // signal options
    void        (*sa_sigaction)(int,siginfo_t *,void *);    //alternate handler

}

pause()使进程挂起直至有信号递达，之后按照信号捕捉流程处理
alarm(seconds)告诉内核在seconds秒后向当前进程发送 SIGALRM 信号，该信号的默认处理方式是终止当前进程，但是也可以自定义捕捉方式。
*/

void sig_alrm(int signo){
    //nothing to do
}

unsigned int mysleep(unsigned int nsecs){
    struct sigaction newact, oldact;
	sigset_t newmask,oldmask,suspmask;
    unsigned int unslept;

    newact.sa_handler = sig_alrm;// 设置newact的自定义处理函数为sig_alrm
    sigemptyset(&newact.sa_mask);//清空newact的阻塞信号掩码
    newact.sa_flags = 0;//默认为0，不详细解释
    sigaction(SIGALRM,&newact,&oldact);

	/*block SIGALRM */
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGALRM);
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);

    // 定时nsecs 秒，在此过程中进程挂起 nsecs后SIGALRM信号到达
    alarm(nsecs);
    // 挂起进程，进入等待，直至SIGALRM到达
	// 此时可能会出现调用pause()前SIGALRM先到并被捕捉的情况(nsecs很短，alarm()执行完以后立即被调度去了高优先级进程，SIGALRM到达时还没有执行pause())，所以要在alarm之后对SIGALRM进行屏蔽，防止其在执行到pause()之前被提前捕捉
	suspmask = oldmask;
	sigdelset(&suspmask,SIGALRM);
    sigsuspend(&suspmask);// pause()改为sigsuspend(suspmask), 调用sigsuspend时进程的信号屏蔽由sigmask指定，可以利用这一点临时解除对SIGALRM的屏蔽。在调用结束后进程的信号屏蔽字恢复为原来的值

    // 此时，SIGALRM处理完毕

    unslept=alarm(0);// alarm(0)取消闹钟
    sigaction(SIGALRM,&oldact,NULL);// 恢复SIGALRM原先的sigaction
	sigprocmask(SIG_SETMASK,&oldmask,NULL);// 恢复原先的进程信号屏蔽集
	
    return unslept;
}
int main(void){
    while(1){
        mysleep(2);
        printf("Two seconds passed\n");
    }
    return 0;
}

