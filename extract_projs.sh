#!/bin/bash
echo "extract projects from "$1" to "$2
include=( 'Makefile' '*.make' '*.xcodeproj' '*.xcodeproj' '*.xcconfig' )
for i in "${include[@]}"
do
  :
  find $1 -name $i
  find $1 -name $i | cpio -pdm $2
done
echo "done"
