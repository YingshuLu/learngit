#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<time.h>
#include<string.h>
#include<errno.h>

#define MAXCH 1024

int main()
{
	struct sockaddr_in seraddr,cliaddr,tmp_addr;
	int sockfd;
	int res;
	socklen_t slen,clen,tlen;
	const char* addr = "127.0.0.1";
	char taddr[128];
	char *sbf;
	char rbuf[MAXCH];
	char sbuf[MAXCH];
	size_t len;

	slen = sizeof(seraddr);
	clen = sizeof(cliaddr);
	
	bzero(&seraddr,slen);
	bzero(&cliaddr,clen);
	bzero(rbuf,MAXCH);

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49992);
	inet_pton(AF_INET, addr, &seraddr.sin_addr);

	if((sockfd = socket(AF_INET, SOCK_DGRAM,0)) == -1)
	{
		perror("\tSERVER --- SOCKET INITALIZE FAILED");
		exit(EXIT_FAILURE);
	}

	if(bind(sockfd, (struct sockaddr*) &seraddr, slen) == -1)
	{
		perror("\tSERVER --- BIND FAILED");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in* ptr_addr;
	int i = 0;
	while(1)
	{
		bzero(&cliaddr,clen);
		bzero(rbuf,sizeof(rbuf));
		bzero(sbuf,sizeof(sbuf));

		if(recvfrom(sockfd,rbuf,MAXCH,0,(struct sockaddr*) &cliaddr,&clen) == -1)
		{
			perror("\tSERVER --- RECVFROM FAILED");
			exit(EXIT_FAILURE);
		}
		
		printf("--- SERVER --- recieve from %s\t:\n",inet_ntop(AF_INET,&cliaddr.sin_addr,taddr,sizeof(taddr)),rbuf);

		if( i == 0)
		{
			memcpy(&tmp_addr,&cliaddr,clen);
			tlen = clen;
			i++;
		}

		if(tlen != clen || memcmp(&cliaddr,&tmp_addr,tlen) != 0 )
		{
			ptr_addr = &cliaddr;
			memcpy(&tmp_addr, &cliaddr,clen);
			tlen = clen;
			sbf = rbuf;
			len =strlen(rbuf);
		}
		else
		{
			ptr_addr = &tmp_addr;	
			sprintf(sbuf,"--- Waiting for connection!\n");
			sbf = sbuf;
			len = strlen(sbuf);
		}


		if(sendto(sockfd,sbf,len,0,(struct sockaddr*) ptr_addr,tlen) == -1)
		{
			perror("\tSERVER --- SENDTO FAILED");
			exit(EXIT_FAILURE);
		}
	}
	return 0;	
}



