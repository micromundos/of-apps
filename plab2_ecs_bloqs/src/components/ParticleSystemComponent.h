#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleSystemComponent : public ECScomponent
{
  public:

    ParticleSystemComponent(string _id) : ECScomponent(_id) {};
};

