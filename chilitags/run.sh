#!/bin/bash
#1080p -w 1920 -h 1080
#720p -w 1280 -h 720
#xga -w 1024 -h 576
#vga -w 640 -h 480
calibration="../data/calib/intrinsics_ps3eye.aruco.yml"
device=1
width=1024
height=576
ip="127.0.0.1"
port=7000
./build/app/plab_chilitags -d $device -i $calibration -w $width -h $height -a $ip -p $port
