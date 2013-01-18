#include <sys/stat.h>
typedef (void*) (* walk_through_t)(void*arg);
int walk_through(const char * path,walk_through_t func)
{
	DIR* dir=opendir(path);
	struct dirent* dentry;
	while(dentry=readdir(dir))
	{
		const char* name=dentry->name;
		if(name && name[0]=='.' && (name[1]=='.' || name[1]=='\0'))
		{
			continue;
		}
		chdir(dentry->name);
		walk_through(".");
		chdir("..");
	}
	return 0;
}
