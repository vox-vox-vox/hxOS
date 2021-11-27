//
// Created by huxiao on 2021/11/14.
   一个只支持GET请求的网页小服务器
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

#define MAXLINE 100
#define SERV_PORT 8022
#define MAXLEN 1000
void doit(int);
int clienterror(int ,char *,char *,char *,char *);
void read_requesthdrs(int );
int parse_uri(char *, char *, char *);
void serve_static(int ,char *,int );
void serve_dynamic(int ,char *,char *);
void getfiletype(char *,char *);

void childHandler() {
    int stat_val;
    wait(&stat_val); // 清理子进程
    printf("child exited with code %d\n", stat_val);
    return;
}

int main(void){
    // 设置信号处理函数
    struct sigaction newact,oldact;
    newact.sa_handler=childHandler;
    sigemptyset(&newact.sa_mask);//清空newact的阻塞信号掩码
    newact.sa_flags=0;
    sigaction(SIGCHLD,&newact,&oldact);//为SIGCHLD配置自定义action

    // 设置网络处理参数
    struct sockaddr_in servaddr, cliaddr;// IPV4 类型地址
    socklen_t cliaddr_len;
    int listenfd, connfd;// 两个重要的套接字
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int i,n;

    listenfd = Socket(AF_INET, SOCK_STREAM ,0);// 打开网络文件，返回listenfd

    int opt=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt) );

    bzero(&servaddr, sizeof(servaddr));// 清零
    servaddr.sin_family=AF_INET;// 设置地址类型为AF_INET
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);// 网络地址为INADDR_ANY 表示本地的任意IP地址
    servaddr.sin_port=htons(SERV_PORT);// 设置端口号


    Bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr));// 将listenfd和servaddr （{INADDR_ANY:SERV_PORT}） 绑定在一起

    Listen(listenfd,20);

    printf("Accepting connections ...\n");

    while(1){	// 死循环，每次循环处理一个客户端连接
        cliaddr_len = sizeof(cliaddr);// cliaddr_len是传入传出参数，每次accept()前要重新赋值
        connfd = Accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len);
        n = fork();// fork出子进程，专门用来处理网络请求
        if(n==-1){
            perror("error to fork");
            exit(1);
        }else if(n==0){//子进程
            Close(listenfd);// 为什么子进程要关掉listenfd？不用就要关
            doit(connfd);
            Close(connfd);
            exit(0);
        }else{ //父进程
            Close(connfd);// 为什么父进程要关闭connfd，不用就要关
        }
    }
    sigaction(SIGCHLD,&oldact,NULL);
    return 1;
}

/* 解析GET请求 */
void doit(int connfd){
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
    char filename[MAXLINE],cgiargs[MAXLINE];
    // header
    n=ReadLine(connfd,buf,MAXLEN);
    if(n==0){
        printf("the other side has been closed\n");
        return;
    }
    printf("Request headers:\n");
    printf("%s",buf);
    sscanf(buf,"%s %s %s",method,uri,version);
    if(strcasecmp(method,"GET")){
        clienterror(fd,method,"501","Notimplemented","Tiny does not implement this method");
        return;
    }
    read_requesthdrs(connfd)
    // Parse URI from GET request
    is_static = parse_uri(uri,filename,cgiargs);
    if(stat(filename,&sbuf)<0){
        clienterror(fd,filename,"404","Not found","Tiny does not find this file");
        return;
    }

    if(is_static){
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR&sbuf.st_mode)){
            clienterror(fd,filename,"403","Forbidden","Tiny could not read this file");
            return;
        }
        serve_static(fd,filename,sbuf.st_size);
    }else{
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR&sbuf.st_mode)){
            clienterror(fd,filename,"403","Forbidden","Tiny could not read this file");
            return;
        }
        serve_dynamic(fd,filename,cgiargs);
    }
}

int clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg){
    char buf[MAXLINE],body[MAXBUF];
    sprintf(body,"<html><title>Tiny Error</title>");
    sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
    sprintf(body,"%s%s: %s\r\n",body,errnum,shortmsg);
    sprintf(body,"%s<p>%s: %s\r\n",body,longmsg,cause);
    sprintf(body,"%s<hr><em>The tiny Web server</em>\r\n",body);

    sprintf(buf,"HTTP/1.0 %s %s\r\n",errnum,shortmsg);
    Writen(fd,buf,strlen(buf));
    sprintf(buf,"Content-type: text/html\r\n");
    Writen(fd,buf,strlen(buf));
    sprintf(buf,"Content-length: %d\r\n\r\n",(int)strlen(body));
    Writen(fd,buf,strlen(buf));
    Writen(fd,buf,strlen(body));
}

void read_requesthdrs(int connfd){
    char buf[MAXLINE];
    ReadLine(connfd,buf,MAXLEN);
    while(strcmp(buf,"\r\n")){
        ReadLine(connfd,buf,MAXLEN);
        printf("%s",buf);
    }
    return;
}

int parse_uri(char *uri, char *filename, char *cgiargs){
    char *ptr;
    if(!strstr(uri,"cgi-bin")){/*static content*/
        strcpy(cgiargs,"");
        strcpy(filename,".");
        strcat(filename,uri);
        if(uri[strlen(uri)-1]=='/'){
            strcat(filename,"home.html");
        }
        retuen 1;
    }else{/*dynamic content*/
        ptr = index(uri,'?');
        if(ptr){
            strcpy(cgiargs,ptr+1);
            *ptr='\0';
        }else{
            strcpy(cgiargs,"");
        }
        strcpy(filename,"");
        strcpy(filename,".");
        strcat(filename,uri);
        return 0;
    }
}

void serve_static(int fd,char *filename,int filesize){
    int srcfd;
    char *srcp, filetype[MAXLINE],buf[MAXBUF];
}

void getfiletype(char *filename,char *filetype){
    if(strstr(filename,".html"))
        strcpy(filetype,"text/html");
    else if(strstr(filename,".gif"))
        strcpy(filetype,"image/gif");
    else if(strstr(filename,".png"))
        strcpy(filetype,"image/png");
    else if(strstr(filename,".jpg"))
        strcpy(filetype,"image/jpg");
    else
        strcpy(filetype,"text/plain");
}

void serve_dynamic(int fd,char *filename,char *cgiargs){
    char buf[MAXLINE],*emptylist[]={NULL};
    sprint(buf,"HTTP/1.0 200 OK\r\n");

}

