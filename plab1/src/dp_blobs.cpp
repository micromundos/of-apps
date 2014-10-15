#include "dp_blobs.h"

void DepthProcess::blobs::init( 
    int depth_width, int depth_height )
{
  this->depth_width = depth_width;
	this->depth_height = depth_height;
}

void DepthProcess::blobs::update( 
    ofxCvGrayscaleImage img )
{
  int img_size = img.width * img.height;

	blobfinder.findContours( 
      img, 10, img_size / 2, 20, false);
	
  //int nblobs = (int)blobfinder.blobs.size();

	//for ( int i = 0; i < nblobs; i++ ) 
	//{
		//ofxCvBlob blob = blobfinder.blobs[i];
		
		//ofRectangle brect = blob.boundingRect;
		//ofPoint bctr = blob.centroid;
		//float barea = blob.area;
		
		//int dx = bctr.x/img.width*depth_width;
		//int dy = bctr.y/img.height*depth_height;
	//}
}

void DepthProcess::blobs::draw( 
    int x, int y, int w, int h )
{
	glColor3f( 1, 1, 1 );
	blobfinder.draw( x, y, w, h );
}

