#!/bin/bash
addons_loc=../../addons
cwd=$(pwd)
echo "install add addons on "$addons_loc
addons=(  
git@github.com:chparsons/ofxECS.git 
git@github.com:chparsons/ofxKeyValueOSC.git 
git@github.com:chparsons/ofxCamaraLucida.git
git@github.com:chparsons/ofxAruco.git
git@github.com:chparsons/ofxGeom.git
git@bitbucket.org:chparsons/ofxplab.git
git@bitbucket.org:chparsons/ofxgpgpu.git
git@bitbucket.org:chparsons/ofxcamaralucidacalibration.git
git@github.com:kylemcdonald/ofxCv.git
git@github.com:jefftimesten/ofxJSON.git
git@github.com:bakercp/ofxPS3EyeGrabber.git
git@github.com:astellato/ofxSyphon.git
git@github.com:armadillu/ofxTimeMeasurements
)
cd $addons_loc ;
for i in "${addons[@]}"
do
  :
  git clone $i
done
cd $cwd ;
echo "done"
