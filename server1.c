#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>

#define MAXCH 1024

int main()
{
	struct sockaddr_in seraddr,cliaddr;
	socklen_t slen, clen;
	int sockfd,connfd;
	time_t tick;
	char buffer[MAXCH];


	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49991);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	slen = sizeof(seraddr);
	clen = sizeof(cliaddr);

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("\tSERVER--- SOCKFD CREATE FAILED");
		exit(EXIT_FAILURE);
	}

	
	if(bind(sockfd,(struct sockaddr *) &seraddr,slen) < 0)
	{
		perror("\tSERVER--- BIND FAILED");
		exit(EXIT_FAILURE);
	}
	if(listen(sockfd, 5) < 0)
	{
		perror("\tSERVER--- LISTEN FAILED");
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
		if((connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clen)) < 0 )
		{
			perror("\tSERVER--- ACCEPT FAILED");
			exit(EXIT_FAILURE);
		}
		printf("\tSERVER --- connection from %s, port %d\n",
			inet_ntop(AF_INET,&cliaddr.sin_addr, buffer, sizeof(buffer)),ntohs(cliaddr.sin_port));
		
		tick = time(NULL);
		
		snprintf(buffer,sizeof(buffer),"%.24s\r\n",ctime(&tick));
		if(write(connfd,buffer,sizeof(buffer)) <= 0)
		{
			perror("\tSERVER --- WRITE FAILED");
			exit(EXIT_FAILURE);
		}
		close(connfd);			
	}
	return 0;
}
