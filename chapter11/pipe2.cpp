#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	int fds[2];
	char str1[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;

	pipe(fds);
	pid=fork();

	// 한개의 파이프로 양방향 통신을 할 수는 있다.
	// 다만 이 경우, 구현에 따라 잘못된 동작이 있을 수 있다. 예를들어 밑 sleep(2)를 주석처리하면 자식 프로세스가 보낸 데이터를 자식 프로세스가 가져간다.
	if(pid==0)
	{
		write(fds[1], str1, sizeof(str1));
		sleep(2);
		read(fds[0], buf, BUF_SIZE);
		printf("Child proc output: %s \n",  buf);
	}
	else
	{
		read(fds[0], buf, BUF_SIZE);
		printf("Parent proc output: %s \n", buf);
		write(fds[1], str2, sizeof(str2));
		sleep(3);
	}
	return 0;
}
