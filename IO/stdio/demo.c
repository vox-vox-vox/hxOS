#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
void errorDemo(void);
void byteIODemo(void);
void moveCurDemo(void);
void dumpFileDemo(void);
void scanfDemo(void);
void testInput(void);

int main(void){
    //errorDemo();
    //byteIODemo();
    //moveCurDemo();
    //dumpFileDemo();
	//scanfDemo();
	testInput();
    return 0;
}
/*
IO 操作
IO 缓冲区
*/
/*
fopen       FILE *fopen(const char *path, const char *mode);
            input: path 路径名 ; mode 操作方式
            output: 成功返回文件指针，出错返回NULL并设置errno
            打开文件
fclose      int flose(FILE *fp);
            input:
            output:
            关闭文件
*/
/*
终端：terminal，对我们来说就是 显示器+键盘
键盘，显示器等设备也可以像普通文件一样被打开、读、写、关闭
程序启动时会自动把终端设备赋给3个FILE* 指针：stdin、stdout、stderror
stdin对应了键盘文件
stdout对应了显示器文件
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
字符操作

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
移动读写位置

fseek      	int fseek(FILE *stream, long offset, int whence);
            input: FILE 文件的指针，也可称为流指针 ; offset 移动量，负值代表向前、正值代表向后 ;whence SEEK_END ; SEEK_CUR ; SEEK_END 
            output: 成功时返回0，错误返回-1并设置errno
            任意移动读写位置
ftell		int ftell(FILE *stream);
			input: FILE 文件的指针
			output: 返回当前读写位置，错误返回-1并设置errno
rewind		rewind(FILE *stream);
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
字符串处理

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
// 如果stream选择stdin，在没有输入任何东西的时候会阻塞吗？
void testInput(void){
	char temp[100];
	fgets(temp,100,stdin);
	printf("%s\n",temp);
	return;
}



/*
格式化IO 格式化字符串<=====>参数

printf		int printf(const char *format, ...);
			将输入的参数通过format格式化，打印到标准输出（屏幕）
scanf		int scanf(const char *format, ...);
			从标准输入读字符，按格式化format转换输入的字符，并赋给后面的参数，后面的参数必须传地址
*/
// 利用scanf实现简单的计算器
void scanfDemo(void){
	double sum,v;
	sum = 0;
	while(scanf("%lf",&v)==1){
		printf("\t%.2f\n",sum+=v);
	}
	return ;
}


/*
用户程序----(C标准库IO函数)----C标准库IO缓冲区-----(内核)-----内存
c标准库的IO缓冲区通常在用户空间，直接在用户空间读数据比进内核读数据要快得多。
fgetc一次从IO缓冲区读一个字符，内核一次从内存中加载1k数据，第二次fgetc就直接从缓冲区读而不是通过内核
fputc一次向IO缓冲区写一个字符，如果IO缓冲区写满了，fputc就通过系统调用把IO缓冲区中的数据传给内核，内核最终把数据写回磁盘
将IO缓冲区的数据立刻传给内核，让内核写回设备，称为Flush操作

1. 内核和磁盘不在一起嘛
2. 什么叫传给内核，内核再写回设备，这居然是两个过程
3. 用户空间/内核空间，用户程序/内核，C标准IO缓冲区/磁盘 之间的关系
4. 标准输入输出 和 终端设备有什么关系？ 

C标准库的IO缓冲区有三种不同类型：
全缓冲：缓冲区写满了就写回内核(flush)。常规文件通常是全缓冲的
行缓冲：用户程序中有换行符就把这一行写回内核/缓冲区满了就写回内核(flush)。对应终端设备时通常是行缓冲
无缓冲：每次调用库函数都要经过系统调用写回内核(flush)。标准错误是无缓冲的，产生的错误信息可以尽快输出到设备

行缓冲有3种情况会flush，分别是: 1.有换行符'\n' 2.IO缓冲区写满 3.用户程序调用库函数从无缓冲文件读取/从行缓冲文件读取且这次读操作会引发系统调用从内核中取数据
*/



































