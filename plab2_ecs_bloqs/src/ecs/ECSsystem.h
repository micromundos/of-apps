#pragma once

#include <Artemis/Artemis.h>

//utils on top of EntityProcessingSystem
class ECSsystem : public artemis::EntityProcessingSystem
{

  protected:

  template<typename TSystem>
  TSystem* system()
  {
    return ((TSystem*)world->getSystemManager()->getSystem<TSystem>());
  };

};

