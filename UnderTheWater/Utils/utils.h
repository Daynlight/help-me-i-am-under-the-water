#pragma once
#include <filesystem>
#include <string>

#include <cmrc/cmrc.hpp>
#include <regex>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else 
  #include <unistd.h>
  #include <limits.h>
#endif



namespace UW::Utils {
inline std::string GetExecutablePath() {
#if defined(_WIN32)
  char buffer[MAX_PATH];
  DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
  if (size == 0) return "";
  return std::string(buffer, size);
#else 
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::string(result, (count > 0) ? count : 0);
#endif
};



inline std::string GetExeDir() {
  std::string path = GetExecutablePath();
  if (path.empty()) return "";
  return std::filesystem::path(path).parent_path().string();
};



template<typename T>
inline void uploadTypedBuffer(
  CW::Renderer::Mesh& mesh,
  const std::vector<std::uint8_t>& buffer,
  unsigned int dimension,
  unsigned int layout,
  GLenum type
) {
  if (buffer.empty()) return;

  if (buffer.size() % sizeof(T) != 0) throw std::runtime_error("Mesh buffer size not aligned with type size");

  size_t count = buffer.size() / sizeof(T);

  std::vector<T> typed;
  typed.resize(count);

  std::memcpy(
    typed.data(),
    buffer.data(),
    buffer.size()
  );

  mesh.setData<T>(
    std::move(typed),
    dimension,
    layout,
    type
  );
};



inline void uploadBufferByType(
  CW::Renderer::Mesh& engine_mesh,
  GLenum type,
  const std::vector<std::uint8_t>& buffer,
  unsigned int dimension,
  unsigned int key)
{
  if (buffer.empty()) return;

  switch (type){
    case GL_FLOAT:
      uploadTypedBuffer<float>(engine_mesh, buffer, dimension, key, type);
      break;
    case GL_UNSIGNED_INT:
      uploadTypedBuffer<unsigned int>(engine_mesh, buffer, dimension, key, type);
      break;
    case GL_UNSIGNED_SHORT:
      uploadTypedBuffer<unsigned short>(engine_mesh, buffer, dimension, key, type);
      break;
    default:
      throw std::runtime_error("Unsupported GL type in mesh data");
  };
};



  inline void scanCmrcDirectory(
    const cmrc::embedded_filesystem& fs,
    const std::string& current_path,
    const std::string& pattern_str,
    std::vector<std::string>& out_mesh_files){
    std::regex pattern(pattern_str);

    for (const auto& entry : fs.iterate_directory(current_path)) {
      std::string entry_path = current_path + (current_path.back() == '/' ? "" : "/") + entry.filename();
    
    if (entry.is_directory()) scanCmrcDirectory(fs, entry_path, pattern_str, out_mesh_files);
    else if (entry.is_file() && std::regex_search(entry.filename(), pattern)) out_mesh_files.emplace_back(entry_path);
  };
};
};