#pragma once

#include <Artemis/Artemis.h>

class ECSsystem : public artemis::EntityProcessingSystem
{

  protected:

  //shortcuts

  template<typename TSystem>
  TSystem* system()
  {
    return ((TSystem*)world->getSystemManager()->getSystem<TSystem>());
  };

  template<typename TComponent>
  TComponent* component(string tag)
  {
    return ((TComponent*)world->getTagManager()->getEntity( tag ).getComponent<TComponent>());
  };

};

