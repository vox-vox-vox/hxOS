#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void perr_exit(const char* );
int Accept(int,struct sockaddr*,socklen_t*);
void Bind(int, const struct sockaddr*,socklen_t);
void Connect(int,const struct sockaddr*,socklen_t);
void Listen(int,int);
int Socket(int,int,int);
ssize_t Read(int, void*,size_t);
ssize_t Write(int,const void*,size_t);
void Close(int);
ssize_t Readn(int , void *,size_t );
ssize_t Writen(int ,const void *, size_t);
static ssize_t my_read(int ,char *);
ssize_t ReadLine(int , void *, size_t);
