#pragma once

#include "ofMain.h"
#include "bloqs/Bloq.h"

namespace BloqEvents
{
  static ofEvent<Bloq> added;
  static ofEvent<Bloq> updated;
  static ofEvent<string> removed;
};

