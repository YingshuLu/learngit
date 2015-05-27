#include<stdio.h>
#include<stdlib.h>
#define MAX_LINE  512
int main()
{
	char name[MAX_LINE],line[MAX_LINE];

	FILE* fp;
	printf("tmp name is : %s\n",tmpnam(NULL));
	
	tmpnam(name);
	printf("2: %s\n",name);
	
	if((fp = tmpfile()) ==NULL)
		perror("open tmp file failed");
	fputs("TEST:one line of output\n",fp);
	rewind(fp);
	fgets(line,sizeof(line),fp);
	fputs(line,stdout);
	return 0;

}
