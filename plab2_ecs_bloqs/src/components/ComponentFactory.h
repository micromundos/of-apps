#pragma once

#include <Artemis/Artemis.h>
#include "components/Components.h"

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

    else if (id == "bloq_events")
    {
      return new BloqEventsComponent(id);
    }

    else if (id == "aruco")
    {
      return new ArucoComponent(id);
    }

    else if (id == "rgb")
    {
      return new RGBComponent(id);
    }

    else if (id == "depth")
    {
      return new DepthComponent(id);
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

    else if (id == "flow_field")
    {
      return new FlowFieldComponent(id);
    }

    else if (id == "particle_flow_field")
    {
      return new ParticleFlowFieldComponent(id);
    }

    return NULL;
  };

};
