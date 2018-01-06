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
char *watchdir;
char *shellprog;
char maskbuf[10];
int testcase;
bool verbose;            // If true, prints additional output
int inode_cache;
int dentry_cache;

#define CACHE_CONTROL_FILE "/proc/sys/vm/drop_caches"
#define CACHE_DROP_ALL_FLAG "3"

#define LOG_FILE "./readings.csv"
#define LOG_FILE_IMPLICIT "./readings_implicit.csv"

#define ALL_DIRS_FILE "./dir_paths.txt"
#define ALL_FILEPATHS_FILE  "./allfile_paths.txt"

#define LOG_OPEN "./log_open.csv"
#define LOG_CLOSE "./log_close.csv"
#define LOG_WRITE "./log_write.csv"
#define LOG_READ "./log_read.csv"
#define LOG_DELETE "./log_delete.csv"
#define LOG_RENAME "./log_rename.csv"
#define LOG_CHMOD "./log_chmod.csv"
#define LOG_CREATE "./log_create.csv"

#define LOG_OPEN_IMPL "./log_open-impl.csv"
#define LOG_CLOSE_IMPL "./log_close-impl.csv"
#define LOG_WRITE_IMPL "./log_write-impl.csv"
#define LOG_READ_IMPL "./log_read-impl.csv"
#define LOG_DELETE_IMPL "./log_delete-impl.csv"
#define LOG_RENAME_IMPL "./log_rename-impl.csv"
#define LOG_CHMOD_IMPL "./log_chmod-impl.csv"
#define LOG_CREATE_IMPL "./log_create-impl.csv"

#define SLAB_LINE_LEN 2048


p_database_t dir_wd[100000];
p_database_t file_wd[600000];
char *dfname;

int random_idx[600000];
int implicit_wd;
int num_files;
int num_dirs;

int generated_random;


void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}


void get_slab_info(void)
{
    FILE *slab_fp;
    char buf[SLAB_LINE_LEN] = {0};
    char name[20] = {0};
    //int size;

    slab_fp = fopen("/proc/slabinfo", "r");
    if(!slab_fp)
    {
        perror("fopen of slab_fp");
    }
    inode_cache = 0;
    dentry_cache =0;
    int active_objs = 0, num_objs = 0, objsize = 0;
    while(1)
    {
        memset(name, 0, 20);
        if(fgets(buf, SLAB_LINE_LEN, slab_fp))
        {
            sscanf(buf, "%s %d %d %d", name, &active_objs, &num_objs, &objsize);
            if(!strcmp("ext4_inode_cache", name))
            {
                inode_cache = (active_objs * objsize) / 1024;
                //printf("Name : %s active_objs %d num_objs %d objsize %d\n", name, active_objs, num_objs, objsize);
            }
            if(!strcmp("dentry", name))
            {
                dentry_cache = (active_objs * objsize) / 1024;
                //printf("Name : %s active_objs %d num_objs %d objsize %d\n", name, active_objs, num_objs, objsize);
                //printf("Name : %s size %d \n", name, dentry_cache);
            }
        }
        else
            break;
    }
    return;
}

static int dirTree(const char *pathname, const struct stat *sbuf,
                    int typeflag, struct FTW *ftwb)
{
    strcpy(file_wd[num_files].path, pathname);
    if(typeflag & FTW_D) {
        strcpy(dir_wd[num_dirs].path, pathname);
        file_wd[num_files].dir = 1;
        dir_wd[num_dirs].wd = -1;
        num_dirs++;
    }
    num_files++;
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
        //printf("syncing..\n");
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

/*
 * blankline - Return true if str is a blank line
 */
int blankline(char *str)
{
    while (*str) {
        if (!isspace(*str++)) {
            return 0;
        }
    }
    return 1;
}

void add_watches(int inotifyFd, int implicit)
{
    int i = 0,wd = 0;
    struct stat statbuf;
    if(implicit) {
        implicit_wd = inotify_add_watch(inotifyFd, dfname, IN_ALL_EVENTS | 0x00800000);
        //printf("%s name %s fd %d\n", __func__, dfname, inotifyFd);
        if(implicit_wd < 0) {
            printf("name %s fd %d\n", dfname, inotifyFd);
            perror("inotify_add_watch recursive");
        }
    }
    else {
        while(i < num_dirs) {
            if(stat(dir_wd[i].path, &statbuf) != -1) {
                wd = inotify_add_watch(inotifyFd, dir_wd[i].path, IN_ALL_EVENTS); 
                //printf("%s name %s wd %d\n", __func__, dir_wd[i].path, wd);
                if(wd < 0) {
                    printf("name %s wd %d\n", dir_wd[i].path, wd);
                    perror("inotify_add_watch");
                }
                dir_wd[i].wd = wd;
            }
            i++;

        }
    }

}

void remove_watches(int inotifyFd, int implicit)
{
    int i;
    struct stat statbuf;
    if(implicit) {
        //printf("%s wd %d\n", __func__, implicit_wd);
        if(inotify_rm_watch(inotifyFd, implicit_wd) < 0) {
            perror("inotify_rm_watch recursive");
        }
    }
    else {
        while(i < num_dirs) {
            if(stat(dir_wd[i].path, &statbuf) != -1) {
                //printf("%s wd %d\n", __func__, dir_wd[i].wd);
                if(inotify_rm_watch(inotifyFd, dir_wd[i].wd) < 0) {
                    printf("inotify_rm_watch wd %d\n", dir_wd[i].wd);
                    perror("inotify_rm_watch");
                }
            }
            i++;
        }
    }
}



void randomize(int arr[],int n)
{
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand (time(NULL) );
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);
                                                  
        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
#if 0
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < i; j++)
        {
            if(arr[i] == arr[j])
            {
                printf("duplicate\n");
            }
        }
        printf("%d ", arr[i]);
    }
    printf("\n");
