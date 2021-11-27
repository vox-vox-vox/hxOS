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
void readShellNonBlockWithFcntl(void);
void printFileStatusFlag(int , char** );
int main(int argc,char *argv[]){
	//openDemo();
	//readShellBlock();
	//readShellNonBlock();
	//readShellNonBlockWithFcntl();
	printFileStatusFlag(argc, argv);

}

/*
open:	int open(const char *pathname, int flags, mode_t mode); 
		input: 	pathname:文件路径; flag:文件读写方式; mode:文件权限(可选)
		output:	文件描述符
close:	int close(int fd);
		input:	fd:要关闭的文件描述符
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
			input: fd 文件描述符 ; buf 缓冲区 ; count 请求读取的字节数
			output: 成功时返回实际读取的字节数，出错时返回-1 并设置errno，达到文件末尾返回0
			从打开的设备或文件中读取数据，读上来的数据保存在buf(有可能是C的IO缓冲区)中
			一些情况下，实际读取的字节数可能会小于请求读取的字节数，如：
			1. 读常规文件时提前到达末尾。比如count=100，实际只剩30个字节没有读，则返回30，下一次返回为0。
			2. 从终端设备读，通常遇到换行符就提前返回
			3. 网络IO读取
write		ssize_t write(int fd, const void *buf, size_t count);
			input: fd 文件描述符 ; buf 缓冲区 ; count 写入的字节数
			output: 成功时返回实际写入的字节数，出错时返回-1 并设置errno，达到文件末尾返回0
			从buf向打开的文件或设备fd中写数据
			
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
	fd = open("/dev/tty", O_RDONLY|O_NONBLOCK);//重新打开stdin，得到新的文件描述符
											   //O_RDONLY: 只读打开 ，O_NONBLOCK:可以做非阻塞IO 
	if(fd<0){
		perror("open /dev/tty error");
		exit(1);
	}
tryagain:
	n = read(fd,buf,10);
	if(n<0){
		if(errno == EAGAIN){// 如果read阻塞 
			sleep(1);// 休眠1s
			write(STDOUT_FILENO,"pls try again\n",strlen("pls try again"));// 打印 pls try again
			goto tryagain;
		}
		perror("read /dev/tty");
		exit(1);
	}
	write(STDOUT_FILENO,buf,n);
	close(fd);
	return ;
}

/*
移动当前读写位置

lseek		off_t lseek(int fd, off_t offset, int whence)
			input:
			output:
			移动读写位置,同fseek相同
*/

/*
重新调整文件属性
文件的 读、写、追加、非阻塞等标志称为 Flag：File Status Flag

fcntl		int fcntl(int fd,int cmd)
			input: fd 文件描述符；cmd 文件权限操作，如F_GETFL(译为getFlag，即得到文件的Status Flag)
			output:
			不需要重新open文件，可以直接设置读、写、追加、非阻塞等标志 (File Status Flag)

*/


// 利用fcntl调整 /dev/tty 的文件属性，将其变为非阻塞，即可改变stdin的阻塞
void readShellNonBlockWithFcntl(void){
	char buf[10];
	int n;
	int flags;
	flags = fcntl(STDIN_FILENO,F_GETFL);// 得到文件的File Status Flag
	flags = flags | O_NONBLOCK;// 设为非阻塞
	if(fcntl(STDIN_FILENO,F_SETFL,flags)==-1){// 重新设置文件的File Status Flag
		perror("fcntl");
		exit(1);
	}
tryagain:
	n = read(STDIN_FILENO,buf,10);
	if(n<0){
		if(errno == EAGAIN){// 如果read阻塞 
			sleep(1);// 休眠1s
			write(STDOUT_FILENO,"pls try again\n",strlen("pls try again"));// 打印 pls try again
			goto tryagain;
		}
		perror("read /dev/tty");
		exit(1);
	}
	write(STDOUT_FILENO,buf,n);
	return ;
}

// argc: 输入参数个数 argv 输入参数
// 此函数的目的是验证重定向之后文件描述符指向文件的变化
void printFileStatusFlag(int argc, char *argv[]){
	int val;
	if(argc!=2){
		fputs("usage : a.out <descriptor#>\n",stderr);
		exit(1);
	}
	if( (val = fcntl(atoi(argv[1]),F_GETFL)) < 0 ){
		printf("fcntl error for fd %d\n",atoi(argv[1]));
		exit(1);
	}
	switch(val & O_ACCMODE){ //O_ACCMODE 是掩码，用于取出file status flag 的读写位
		case O_RDONLY:
			printf("read only");
			break;
		case O_WRONLY:
			printf("write only");
			break;
		case O_RDWR:
			printf("read write");
			break;
		default:
			fputs("invalid access mode\n", stderr);
			exit(1);
	}
	if(val & O_APPEND)
		printf(", append");
	if(val & O_NONBLOCK)
		printf(", nonblocking");
	putchar('\n');
	return ;	
}

/*
mmap		void *mmap(void *addr, size_t len, int prot, int flag, int filedes, off_t off);
			input:
			output:
			将磁盘的一部分直接映射到内存，这样文件中的位置就有内存地址
*/













































