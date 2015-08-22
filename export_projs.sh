#!/bin/bash
#src=$1
#src=$2
src=./
dst=../plab_projs
echo "extract projects from "$src" to "$dst
include=( '*.xcodeproj' '*.xcconfig' 'openFrameworks-Info.plist' )
for i in "${include[@]}"
do
  :
  find $src -name $i
  find $src -name $i | xargs tar cvf - | (mkdir -p $dst ; cd $dst ; tar xfp -)
done
echo "done"
