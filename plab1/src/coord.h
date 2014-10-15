#pragma once

class coord
{
public:

  coord() { set( 0, 0, 0, 0 ); };
  ~coord(){};

  coord(
      int src_w, int src_h,
      int dst_w, int dst_h )
  {
    set( src_w, src_h, dst_w, dst_h );
  };

  void set(
      int src_w, int src_h,
      int dst_w, int dst_h )
  {
    this->src_w = src_w;
    this->src_h = src_h;
    this->dst_w = dst_w;
    this->dst_h = dst_h;
  };

  void dst_xy( 
      int src_x, int src_y,
      int *dst_x, int *dst_y )
  {
    *dst_x = (float)src_x / src_w * dst_w;
    *dst_y = (float)src_y / src_h * dst_h;

    *dst_x = CLAMP( *dst_x, 0, dst_w - 1 );
    *dst_y = CLAMP( *dst_y, 0, dst_h - 1 );
  };

  void src_xy( 
      int dst_x, int dst_y,
      int *src_x, int *src_y )
  {
    *src_x = (float)dst_x / dst_w * src_w;
    *src_y = (float)dst_y / dst_h * src_h;

    *src_x = CLAMP( *src_x, 0, src_w - 1 );
    *src_y = CLAMP( *src_y, 0, src_h - 1 );
  };

  void dst_xy( 
      int src_idx,
      int *dst_x, int *dst_y )
  {
    *dst_x = src_idx % dst_w;
    *dst_y = (float)(src_idx - *dst_x) / dst_w;

    *dst_x = CLAMP( *dst_x, 0, dst_w - 1 );
    *dst_y = CLAMP( *dst_y, 0, dst_h - 1 );
  };

  void src_xy( 
      int src_idx,
      int *src_x, int *src_y )
  {
    *src_x = src_idx % src_w;
    *src_y = (float)(src_idx - *src_x) / src_w;

    *src_x = CLAMP( *src_x, 0, src_w - 1 );
    *src_y = CLAMP( *src_y, 0, src_h - 1 );
  };

  int src_idx( int x, int y )
  {
		return y * src_w + x;
  };

  int dst_idx( int x, int y )
  {
		return y * dst_w + x;
  };

  int src_w, src_h;
  int dst_w, dst_h;

};
