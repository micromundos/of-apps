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

    artemis::Component* make(string name)
    {
      ofLogNotice("ComponentFactory") << "\t make " << name;

      if (name == "bloq")
      {
        return new BloqComponent(name);
      }

      else if (name == "mesh")
      {
        return new MeshComponent(name);
      }

      else if (name == "particle_system")
      {
        return new ParticleSystemComponent(name);
      }

      else if (name == "particle_emitter")
      {
        return new ParticleEmitterComponent(name);
      }

      return NULL;
    };

  private:

};
