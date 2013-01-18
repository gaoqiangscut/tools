#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "hash.h"
#include "list.h"


#define MAX_INOTIFY_EVENT  (10240)
#define MAX_BUFLEN   (MAX_INOTIFY_EVENT*sizeof(struct inotify_event))
#define MAX_INOTIFY_WATCH  (1024000)


char** fd2path;
struct inotify_fd
{
	struct list_head  list;
	char * path;
	int fd;
};

int  path2fd(const char*path)
{
	static struct list_head pathtofd[MAX_INOTIFY_WATCH];
	int h=hash(path) % MAX_INOTIFY_WATCH;
	struct inotify_fd* id;
	struct list_head *head = &pathtofd[h];
	if(list_empty(head))
	{
		return -1;
	}
	list_for_each_entry(id,head,list)
	{
		if(strcmp(path,id->path)==0)
		{
			break;
		}
	}
	return id->fd;
}

int walk_through(int fd,const char * path)
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

int recursive_add_watch(int fd,const char* path)
{
}


int main(int argc,char*argv[])
{
	int fd=inotify_init();
	int wd=inotify_add_watch(fd,argv[1],IN_CREATE|IN_DELETE);
	fd2path=malloc(MAX_INOTIFY_WATCH*sizeof(char*));
	fd2path[wd]=malloc(1024);
	strcpy(fd2path[wd],argv[1]);
	void* arr=malloc(MAX_BUFLEN);
	struct inotify_event* ie;
	int ie_num;
	ssize_t ret;
	int i=0;
	uint32_t mask;
	char newpath[1024];
	int n=0;
	uint32_t watch_mask;
	while(ret=read(fd,arr,MAX_BUFLEN))
	{
		ie_num=ret/sizeof(struct inotify_event);
		for(i=0;i<ie_num;i++)
		{
			ie=(struct inotify_event*)arr+i;
			mask=ie->mask;
			if(mask & (IN_DELETE_SELF | IN_MOVE_SELF))
			{
				printf("delted: fd= %d,path= %s\n",ie->wd,fd2path[ie->wd]);
				inotify_rm_watch(fd,ie->wd);
			}
			if(mask & IN_DELETE)
			{
				char path[BUFSIZ];
				strcpy(path,fd2path[ie->wd]);
				strcat(path,"/");
				strcat(path,ie->name);
				inotify_rm_watch(fd,path2fd(path));
			}
			if(mask & IN_CREATE)
			{
				strcpy(newpath,fd2path[ie->wd]);
				strcat(newpath,"/");
				strcat(newpath,ie->name);
				/*fstat*/
				if(filetype(newpath,S_IFDIR))
				{
					watch_mask=IN_CREATE | IN_DELETE;
				}
				else
				{
					watch_mask=IN_MODIFY| IN_ACCESS |IN_DELETE_SELF;
				}
				n=inotify_add_watch(fd,newpath,watch_mask);
				printf("created: %s,n= %d\n",newpath,n);
				if(!fd2path[n])
				{
					fd2path[n]=malloc(1024);
				}
				strcpy(fd2path[n],newpath);
			}
			if(mask & IN_ACCESS)
			{
				printf("file accessed: %s\n",fd2path[ie->wd]);
			}
			if(mask & IN_MODIFY)
			{
				printf("file modified: %s\n",fd2path[ie->wd]);
			}
		}
	}
	for(i=0;i<MAX_INOTIFY_WATCH;i++)
	{
		if(fd2path[i])
		{
			free(fd2path[i]);
		}
	}
	free(fd2path);
	return 0;
}
