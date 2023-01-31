#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
	pthread_t t_id;
	int thread_param=5;

	// pthread_create 함수로 새로운 스레드를 생성한다.
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	};

	// 프로세스가 종료되면 쓰레드도 같이 종료되기에 sleep을 걸었다. 하지만 이것보다 더 좋은 방법이 있으니... 이는 thread2 파일에서
	sleep(10);  puts("end of main");
	return 0;
}

void* thread_main(void *arg)
{
	int i;
	int cnt=*((int*)arg);
	for(i=0; i<cnt; i++)
	{
		sleep(1);  puts("running thread");
	}
	return NULL;
}