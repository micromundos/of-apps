#pragma once

#include <Artemis/Artemis.h>
#include "components/TemplateComponent.h"

using namespace artemis;

class TemplateSystem : public EntityProcessingSystem 
{

  private:

    ComponentMapper<TemplateComponent> template_m;

  public:

    TemplateSystem() 
    {
      addComponentType<TemplateComponent>();
    };

    virtual void initialize() 
    {
      template_m.init( *world );
    };

    virtual void processEntity(Entity &e) 
    {
      template_m.get(e)->data += world->getDelta();
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for ( int i = 0; i < bag.getCount(); i++ )
      {
        processEntity( *bag.get(i) );
      }
    };

};

