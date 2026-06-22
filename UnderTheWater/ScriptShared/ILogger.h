// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#include <string>



namespace UW {
class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void info(const std::string& module, const std::string& text) = 0;
  virtual void warn(const std::string& module, const std::string& text) = 0;
  virtual void erro(const std::string& module, const std::string& text) = 0;

};
};
