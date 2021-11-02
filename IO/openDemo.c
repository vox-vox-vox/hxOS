#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
/*
open:	int open(const char *pathname, int flags, mode_t mode); 
		input: 	pathname:文件路径; flag:文件读写方式; mode:文件权限(可选)
		output:	文件描述符
close:	int close(int fd);
		inout:	fd:要关闭的文件描述符
*/
int main(void){
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
	return 0;
}
