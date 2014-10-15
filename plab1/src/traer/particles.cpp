#include "particles.h"

void Particles::init( 
    Params* params, 
    coord tex2depth )
{
  Parametrizable::init( params );

  this->tex2depth = tex2depth;

  ps = new ParticleSystem( 0., 0. );

  //ps->setIntegrator( 
  //new EulerIntegrator( ps ) );

  ps->clear();

  //def:OF_TRIANGLES_MODE
  bodies.setMode( OF_PRIMITIVE_POINTS ); 
  bodies.setUsage( GL_DYNAMIC_DRAW );

  //add( 100 );

};

void Particles::update()
{
  ps->setDrag( params->drag );
  ps->tick();
};

void Particles::update( uint16_t *mm_depth_pix )
{
  int i = to_remove.size();
  while ( --i >= 0 )
    ps->removeParticle( to_remove[ i ] );
  to_remove.clear();

  //FIXME
  clear_forces(); 
  clear_springs(); 
  clear_attractions();

  strats.update();

  depth_bounds( mm_depth_pix );
  tex_bounds();

  update();
};

void Particles::depth_bounds( 
    uint16_t *mm_depth_pix )
{
  int i = ps->numberOfParticles();
  while ( --i >= 0 )
  {
    Particle* p = ps->getParticle( i ); 

    uint16_t mm = get_depth( 
        p->position, mm_depth_pix );

    if ( 
        mm > params->dpfar 
        || mm < params->dpnear 
       )
    {
      //p->velocity *= -1;
      to_remove.push_back( p );
    }
  } 
};

void Particles::tex_bounds()
{
  int i = ps->numberOfParticles();
  while ( --i >= 0 )
  {
    Particle* p = ps->getParticle( i ); 

    if ( p->position.x < 0 )
    {
      p->position.x = 0;
      p->velocity.x *= -1;
    }
    else if ( p->position.x > tex2depth.src_w )
    {
      p->position.x = tex2depth.src_w-1;
      p->velocity.x *= -1;
    }

    if ( p->position.y < 0 )
    {
      p->position.y = 0;
      p->velocity.y *= -1;
    }
    else if ( p->position.y > tex2depth.src_h )
    {
      p->position.y = tex2depth.src_h-1;
      p->velocity.y *= -1;
    }
  }
};

void Particles::render()
{
  //ofEnableAlphaBlending();
  render_bodies();
  strats.render();
};

void Particles::render_bodies()
{
  bodies.clear();

  int i = ps->numberOfParticles();
  while ( --i >= 0 )
  {
    Particle* p = ps->getParticle( i ); 

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

void Particles::add( int amt, int x, int y )
{	
  if ( amt < 0 )
  {
    remove( abs(amt) );
    return;
  }

  float rand = params->pbirthrandarea;

  for ( int i = 0; i < amt; i++ )
  {
    Particle* p = ps->makeParticle( 1.,
        ofRandom( x - rand, x + rand ),
        ofRandom( y - rand, y + rand ),
        0.);
  }
};

void Particles::remove( int amt )
{
  int i = ps->numberOfParticles();
  int rm = 0;
  while ( --i >= 0 && rm++ < amt )
  {
    ps->removeParticle( i );
    //to_remove.push_back(ps->getParticle(i));
  }
};

uint16_t Particles::get_depth( 
    ofVec3f& ploc, 
    uint16_t *depth_pix )
{
  int dx, dy;

  tex2depth.dst_xy( ploc.x, ploc.y, &dx, &dy );

  int depth_idx = tex2depth.dst_idx( dx, dy );

  return depth_pix[ depth_idx ];
};

void Particles::clear_forces()
{
  int nf = ps->customForces.size(); 
  for (int i = 0; i < nf; i++) 
    delete ps->customForces[i];
  ps->customForces.clear();
};

void Particles::clear_springs()
{
  int nspr = ps->springs.size(); 
  for (int i = 0; i < nspr; i++) 
    delete ps->springs[i];
  ps->springs.clear();
};

void Particles::clear_attractions()
{
  int nattr = ps->attractions.size();
  for (int i = 0; i < nattr; i++) 
    delete ps->attractions[i];
  ps->attractions.clear();
};

/*
 * strategies
 */

namespace strat
{

void DepthFlowField::update()
{
  if ( ! params->pstratflowfield )
    return; 

  ParticleSystem* ps = particles->ps;

  int i = ps->numberOfParticles();
  while ( --i >= 0 )
  {
    Particle* p = ps->getParticle( i );
    ofVec2f flow = ff->get_force(p->position);

    ps->addCustomForce( 
        new Flow( params, p, flow ) );
  }
};

void Mesh::update()
{
  if ( ! params->pstratmesh )
    return;

  ParticleSystem* ps = particles->ps;

  int i = ps->numberOfParticles();
  while ( --i >= 0 )
  {
    Particle* p = ps->getParticle( i );  

    int j = i;
    while ( --j >= 0 ) 
    {
      Particle* o = ps->getParticle( j );

      float d = p->position
        .distance( o->position );

      if ( d > params->pradioatt )
      {
        //-- out of radius
      }
      else if ( d > params->pradiorep )
      {
        // attraction

        Attraction* a = ps->makeAttraction(
            p, o, 
            params->katt, 
            params->pradioattmin );  
      }
      else
      {
        // repulsion

        Attraction* a = ps->makeAttraction(
            p, o, 
            params->krep, 
            params->pradioattmin );
      }
    }
  }
};

void Mesh::render()
{
  if ( ! params->pstratmesh )
    return;

  ParticleSystem* ps = particles->ps;

  if ( params->renderpradioatt )
  {
    int i = ps->numberOfParticles();
    while ( --i >= 0 )
    {
      Particle* p = ps->getParticle( i ); 

      ofNoFill();
      ofSetColor( ofColor::green, 50 );
      ofCircle( p->position, params->pradioatt );
      //ofSetColor( ofColor::green, 50 );
      //ofCircle( p->position, params->pradiorep );
      //ofSetColor( ofColor::green, 50 );
      //ofCircle( p->position, params->pradioattmin );
      ofFill();
    }
  }

  if ( params->renderlinks )
  {
    int nattr = ps->attractions.size();
    for (int i = 0; i < nattr; i++) 
    {
      Attraction* a = ps->attractions[i];
      Particle* p = a->getOneEnd();
      Particle* o = a->getTheOtherEnd();

      //float alfa = ofMap( d, 
          //0, params->pradioatt, 0, 255 );

      glLineWidth( 0.1 );
      ofSetColor( ofColor::blue );
      ofLine( p->position, o->position );
    }
  }
};

void Birth::update()
{
  ParticleSystem* ps = particles->ps;

  vector<ofxCvBlob>& bls = blobs->list();

  int nblobs = bls.size();
  int nparticles = ps->numberOfParticles();
  int q = params->pcant - nparticles;

  if ( nblobs == 0 || q == 0 )
    return;

  q /= nblobs;

  for ( int i = 0; i < nblobs; i++ ) 
  {
    ofxCvBlob blob = bls[i];

    ofPoint bctr = blob.centroid;
    //ofRectangle brect = blob.boundingRect;

    int x, y;

    cv2tex.dst_xy( bctr.x, bctr.y, &x, &y );

    particles->add( q, x, y ); 
  }
};

}; //namespace strat

