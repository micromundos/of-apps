#pragma once

#include "ofxCv.h"
#include "Components.h"

class DepthFloatData
{
  public:

    DepthFloatData(){};
    ~DepthFloatData()
    { dispose(); };

    void init( DepthComponent* depth_data, int w, int h )
    {
      this->w = w;
      this->h = h;
      int dw = depth_data->width;
      int dh = depth_data->height;
      int channels = depth_data->channels;

      xscale = (float)w / dw; 
      yscale = (float)h / dh; 

      pix_mm.allocate( dw, dh, channels );
      pix_mm.set(0);
      pix_scaled.allocate( w, h, channels );
      pix_scaled.set(0);
      tex.allocate( w, h, GL_LUMINANCE32F_ARB );
    };

    ofTexture& update( DepthComponent* depth_data )
    {
      pix_mm.setFromPixels( depth_data->f_depth_ofpix_mm->getPixels(), depth_data->width, depth_data->height, depth_data->channels );

      if ( xscale != 1.0 || yscale != 1.0 )
      {
        ofxCv::resize( pix_mm, pix_scaled, xscale, yscale );
        tex.loadData( pix_scaled );
      }
      else 
        tex.loadData( pix_mm );

      return tex;
    };

    int width() { return w; };
    int height() { return h; };

    void dispose()
    {
      tex.clear();
      pix_mm.clear();
      pix_scaled.clear();
    };

    bool inited()
    {
      return tex.isAllocated();
    };

  private:

    int w, h;
    float xscale, yscale;

    ofTexture tex;
    ofFloatPixels pix_mm;
    ofFloatPixels pix_scaled;
};

