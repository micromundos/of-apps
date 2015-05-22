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

      xscale = (float)w / dw; 
      yscale = (float)h / dh; 
      channels = 1;

      pix_mm.allocate( dw, dh, channels );
      pix_mm.set(0);
      pix_resized.allocate( w, h, channels );
      pix_resized.set(0);
      tex.allocate( w, h, GL_LUMINANCE32F_ARB );
    };

    ofTexture& update( DepthComponent* depth_data )
    {
      pix_mm.setFromPixels( depth_data->f_depth_ofpix_mm->getPixels(), depth_data->width, depth_data->height, channels );

      if ( xscale != 1.0 || yscale != 1.0 )
      {
        ofxCv::resize( pix_mm, pix_resized, xscale, yscale );
        tex.loadData( pix_resized );
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
      pix_resized.clear();
    };

  private:

    int w, h;
    float xscale, yscale;
    int channels;

    ofTexture tex;
    ofFloatPixels pix_mm;
    ofFloatPixels pix_resized;
};

