#include<stdio.h>
#include<stdlib.h>

void my_exit1(void)
{
	printf("register my_exit1\n");
}

void my_exit2(void)
{
	printf("register my_exit2\n");
}

int main()
{
	char *name = "HOME";
	char *res = getenv(name);
	printf("%s = %s\n",name,res);
	if(atexit(my_exit1) != 0)
	{
		perror("can't register my_exit1");
		exit(EXIT_FAILURE);
	}

	if(atexit(my_exit2) != 0)
	{
		perror("can't register my_exit2");
		exit(EXIT_FAILURE);
	}

	if(atexit(my_exit2) != 0)
	{
		perror("can't register my_exit2");
		exit(EXIT_FAILURE);
	}
	printf("register comlpeted!\n");
	return 0;
}
