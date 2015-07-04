#pragma once

#include "ofMain.h"

struct Bloq
{
    string id; 
    ofVec2f loc; //normalized [0,1]
    ofVec2f dir; //normalized len vec
    float radians; //angle
};

