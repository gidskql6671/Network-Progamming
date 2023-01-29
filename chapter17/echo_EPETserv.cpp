#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50
void setnonblockingmode(int fd);
void error_handling(char *buf);

// 엣지 트리거 방식
int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	// 엣지 트리거 방식의 특성상 블로킹 방식으로 동작하는 read & write 함수의 호출은 서버를 오랜 시간 멈추는 상황으로까지 이어지게 할 수 있다.
	// 때문에 엣지 트리거 방식에서는 반드시 논 블로킹 소켓을 기반으로 하는 read & write 함수를 호출해야 한다.
	setnonblockingmode(serv_sock);
	event.events=EPOLLIN;
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		puts("return epoll_wait");
		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				// 48라인과 마찬가지의 이유
				setnonblockingmode(clnt_sock);
				// 수신할 데이터가 존재하는지 여부를 감지하는 이벤트를 엣지 트리거 방식으로 감지한다.
				event.events=EPOLLIN|EPOLLET;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else
			{
					// 엣지 트리거 방식에서는 이벤트가 발생시 입력 버퍼에 존재하는 데이터를 모두 수신해야하기 때문에 while을 씀
					while(1)
					{
						str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
						if(str_len==0)    // close request!
						{
							epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
							close(ep_events[i].data.fd);
							printf("closed client: %d \n", ep_events[i].data.fd);
							break;
						}
						else if(str_len<0)
						{
							if(errno==EAGAIN)
								break;
						}
						else
						{
							write(ep_events[i].data.fd, buf, str_len);    // echo!
						}
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void setnonblockingmode(int fd)
{
	int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}
void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}