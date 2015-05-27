#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<errno.h>
#include<time.h>

int main(int argc,char* argv[])
{
	int sockfd,msg_flags;
	char buf[BUFSIZ];
	struct sockaddr_in seraddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	int stream_increment = 1;
	socklen_t len;
	size_t rd_sz;
	
	if(argc>1)
	{
		stream_increment = atoi(argv[1]);
	}

	sockfd = socket(AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	if(sockfd < 0)
	{
		perror("SERVER: socket failed");
		exit(EXIT_FAILURE);
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49992);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr*) &seraddr,sizeof(seraddr)) < 0)
	{
		perror("SERVER: bind failed");
		exit(EXIT_FAILURE);
	}

	bzero(&evnts,sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	
	if(setsockopt(sockfd,IPPROTO_SCTP,SCTP_EVENTS,&evnts,sizeof(evnts)) < 0)
	{
		perror("SERVER: setsockopt failed");
		exit(EXIT_FAILURE);
	}

	if(listen(sockfd,5) < 0)
	{
		perror("SERVER: listen failed");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		len = sizeof(struct sockaddr_in);
		rd_sz = sctp_recvmsg(sockfd,buf,sizeof(buf),
				(struct sockaddr*) &cliaddr,&len,&sri,&msg_flags);
		if(rd_sz == -1)
		{
			perror("SERVER: sctp_rcvmsg failed");
			exit(EXIT_FAILURE);
		}

		if(stream_increment--)
		{
			sri.sinfo_stream++;
			if(sri.sinfo_stream >= sctp_get_no_strms(sockfd,(struct sockaddr*) &cliaddr,len))
			{
				sri.sinfo_stream = 0;
			}
		}

		if(sctp_sendmsg(sockfd,buf,strlen(buf),(struct sockaddr*) &cliaddr,len,sri.sinfo_ppid,sri.sinfo_flags,sri.sinfo_stream,0,0) == -1)		  {
			perror("SERVER: sctp_sendmsg failed");
			exit(EXIT_FAILURE);
		}
		
	}
	return 0;
}


