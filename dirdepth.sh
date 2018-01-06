#!/bin/sh

dir_depth() {
cd "$1"
maxdepth=0
    for d in */.; do
        [ -d "$d" ] || continue
        depth=`dir_depth "$d"`
        maxdepth=$(($depth > $maxdepth ? $depth : $maxdepth))
    done
    echo $((1 + $maxdepth))
}
echo "dir depth:"
dir_depth "$@"
