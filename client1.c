#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>

#define MAXCH 1024

int main(int argc, char* argv[])
{
	char *saddr;
	// get server IP address
	if(argc == 1)
	{
		saddr = "127.0.0.1";	
	}
	else
	{
		saddr = argv[1];
	}
	
	// server socket address
	struct sockaddr_in seraddr;
	socklen_t slen;
	int sockfd;
	char buffer[MAXCH];

	// initalize server socket address
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49991);
	inet_pton(AF_INET,saddr, &seraddr.sin_addr);
	
	slen = sizeof(seraddr);
	// get 'sockfd' using Internet proctel and TCP connection
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("\tCLIENT --- SOCKFD CREATE FAILED");
		exit(EXIT_FAILURE);
	}
	// connect to server and get server 'sockfd'
	if(connect(sockfd,(struct sockaddr *) &seraddr, slen) < 0)
	{
		perror("\tCLIENT --- CONNECTION FAILED");
		exit(EXIT_FAILURE);
	}
	
	// get message from server
	if(read(sockfd,buffer,sizeof(buffer)) <= 0)
	{
		perror("\tCLIENT --- READ FAILED");
		exit(EXIT_FAILURE);
	}
	// display what recieve from server
	printf("client get time : \n\t%s",buffer);
	return 0;
}

