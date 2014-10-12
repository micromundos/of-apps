#pragma once

#include <Artemis/Artemis.h>
#include "components/BloqComponent.h"
#include "components/MeshComponent.h"
#include "components/ParticleEmitterComponent.h"

class ComponentFactory
{
  public:

    ComponentFactory(){};
    ~ComponentFactory(){};

    //TODO FIXME esto no está bien, pasar parámetros dinámicamente!
    artemis::Component* make( string name, Bloq& bloq )
    {
      ofLog() << "\t component factory make " << name;

      if ( name == "bloq" )
      {
        BloqComponent* bloq_comp = new BloqComponent(name);
        bloq_comp->update( bloq );
        return bloq_comp;
      }

      else if ( name == "mesh" )
      {
        return new MeshComponent(name);
      }

      else if ( name == "particle_emitter" )
      {
        return new ParticleEmitterComponent(name);
      }

      return NULL;
    };

  private:

};
