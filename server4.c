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
#include<sys/select.h>


#define MAXCH 2048

void sig_act(int signo);
int main()
{
	struct sockaddr_in seraddr,cliaddr; 	 // server and client address 
	socklen_t slen, clen;
	int sockfd,connfd;			 // socket and connected file descriptor 
	char buffer[MAXCH];			 // apply for menmory to read and write
	pid_t pid;
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

	int maxfd = sockfd; //rset maxfd
	fd_set rset;	// readset
	int fd;     // tmp fd for multi-connfd  
	int client[FD_SETSIZE]; // connfd members
	int i,n,maxi, nready; // maxi : the position of vaild connfd in client[]; nready : the number of ready file descriptor by select
	// initalize the client[]
	for(i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;
	}

	// emptize the rset used in select function
	FD_ZERO(&rset);
	maxi = 0;
	// server main loop
	while(1)
	{
		FD_SET(sockfd,&rset);

		if((nready = select(maxfd + 1,&rset,NULL,NULL,NULL)) < 0 )
		{
			// caused by signal waiting for fork()
			if(errno == EINTR)
			{
				continue;
			}
			perror("\tSERVER --- SELECT FAILED");
			exit(EXIT_FAILURE);
		}
			
		if(FD_ISSET(sockfd,&rset))
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
			
			// put the vaild connfd in the first empty room
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i] == -1)
				{
					client[i] = connfd;
					break;
				}
			}

			// if having no left room, tell server quit or discard the connfd	
			if(i == FD_SETSIZE)
			{
				printf("\tSERVER --- TOO MANY CLIENTS\n");
				exit(EXIT_FAILURE);
			}
			
			// select for connfd in next loop
			FD_SET(connfd,&rset);
			if(connfd > maxfd)
			{
				maxfd = connfd;
			}
			// get maximum used label
			if(maxi < i)
			{
				maxi = i;
			}
			
			// eccept this sockfd, if having no ready fd , keep waiting for select
			if(--nready <= 0)
			{
				continue;
			}
			
		}

		// check client[], and pick out the vaild connfd for further process by fork()
		for(i=0;i<=maxi;i++)
		{
			
			if( (fd = client[i]) < 0)
				continue;
			else if(FD_ISSET(fd,&rset))
			{

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
				
						if((n = read(fd,buffer,MAXCH)) < 0)
						{
							perror("\tSERVER --- READ FROM CLIENT FAILED");
							exit(EXIT_FAILURE);
						}
						else if(n == 0)
						{
							close(fd);
						}
						else
						{
							printf("\tFROM SERVER %d --- has recieved:\n %s\n",getpid(),buffer);	
							// write to client 
							if(write(fd,buffer,sizeof(buffer)) < 0)
							{
								perror("\tSERVER --- WRITE FAILED");
								exit(EXIT_FAILURE);
							}
						}
						// close the copied 'connfd' of child process
						exit(EXIT_SUCCESS);			
				}
				// close the orginal 'connfd'
				close(connfd);
				// discard this closed fd in select and client[]
				FD_CLR(fd,&rset);
				client[i] = -1;
				// check if haing vaild fd waiting process 
				if(--nready <= 0)
					break;
			}
	
		}		
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

