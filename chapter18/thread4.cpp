#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD	100

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num=0;

int main(int argc, char *argv[])
{
	pthread_t thread_id[NUM_THREAD];
	int i;

	printf("sizeof long long: %lu \n", sizeof(long long));
	// 쓰레드의 개수가 많기에 Thread-unsafe한 함수를 호출했을 때, 문제가 발생할 확률이 높다.
	// 기대하는 결과는 0이지만, 그렇지 않을 경우가 많다.
	for(i=0; i<NUM_THREAD; i++)
	{
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);
	return 0;
}

void * thread_inc(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
		num+=1;
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
		num-=1;
	return NULL;
}