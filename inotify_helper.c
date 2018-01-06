#include "inotify_helper.h"
char *mask_all = "access,modify,attrib,close_write,close_nowrite,close,open,moved_to,moved_from,move,move_self,create,delete,delete_self,unmount";

int inotifyFdx;

static int dirTree(const char *pathname, const struct stat *sbuf,
                    int typeflag, struct FTW *ftwb)
{
	int wd;
	if(typeflag & FTW_D) {
	    wd = inotify_add_watch(inotifyFdx, pathname, IN_ALL_EVENTS);
	    if(wd == -1) {
		    printf("error: inotify_add_watch %s", strerror(errno));
		    exit(-1);
	    }
        if(verbose)
	        printf("Watching %s using wd %d\n", pathname, wd);
	}
	return 0; 
}


/*
 * usage - print a help message
 */
void usage(void)
{
    printf("Usage: inotify_watch [-hvrtx] [e] <arguments>\n");
    printf("   -e   Events to be watched \n");
    printf("   -h   print this message\n");
    printf("   -i   add explicit recursive watches\n");
    printf("   -r   add recursive watches\n");
    printf("   -t   use inotify_tools. Use native implementation if not specified\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -x   \n");
    exit(EXIT_FAILURE);
}


int get_mask(char *mstr) 
{
    if(!strcmp(mstr, "access"))
        return IN_ACCESS;
    if(!strcmp(mstr, "modify"))
        return IN_MODIFY;
    if(!strcmp(mstr, "attrib"))
        return IN_ATTRIB;
    if(!strcmp(mstr, "close_write"))
        return IN_CLOSE_WRITE;
    if(!strcmp(mstr, "close_nowrite"))
        return IN_CLOSE_NOWRITE;
    if(!strcmp(mstr, "close"))
        return IN_CLOSE;
    if(!strcmp(mstr, "open"))
        return IN_OPEN;
    if(!strcmp(mstr, "moved_to"))
        return IN_MOVED_TO;
    if(!strcmp(mstr, "moved_from"))
        return IN_MOVED_FROM;
    if(!strcmp(mstr, "move"))
        return IN_MOVE;
    if(!strcmp(mstr, "move_self"))
        return IN_MOVE_SELF;
    if(!strcmp(mstr, "create"))
        return IN_CREATE;
    if(!strcmp(mstr, "delete"))
        return IN_DELETE;
    if(!strcmp(mstr, "delete_self"))
        return IN_DELETE_SELF;
    if(!strcmp(mstr, "unmount"))
        return IN_UNMOUNT;
    return 255; 
}

int parse_args(char * str)
{
    char delim[2] = ",";
    char *token;
    int mask = 0;
    int ret;
    token = strtok(str, delim);
    while(token != NULL) {
        //printf("token  %s \n ", token);
        ret = get_mask(token);
        if(ret == 255)
            return 0;
        else
            mask |= ret;
        token = strtok(NULL, delim);
    }
    return mask;
}



int add_watch_tool(char *path, int t) 
{
    char buf[MAXBUF];
    if(t != 0) { //wait for given amount of time
        sprintf(buf, "/usr/bin/inotifywait -r -t %d -e %s %s\n", t, mask_all, path);
    }
    else // Wait indefinitely
        sprintf(buf, "/usr/bin/inotifywait -r -m -e %s %s\n",  mask_all, path);
    printf("add_watch_tool %s\n", buf);
    if(system(buf) == -1) {
        printf("inotify_tool unable to run\n");
        return -1;
    }
    return 0;
}

int add_watch_implicit(char *path) 
{
    int inotifyFd, wd;
    inotifyFd = inotify_init();
    if(inotifyFd == -1) {
        printf("error : inotify_init");
        return -1;
    }
    wd = inotify_add_watch(inotifyFd, path, (IN_ALL_EVENTS | 0x00800000));
    if(wd == -1) {
        printf("error: inotify_add_watch %s", strerror(errno));
        return -1;
    }
    printf("Established implicit wathes on directory %s\n", path);
    close(inotifyFd);
    return 0;
}

int add_watch_explicit(char *path)
{
    int flags = 0;
    inotifyFdx = inotify_init();
    if(inotifyFdx == -1) {
        printf("error : inotify_init");
        exit(-1);
    }
    flags = FTW_DEPTH;
    if(nftw(path, dirTree, 10, flags) == -1) {
        perror("nftw");
        exit(-1);
    }   
    close(inotifyFdx);
    printf("Established watches for %s \n", path);
    return 0;
}

int my_add_watch(char *path)
{
   struct timespec start;
   struct timespec end;
    uint64_t diff;
    //struct timespec start, now;
    //int secs, nanosecs;
    int ret;
    // Add watches using inotify_tool
    if(tool) {
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
            perror("clock_gettime");
        ret = add_watch_tool(path, 1);
        if(ret) {
            printf("add watches using inotify_tool failed\n");
            return -1;
        }
       if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
           perror("clock_gettime");
    }
    else {
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
            perror("clock_gettime");
        if(implicit) {
            ret = add_watch_implicit(path);
        }
        else {
            ret = add_watch_explicit(path);
        }
        if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
            perror("clock_gettime");
    }
    diff = 1000000000L * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
//    secs = now.tv_sec - start.tv_sec;
//    nanosecs = now.tv_nsec - start.tv_nsec;
//    if (nanosecs < 0) {
//        secs--;
//        nanosecs += 1000000000;
//    }
//    printf(" >>>>> %d.%03d\n", secs, (nanosecs + 500000) / 1000000); 
    //printf(" >>>>> %6.8f\n", ((curr.tv_sec - start.tv_sec) - (tool? 1 : 0))+
    //                    (curr.tv_usec - start.tv_usec) / 1000000.0);
    printf(" time = %llu nanoseconds\n", (long long unsigned int) diff);
    return 0;
}

