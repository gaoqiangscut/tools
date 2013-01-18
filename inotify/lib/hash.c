#include <stdint.h>
#include <stdio.h>
uint64_t hash(const char* str)
{
	const char*pt=str;
	uint64_t ret;
	while(pt&&*pt)
	{
		ret=ret*(*pt++);
	}
	return ret;
}
/*int main(int argc,char*argv[])
{
	FILE*fp=fopen(argv[1],"r");
	char buf[BUFSIZ];
	while(1)
	{
		if(feof(fp))
		{
			break;
		}
		fgets(buf,sizeof(buf),fp);
		printf("%lu\n",hash(buf));
	}
	return 0;
}*/
