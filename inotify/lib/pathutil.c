#include "pathutil.h"
int filetype(const char* path,mode_t mode)
{
	struct stat st;
	int ret=0;
	ret=stat(path,&st);
	if(ret!=0)
		return ret;
	return st.st_mode & mode;
}
