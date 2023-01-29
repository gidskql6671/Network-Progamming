#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int cfd1, cfd2;
	char str1[]="Hi~ \n";
	char str2[]="It's nice day~ \n";

	// 파일 디스크립터를 복사해서 반환한다.
	cfd1=dup(1);
	// 파일 디스크립터를 복사해서 반환하되, 두번째 인자의 값(7)을 복사되는 파일 디스크립터의 정수 값으로 지정해준다.
	cfd2=dup2(cfd1, 7);

	printf("fd1=%d, fd2=%d \n", cfd1, cfd2);
	write(cfd1, str1, sizeof(str1));
	write(cfd2, str2, sizeof(str2));

	close(cfd1);
	close(cfd2);
	write(1, str1, sizeof(str1));
	close(1);
	write(1, str2, sizeof(str2));
	return 0;
}
