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
