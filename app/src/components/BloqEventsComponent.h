#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class BloqEventsComponent : public ECScomponent
{
  public:

    ofEvent<Bloq> added;
    ofEvent<Bloq> updated;
    ofEvent<string> removed;

    BloqEventsComponent(string _id) : ECScomponent(_id) {};

};

