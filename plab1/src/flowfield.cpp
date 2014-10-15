#include "flowfield.h"

FlowField::FlowField(){}

FlowField::~FlowField()
{
  delete[] field;
  field = NULL;
}

void FlowField::init(
    Params* params,
    coord ff2depth,
    coord tex2ff,
    coord tex2depth )
{
  Parametrizable::init( params );

  this->ff2depth = ff2depth;
  this->tex2ff = tex2ff;
  this->tex2depth = tex2depth;

  this->width = ff2depth.src_w;
  this->height = ff2depth.src_h;

  //ffgpu.allocate(
      //tex2depth.dst_w, 
      //tex2depth.dst_h, 
      //GL_RGBA32F_ARB ); 
  //ffgpu.load( 
      //"camara_lucida/glsl/flowfield.frag" );

  field = new ofVec2f[ width * height ];
  for ( int i = 0; i < width * height; i++ )
    field[i] = ofVec2f();
}

const ofVec2f& FlowField::get_force( 
    ofVec2f texloc )
{
  int c, r;

  //=== gpu ===
  //tex2depth.dst_xy(texloc.x,texloc.y,&c,&r);
  //ofFloatColor f = ffpix.getColor( c,r );
  //return ofVec2f( f.r, f.g );
  //=== gpu ===

  tex2ff.dst_xy( texloc.x, texloc.y, &c, &r );
  int i = tex2ff.dst_idx( c, r );
  return field[ i ];
};

void FlowField::update(
    uint16_t *mm_depth_pix,
    ofTexture& depth_ftex )
{
  //ffgpu.setTexture( depth_ftex, 0 ); 
  //ffgpu.update(); 
  //ffgpu
    //.getTextureReference()
    //.readToPixels( ffpix );
};

void FlowField::update( uint16_t *mm_depth_pix )
{
  for ( int c = 0; c < width; c++ )
  for ( int r = 0; r < height; r++ )
  {
    int i = r * width + c;

    ofVec2f	*f = &(field[ i ]);

    uint16_t mm = get_depth( i, mm_depth_pix );

    int k = 3; //kernel

    ofVec2f loc = ofVec2f( c,r );
    ofVec3f dst( c, r, mm );

    for ( int _c = c-k; _c <= c+k; _c++ )
    for ( int _r = r-k; _r <= r+k; _r++ )
    {
      // image boundaries
      if (
          _c == c && _r == r
          || _c < 0 || _r < 0
          || _c >= width || _r >= height
         )
        continue;

      int j = _r * width + _c;

      uint16_t _mm = get_depth(j, mm_depth_pix);

      //==================
      //TODO behavior rules stack
      if ( 
          mm > params->dpfar 
          || mm < params->dpnear 
          || abs(_mm - mm) > params->dpslope 
         )
        continue;

      if ( _mm > dst.z )
      {
        dst.set( _c, _r, _mm );
      }
      //==================
    }

    f->set( ofVec2f(dst) - loc );
  }
}

uint16_t FlowField::get_depth( 
    int idx, 
    uint16_t *depth_pix )
{
  int x, y;
  ff2depth.src_xy( idx, &x, &y );

  int dx, dy;
  ff2depth.dst_xy( x, y, &dx, &dy );

  int depth_idx = ff2depth.dst_idx( dx, dy );

  return depth_pix[ depth_idx ];
};

void FlowField::render()
{
  //ffgpu.draw( x, y, w, h );

  if ( ! params->renderflowfield )
    return;

  glColor3f( 0, 1, 0 );
  glPointSize( 2 );

  for ( int c = 0; c < width; c++ )
  for ( int r = 0; r < height; r++ )
  {
    int x, y;
    tex2ff.src_xy( c, r, &x, &y );

    glBegin( GL_POINTS );
    glVertex3f( x, y, 0.);
    glEnd();

    int i = r * width + c;

    ofVec2f	*f = &(field[ i ]);

    ofVec2f loc = ofVec2f( x,y );
    ofVec2f dst = loc + ( (*f) * 2 );


    //=== gpu ===
    //int dx, dy;
    //ff2depth.dst_xy( c, r, &dx, &dy );
    //int x, y;
    //tex2depth.src_xy( dx, dy, &x, &y );
    //glBegin( GL_POINTS );
    //glVertex3f( x, y, 0.);
    //glEnd();
    //ofFloatColor _f = ffpix.getColor( dx,dy );
    //ofVec2f	*f = &(ofVec2f( _f.r, _f.g ));
    //ofVec2f loc = ofVec2f( x,y );
    //ofVec2f dst = loc + ( (*f) * 5 );
    //=== gpu ===


    glLineWidth( 0.1 );
    ofSetColor( ofColor::green );
    ofLine( loc, dst );
  }
};

