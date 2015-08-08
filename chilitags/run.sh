#!/bin/bash
#1080p  = 1920 x 1080
#720p   = 1280 x 720
#xga    = 1024 x 576
#vga    = 640  x 480
calibration="../data/calib/intrinsics_ps3eye.aruco.yml"
device=0
width=1024
height=576
ip="127.0.0.1"
port=7000
./build/app/plab_chilitags -d $device -i $calibration -w $width -h $height -a $ip -p $port
