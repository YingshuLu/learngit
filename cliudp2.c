#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<time.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>

#define MAXCH 1024

int main(int argc, char* argv[])
{
	char* addr;
	if(argc < 2)
	{
		addr = "127.0.0.1";	
	}
	else
	{
		addr = argv[1];
	}
	struct sockaddr_in seraddr;
	socklen_t slen = sizeof(seraddr);
	int sockfd;
	char buf[MAXCH];
	
	bzero(&seraddr,slen);
	bzero(buf,MAXCH);
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49992);
	if(inet_pton(AF_INET,addr,&(seraddr.sin_addr)) == -1)
	{
		perror("\tCLIENT --- INET TRANSLATE FAILED");
		exit(EXIT_FAILURE);
	}

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("\tCLIENT --- SOCKET FAILED");
		exit(EXIT_FAILURE);
	}

	ssize_t n;
	struct sockaddr_in* reply_addr = calloc(sizeof(struct sockaddr_in), 1);
	socklen_t rlen;
	char taddr[128];

	while(fgets(buf,MAXCH,stdin) != NULL)
	{
		if(sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*) &seraddr,slen) == -1)
		{
			perror("\tCLIENT --- SENDTO FAILED");
			exit(EXIT_FAILURE);
		}
		
		bzero(buf,sizeof(buf));
		bzero(reply_addr,sizeof(struct sockaddr_in));
		if((n = recvfrom(sockfd,buf,MAXCH,0,(struct sockaddr*) reply_addr,&rlen)) == -1)
		{
			perror("\tCLIENT --- RECVFROM FAILED");
			exit(EXIT_FAILURE);
		}

		if(rlen != slen || memcmp(reply_addr,&seraddr,rlen) != 0)
		{

			printf("\tCLIENT --- reply from %s\n",inet_ntop(AF_INET,&(reply_addr->sin_addr),taddr,sizeof(taddr)));
			continue;
		}
		buf[n] = '\0';
		printf("\tCLIENT --- has recieved as :\n%s\n",buf);
	}
	
	return 0;
}
