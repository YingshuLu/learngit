#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int global = 6;
char buf[]="write to stdout\n";

int main()
{
	int var;
	pid_t pid;

	var = 88;
	if(write(fileno(stdout),buf,strlen(buf)) != strlen(buf))
	{
		perror("write error");
		exit(-1);
	}

	printf("Before fork\n");
	if((pid = vfork()) < 0)
	{
		perror("fork error");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0)
	{
		global++;
		var++;
	}
	else{
		sleep(2);
	}

	printf("pid = %ld, global = %d, var = %d\n", (long) getpid(),global, var);
	exit(0);
}
