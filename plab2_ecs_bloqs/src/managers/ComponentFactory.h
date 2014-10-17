#pragma once

#include <Artemis/Artemis.h>
#include "components/BloqComponent.h"
#include "components/MeshComponent.h"
#include "components/ParticleSystemComponent.h"
#include "components/ParticleEmitterComponent.h"

class ComponentFactory
{
  public:

    ComponentFactory(){};
    ~ComponentFactory(){};

    artemis::Component* make(string id)
    {
      ofLogNotice("ComponentFactory") << "\t make " << id;

      if (id == "bloq")
      {
        return new BloqComponent(id);
      }

      else if (id == "mesh")
      {
        return new MeshComponent(id);
      }

      else if (id == "particle_system")
      {
        return new ParticleSystemComponent(id);
      }

      else if (id == "particle_emitter")
      {
        return new ParticleEmitterComponent(id);
      }

      return NULL;
    };

  private:

};
