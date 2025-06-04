#pragma once

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

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

class Logger {
public:
  struct LogMessage {
    LogLevel level;
    std::string message;
    std::string timestamp;
  };

  static void init();
  static void shutdown();
  static void log(LogLevel level, const std::string &message);

private:
  static void loggingThreadWorker();
  static void processBatch(const std::vector<LogMessage> &batch);

  // Queue management
  static std::mutex queueMutex_;
  static std::condition_variable queueCV_;
  static std::queue<LogMessage> messageQueue_;
  static std::atomic<bool> shutdownRequested_;

  // File management
  static std::mutex fileMutex_;
  static std::string fileName_;
  static std::unique_ptr<std::ofstream> logFile_;

  // Console logging
  static std::mutex consoleMutex_;

  // Thread management
  static std::thread loggingThread_;
  static std::atomic<bool> initialized_;

  // Configuration
  static constexpr size_t MAX_BATCH_SIZE = 100;   // Messages per batch
  static constexpr size_t MAX_QUEUE_SIZE = 10000; // Prevent memory exhaustion
};
