// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the Apache License, Version 2.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"
#include "Gui.h"

#include <string>
#include <vector>



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

public:
  static Logger& get();

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

private:
  Logger() = default;
  ~Logger() = default;
  
public:
  void info(const std::string& module, const std::string& text);
  void warn(const std::string& module, const std::string& text);
  void erro(const std::string& module, const std::string& text);

  const std::vector<Log>& getLogs() const;

};
};
