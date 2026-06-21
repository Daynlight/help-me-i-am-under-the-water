// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "Logger.h"



UW::Log::Log(UW::LogType type, const std::string& module, const std::string& text)
  :type(type), module(module), text(text){};



std::string UW::Log::getText() const {
  return "["+ getTypeText() +"] ("+ module +"): " + text;
};



std::string UW::Log::getTypeText() const{
  #ifndef PRODUCTION
  switch (type){
    case UW::LogType::INFO:
      return "INFO";
    case UW::LogType::WARN:
      return "WARN";
    case UW::LogType::ERRO:
      return "ERRO";  
    default:
      return "NO TYPE";
  };
  #endif

  return "NO TYPE";
};



ImVec4 UW::Log::getLogColor() const{
  #ifndef PRODUCTION
  switch(type){
    case UW::LogType::INFO:
      return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    case UW::LogType::WARN:
      return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    case UW::LogType::ERRO:
      return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    default:
      return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
  };
  #endif

  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};






UW::Logger &UW::Logger::get(){
  static Logger instance;
  return instance;
};



UW::Logger::Logger(){
  calculateInitialLineCount();
};



void UW::Logger::info(const std::string& module, const std::string& text){
#ifndef PRODUCTION
  data.emplace_back(UW::LogType::INFO, module, text);
  log_to_file(data[data.size() - 1]);
#endif
};



void UW::Logger::warn(const std::string& module, const std::string& text){
#ifndef PRODUCTION
  data.emplace_back(UW::LogType::WARN, module, text);
  log_to_file(data[data.size() - 1]);
#endif
};



void UW::Logger::erro(const std::string& module, const std::string& text){
#ifndef PRODUCTION
  data.emplace_back(UW::LogType::ERRO, module, text);
  log_to_file(data[data.size() - 1]);
#endif
};



const std::vector<UW::Log>& UW::Logger::getLogs() const {
  return data;
};



void UW::Logger::checkAndTrimLog() {
  std::ifstream infile(UW::Config::LOG_FILE_PATH);
  if (!infile.is_open()) return;

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }
  infile.close();

  if (lines.size() >= UW::Config::LOGS_MAX_LINES) {
    std::ofstream outfile(UW::Config::LOG_FILE_PATH, std::ios::trunc);
    if (outfile.is_open()) {
      size_t start_index = lines.size() - UW::Config::LOGS_TARGET_TRIM_LINES;
      for (size_t i = start_index; i < lines.size(); ++i) {
        outfile << lines[i] << "\n";
      }
      current_lines = UW::Config::LOGS_TARGET_TRIM_LINES;
    };
  };
};



void UW::Logger::calculateInitialLineCount() {
  if (!std::filesystem::exists(UW::Config::LOG_FILE_PATH)) return;
  std::ifstream infile(UW::Config::LOG_FILE_PATH);
  std::string line;
  while (std::getline(infile, line)) {
    current_lines++;
  };
};



void UW::Logger::log_to_file(Log log){
  std::ofstream log_file(UW::Config::LOG_FILE_PATH, std::ios::app);
  if (log_file.is_open()) {
    log_file << log.getText() << "\n";
    current_lines++;
    log_file.close();

    if (current_lines >= UW::Config::LOGS_MAX_LINES) {
      checkAndTrimLog();
    };
  };
};