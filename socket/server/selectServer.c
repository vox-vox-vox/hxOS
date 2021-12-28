//
// Created by huxiao on 2021/11/27.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include "../warp.h"
/*
socket地址
	socketAPI是一层抽象的网络编程接口，适用于各种底层协议，如IPV4、IPV6、UNIX Domain Socket等，但是各种网络协议的地址不相同:
	IPV4:IPV4地址用 sockaddr_in 表示，包括16位端口号和32位IP地址
	IPV6:IPV6地址用 sockaddr_in6 表示，包括16端口号、128IP地址和一些控制字段
	UNIX Domain Socket:用sockaddr_un结构体表示
虽然地址不同，但也有一部分相同。各种socket地址结构体的开头是相同的，前16位是长度，后16位表示整个地址类型。
这样，为了编程的通用性，socket地址统一都用 struct sockaddr* 类型表示，只需要取得sockaddr结构体中的首地址，可以根据地址类型字段确定sockaddr中的内容。

*/

/*
socket		int socket(int family,int type,int protocol);
			input:
			output: 成功返回一个网络文件描述符
			打开一个网络通讯端口。成功的话，就像open()一样返回一个文件描述符
bind		int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
			input:sockfd 网络通讯文件描述符；myaddr 地址,sockaddr类型,不同协议各不相同
			output:成功返回0，失败返回-1
			将参数sockfd和myaddr绑定在一起，使sockfd这个网络文件描述符监听myaddr所描述的地址和端口号
listen		int listen(int sockfd, int backlog);
			input:
			output:成功返回0，失败返回-1
			声明sockfd处于监听状态，允许有backlog个客户端处于连接状态，收到更多的连接请求则忽略
accept		int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
			input: sockfd 网络文件描述符；cliaddr 传出参数,返回客户端地址和端口号；addrlen 传入传出参数,传入cliaddr长度,传出客户端地址结构体实际长度，传NULL说明不关系客户端地址
			output: 网络文件描述符，相当于一个可以读写的文件
			服务器调用accept()接受连接。如果此时还没有客户端的连接请求，就阻塞直至有客户端连接上来。

*/
#define MAXLINE 100
#define SERV_PORT 8022

void childHandler() {
    int stat_val;
    wait(&stat_val); // 清理子进程
    printf("child exited with code %d\n", stat_val);
    return;
}

/*
 提高server容错
 server断开，client未断开时，TCP连接还没有完全中断，此时无法重启server，会出现端口占用错误
 解决方法见 opt=1 处
*/
int main(void){
    int i,maxi,maxfd,listenfd,connfd,sockfd;
    int nready, client[FD_SETSIZE];// FD_SETSIZE 被定义为"#define FD_SETSIZE __DARWIN_FD_SETSIZE "，而"__DARWIN_FD_SETSIZE"就是fd_set 的最大bit数：" #define __DARWIN_FD_SETSIZE 1024"
    ssize_t n;
    fd_set rset, allset; // rset：recent fd_set
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    socklen_t cliaddr_len;
    struct sockaddr_in cliaddr,servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM ,0);// 打开网络文件，返回listenfd

    int opt=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt) );

    bzero(&servaddr, sizeof(servaddr));// 清零
    servaddr.sin_family=AF_INET;// 设置地址类型为AF_INET
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);// 网络地址为INADDR_ANY 表示本地的任意IP地址
    servaddr.sin_port=htons(SERV_PORT);// 设置端口号

    Bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr));// 将listenfd和servaddr （{INADDR_ANY:SERV_PORT}） 绑定在一起

    Listen(listenfd,20);

    maxfd = listenfd;
    maxi = -1;
    for(i=0;i<FD_SETSIZE;i++)
        client[i]=-1;

    FD_ZERO(&allset);// 清空socket连接列表
    FD_SET(listenfd,&allset);
    printf("Accepting connections ...\n");
    for(;;){
        rset=allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);// 成功后rset将会被监听到的文件描述符取代，todo:nready真的可以大于1吗
        printf("select has received %d fds\n",nready);
        if (nready < 0){
            perror("select error");
            exit(2);
        }
        if (FD_ISSET(listenfd, &rset)) {// 如果 rset中包含listenfd，说明select此次监听到的是一次连接
            /* new client connect */
            cliaddr_len = sizeof(cliaddr);
            connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len);
            printf("received from %s at PORT %d\n", inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)), ntohs(cliaddr.sin_port));
            for (i=0; i<FD_SETSIZE; i++){
                if (client[i] < 0) {
                    client[i] = connfd; /* save descriptor */
                    break;
                }
            }
            if (i == FD_SETSIZE){
                fputs("too much clients\n",stderr);
                exit(1);
            }
            FD_SET(connfd, &allset);  /* add new descriptor to set */
            if (connfd > maxfd)
                maxfd = connfd; /* for select */
            if (i > maxi)
                maxi = i;       /* max. index in client[] array */
            if (--nready <= 0)
                continue; 	/* no more readable descriptors */
        }
        for (i=0; i<= maxi; i++){ /* check all clients for data */
            if ( (sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)){
                if ((n = Read(sockfd, buf, MAXLINE)) == 0){/* connection closed by client */
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else {
                    int j;
                    for(j=0;j<n;j++){
                        buf[j]= toupper(buf[j]);
                    }
                    Write(sockfd, buf, n);
                }
                if (--nready == 0){
                    break;	   /* no more readable sockets */
                }

            }
        }
    }
    return 1;
}














