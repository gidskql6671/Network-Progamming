#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg);

int sum=0;

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;
	int range1[]={1, 5};
	int range2[]={6, 10};

	pthread_create(&id_t1, NULL, thread_summation, (void *)range1);
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2);

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d \n", sum);
	return 0;
}

void * thread_summation(void * arg)
{
	int start=((int*)arg)[0];
	int end=((int*)arg)[1];

	while(start<=end)
	{
		// 두 쓰레드가 하나의 전역변수에 직접 접근하고 있다. 이는 문제가 될 수 있는 임계 영역이고, 그렇기에 해당 함수는 Thread-unsafe하다.
		// 다만 쓰레드의 수가 2개밖에 없기에 실제로 문제를 관측하기에는 확률이 높지않다.
		sum+=start;
		start++;
	}
	return NULL;
}