#endif
}


int main(int argc, char **argv)                                                  
{
    char c;                                                                      
	int i;
    char *fname = NULL;
    int flags = 0;
    int inotifyFd;
    FILE *log_fp; 
    // Parse the command line                                                    
 	while ((c = getopt(argc, argv, "hvitf:d:")) != EOF) {
        switch (c) {
			case 'h':                   // Prints help message
				usage();
				break;
			case 'v':                   // Emits additional diagnostic info
                verbose = true;
				break;
			case 'i':
				implicit = true;
                break;
            case 't':
                tool = true;
				break;
			case 'f':
				fname = strdup(optarg);
                fname[strlen(fname)] = '\0';
                break;
            case 'd':
                dfname = strdup(optarg);
                dfname[strlen(dfname)] = '\0';
                break;
			default :
				usage();
		}
	}

    flush_caches();
    if (optind < argc)
		printf("First nonoption argument is \"%s\" at argv[%d]\n", 
                                                argv[optind], optind);
    printf("dfname %s\n", dfname);
    flags = FTW_DEPTH;
    if(nftw(dfname, dirTree, 100, flags) == -1) {
        perror("nftw");
        exit(-1);
    }
    printf(" Running test using %s \n", tool ? "inotify_tool" : (implicit ? "implicit watch" : "explicit watch"));
    inotifyFd = inotify_init();
    if(inotifyFd == -1) {
        perror("inotify_init");
    }
   
   
    srand(15746);
    flush_caches();
    for(i = 0; i< num_files; i++) {
        random_idx[i] = i; //(rand() % num_files);
    } 
    randomize(random_idx, num_files);
    
    if(!implicit)
    {
        log_fp = fopen(LOG_FILE, "a+");
    }
    else
    {
        log_fp = fopen(LOG_FILE_IMPLICIT, "a");
    }
    if(!log_fp)
    {
        perror("fopen log_fp");
    }
    fprintf(log_fp, "num files %d\n", num_files);
    fprintf(log_fp, "num dirs %d \n", num_dirs);
    fprintf(log_fp, "operation, time(ns), inode_cache before, inode_cache after, dentry_cache before, dentry_cache after\n");
    printf("num files %d \n", num_files); 
    printf("num dirs %d \n", num_dirs); 
    long long unsigned avg_time = 0;
    int inode_cache_before = 0, inode_cache_after = 0, dentry_before = 0, dentry_after = 0;


//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;

    add_watches(inotifyFd, implicit);
    flush_caches();
#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        open_fp = fopen(LOG_OPEN, "w+");
    }
    else
    {
        open_fp = fopen(LOG_OPEN_IMPL, "w+");
    }
    if(!open_fp)
    {
        printf("error opening %s\n", implicit ? LOG_OPEN_IMPL:LOG_OPEN);
        perror("fopen open_fp");
    }
    fprintf(open_fp, "num files %d\n", num_files);
    fprintf(open_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_open_file_test(file_wd, random_idx, num_files);
#if LATENCY_BOXPLOT
    fclose(open_fp);
#endif
    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "open", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "open", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //printf("%s, %llu \n", "open", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
    

//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();

#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        close_fp = fopen(LOG_CLOSE, "w+");
    }
    else
    {
        close_fp = fopen(LOG_CLOSE_IMPL, "w+");
    }
    if(!close_fp)
    {
        perror("fopen close_fp");
    }
    fprintf(close_fp, "num files %d\n", num_files);
    fprintf(close_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_close_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(close_fp); 
#endif
    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "close", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "close", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //fprintf(log_fp, "%s, %llu \n", "close", avg_time);
    //printf("close %llu \n", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
    
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();

#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        write_fp = fopen(LOG_WRITE, "w+");
    }
    else
    {
        write_fp = fopen(LOG_WRITE_IMPL, "w+");
    }
    if(!write_fp)
    {
        perror("fopen write_fp");
    }
    fprintf(write_fp, "num files %d\n", num_files);
    fprintf(write_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_write_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(write_fp);
#endif

    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;

    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "write", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "write", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //fprintf(log_fp, "%s, %llu \n", "write", avg_time);
    //printf("Write %llu \n", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();
#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        read_fp = fopen(LOG_READ, "w+");
    }
    else
    {
        read_fp = fopen(LOG_READ_IMPL, "w+");
    }
    if(!read_fp)
    {
        perror("fopen read_fp");
    }
    fprintf(read_fp, "num files %d\n", num_files);
    fprintf(read_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_read_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(write_fp);
#endif

    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "read", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "read", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    
    //fprintf(log_fp, "%s, %llu \n", "read", avg_time);
    //printf("Read %llu \n", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
    
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();

#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        chmod_fp = fopen(LOG_CHMOD, "w+");
    }
    else
    {
        chmod_fp = fopen(LOG_CHMOD_IMPL, "w+");
    }
    if(!chmod_fp)
    {
        perror("fopen chmod_fp");
    }
    fprintf(chmod_fp, "num files %d\n", num_files);
    fprintf(chmod_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_chmod_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(chmod_fp);
#endif

    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "chmod", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "chmod", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //printf("chmod %llu \n", avg_time);
    //fprintf(log_fp, "%s, %llu \n", "chmod", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();
    
#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        rename_fp = fopen(LOG_RENAME, "w+");
    }
    else
    {
        rename_fp = fopen(LOG_RENAME_IMPL, "w+");
    }
    if(!rename_fp)
    {
        perror("fopen rename_fp");
    }
    fprintf(rename_fp, "num files %d\n", num_files);
    fprintf(rename_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_rename_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(rename_fp);
#endif
    
    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "rename", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "rename", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    
    //fprintf(log_fp, "%s, %llu \n", "rename", avg_time);
    //printf("Rename %llu \n", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();
    
#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        delete_fp = fopen(LOG_DELETE, "w+");
    }
    else
    {
        delete_fp = fopen(LOG_DELETE_IMPL, "w+");
    }
    if(!delete_fp)
    {
        perror("fopen delete_fp");
    }
    fprintf(delete_fp, "num files %d\n", num_files);
    fprintf(delete_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_delete_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(delete_fp);
#endif

    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "remove", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "remove", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //fprintf(log_fp, "%s, %llu \n", "delete", avg_time);
    //printf("Remove %llu \n", avg_time);
    remove_watches(inotifyFd, implicit);
    flush_caches();
//##############################################
    get_slab_info();
    inode_cache_before = inode_cache;
    dentry_before = dentry_cache;
    
    add_watches(inotifyFd, implicit);
    flush_caches();

#if LATENCY_BOXPLOT 
    if(!implicit)
    {
        create_fp = fopen(LOG_CREATE, "w+");
    }
    else
    {
        create_fp = fopen(LOG_CREATE_IMPL, "w+");
    }
    if(!create_fp)
    {
        perror("fopen create_fp");
    }
    fprintf(create_fp, "num files %d\n", num_files);
    fprintf(create_fp, "num dirs %d\n", num_dirs);
#endif
    avg_time = inotify_create_file_test(file_wd, random_idx, num_files); 
#if LATENCY_BOXPLOT 
    fclose(create_fp);
#endif

    flush_caches();
    get_slab_info();
    inode_cache_after = inode_cache;
    dentry_after = dentry_cache;
    
    fprintf(log_fp, "%s, %llu, %d, %d, %d, %d\n", "create", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    printf("%s, %llu, %d, %d, %d, %d\n", "create", avg_time,
            inode_cache_before, inode_cache_after, dentry_before, dentry_after);
    //fprintf(log_fp, "%s, %llu \n\n\n", "create", avg_time);
    //printf("Create %llu \n", avg_time);
    if(implicit)
        remove_watches(inotifyFd, implicit);
    flush_caches();
    
//##############################################
    fflush(log_fp);
    fclose(log_fp);
}




