#ifndef __INOTIFY_HELPER_H__
#define __INOTIFY_HELPER_H__
#define _XOPEN_SOURCE 500
#include <sys/inotify.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <ftw.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ftw.h>
#include <ctype.h>
//#include "csapp.h"
#define MAXBUF 255
#define LATENCY_BOXPLOT 0
extern bool verbose;            // If true, prints additional output
extern bool tool;
extern bool implicit;
extern void usage(void);
extern int parse_args(char * str);
extern int my_add_watch(char *path);

FILE *open_fp;
FILE *close_fp;
FILE *write_fp;
FILE *read_fp;
FILE *delete_fp;
FILE *rename_fp;
FILE *chmod_fp;
FILE *create_fp;




typedef struct {
    char path[MAXBUF];
    int dir;
    int wd; 
}p_database_t;

extern uint64_t inotify_open_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_close_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_write_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_read_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_delete_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_rename_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_chmod_file_test(p_database_t *flist, int *index, int num_files);
extern uint64_t inotify_create_file_test(p_database_t *flist, int *index, int num_files);
#endif
