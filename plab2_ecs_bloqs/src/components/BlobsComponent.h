#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class Blob
{
  public:
    //TODO normalized coords
    ofPoint centroid;
    vector<ofPoint> points;
    ofRectangle bounds;
    float area;
    float length;
};

class BlobsComponent : public ECScomponent 
{
  public:
    vector<Blob> blobs;

    BlobsComponent(string _id) : ECScomponent(_id) {};

};

