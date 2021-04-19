
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <cstring>

#define SERVERPORTID 8888    
#define SERVERIPADRR "172.17.140.154"
int main()
{
	
	
	
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sfd)
		{
			perror("socket");
			exit(-1);
		}
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORTID);
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIPADRR);
	int ret = connect(sfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(ret == -1)
	{
		perror("connect");
		close(sfd);
		exit(-1);
	}
	ret = send(sfd,"1234",5,0);
	printf("ret:%d\n",ret);
		//close(sfd);
	
	return 0;

}