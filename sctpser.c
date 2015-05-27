#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<limits.h>

int main(int argc, char* argv[])
{
	int sockfd, msg_flag;
	char buf[BUFSIZ];
	struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	socklen_t len;
	size_t rd_sz;
	int increment;
	
	if(argc > 1)
	{
		increment = atoi(argv[1]);
	}
	sockfd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	if(sockfd < 0)
	{
		perror("\tSERVER --- SOCKET FAILED");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr,sizeof(servaddr));
	bzero(&sri,sizeof(sri));
	bzero(&evnts,sizeof(evnts));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(49993);
	servaddr.sin_addr.s_addr = htol(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr)) < 0)
	{
		perror("\tSERVER --- BIND FAILED");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(sockfd,IPPROTO,SCTP_EVENTS,&evnts,sizeof(evnts)) < 0)
	{
		perror("\tSERVER --- SETSOCKOPT FAILED");
		exit(EXIT_FAILURE);
	}

	if(listen(sockfd,5) < 0)
	{
		perror("\tSERVER --- LISTEN FAILED");
		exit(EXIT_FAILURE);
	}


	while(1)
	{
		len = sizeof(struct sockaddr_in);
		rd_sz = sctp_recvmsg(sockfd,buf,sizeof(buf),(struct sockaddr*) &cliaddr,&len, &sri,&msg_flag);
		if(rd_sz < 0)
		{
			perror("\tSERVER --- SCTP_RECVMSG FAILED");
			exit(EXIT_FAILURE);
		}
		
		if(increment)
		{
			sri.sinfo_stream++;
			if(sri.sinfo_stream >= sctp_get_no_strms(sockfd, (struct sockaddr*) &cliaddr,len))
			{
				sri.sinfo_stream = 0;
			}
		}


		if(sctp_sndmsg(sockfd, buf,strlen(buf),(struct sockaddr*) &cliaddr, len,sri.sinfo_ppid,
			sri.sinfo_flags,sri.sinfo_stream,0,0) < 0)
		{
			perror("\tSERVER --- SCTP_SNDMSG FAILED");
			exit(EXIT_FAILURE);
		}
	}
}
