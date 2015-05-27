#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include<math.h>

#define MAXCH 2048
void str_cli(FILE* fp,int sockfd);

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


	str_cli(stdin,sockfd);
/*	// get message from server
	if(read(sockfd,buffer,sizeof(buffer)) <= 0)
	{
		perror("\tCLIENT --- READ FAILED");
		exit(EXIT_FAILURE);
	}
	// display what recieve from server
	printf("client get time : \n\t%s",buffer);
*/
	close(sockfd);
	return 0;
}

void str_cli(FILE* fp, int sockfd)
{

	int maxfd,stdineof;
	fd_set rset;
	char buf[MAXCH];
	char tbuf[MAXCH];
	int n;
	// point out if the input has get EOF
	stdineof = 0;
	FD_ZERO(&rset);
	while(1)
	{
		//if the first input charactor is not 'EOF', put fp into rset in select
		if(stdineof == 0)
		{
			FD_SET(fileno(fp),&rset);
		}
		// each loop, always interset in sockfd in select function
		FD_SET(sockfd,&rset);
		maxfd = (fileno(fp) > sockfd? fileno(fp) : sockfd) +1;

		if(select(maxfd,&rset,NULL,NULL,NULL) == -1)
		{
			perror("\tCLIENT --- SELECT FAILED");
			exit(EXIT_FAILURE);
		}
		
		if(FD_ISSET(sockfd,&rset))
		{
			// the first readable charactor is 'EOF',terminate the funtion
			if((n= read(sockfd,tbuf,MAXCH)) == 0)
			{
				if(stdineof == 1)
				{
					return;
				}

			}
			else if(n < 0)
			{
				perror("\tCLIENT --- READ FROM SERVER FAILED");
				exit(EXIT_FAILURE);
			}
			printf("\tCLIENT --- RECIEVE FROM SERVER:\n%s\n",tbuf);			
			/*if(write(fileno(stdout),buf,MAXCH) < 0)
			{

				perror("\tCLIENT --- WRITE TO STDOUT FAILED");
				exit(EXIT_FAILURE);
			}*/
		}

		// if fp has ready in select
		if(FD_ISSET(fileno(fp),&rset))
		{
			if((n=read(fileno(stdin),buf,MAXCH)) < 0)
			{
				perror("\tCLIENT --- READ FROM STDIN FAILED");
				exit(EXIT_FAILURE);
			}
			else if(n == 0)
			{
				// read nothing but 'EOF'	
				stdineof = 1;
				// point out this point want to finished sending data
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			
			if(write(sockfd,buf,n) < 0)
			{
				perror("\tCLIENT --- WRITE TO SERVER FAILED");
				exit(EXIT_FAILURE);

			}
			

		}
	}		
}
