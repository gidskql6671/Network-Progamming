#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	FILE * readfp;
	FILE * writefp;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	char buf[BUF_SIZE]={0,};

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr));
	listen(serv_sock, 5);
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

	// writefp를 clnt_sock 파일 디스크립터를 복사한 파일 디스크립터를 인자로 넣어주었다.
	// 즉, readfp와 writefp는 서로 다른 파일 디스크립터를 가리킨다.
	// 다만, 서로 가리키는 소켓은 똑같다.
	readfp=fdopen(clnt_sock, "r");
	writefp=fdopen(dup(clnt_sock), "w");

	fputs("FROM SERVER: Hi~ client? \n", writefp);
	fputs("I love all of the world \n", writefp);
	fputs("You are awesome! \n", writefp);
	fflush(writefp);

	// fileno 함수로 출력 용으로 쓰던 파일 디스크립터를 알아낸 뒤, 출력 스트림을 half-close 시켰다.
	shutdown(fileno(writefp), SHUT_WR);
	// 이후 해당 파일 디스크립터 & 파일 포인터는 필요없으니 닫아준다.
	fclose(writefp);

	// 여전히 열려있는 입력 스트림으로 데이터를 마저 받고, 소켓을 close시킨다.
	fgets(buf, sizeof(buf), readfp);
	fputs(buf, stdout);
	fclose(readfp);
	return 0;
}
