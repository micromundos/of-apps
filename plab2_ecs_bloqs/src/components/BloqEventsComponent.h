#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "bloqs/Bloq.h"

class BloqEventsComponent : public artemis::Component 
{
  public:
    string id;

    ofEvent<Bloq> added;
    ofEvent<Bloq> updated;
    ofEvent<string> removed;

    BloqEventsComponent(string id)
    {
      this->id = id;
    };

};

