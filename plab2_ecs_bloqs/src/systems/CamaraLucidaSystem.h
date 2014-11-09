#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxCamaraLucida.h"

using namespace artemis;

class CamaraLucidaSystem : public ECSsystem 
{ 

  public:

    CamaraLucidaSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<CamaraLucidaComponent>();
      addComponentType<RenderComponent>();
    };

    virtual void initialize() 
    {
      camara_lucida_m.init( *world );
      render_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //cml::CamaraLucida* cml = camara_lucida_m.get(e)->cml;
      //float w = cml->tex_width();
      //float h = cml->tex_height();
      //render_m.get(e)->update(w,h);
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth = component<DepthComponent>("input");

      if ( depth->dirty )
      {
        camara_lucida_m.get(e)->cml->update( depth->depth_pix_mm );
      }
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      _bag = &bag;
      int len = bag.getCount();

      if ( len == 1 )
        processEntity( *bag.get(0) );

      else if ( len > 1 )
        ofLogError("CamaraLucidaSystem") << "there are more than 1 entities with a CamaraLucidaComponent and there should be only 1";

      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    };

    virtual void render()
    {
      //int len = _bag->getCount();
      //for ( int i = 0; i < len; i++ )
        //render_entity( *_bag->get(i) );
    };

    void render_entity(Entity &e)
    {
      DepthComponent* depth = component<DepthComponent>("input");

      RenderComponent* render_data = render_m.get(e);
      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      camara_lucida_m.get(e)->cml->depth_camera()->get_hue_tex_ref( depth->depth_pix_mm ).draw( 0, 0, w, h ); 
    };

  private:

    ImmutableBag<Entity*>* _bag;

    ComponentMapper<CamaraLucidaComponent> camara_lucida_m;
    ComponentMapper<RenderComponent> render_m;

};

