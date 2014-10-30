#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class Blob
{
  public:
  ofPoint centroid;
  vector<ofPoint> points;
  ofRectangle bounds;
  float area;
  float length;
};

class BlobsComponent : public artemis::Component 
{
  public:
    string id;
    vector<Blob> blobs;

    BlobsComponent( string id )
    {
      this->id = id;
    };
};

