// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#ifdef PRODUCTION

#include <string>
#include <unordered_map>
#include <functional>
#include "GameObjectScriptInterface.h"
#include "ILogger.h"



namespace UW {
  class GameObjectScriptInterface;

  using ScriptFactory = std::function<GameObjectScriptInterface*()>;

  class ScriptRegistry {
  private:
    std::unordered_map<std::string, ScriptFactory> factories;

    ScriptRegistry() = default;

public:
    static ScriptRegistry& get() {
      static ScriptRegistry instance;
      return instance;
    };

    void registerScript(const std::string& name, ScriptFactory factory) {
      factories[name] = factory;
    };

    GameObjectScriptInterface* createScript(const std::string& name) {
      auto it = factories.find(name);
      if (it != factories.end()) return it->second();
      return nullptr;
    };
  };
};

#endif



#ifdef PRODUCTION
  #define REGISTER_SCRIPT_INTERNAL(RegKey, ScriptClassName) \
    namespace { \
      struct ScriptRegisterer_##ScriptClassName { \
        ScriptRegisterer_##ScriptClassName() { \
          UW::ScriptRegistry::get().registerScript(RegKey, []() -> UW::GameObjectScriptInterface* { \
            return new UW::ScriptClassName(); \
          }); \
        } \
      }; \
      static ScriptRegisterer_##ScriptClassName global_ScriptRegisterer_##ScriptClassName; \
    }

  #define REGISTER_SCRIPT(RegKey, ScriptClassName) REGISTER_SCRIPT_INTERNAL(RegKey, ScriptClassName)
#else
  #define REGISTER_SCRIPT(RegKey, ScriptClassName)
#endif