#!/bin/bash
sysctl fs.inotify.max_user_watches=524288
sysctl -p
###################### 1 #####################
rm -rf tmp*
rm readings*
mkdir tmp
python maketree.py -t 100000 -d 5 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 5 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp

###################### 2 #####################

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 10 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 10 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp

###################### 3 #####################
rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 15 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 15 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp

###################### 4 #####################
rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 20 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 20 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp

###################### 5 #####################

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 25 -c 5 -f 1 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for explicit watches"
./inotify_test -d tmp

rm -rf tmp*
mkdir tmp
python maketree.py -t 100000 -d 25 -c 5 -f 1 -s 0
#python maketree.py -t 50 -d 2 -c 1 -f 10 -s 0
#tree tmp
sh dirdepth.sh tmp
echo "running for implicit watches"
./inotify_test -i -d tmp
