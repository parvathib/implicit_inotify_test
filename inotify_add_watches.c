#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include "inotify_helper.h"
#include <sys/stat.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ftw.h>
#include <stdint.h>
char command[MAXBUF];
char dir[MAXBUF];
bool tool;
bool implicit;
bool verbose;            // If true, prints additional output

#define CACHE_CONTROL_FILE "/proc/sys/vm/drop_caches"
#define CACHE_DROP_ALL_FLAG "3"
#define CACHE_DROP_CACHES_FLAG "2"


char *dfname;
int inotifyFd;


static int dirTree(const char *pathname, const struct stat *sbuf,
                    int typeflag, struct FTW *ftwb)
{
    int wd;
    if(typeflag & FTW_D) {
        wd = inotify_add_watch(inotifyFd, pathname, IN_ALL_EVENTS);
        if(wd == -1)
        {
            perror("inotify_add_watch");
        }
        printf("inotfyFD %d Adding watch to %s, wd %d \n",inotifyFd, pathname, wd);
    }
	return 0; 
}

static int flush_caches()
{
    int retVal = 0;
    int fd;
    if (geteuid() != 0) {
        fprintf(stderr, "Cache flushing requires root.\n");
    }
    else {
        sync();
        fd = open(CACHE_CONTROL_FILE, O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: %s\n", strerror(errno),
                            CACHE_CONTROL_FILE);
        }
        else {
            char drop_all_message[] = CACHE_DROP_ALL_FLAG;
            int length = strlen(drop_all_message);
            int written = write(fd, drop_all_message,
                                    length);
            if (written == -1) {
                fprintf(stderr, "%s: %s\n", strerror(errno),
                                    drop_all_message);
            }
            else if (written != length) {
                fprintf(stderr, "Error: only wrote %d of %d bytes.\n", 
                            written, length);
            }
            else {
                //successful write 
                retVal = 1;
            }
            close(fd);
        }
    }
            
    return retVal;
}


int main(int argc, char **argv)                                                  
{
    char c;                                                                      
	int i;
    char *fname = NULL;
    int flags = 0;
    FILE *log_fp; 
    // Parse the command line                                                    
 	while ((c = getopt(argc, argv, "hvid:")) != EOF) {
        switch (c) {
			case 'h':                   // Prints help message
				break;
			case 'v':                   // Emits additional diagnostic info
                verbose = true;
				break;
			case 'i':
				implicit = true;
                break;
            case 'd':
                dfname = strdup(optarg);
                dfname[strlen(dfname)] = '\0';
                break;
			default :
		        printf("Wrong usage. try again\n");
                break;
        }
	}

    flush_caches();
    if (optind < argc)
		printf("First nonoption argument is \"%s\" at argv[%d]\n", 
                                                argv[optind], optind);
    
    printf(" Running test using %s \n", tool ? "inotify_tool" : (implicit ? "implicit watch" : "explicit watch"));
    inotifyFd = inotify_init();
    if(inotifyFd == -1) {
        perror("inotify_init");
    }
    printf("dfname %s\n", dfname);
    if(implicit)
    {
        int wd;
        wd = inotify_add_watch(inotifyFd, dfname, (IN_ALL_EVENTS | 0x00800000));
        if(wd == -1)
        {
            perror("inotify_add_watch implicit");
            exit(-1);
        }
        printf("Added watch to %s, wsd %d \n", dfname, wd);
    }
    else
    {

        flags = FTW_DEPTH;
        if(nftw(dfname, dirTree, 10, flags) == -1) {
            perror("nftw");
            exit(-1);
        }
    }
   
   
    flush_caches();
    while(1);


}




