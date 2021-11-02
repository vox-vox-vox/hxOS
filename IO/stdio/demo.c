#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
void errorDemo(void);
void byteIODemo(void);
void moveCurDemo(void);
void dumpFileDemo(void);

int main(void){
    //errorDemo();
    //byteIODemo();
    //moveCurDemo();
    dumpFileDemo();
    return 0;
}

/*
fopen       FILE *fopen(const char *path, const char *mode);
            input: path 路径名 ; mode 操作方式
            output:
            打开文件
fclose      int flose(FILE *fp);
            input:
            output:
            关闭文件

    printf
    scanf
    stdin
    stdout

*/


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
            output: 成功时返回读到的字节，出错/读到文件末尾时返回EOF。本来应该返回unsigned char，但是由于函数原型中返回是int，所以要将char转为int再返回。
                    这是为了兼容EOF
            从指定的文件中读一个字节。
getchar     int getchar(void);
            input: -
            output: 成功时返回读到的字节，出错/读到文件末尾时返回EOF
            从标准输入中读一个字节，相当于fgetc(stdin)
fputc       int fputc(int c,FILE *stream);
            input: FILE 文件的指针，也可称为流指针 ; c 输入的字节
            output: 成功时返回写入的字节，出错/读到文件末尾时返回EOF
            向指定的文件中写入一个字节
putchar     int putchar(int c);
            input: 待写入的字符串
            output: 成功时返回写入的字节，出错/读到文件末尾时返回EOF
            向标准输出中写入一个字节，相当于fputc(c,stdout)
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
fseek      	int fseek(FILE *stream, long offset, int whence);
            input: FILE 文件的指针，也可称为流指针 ; offset 移动量，负值代表向前、正值代表向后 ;whence SEEK_END ; SEEK_CUR ; SEEK_END 
            output: 成功时返回0，错误返回-1并设置errno
            任意移动读写位置
ftell		int ftell(FILE *stream);
			input: FILE 文件的指针
			output: 返回当前读写位置，错误返回-1并设置errno
void		rewind(FILE *stream);
			重新将读写位置移到开头
*/
void moveCurDemo(void){
	FILE* fp;
	fp = fopen("file2","r+");
	if(fp==NULL){
	    perror("Open file textfile fail");
	    exit(1);
	}
	if(fseek(fp,10,SEEK_SET)!=0){
	    perror("Seek file textfile fail");
	    exit(1);
	}
	fputc('K',fp);
	fclose(fp);
	return ;
} 


/*
fgets       char *fgets(char *s,int size,FILE *stream);
            input: s 缓冲区首地址 ; size 缓冲区长度 ; FILE 文件的指针，也可称为流指针
            output: 成功时s指向哪返回的指针就指向哪，出错/读到文件末尾时返回NULL
            从FILE中读取以'\n'结尾的一行(包括'\n')存到缓冲区s中，并在后面加上一个'\0'组成完整的字符串
gets        int gets(void);
            NEVER USE
fputs       int fputc(const char *s,FILE *stream);
            input: FILE 文件的指针，也可称为流指针 ; s 输入的字符串
            output: 成功时返回非负整数，出错/读到文件末尾时返回EOF
            向指定的文件中写入一个字符串
puts        int puts(const char *s);
            input: 字符串
            output: 成功时返回非负整数，出错/读到文件末尾时返回EOF
            向标准输入中写入一个字符串
*/
// 利用fputs和fgets写一个拷贝文件的程序
// 只能拷贝文本文件
void dumpFileDemo(void){
    FILE* fp1;
    FILE* fp2;
    fp1 = fopen("file1","r");
    if(fp1==NULL){
        perror("Open file1 fail");
        exit(1);
    }
    fp2 = fopen("file2","w");
    if(fp2==NULL){
        perror("Open file2 fail");
        exit(1);
    }

    char temp[100];
    while(fgets(temp,100,fp1)!=NULL){
        printf("%s\n",temp);
        fputs(temp,fp2);
    }
    fclose(fp1);
    fclose(fp2);
    return;
}






































