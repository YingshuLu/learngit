#include<stdlib.h>
#include<stdio.h>      // 
#include<unistd.h>     // for fork(), exec(), exel(),... 
#include<sys/wait.h>   // for wait(),waitpid()
#include<sys/types.h>  
#include<sys/socket.h> // for socket 
#include<arpa/inet.h>
#include<netinet/in.h> // for htonl(), htons(),...
#include<string.h>
#include<time.h>
#include<signal.h>
#include<errno.h> // for error sigation

#define MAXCH 1024

void sig_act(int signo);
int main()
{
	struct sockaddr_in seraddr,cliaddr; 	 // server and client address 
	socklen_t slen, clen;
	int sockfd,connfd;			 // socket and connected file descriptor 
	time_t tick;				 // for getting time
	char buffer[MAXCH];			 // apply for menmory to read and write
	pid_t pid; 				 // pid
	
	// initialize the server address
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(49991);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// get address length
	slen = sizeof(seraddr);
	clen = sizeof(cliaddr);

	// initialize socket file descriptor
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("\tSERVER--- SOCKFD CREATE FAILED");
		exit(EXIT_FAILURE);
	}

	// bind 'sockfd' to server address
	if(bind(sockfd,(struct sockaddr *) &seraddr,slen) < 0)
	{
		perror("\tSERVER--- BIND FAILED");
		exit(EXIT_FAILURE);
	}
	
	//listen for clients' connections
	if(listen(sockfd, 10) < 0)
	{
		perror("\tSERVER--- LISTEN FAILED");
		exit(EXIT_FAILURE);
	}
	
	// choose sigaction or signal function
	// parent process wait for child processes
	// NOTE: this waiting action would cause the interuption of the following 'accept' function 
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

	// server main loop
	while(1)
	{
		// server accept client connection and get 'connfd'
		if((connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clen)) < 0 )
		{
			// ignore the interuption
			// caused by waiting action by signal / sigaction function
			if(errno == EINTR)
			{
				continue;
			}
		
			perror("\tSERVER--- ACCEPT FAILED");
			exit(EXIT_FAILURE);
		}
	
		// create a child process
		pid = fork();
		
		switch(pid)
		{
			case -1:
				perror("\tSERVER --- FORK FAILED");
				exit(EXIT_FAILURE);
			// child process 
			case 0:
				// close the copied 'sockfd' of child process 
				close(sockfd);
				// display where the client is from
				printf("\tSERVER --- connection from %s, port %d\n",
					inet_ntop(AF_INET,&cliaddr.sin_addr, buffer, sizeof(buffer)),ntohs(cliaddr.sin_port));
				// get time number
				tick = time(NULL);
				snprintf(buffer,sizeof(buffer),"%.24s\r\n",ctime(&tick));
				
				// write to client 
				if(write(connfd,buffer,sizeof(buffer)) <= 0)
				{
					perror("\tSERVER --- WRITE FAILED");
					exit(EXIT_FAILURE);
				}
				// close the copied 'connfd' of child process
				close(connfd);
				exit(EXIT_SUCCESS);			
		}
		// close the orginal 'connfd'
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
