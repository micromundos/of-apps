#include "particles.h"

void Particles::init( 
    Params* params, 
    int width, int height,
    int depth_width, int depth_height )
{
  Parametrizable::init( params );

  this->width = width;
  this->height = height; 
  this->depth_width = depth_width;
  this->depth_height = depth_height;

  //def:OF_TRIANGLES_MODE
  bodies.setMode( OF_PRIMITIVE_POINTS ); 
  bodies.setUsage( GL_DYNAMIC_DRAW );

  //add( 100 );

  //strats.add( "pstratmesh", 
      //new MeshStrategy( params, this ) );

  //strats.add( "pstratflowfield", 
      //new FlowFieldStrategy( params, this ) );

};

void Particles::update()
{
  add( params->pcant - ps.size() );

  // tick

  //float fps = 33.3333333;
  //float step = 1000.0/fps;
	//accum += step;  	
	//while ( accum > step )  
	//{
    //accum -= step;
  //}

  int i = ps.size();
  while( --i >= 0 )
  {
    Particle* p = ps[i];
    p->update();
    p->bounds(); 
  }

};

void Particles::update( uint16_t *mm_depth_pix )
{
  strats.update();
  update();
};

void Particles::render()
{
  //ofEnableAlphaBlending();
  strats.render();
  render_bodies();
};

void Particles::render_bodies()
{
  bodies.clear();

  int i = ps.size();
  while ( --i >= 0 )
  {
    Particle* p = ps[ i ]; 

    bodies.addVertex( ofVec3f( p->position ) );
    bodies.addColor( ofColor::red );

    //ofSetColor( ofColor::red );
    //ofRect( 
        //p->position, 
        //params->psize, 
        //params->psize );
  }

  glPointSize( params->psize );

  ((ofPtr<ofGLRenderer>&)ofGetCurrentRenderer())->draw( bodies );
  //ofGetCurrentRenderer()->draw( bodies );
  //bodies.drawVertices(); //bug!
};

void Particles::add( int amt )
{	
  if ( amt < 0 )
  {
    remove( abs(amt) );
    return;
  }

  for ( int i = 0; i < amt; i++ )
  {
    Particle *p = new Particle( 
        params,
        ofRandom( width ),
        ofRandom( height ),
        width, height
        );
    ps.push_back( p );
  }
};

void Particles::remove( int amt )
{
  int i = ps.size();
  int rm = 0;
  while ( --i >= 0 && rm++ < amt )
  {
    //delete ps.back();
		//ps.pop_back();
    Particle* erased = ps[ i ];
    ps.erase( ps.begin() + i );
    delete erased;
  }
};

uint16_t Particles::get_depth( 
    ofVec2f& ploc, 
    uint16_t *depth_pix )
{
  int dx = (float)ploc.x / width*depth_width;
  int dy = (float)ploc.y / height*depth_height;

  int depth_idx = dy * depth_width + dx;

  return depth_pix[ depth_idx ];
};

void FlowFieldStrategy::update()
{
  if ( ! params->pstratflowfield )
    return;

  vector<Particle*>& ps = particles->ps;

  int i = ps.size();
  while ( --i >= 0 )
  {
    Particle* p = ps[ i ];
    ofVec2f flow = ff->get_force(p->position);

    //if ( flow == ofVec2f() )
      //continue;

    //cout << "ff: " 
      //<< flow.x 
      //<< ", " 
      //<< flow.y 
      //<< ", loc: " 
      //<< p->position.x 
      //<< ", "
      //<< p->position.y
      //<< endl;

    p->flowfield( flow );
  }
};

void FlowFieldStrategy::render()
{};

void MeshStrategy::update()
{
  if ( ! params->pstratmesh )
    return;

  vector<Particle*>& ps = particles->ps;

  //float damp = .99;
  //float strength;

  int i = ps.size();
  while ( --i >= 0 )
  {
    Particle* p = ps[ i ];  

    int j = i;
    while ( --j >= 0 ) 
    {
      Particle* o = ps[ j ];

      //ofVec2f dvec = p->position - o->position;

      float d = p->position
        .distance( o->position );

      if ( d > params->pratt )
      {
        //-- out of radius
      }
      else if ( d > params->prrep )
      {
        // attraction

        //p->velocity -= dvec / 200;
        //o->velocity += dvec / 200;
        //p->velocity *= damp;
        //o->velocity *= damp;
        //strength = 1-( d - params->katt ) / 8;

        p->seek( o->position );
        o->seek( p->position );

        //cout << "link " 
          //<< p->position.x
          //<< ", "
          //<< p->position.y
          //<< " // "
          //<< o->position.x
          //<< ", "
          //<< o->position.y
          //<< endl;
      }
      else
      {
        // repulsion

        //float dd = 0.5*(d - params->krep) / d;
        //dvec *= dd;
        //p->velocity -= dvec;
        //o->velocity += dvec;

        p->seek( o->position, -1 );
        o->seek( p->position, -1 );
      }
    }
  }
};

void MeshStrategy::render()
{
  if ( ! params->pstratmesh )
    return;

  vector<Particle*>& ps = particles->ps;

  if ( params->renderpratt )
  {
    int i = ps.size();
    while ( --i >= 0 )
    {
      Particle* p = ps[ i ]; 

      ofNoFill();
      ofSetColor( ofColor::green, 50 );
      ofCircle( p->position, params->pratt );
      //ofSetColor( ofColor::green, 50 );
      //ofCircle( p->position, params->prrep );
      //ofSetColor( ofColor::green, 50 );
      //ofCircle( p->position, params->prattrmin );
      ofFill();
    }
  }

  //if ( params->renderlinks )
  //{
    //int nattr = ps->attractions.size();
    //for (int i = 0; i < nattr; i++) 
    //{
      //Attraction* a = ps->attractions[i];
      //Particle* p = a->getOneEnd();
      //Particle* o = a->getTheOtherEnd();

      ////float alfa = ofMap( d, 
          ////0, params->pratt, 0, 255 );

      //glLineWidth( 0.1 );
      //ofSetColor( ofColor::blue );
      //ofLine( p->position, o->position );
    //}
  //}

};


