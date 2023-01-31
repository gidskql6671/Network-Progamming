#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD	100

void * thread_inc(void * arg);
void * thread_des(void * arg);

long long num=0;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
	pthread_t thread_id[NUM_THREAD];
	int i;

	// 뮤텍스 초기화
	pthread_mutex_init(&mutex, NULL);

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

	// 뮤텍스 삭제
	pthread_mutex_destroy(&mutex);
	return 0;
}

void * thread_inc(void * arg)
{
	int i;
	// 뮤텍스 락. 임계영역에 들어가면서 뮤텍스를 가지고 들어간다.
	// 뮤텍스를 가지지 못한 쓰레드는 대기한다.
	pthread_mutex_lock(&mutex);
	for(i=0; i<50000000; i++)
		num+=1;
	// 할 일을 다 했으면 뮤텍스를 풀고 나온다.
	pthread_mutex_unlock(&mutex);
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
	{
		// thread_inc와는 달리 여기서는 임계영역을 좁게 잡고, For문 안에서 lock & unlock을 한다.
		// 그런데, 뮤텍스 lock과 unlock 작업은 비용이 크기 때문에 가능한 호출 수를 줄여주는 것이 좋다.
		pthread_mutex_lock(&mutex);
		num-=1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
