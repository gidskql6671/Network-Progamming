#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	struct sockaddr_in my_adr, your_adr;
	socklen_t adr_sz;
	int str_len, i;

	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
		error_handling("socket() error");

	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family=AF_INET;
	my_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	my_adr.sin_port=htons(atoi(argv[1]));

	if(bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr))==-1)
		error_handling("bind() error");

	for(i=0; i<3; i++)
	{
		// delay 5 sec.
		// 만약 TCP였다면, 상대방이 3번 보낸 데이터가 입력버퍼에 쌓여있다가 이를 한번에 받았을 것이다.
		// 그러나 UDP는 상대방이 3번 메시지를 보냈기에, 3번 recvfrom을 호출해 받아야한다.
		sleep(5);
		adr_sz=sizeof(your_adr);
		str_len=recvfrom(sock, message, BUF_SIZE, 0,
								(struct sockaddr*)&your_adr, &adr_sz);

		printf("Message %d: %s \n", i+1, message);
	}
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
root@my_linux:/home/swyoon/tcpip# gcc bound_host1.c -o host1
root@my_linux:/home/swyoon/tcpip# ./host1
Usage : ./host1 <port>
root@my_linux:/home/swyoon/tcpip# ./host1 9190
Message 1: Hi!
Message 2: I'm another UDP host!
Message 3: Nice to meet you
root@my_linux:/home/swyoon/tcpip#

*/
