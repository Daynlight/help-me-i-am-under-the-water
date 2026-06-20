// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once



#if defined(_WIN32) || defined(_WIN64)
  #ifdef BUILDING_SCRIPT_DLL
    #define SCRIPT_API __declspec(dllexport)
  #else
    #define SCRIPT_API __declspec(dllimport)
  #endif
#else
  #define SCRIPT_API 
#endif



namespace UW{
class GameObjectScriptInterface {
public:
  virtual ~GameObjectScriptInterface() = default;
  
  virtual void OnLoad() = 0;
  virtual void OnUpdate(float delta_time) = 0;
  virtual void OnFixedUpdate(float fixed_delta_time) = 0;
  virtual void OnRender() = 0;
  virtual void OnDestroy() = 0;

};
};