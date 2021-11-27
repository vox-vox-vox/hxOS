#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
dup			int dup(int oldfd);
			input: oldfd 原来的文件描述符
			output:	成功时返回新的文件描述符，失败时返回-1
			复制一个文件描述符，指向oldfd所指向的file结构体
dup2		int dup2(int oldfd, int newfd);
			input: oldfd 原先的文件描述符；newfd 新的文件描述符
			output: 成功时返回newfd，失败返回-1
			复制一个文件描述符，同时可以指定新的文件描述符的数值。如果新的文件描述符newfd此时已经打开，则先关闭再进行复制。看起来有
			点绕，实际上相当于把newfd指向的file结构体断开，设定其指向oldfd所指向的file结构体
*/
// dup函数操作demo
int main (void){
	int fd, save_fd;
	char msg[]="this is a test\n";
	fd =open("somefile",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);// 只读，不存在则创建
	if(fd<0){
		perror("open failed");
		exit(1);
	}
	save_fd = dup(STDOUT_FILENO);
	dup2(fd,STDOUT_FILENO);
	close(fd);
	write(STDOUT_FILENO,msg,strlen(msg));
	dup2(save_fd,STDOUT_FILENO);
	write(STDOUT_FILENO,msg,strlen(msg));
	close(save_fd);
	return 0;
}
