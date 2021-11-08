#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
打印错误并退出的函数
*/
void perr_exit(const char *s){
	perror(s);
	exit(1);
}

/*
accept 的包装函数 失败自动重试
*/
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr){
	int n;
again:
	if( (n=accept(fd,sa,salenptr))<0 ){
		if((errno == ECONNABORTED)||(errno == EINTR))
			goto again;
		else
			perr_exit("accept error");
	}
	return n;
}

/*
bind 的包装函数
*/
void Bind(int fd, const struct sockaddr *sa, socklen_t salen){
	if( bind(fd,sa,salen) < 0)
		perr_exit("bind error");
}

/*
Connect 的包装函数
*/

void Connect(int fd, const struct sockaddr *sa,socklen_t salen){
	if(connect(fd,sa,salen)<0){
		perr_exit("connect error");
	}
}

/*
Listen 的包装函数
*/
void Listen(int fd, int backlog){
	if(listen(fd,backlog)<0){
		perr_exit("listen error");
	}
}

/*
Socket 的包装函数
*/
int Socket(int family, int type, int protocol){
	int n;
	if((n = socket(family,type,protocol))<0){
		perr_exit("socket error");
	}
	return n;
}

/*
Read 的包装函数
Read 被信号中断时会重试
*/
ssize_t Read(int fd, void *ptr,size_t nbytes){
	ssize_t n;
again:
	if(	(n = read(fd , ptr , nbytes )) ==-1){
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

/*
Write 的包装函数
Write 被信号中断时会重试
*/
ssize_t Write(int fd, const void *ptr,size_t nbytes){
	ssize_t n;
again:
	if((n=write(fd,ptr,nbytes)) == -1){
		if(errno== EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

/*
Close 的包装函数
*/
void Close(int fd){
	if(close(fd)==-1)
		perr_exit("close error");
}

/*
TCP 面向流，每一次能从接收缓存区读取的数据大小不确定，往往能读到的数据小于期望读到的数据
为了确保读一个我们所期望的字节数，写一个包装函数Readn，如果一次读到的数据小于期望的数据，则继续进行read操作，直至读完。
*/
// 一个从网络文件读n个字节，且保证读到n个字节才返回的包装函数
ssize_t Readn(int fd, void *vptr,size_t n){
	size_t	 nleft; //剩余字节数
	ssize_t	 nread;	//一次读取的字节数
	char	 *ptr;	//缓冲区指针
	ptr=vptr;	//初始缓冲区
	nleft=n;	//初始剩余n字节
	while(nleft>0){
		if((nread=read(fd,ptr,nleft))<0){
			if(errno == EINTR)	nread=0;// 如果被信号打断，则重新读取
			else return -1;
		}
		nleft -= nread;
		ptr += nread;
	}
	return n-nleft;
}

/*
TCP面向流，每一次如果请求写100个字节，而发送缓冲区里只有20个字节的空闲位置，write会阻塞。
为了确保写入我们所期望的字节数，准备一个包装函数Writen，如果一次可以写入的数据小于期望的数据，则继续进行写操作，直至写完
*/
// 一个向网络文件写n个字节，且一定会写完的包装函数
ssize_t Writen(int fd,const void *vptr, size_t n){
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr=vptr;
	nleft=n;
	while(nleft>0){
		if((nwritten=write(fd,ptr,nleft))<=0){
			if(errno==EINTR&&nwritten<0) nwritten=0;// 如果被信号打断，则重新写入
			else return -1;
		}
		nleft-= nwritten;
		ptr += nwritten;
	}
	return n;
}


/*
ReadLine辅助函数
output: -1 出现错误 0 
*/
static ssize_t my_read(int fd,char *ptr){
	static int read_cnt;
	static char* read_ptr;
	static char read_buf[100];

	if(read_cnt <=0){
	again:
		if((read_cnt = read(fd,read_buf,sizeof(read_buf)))<0){
			if(errno == EINTR)
				goto again;
			return -1;
		}else if(read_cnt == 0){
			return 0;
		}else{
			read_ptr = read_buf;
		}
	}
	read_cnt --;
	*ptr = *read_ptr++;
	return 1;
}
/*
常见的应用层协议都是带有可变长字段的，字段之间用换行的比用'\0'的更常见，如HTTP协议。可变长字段用readn来读很不方便，为此实现一个类似fgets的 Readline
*/
ssize_t ReadLine(int fd, void *vptr, size_t maxlen){
	ssize_t n,rc;
	char c,*ptr;
	ptr = vptr;
	for(n=1;n<maxlen;n++){
		if((rc = my_read(fd,&c))==1){
			*ptr++ =c;
			if(c == '\n')
				break;
		}else if( rc ==0){
			*ptr = 0;
			return n-1;
		}else{
			return -1;
		}
	}
	*ptr=0;
	return n;
}



























