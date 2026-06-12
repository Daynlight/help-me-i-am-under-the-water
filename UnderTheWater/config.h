#pragma once
#include "Renderer.h"

#include <string>
#include <unordered_map>


namespace UW::Config{
  inline constexpr float PI = 3.14159265358979f;
  inline constexpr float EPS = 1e-5f;
  
  inline constexpr float FIXED_HZ = 60.0f;
  inline constexpr unsigned int FPS_SAMPLES = 200;
  inline constexpr unsigned int VSYNC = 0;
  inline const std::string DEFAULT_GUI_MATERIAL = "Terrain";
  inline const unsigned int DEFAULT_GUI_OBJECT = 0;
  inline const unsigned int SHADER_EDITOR_BUFFER_SIZE = 20480;
  inline const unsigned int OBJECT_NAME_BUFFER_SIZE = 1024;
  inline const float SAVE_TIMESTAMP = 350;
  inline const std::string GAME_DATA_FOLDER = "GameData/";
  inline const std::string ASSETS_FOLDER = "Assets/";
  inline const std::string SHADERS_FOLDER = "Shaders/";
  inline const std::string TEXTURES_FOLDER = "Textures/";
  inline const std::string MESHES_FOLDER = "Meshes/";
  inline const std::string MESH_EXTENSION = ".msh";
  inline const std::string MATERIALS_FILENAME = "Materials.pbr";
  inline const std::string OBJECTS_FILENAME = "Objects.obj";
  inline const std::string LIGHTS_FILENAME = "Lights.lit";
  inline const std::string WINDOW_TITLE = "Under the Water (Daniel Stodulski)";
  
  inline constexpr bool DEFAULT_CURSOR_IS_VISIBLE = true;
  inline constexpr bool DEFAULT_DEBUG_CAMERA_ON = true;
  inline constexpr float CAMERA_SWAP_COOLDOWN = 0.5f;
  inline constexpr float CAMERA_DEFAULT_VELOCITY = 200.0f;

  inline constexpr float MAX_HEIGHT = 1024.0f;
  inline constexpr float WATER_HEIGHT = 300.0f;
  inline constexpr float CHUNK_SIZE = 512.0f;
  inline constexpr unsigned int CHUNK_RADIUS = 5;
  inline constexpr glm::vec2 TESS_BOUND = glm::vec2(8.0f, 512.0f);
  inline constexpr float TESS_DISTANCE_COFF = 0.0000003f;
  
  inline const std::string SWAP_CAMERA_BTN = "P";
  inline const std::string CAMERA_SWAP_MODE_BTN = "ESC";
  inline const std::string CAMERA_ACCELERATE = "O";
  inline const std::string CAMERA_DECELERATE = "I";
  inline const std::string CAMERA_MOVE_FORWARD = "W";
  inline const std::string CAMERA_MOVE_BACK = "S";
  inline const std::string CAMERA_MOVE_RIGHT = "D";
  inline const std::string CAMERA_MOVE_LEFT = "A";

  inline std::unordered_map<std::string, GLuint> SHADER_NAME_TO_TYPE = {
    {"vertex.glsl", GL_VERTEX_SHADER},
    {"fragment.glsl", GL_FRAGMENT_SHADER},
    {"geometry.glsl", GL_GEOMETRY_SHADER},
    {"tess_control.glsl", GL_TESS_CONTROL_SHADER},
    {"tess_evaluation.glsl", GL_TESS_EVALUATION_SHADER},
  };

  inline std::unordered_map<GLuint, std::string> SHADER_TYPE_TO_NAME = {
    {GL_VERTEX_SHADER, "vertex.glsl"},
    {GL_FRAGMENT_SHADER, "fragment.glsl"},
    {GL_GEOMETRY_SHADER, "geometry.glsl"},
    {GL_TESS_CONTROL_SHADER, "tess_control.glsl"},
    {GL_TESS_EVALUATION_SHADER, "tess_evaluation.glsl"},
  };
};