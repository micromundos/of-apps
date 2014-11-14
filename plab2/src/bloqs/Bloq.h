#pragma once

#include "ofMain.h"

class Bloq
{
  public:

    Bloq(){};
    ~Bloq(){};

    string id; 
    ofVec2f loc; //normalized [0,1]
    ofVec2f dir; //normalized ang vec
    //float angle; //radians
};

