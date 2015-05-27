#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<errno.h>

#define MAXCH 1024

void sig_act(int signo);
int main()
{
	struct sockaddr_in seraddr,cliaddr;
	socklen_t slen, clen;
	int sockfd,connfd;
	time_t tick;
	char buffer[MAXCH];
	pid_t pid;
	
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
	
	#ifndef DEBUGS
		struct sigaction act;
		act.sa_handler = sig_act;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		sigaction(SIGCHLD,&act,NULL);
		printf("\tSERVER --- USING SIGACTION FUNCTION\t");
	#else
		signal(SIGCHLD,sig_act);
	#endif

	while(1)
	{
		if((connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clen)) < 0 )
		{
			if(errno == EINTR)
			{
				continue;
			}
			perror("\tSERVER--- ACCEPT FAILED");
			exit(EXIT_FAILURE);
		}
	
		pid = fork();
		
		switch(pid)
		{
			case -1:
				perror("\tSERVER --- FORK FAILED");
				exit(EXIT_FAILURE);
			case 0:
				close(sockfd);
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
				exit(EXIT_SUCCESS);			
		}
		close(connfd);
	}		
	close(sockfd);
	return 0;
}

void sig_act(int signo)
{
	pid_t pid;
	int stat;
	
	while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
	{
		printf("\tSERVER --- child process (pid = %d) terminated\n", pid);
	}
	return;
}
