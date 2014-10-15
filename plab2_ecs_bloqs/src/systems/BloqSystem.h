#pragma once

#include <Artemis/Artemis.h>
#include "Bloqs.h"
#include "components/BloqComponent.h"

//not used

class BloqSystem : public artemis::EntityProcessingSystem 
{

  private:

    artemis::ComponentMapper<BloqComponent> bloq_m;

  public:

    BloqSystem() 
    {
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      bloq_m.init( *world );
    };

    virtual void processEntity(artemis::Entity &e) 
    {
      //bloq_m.get(e)->bloq;
    };

};

