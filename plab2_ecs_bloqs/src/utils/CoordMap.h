#pragma once

#include "ofMain.h"

class CoordMap
{
  public:

    float src_w, src_h;
    float dst_w, dst_h;

    CoordMap(){};
    ~CoordMap(){};

    void set( float src_w, float src_h, float dst_w, float dst_h  )
    {
      this->src_w = src_w;
      this->src_h = src_h;
      this->dst_w = dst_w;
      this->dst_h = dst_h;
    };

    void dst( const ofVec2f& _src, ofVec2f& _dst )
    {
      dst( _src.x, _src.y, _dst );
    };

    void dst( float src_x, float src_y, ofVec2f& _dst )
    {
      _dst.x = (float)src_x/src_w*dst_w;
      _dst.x = CLAMP(_dst.x,0,dst_w-1);
      _dst.y = (float)src_y/src_h*dst_h;
      _dst.y = CLAMP(_dst.y,0,dst_h-1);
    };

    void src( const ofVec2f& _dst, ofVec2f& _src )
    {
      src( _dst.x, _dst.y, _src );
    };

    void src( float dst_x, float dst_y, ofVec2f& _src )
    {
      _src.x = (float)dst_x/dst_w*src_w;
      _src.x = CLAMP(_src.x,0,src_w-1);
      _src.y = (float)dst_y/dst_h*src_h;
      _src.y = CLAMP(_src.y,0,src_h-1);
    };

    void dst(int src_idx, ofVec2f& _dst)
    {
      _dst.x = src_idx % (int)dst_w;
      _dst.x = CLAMP(_dst.x,0,dst_w-1);
      _dst.y = (float)(src_idx-_dst.x)/dst_w;
      _dst.y = CLAMP(_dst.y,0,dst_h-1);
    };

    void src( int src_idx, ofVec2f& _src )
    {
      _src.x = src_idx % (int)src_w;
      _src.x = CLAMP(_src.x,0,src_w-1);
      _src.y = (float)(src_idx-_src.x)/src_w;
      _src.y = CLAMP(_src.y,0,src_h-1);
    };

    int src_idx( const ofVec2f& _src )
    {
      return src_idx( _src.x, _src.y );
    };

    int src_idx( float x, float y )
    {
      return y * src_w + x;
    };

    int dst_idx( const ofVec2f& _dst )
    {
      return dst_idx( _dst.x, _dst.y );
    };

    int dst_idx( float x, float y )
    {
      return y * dst_w + x;
    };
};

