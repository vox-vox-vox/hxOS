#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void openDemo(void);
void readShellBlock(void);
void readShellNonBlock(void);

int main(void){
	//openDemo();
	readShellBlock();
	readShellNonBlock();

}

/*
open:	int open(const char *pathname, int flags, mode_t mode); 
		input: 	pathname:文件路径; flag:文件读写方式; mode:文件权限(可选)
		output:	文件描述符
close:	int close(int fd);
		inout:	fd:要关闭的文件描述符
*/

// open 函数的测试
void openDemon(void){
	int fd;
	//fd=open("./exercise/akae.txt",O_APPEND);//以追加方式打开
	//fd=open("./tempFiles/akae.txt",O_RDWR|O_CREAT,0777);//不存在则创建，需要指定mode
	//fd=open("./tempFiles/akae.txt",O_RDWR|O_TRUNC,0777);//如果已存在，并且只写或可读可写，则截断为0字节
	fd=open("./tempFiles/akae.txt",O_RDWR|O_CREAT|O_EXCL,0777);//如果同时指定了O_CREAT,并且文件以存在，则出错返回
	if(fd == -1){
		char *errmsg=strerror(errno);
		printf("err is %s\n",errmsg);
		exit(1);
	}else{
		printf("file open success, fd is %d\n",fd);
	}
	return ;
}

/*
read		ssize_t read(int fd, void *buf,size_t count);
			input: fd 文件描述符 ; buf 缓冲区 ; count 读取的字节数
			output: 成功时返回读取的字节数，出错时返回-1 并设置errno，达到文件末尾返回0
			从打开的设备或文件中读取数据，读上来的数据保存在buf(有可能是C的IO缓冲区)中
write		ssize_t write(int fd, const void *buf, size_t count);
			input: fd 文件描述符 ; buf 缓冲区 ; count 读取的字节数
			output: 成功时返回写入的字节数，出错时返回-1 并设置errno，达到文件末尾返回0
			
			
*/
// 阻塞读终端，输入超过10个字节时terminal会将剩下的字节视为输入命令
void readShellBlock(void){
	char buf[10];
	int n;
	n=read(STDIN_FILENO,buf,10);
	if(n<0){
		perror("read STDIN_FILENO error");
		exit(1);
	}
	write(STDOUT_FILENO,buf,n);
	return ;
}

// open 一个设备时指定了O_NONBLOCK标志，read/write就不会阻塞。程序在开始时在0，1，2文件描述符上自动打开的文件就是终端，但是没有O_NONBLOCK标志。我们可以重新打开一遍/dev/tty，在打开时指定O_NONBLOCK标志
// 非阻塞读终端 同阻塞不同，采取一种轮询的方式，每1s轮询一次，查看是否可以进行read。
void readShellNonBlock(void){
	char buf[10];
	int fd,n;
	fd = open("dev/tty", O_RDONLY|O_NONBLOCK);// O_RDONLY: 只读打开 ，O_NONBLOCK:可以做非阻塞IO 
	if(fd<0){
		perror("open /dev/tty error");
		exit(1);
	}
tryagain:
	n = read(fd,buf,10);
	if(n<0){
		if(errno == EAGAIN){// 如果read阻塞 
			sleep(1);// 休眠1s
			write(STDOUT_FILENO,"pls try again",strlen("pls try again"));// 打印 pls try again
			goto tryagain;
		}
		perror("read /dev/tty");
		exit(1);
	}
	write(STDOUT_FILENO,buf,n);
	close(fd);
	return ;
}





















































