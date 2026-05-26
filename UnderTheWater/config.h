#pragma once
#include "Renderer.h"

#include <string>



namespace UW::Config{
  inline constexpr float PI = 3.14159265358979f;
  inline constexpr float EPS = 1e-5f;
  
  inline constexpr float FIXED_HZ = 60.0f;
  inline constexpr unsigned int FPS_SAMPLES = 200;
  inline constexpr unsigned int VSYNC = 0;
  inline const std::string DEFAULT_GUI_MATERIAL = "terrain";
  inline const std::string WINDOW_TITLE = "Under the Water (Daniel Stodulski)";
  
  inline constexpr bool DEFAULT_CURSOR_IS_VISIBLE = true;
  inline constexpr bool DEFAULT_DEBUG_CAMERA_ON = true;
  inline constexpr float CAMERA_SWAP_COOLDOWN = 0.5f;
  inline constexpr float CAMERA_DEFAULT_VELOCITY = 200.0f;

  inline constexpr float MAX_HEIGHT = 1024.0f;
  inline constexpr float CHUNK_SIZE = 512.0f;
  inline constexpr unsigned int CHUNK_RADIUS = 5;
  inline constexpr glm::vec2 TESS_BOUND = glm::vec2(8.0f, 512.0f);
  inline constexpr float TESS_DISTANCE_COFF = 0.0000005f;
  
  inline const std::string SWAP_CAMERA_BTN = "P";
  inline const std::string CAMERA_SWAP_MODE_BTN = "ESC";
  inline const std::string CAMERA_ACCELERATE = "O";
  inline const std::string CAMERA_DECELERATE = "I";
  inline const std::string CAMERA_MOVE_FORWARD = "W";
  inline const std::string CAMERA_MOVE_BACK = "S";
  inline const std::string CAMERA_MOVE_RIGHT = "D";
  inline const std::string CAMERA_MOVE_LEFT = "A";
};