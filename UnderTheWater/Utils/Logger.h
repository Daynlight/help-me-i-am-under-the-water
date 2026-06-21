// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"
#include "Gui.h"

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>



namespace UW{
enum LogType{
  INFO = 0,
  WARN = 1,
  ERRO = 2,
};



struct Log{
  LogType type;
  std::string module;
  std::string text;

  Log(LogType type, const std::string& module, const std::string& text);
  std::string getText() const;

  std::string getTypeText() const;
  ImVec4 getLogColor() const;
};



class Logger{
private:
  std::vector<Log> data;
  std::string log_file_path = "app.log";
  size_t current_lines = 0;
  const size_t MAX_LINES = 10000;
  const size_t TARGET_TRIM_LINES = 8000;

public:
  static Logger& get();

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

private:
  Logger();
  ~Logger() = default;
  
public:
  void info(const std::string& module, const std::string& text);
  void warn(const std::string& module, const std::string& text);
  void erro(const std::string& module, const std::string& text);

  const std::vector<Log>& getLogs() const;

private:
  void checkAndTrimLog();
  void calculateInitialLineCount();
  void log_to_file(Log log);

};
};
