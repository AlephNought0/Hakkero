#include "logger.hpp"
#include "time_utils.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

std::string Logger::currentFile_;
std::mutex Logger::consoleMutex_;
std::mutex Logger::fileMutex_;
std::atomic<bool> Logger::initialized_{false};

void Logger::init() {
  std::lock_guard<std::mutex> lock(fileMutex_);
  if (initialized_)
    return;

  auto dir_status = std::filesystem::create_directory("logs");
  if (!dir_status && !std::filesystem::exists("logs")) {
    throw std::runtime_error("Failed to create logs directory");
  }

  // Each thread will open its own copy when first logging
  initialized_ = true;
}

void Logger::log(LogLevel level, const std::string &message) {
  // Thread-safe lazy init
  if (!initialized_.load(std::memory_order_acquire))
    init();

  {
    std::lock_guard<std::mutex> lock(consoleMutex_);
    if (level <= LogLevel::ERROR) {
      std::cerr << logLevelToColor(level) << "[" << logLevelToString(level)
                << "]: "
                << "\033[0m" << message << '\n';
    }
  }

  fileLog(level, message);
}

void Logger::fileLog(LogLevel level, const std::string &message) {
  thread_local std::ofstream logFile_;
  thread_local std::string filename;
  thread_local std::string current_date;
  thread_local std::string buffer;

  auto timeObj = TimeUtils::captureCurrentTime();
  std::string new_date = TimeUtils::formatAsDate(timeObj);

  if (filename.empty() || current_date != new_date) {
    current_date = new_date;
    filename = "logs/" + current_date + ".log";
    logFile_.open(filename, std::ios::app);

    if (!logFile_.is_open()) {
      throw std::runtime_error("Failed to open log file: " + filename);
    }
  }

  buffer.clear();
  buffer.reserve(50 + message.size());
  buffer.append("[")
      .append(TimeUtils::formatAsHourMinSec(TimeUtils::captureCurrentTime()))
      .append("] [")
      .append(logLevelToString(level))
      .append("]: ")
      .append(message)
      .append("\n");

  // Synchronized file write
  {
    std::lock_guard<std::mutex> lock(fileMutex_);
    logFile_ << buffer;
    if (level >= LogLevel::WARN)
      logFile_.flush();
  }
}
