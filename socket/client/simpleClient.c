#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../warp.h"
#define MAXLINE 80
#define SERV_PORT 8022
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
/*
提高client容错
1.  client向server发送信息时要判断连接是否依然健在，否则会：write调用将数据放到TCP缓存区，TCP发送，
    server收到但是发现不存在连接，返回一个RST段，client收到RST段后将状态保存在TCP协议层，下一次再发送
    的时候，应用层会收到SIGPIPE信号，从而出现brokenPipe
 */
int main(int argc, char *argv[]){
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	Connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)); //cli连接ser

    while(fgets(buf,MAXLINE,stdin)!=NULL){
        Write(sockfd,buf,strlen(buf));
        n=Read(sockfd,buf,MAXLINE);
        if(n==0){//收到了server端的FIN，说明server关闭了当前连接
            printf("the other side has been closed\n");
            break;// 必须跳出当前循环，因为此时连接已关闭，否则再写会出现BrokenPIPE
        }else{
            Write(STDOUT_FILENO,buf,n);
        }
    }
	Close(sockfd);
	return 0;
}



















