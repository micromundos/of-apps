#pragma once

#include <Artemis/Artemis.h>
#include "components/TemplateComponent.h"

class TemplateSystem : public artemis::EntityProcessingSystem 
{

  private:

    artemis::ComponentMapper<TemplateComponent> template_m;

  public:

    TemplateSystem() 
    {
      addComponentType<TemplateComponent>();
    };

    virtual void initialize() 
    {
      template_m.init( *world );
    };

    virtual void processEntity(artemis::Entity &e) 
    {
      template_m.get(e)->data += world->getDelta();
    };

};

