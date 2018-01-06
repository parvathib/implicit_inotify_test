#
# Makefile for the CS:APP Shell Lab
# 
# Type "make" to build your shell and driver
#
CC = /usr/bin/gcc
CFLAGS = -Wall -g -Werror
LIBS = -lpthread

#FILES = sdriver runtrace tsh myspin1 myspin2 myenv myintp \
      myints mytstpp mytstps mysplit mysplitp mycat
FILES = inotify_test #get_dirpaths
#FILES = inotify_watch
all: $(FILES)

#
# Using link-time interpositioning to introduce non-determinism in the
# order that parent and child execute after invoking fork
#
#inotify_watch: inotify_watch.c inotify_helper.c fork.c
#	$(CC) $(CFLAGS) -Wl,--wrap,fork -o inotify_watch inotify_watch.c inotify_helper.c fork.c csapp.c $(LIBS)
inotify_test: inotify_test.c inotify_helper.c inotify_file_operations.c
	$(CC) $(CFLAGS) -Wl,--wrap,fork -o inotify_test inotify_test.c inotify_helper.c inotify_file_operations.c $(LIBS)

#get_dirpaths: get_dirpaths.c
#	$(CC) $(CFLAGS) -Wl,--wrap,fork -o get_dirpaths get_dirpaths.c 

# Clean up
clean:
	rm -f $(FILES) *.o *~
