#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class RenderSurfacesSystem : public ECSsystem 
{ 

  public:

    RenderSurfacesSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<RenderSurfacesComponent>();
    };

    virtual void initialize() 
    {
      render_surfaces_m.init( *world );
      
      shader_surfaces.setupShaderFromFile(GL_FRAGMENT_SHADER, "glsl/surfaces_draw.frag");
      shader_surfaces.linkProgram();
      
    };

    //virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::processEntities(bag);
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    //};

    //virtual void renderEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::renderEntities(bag);
    //};

    virtual void removed(Entity &e) 
    {
    };

    virtual void added(Entity &e) 
    {
    }; 

    virtual void processEntity(Entity &e) 
    {

      //      DepthComponent* depth_data = component<DepthComponent>("input");

      //ofLogNotice("RenderSurfacesSystem") << "process entity " << e.getId();
      //render_surfaces_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthProcessingComponent* surfaces_data = component<DepthProcessingComponent>("input");
      RenderComponent* render_data = component<RenderComponent>("output");
      RenderSurfacesComponent* render_surfaces_data = render_surfaces_m.get(e);
      if(!surfaces_data)return;
      shader_surfaces.begin();
      ofEnableAlphaBlending();

      shader_surfaces.setUniform1f("margin", render_surfaces_data->alpha_margin);
      ofSetColor(255);
      surfaces_data->surfaces().get().draw(0,0,render_data->width,render_data->height);
      ofDisableAlphaBlending();

      shader_surfaces.end();
    
    };

  private:
  
    ofShader  shader_surfaces;

    ComponentMapper<RenderSurfacesComponent> render_surfaces_m;

};

