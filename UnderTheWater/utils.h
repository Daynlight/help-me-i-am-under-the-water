#pragma once
#include <filesystem>
#include <string>

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
};
