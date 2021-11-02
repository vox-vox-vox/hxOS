#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/*
    fopen       FILE *fopen(const char *path, const char *mode);    path:路径名    mode:操作方式
    fclose      int flose(FILE *fp);


    printf
    scanf
    stdin
    stdout

*/
void errorDemo(void);
void byteIODemo(void);

int main(void){
    //errorDemo();
    byteIODemo();
    return 0;
}

/*
errno 与 perror 函数
perror      void perror(const char *s);
            input: 用户自定义的字符串
            output: -
            将错误信息打印到标准错误输出，首先打印s所指的字符串，然后打印 ":" ，然后根据errno的值打印错误原因
strerror    char *strerror(int errnum);
            input: 错误码
            output：错误码对应的字符串
            将错误码打印成对应字符串
errno       int
            错误码，所有错误码都是正整数
*/
void errorDemo(void){
    FILE *fp = fopen("abcde","r");
    if ( fp == NULL){
        perror("Open file abcde error");// 打开一个不存在的abcde，会报错
        printf("errno: %d\n",errno);
        printf("errno %d is %s\n",errno,strerror(errno));
        exit(1);
    }
    return ;
}

/*
fgetc       int fgetc(FILE *stream);
            input: FILE 文件的指针，也可称为流指针
            output: 成功时返回读到的字节，出错/读到文件末尾时返回EOF
            从指定的文件中读一个字节
getchar     int getchar(void);
            input: -
            output: 成功时返回读到的字节，出错/读到文件末尾时返回EOF
            从标准输入中读一个字节，相当于fgetc(stdin)
fputc       int fputc(int c,FILE *stream);
            input: FILE 文件的指针，也可称为流指针 ; c 输入的字节
            output: 成功时返回写入的字节，出错/读到文件末尾时返回EOF
            向指定的文件中写入一个字节
putchar     int putchar(int c);
            input: -
            output: 成功时返回写入的字节，出错/读到文件末尾时返回EOF
            向标准输入中写入一个字节，相当于fputc(c,stdout)
*/
//
// 1. 终端 ctrl+D = EOF；回车 != EOF
// 2. 不同终端getchar()函数返回情况不同，有的是读入一个char就返回，有的是读到EOF才返回
void byteIODemo(void){
    FILE *fp;
    int ch;
    fp = fopen("file2","w+");
    if(fp==NULL){
        perror("open file2 error\n");
        exit(1);
    }
    /*标准输入中读一个字节，然后写入文件file2中*/
    while( (ch = getchar()) != EOF){
        printf("yes");
        fputc(ch,fp);
    }

    /*将读写cursor移到文件开头*/
    rewind(fp);

    /*文件读取一个字节，然后写到标准输出中*/
    while((ch = fgetc(fp)) != EOF){
        putchar(ch);
        // fputc(ch,stdout); //和上句等价
    }
    /*关闭文件file2*/
    fclose(fp);
    return ;
}


/*
fgetc       int fgetc(FILE *stream);
             input: FILE 文件的指针，也可称为流指针
             output: 成功时返回读到的字节，出错/读到文件末尾时返回EOF
             从指定的文件中读一个字节

*/






















