#include "dp_cv.h"

void DepthProcess::cv::init( 
    Params* params,
    coord cv2depth )
{
  Parametrizable::init( params );

  this->cv2depth = cv2depth;

	img.setUseTexture( true );
	img.allocate( cv2depth.src_w, cv2depth.src_h );
	img.set( 0 );
}

void DepthProcess::cv::update(
    uint16_t *mm_depth_pix )
{ 
  //img.setFromPixels(
      //kinect.getDepthPixels(), 
      //kinect.width, kinect.height);

	unsigned char *img_px = img.getPixels();

  int img_size = img.width * img.height;

  for ( int i = 0; i < img_size; i++ ) 
  {
    int x, y;
    cv2depth.src_xy( i, &x, &y );

    int dx, dy;
    cv2depth.dst_xy( x, y, &dx, &dy );

    uint16_t mm = get_depth(dx,dy, mm_depth_pix);

    //uint16_t mm = get_depth(i, mm_depth_pix);

    bool clip = mm > params->dpfar 
      || mm < params->dpnear
      || dx < params->dpleft
      || dx > params->dpright
      || dy < params->dptop
      || dy > params->dpbottom ;

    img_px[i] = clip ? 0 : 255;
  }

	img.flagImageChanged();
}

void DepthProcess::cv::draw( int x, int y, int w, int h )
{
	glColor3f( 1, 1, 1 );
	img.draw( x, y, w, h );
}

/*
 * conversion utils 
 */

uint16_t DepthProcess::cv::get_depth( 
    int dx, int dy, 
    uint16_t *depth_pix )
{
  int depth_idx = cv2depth.dst_idx( dx, dy );
  return depth_pix[ depth_idx ];
}

//uint16_t DepthProcess::cv::get_depth( 
    //int idx, 
    //uint16_t *depth_pix )
//{
  //int x, y;
  //cv2depth.src_xy( idx, &x, &y );

  //int dx, dy;
  //cv2depth.dst_xy( x, y, &dx, &dy );

  //int depth_idx = cv2depth.dst_idx( dx, dy );

  //return depth_pix[ depth_idx ];
//}


