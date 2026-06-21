// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include <vector>

#include "config.h"
#include "DataSerializer/DataSerializer.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"



namespace UW{
class ObjectManager{
private:

public:
  std::vector<UW::GameObject> objects;
  
};
}; // namespace UW
