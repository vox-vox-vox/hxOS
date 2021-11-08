#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXLINE 80
#define SERV_PORT 8000
/*
socket		int socket(int family,int type,int protocol);
			input:  
			output: 成功返回一个网络文件描述符
			打开一个网络通讯端口。成功的话，就像open()一样返回一个文件描述符
connect		int connect(int sockfd, const struct sockaddr *seraddr, socklen_t addrlen);
			input:sockfd 网络通讯文件描述符；seraddr server的地址,sockaddr类型,不同协议各不相同
			output:成功返回0，失败返回-1
			将参数sockfd去连接server的地址，使sockfd这个网络文件描述符监听seraddr所描述的地址和端口号。connect和bind的参数一致，bind的sockaddr参数是自己的地址，connect的sockaddr是server的地址。
*/

int main(int argc, char *argv[]){
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;
	char *str;

	if(argc !=2){
		fputs("usage: ./client message\n", stderr);
		exit(1);
	}
	str = argv[1];
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	Connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr));

	Write(sockfd,str,strlen(str));

	n=Read(sockfd,buf,MAXLINE);
	printf("Response from server:\n");
	Write(STDOUT_FILENO,buf,n);

	Close(sockfd);
	return 0;
}



















