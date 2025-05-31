#pragma once

#include <atomic>
#include <mutex>
#include <string>

enum class LogLevel { INFO, WARN, ERROR, DEBUG };

constexpr std::string_view logLevelToString(LogLevel level) noexcept {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

constexpr std::string_view logLevelToColor(LogLevel level) noexcept {
  switch (level) {
  case LogLevel::ERROR:
    return "\033[31m"; // Red
  case LogLevel::WARN:
    return "\033[33m"; // Yellow
  case LogLevel::INFO:
    return "\033[32m"; // Green
  case LogLevel::DEBUG:
    return "\033[36m"; // Cyan
  default:
    return "";
  }
}

#define LOG_INFO(msg) Logger::log(LogLevel::INFO, msg)
#define LOG_WARN(msg) Logger::log(LogLevel::WARN, msg)
#define LOG_ERROR(msg) Logger::log(LogLevel::ERROR, msg)
#define LOG_DEBUG(msg) Logger::log(LogLevel::DEBUG, msg)
#define LOG_TRACE(msg) Logger::log(LogLevel::TRACE, msg)

class Logger {
public:
  static void init();
  static void log(LogLevel level, const std::string &message);

private:
  static void fileLog(LogLevel level, const std::string &message);

  static std::string currentFile_;
  static std::mutex consoleMutex_;
  static std::mutex fileMutex_;
  static std::atomic<bool> initialized_;

  // Per-thread file streams (avoid contention)
  static thread_local std::ofstream logFile_;
};
