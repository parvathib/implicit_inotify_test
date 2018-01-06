/* To test open operation */
#include "inotify_helper.h"
#include <sys/stat.h>
#include <fcntl.h>

#define BILLION 1000000000L

uint64_t inotify_open_file_test(p_database_t *flist, int *index, int num_files)
{
    FILE *filep;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    for(i = 0; i < num_files; i++) {
        if (flist[index[i]].path != NULL) {
            //printf("path %s \n", flist[index[i]].path);
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                perror("clock_gettime"); 
            filep = fopen(flist[index[i]].path, "r");
            if(!filep)
                perror("open_what happened");
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                perror("clock_gettime");
        uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
        fprintf(open_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
        time += t;
        //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
        fclose(filep);
        count++;
        }
        //printf("%s time %f\n",flist[index[i]].path, time); 
    }
    
    return (time / (count * 1.0));
        
}

uint64_t inotify_close_file_test(p_database_t *flist, int *index, int num_files)
{
    FILE *filep;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    for(i = 0; i < num_files; i++) {
        if (flist[index[i]].path != NULL) {
            filep = fopen(flist[index[i]].path, "r");
            if(!filep)
                perror("open");
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                perror("clock_gettime"); 
            fclose(filep);
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                perror("clock_gettime");
            uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
            fprintf(close_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
            time += t;
            //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
            count++; 
        }
    }
    
    return (time / (count * 1.0));
        
}

uint64_t inotify_write_file_test(p_database_t *flist, int *index, int num_files)
{
    FILE *filep;
    struct stat statbuf;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    char *buf = "Hello World";
    for(i = 0; i < num_files; i++) {
        if (flist[index[i]].path != NULL) {
            if(stat(flist[index[i]].path, &statbuf) == -1)
                perror("stat");
            if(!S_ISDIR(statbuf.st_mode)) {
                filep = fopen(flist[index[i]].path, "w");
                if(!filep)
                    perror("open");
                if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                    perror("clock_gettime");
                if(fwrite(buf, sizeof(char), strlen(buf), filep) < strlen(buf))
                    perror("fwrite");
                fflush(filep);
                if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                    perror("clock_gettime");
                uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
                fprintf(write_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
                time += t;
                //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
                count++;
                fclose(filep);
            }
        }
    }
    
    return (time / (count * 1.0));
        
}

uint64_t inotify_read_file_test(p_database_t *flist, int *index, int num_files)
{
    FILE *filep;
    int i;
    struct timespec start;
    struct timespec end;
    char buf[MAXBUF] = {0};
    uint64_t time = 0;
    int count = 0;
    for(i = 0; i < num_files; i++) {
        if (flist[index[i]].path != NULL) {
            filep = fopen(flist[index[i]].path, "r");
            if(!filep)
                perror("open");
        }
        if(!flist[index[i]].dir) {
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                perror("clock_gettime");
            if(fread(buf, MAXBUF, 1, filep) < 0) 
                perror("fread");
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                perror("clock_gettime");
            uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
            fprintf(read_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
            time += t;
            //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
            count++;
        }
        fclose(filep);
    }
    return (time / (count * 1.0));
}

uint64_t inotify_rename_file_test(p_database_t *flist, int *index, int num_files)
{
    int i;
    struct timespec start;
    struct timespec end;
    char old_path[MAXBUF] = {0};
    uint64_t time = 0;
    int count = 0;
    struct stat statbuf;
    for(i = 0; i < num_files; i++) {
        if ((flist[index[i]].path != NULL) && 
            strcmp(flist[index[i]].path, "tmp")) {
            if(!flist[index[i]].dir) {
                strcpy(old_path, flist[index[i]].path);
                strcat(flist[index[i]].path, "new");
                if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                    perror("clock_gettime");
                if(rename(old_path, flist[index[i]].path) < 0) {
                    perror("rename");
                }
                if(stat(flist[index[i]].path, &statbuf) < 0)
                    perror("stat");
                
                if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                    perror("clock_gettime");
                uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
                fprintf(rename_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
                time += t;
                //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));

                count++;
            }
        }
    }

    return (time / (count * 1.0));
        
}
uint64_t inotify_chmod_file_test(p_database_t *flist, int *index, int num_files)
{
    int ret = 0;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    mode_t mode = (S_IRUSR | S_IRGRP | S_IROTH | 
                S_IWUSR | S_IWGRP | S_IWOTH | S_IXUSR | S_IXGRP | S_IXOTH);
    for(i = 0; i < num_files; i++) {
        if (flist[index[i]].path != NULL) {
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                perror("clock_gettime");
            ret = chmod(flist[index[i]].path, mode);
            if(ret < 0)
                 perror("chmod");
                
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                perror("clock_gettime");
            uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
            fprintf(chmod_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
            time += t;
            //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
            count++;
        }
    }
    return (time / (count * 1.0));
}

uint64_t inotify_delete_file_test(p_database_t *flist, int *index, int num_files)
{
    struct stat statbuf;
    int ret;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    for(i = 0; i < num_files; i++) {
        if (flist[i].path != NULL) {
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) == -1)
                perror("clock_gettime");
            if(stat(flist[i].path, &statbuf) == -1)
                perror("stat");
            if(!S_ISDIR(statbuf.st_mode))
            {
                ret = unlink(flist[i].path);
            } 
            else
            {
                if(strcmp(flist[i].path, "tmp"))
                    ret = rmdir(flist[i].path);
            }
            if(ret)
                perror("remove");
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) == -1)
                perror("clock_gettime");
            uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
            fprintf(delete_fp, "%lu, %s\n", t, flist[i].path);
#endif
            time += t;
            //time += (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
            count++;
        }
    }
    
    return (time / (count * 1.0));
        
}

uint64_t inotify_create_file_test(p_database_t *flist, int *index, int num_files)
{
    struct stat statbuf;
    int ret;
    int i;
    struct timespec start;
    struct timespec end;
    uint64_t time = 0;
    int count = 0;
    mode_t mode = (S_IRUSR | S_IRGRP | S_IROTH | 
                S_IWUSR | S_IWGRP | S_IWOTH | S_IXUSR | S_IXGRP | S_IXOTH);
    for(i = num_files - 1; i >= 0; i--) {
        if (flist[i].path != NULL) {
            if(stat(flist[i].path, &statbuf) == -1) 
            {
              // printf(" path %s is %s\n", flist[i].path, flist[i].dir ? "dir":"file");
                if(clock_gettime(CLOCK_MONOTONIC, &start) == -1)
                    perror("clock_gettime");
                if(!flist[i].dir) { 
                    //printf("file path %s\n", flist[i].path);
                    ret = creat(flist[i].path, mode);
                }
                else {
                    //printf("dir path %s\n", flist[i].path);
                    ret = mkdir(flist[i].path, mode);
                }
                if(ret == -1)
                    perror("create");
                if(stat(flist[i].path, &statbuf) == -1)
                    perror("stat");
                if(clock_gettime(CLOCK_MONOTONIC, &end) == -1)
                    perror("clock_gettime");
                if(!flist[i].dir) {
                    close(ret);
                }
                uint64_t t = (BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec));
#if LATENCY_BOXPLOT
                fprintf(create_fp, "%lu, %s\n", t, flist[index[i]].path);
#endif
                time += t;
                //time += BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
                count++;
            }
        }
    }
    
    return (time / (count * 1.0));
        
}
