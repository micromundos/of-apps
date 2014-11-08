#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

//TODO refactor Blob to be a ofPolyline !!
//and blobs will be a vector<ofPolyline>
class Blob
{
  public:
    ofPoint centroid;
    vector<ofPoint> points;
    ofRectangle bounds;
    float area;
    float perimeter;
};

class BlobsComponent : public ECScomponent 
{
  public:
    vector<Blob> blobs;

    BlobsComponent(string _id) : ECScomponent(_id) {};

};

