#!/bin/bash
sysctl fs.inotify.max_user_watches=524288
sysctl -p
###################### 1 #####################
rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 5 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp
mv log_open.csv log_open_1.csv
mv log_close.csv log_close_1.csv
mv log_write.csv log_write_1.csv
mv log_read.csv log_read_1.csv
mv log_delete.csv log_delete_1.csv
mv log_rename.csv log_rename_1.csv
mv log_chmod.csv log_chmod_1.csv
mv log_create.csv log_create_1.csv

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 5 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
mv log_open-impl.csv log_open-impl_1.csv
mv log_close-impl.csv log_close-impl_1.csv
mv log_write-impl.csv log_write-impl_1.csv
mv log_read-impl.csv log_read-impl_1.csv
mv log_delete-impl.csv log_delete-impl_1.csv
mv log_rename-impl.csv log_rename-impl_1.csv
mv log_chmod-impl.csv log_chmod-impl_1.csv
mv log_create-impl.csv log_create-impl_1.csv

###################### 2 #####################

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 10 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp
mv log_open.csv log_open_2.csv
mv log_close.csv log_close_2.csv
mv log_write.csv log_write_2.csv
mv log_read.csv log_read_2.csv
mv log_delete.csv log_delete_2.csv
mv log_rename.csv log_rename_2.csv
mv log_chmod.csv log_chmod_2.csv
mv log_create.csv log_create_2.csv

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 10 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
mv log_open-impl.csv log_open-impl_2.csv
mv log_close-impl.csv log_close-impl_2.csv
mv log_write-impl.csv log_write-impl_2.csv
mv log_read-impl.csv log_read-impl_2.csv
mv log_delete-impl.csv log_delete-impl_2.csv
mv log_rename-impl.csv log_rename-impl_2.csv
mv log_chmod-impl.csv log_chmod-impl_2.csv
mv log_create-impl.csv log_create-impl_2.csv

###################### 3 #####################
rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 15 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp
mv log_open.csv log_open_3.csv
mv log_close.csv log_close_3.csv
mv log_write.csv log_write_3.csv
mv log_read.csv log_read_3.csv
mv log_delete.csv log_delete_3.csv
mv log_rename.csv log_rename_3.csv
mv log_chmod.csv log_chmod_3.csv
mv log_create.csv log_create_3.csv

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 15 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
mv log_open-impl.csv log_open-impl_3.csv
mv log_close-impl.csv log_close-impl_3.csv
mv log_write-impl.csv log_write-impl_3.csv
mv log_read-impl.csv log_read-impl_3.csv
mv log_delete-impl.csv log_delete-impl_3.csv
mv log_rename-impl.csv log_rename-impl_3.csv
mv log_chmod-impl.csv log_chmod-impl_3.csv
mv log_create-impl.csv log_create-impl_3.csv

###################### 4 #####################
rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 20 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp
mv log_open.csv log_open_4.csv
mv log_close.csv log_close_4.csv
mv log_write.csv log_write_4.csv
mv log_read.csv log_read_4.csv
mv log_delete.csv log_delete_4.csv
mv log_rename.csv log_rename_4.csv
mv log_chmod.csv log_chmod_4.csv
mv log_create.csv log_create_4.csv

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 20 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
mv log_open-impl.csv log_open-impl_4.csv
mv log_close-impl.csv log_close-impl_4.csv
mv log_write-impl.csv log_write-impl_4.csv
mv log_read-impl.csv log_read-impl_4.csv
mv log_delete-impl.csv log_delete-impl_4.csv
mv log_rename-impl.csv log_rename-impl_4.csv
mv log_chmod-impl.csv log_chmod-impl_4.csv
mv log_create-impl.csv log_create-impl_4.csv

###################### 5 #####################

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 25 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp
mv log_open.csv log_open_5.csv
mv log_close.csv log_close_5.csv
mv log_write.csv log_write_5.csv
mv log_read.csv log_read_5.csv
mv log_delete.csv log_delete_5.csv
mv log_rename.csv log_rename_5.csv
mv log_chmod.csv log_chmod_5.csv
mv log_create.csv log_create_5.csv

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 25 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
mv log_open-impl.csv log_open-impl_5.csv
mv log_close-impl.csv log_close-impl_5.csv
mv log_write-impl.csv log_write-impl_5.csv
mv log_read-impl.csv log_read-impl_5.csv
mv log_delete-impl.csv log_delete-impl_5.csv
mv log_rename-impl.csv log_rename-impl_5.csv
mv log_chmod-impl.csv log_chmod-impl_5.csv
mv log_create-impl.csv log_create-impl_5.csv
