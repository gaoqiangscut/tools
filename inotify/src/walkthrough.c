#include <sys/stat.h>
typedef (void*)
int walk_through(const char * path,)
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
