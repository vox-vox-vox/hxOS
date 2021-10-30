#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NLOOP 5000

int counter; /*incr by thread*/

void *doit(void *);

int main(int argc, char**argv){
	pthread_t tidA,tidB;
	pthread_create(&tidA,NULL,&doit,NULL);
	pthread_create(&tidB,NULL,&doit,NULL);

	/*wait for thread to terminate*/
	pthread_join(tidA,NULL);
	pthread_join(tidB,NULL);

	return 0;
}

void *doit(void *vptr){

	int i,val;
	
	for(i=0 ; i<NLOOP ; i++){
		/* simulate counter++ problem
		实际上 counter++ 等价于 temp=count count=temp+1  
		因为在汇编, count++ 实际上是
		{
			mov count_addr,%eax
			add 1,%eax
			mov %eax,count_addr
		}
		*/ 
		val = counter;
		
		printf("%x: %d\n",(unsigned int)pthread_self(),val+1);
		
		counter = val +1;
	}
	return NULL;
}
