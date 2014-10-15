#include "dp_gpu.h"

void DepthProcess::gpu::init( 
    Params* params,
    int depth_width, 
    int depth_height )
{
  Parametrizable::init( params );

  this->depth_width = depth_width;
	this->depth_height = depth_height;

  int w = depth_width;
  int h = depth_height;

  //flowfield.allocate(w, h, GL_RGBA32F_ARB); 
  //flowfield.load( 
      //"camara_lucida/glsl/flowfield.frag" );

  //p3d.allocate(w, h, GL_RGBA32F_ARB); 
  //p3d.load( 
      //"camara_lucida/glsl/depth_to_p3d.frag" );

  //normals.allocate(w, h, GL_RGBA32F_ARB); 
  //normals.load( 
      //"camara_lucida/glsl/normals.frag" );

  //normals_vis.allocate(w, h, GL_RGBA32F_ARB); 
  //normals_vis.load( 
      //"camara_lucida/glsl/normals_vis.frag" );

  //bilateral.allocate(w, h, GL_RGBA32F_ARB); 
  //bilateral.load( 
      //"camara_lucida/glsl/bilateral.frag" ); 

}

void DepthProcess::gpu::update(
    uint16_t *mm_depth_pix,
    ofTexture& depth_ftex )
{ 
  ////depth_ftex = depthmap->get_float_tex_ref( mm_depth_pix, _near_mm, _far_mm ); 

  //flowfield.setTexture( depth_ftex, 0 ); 
  //flowfield.update();

  //ofFloatPixels ffpix;
  //flowfield
    //.getTextureReference()
    //.readToPixels( ffpix );
  //ofFloatColor n = ffpix.getColor( x,y );


  //bilateral.setTexture( depth_ftex, 0 ); 
  //bilateral.update();

  ////p3d.setTexture( depth_ftex, 0 );
  //p3d.setTexture( 
      //bilateral.getTextureReference(), 0 ); 
  //p3d.update();

  //normals.setTexture( 
      //p3d.getTextureReference(), 0 );
  //normals.update();  

  //normals_vis.setTexture( 
      //normals.getTextureReference(), 0 );
  //normals_vis.update();

  //flowfield.setTexture( 
      //normals.getTextureReference(), 0 );
  //flowfield.update();

  //ofFloatPixels dfpix;
  //depth_ftex.readToPixels(dfpix);
  //ofFloatColor df = dfpix.getColor(x,y);

  //ofFloatPixels p3pix;
  //p3d.getTextureReference()
    //.readToPixels(p3pix);
  //ofFloatColor p3a = p3pix.getColor(x, y);

  //ofFloatPixels npix;
  //normals.getTextureReference()
    //.readToPixels(npix);
  //ofFloatColor n = npix.getColor(x,y);
  //cout << "normal " 
    //<< n.r << ", " 
    //<< n.g << ", " 
    //<< n.b << ", " 
    //<< n.a 
    //<< endl;
}

void DepthProcess::gpu::draw( 
    int x, int y, int w, int h )
{
  //flowfield.draw( x, y, w, h );
  //p3d.draw( x, y, w, h );
  //normals.draw( x, y, w, h );
  //bilateral.draw( x, y, w, h );
  //normals_vis.draw( x, y, w, h );
}

