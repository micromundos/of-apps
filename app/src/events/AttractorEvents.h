#pragma once

#include "ofMain.h"
#include "bloqs/Attractor.h"

namespace AttractorEvents
{
  static ofEvent<Attractor> added;
  static ofEvent<Attractor> updated;
  static ofEvent<string> removed;
};

