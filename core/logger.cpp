#include "logger.hpp"
#include "time_utils.hpp"

#include <csignal>
#include <filesystem>
#include <iostream>
#include <stdexcept>

// Static member initialization
std::mutex Logger::queueMutex_;
std::condition_variable Logger::queueCV_;
std::queue<Logger::LogMessage> Logger::messageQueue_;
std::atomic<bool> Logger::shutdownRequested_{false};

std::mutex Logger::fileMutex_;
std::string Logger::fileName_;
std::unique_ptr<std::ofstream> Logger::logFile_;

std::mutex Logger::consoleMutex_;
std::thread Logger::loggingThread_;
std::atomic<bool> Logger::initialized_{false};

constexpr size_t Logger::MAX_BATCH_SIZE;
constexpr size_t Logger::MAX_QUEUE_SIZE;

void Logger::init() {
  if (initialized_.exchange(true)) {
    return;
  }

  auto timeObj = TimeUtils::captureCurrentTime();
  std::string date = TimeUtils::formatAsDate(timeObj);

  if (!std::filesystem::create_directory("logs") &&
      !std::filesystem::exists("logs"))
    throw std::runtime_error("Failed to create logs directory");

  std::string date_dir = "logs/" + date;
  if (!std::filesystem::exists(date_dir) &&
      !std::filesystem::create_directory(date_dir))
    throw std::runtime_error("Failed to create directory: " + date_dir);

  int count = 0;
  try {
    count = std::distance(std::filesystem::directory_iterator(date_dir),
                          std::filesystem::directory_iterator{});
  } catch (...) {
    throw std::runtime_error("Failed to count log files");
  }

  fileName_ = std::format("logs/{}/{}.log", date, count);

  // Start the logging thread
  loggingThread_ = std::thread(&Logger::loggingThreadWorker);
}

void Logger::shutdown() {
  if (!initialized_)
    return;

  shutdownRequested_ = true;
  queueCV_.notify_all();

  if (loggingThread_.joinable()) {
    loggingThread_.join();
  }

  std::unique_lock<std::mutex> fileLock(fileMutex_, std::try_to_lock);
  if (fileLock && logFile_) {
    logFile_->flush();
    logFile_->close();
  }

  initialized_ = false;
}

void Logger::log(LogLevel level, const std::string &message) {
  if (!initialized_) {
    init();
    registerCrashHandler();
  }

  // Console output
  {
    std::lock_guard<std::mutex> consoleLock(consoleMutex_);
    if (level <= LogLevel::FATAL) {
      std::cerr << logLevelToColor(level) << "[" << logLevelToString(level)
                << "]"
                << "\033[0m: " << message << '\n';
    }
  }

  // Queue the message for file writing
  LogMessage logMsg{
      level, message,
      TimeUtils::formatAsHourMinSec(TimeUtils::captureCurrentTime())};

  {
    std::unique_lock<std::mutex> queueLock(queueMutex_);

    // Prevent queue from growing indefinitely
    if (messageQueue_.size() >= MAX_QUEUE_SIZE)
      queueCV_.wait(queueLock,
                    [] { return messageQueue_.size() < MAX_QUEUE_SIZE; });

    messageQueue_.push(std::move(logMsg));
  }

  queueCV_.notify_one();
}

void Logger::loggingThreadWorker() {
  std::vector<LogMessage> batch;
  batch.reserve(MAX_BATCH_SIZE);

  while (!shutdownRequested_ || !messageQueue_.empty()) {
    // Wait for messages or shutdown
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      queueCV_.wait(
          lock, [] { return !messageQueue_.empty() || shutdownRequested_; });
    }

    // Process messages in batches
    while (true) {
      // Grab a batch of messages
      {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!messageQueue_.empty() && batch.size() < MAX_BATCH_SIZE) {
          batch.push_back(std::move(messageQueue_.front()));
          messageQueue_.pop();
        }
      }

      if (batch.empty())
        break;

      // Process the batch
      processBatch(batch);
      batch.clear();
    }
  }
}

void Logger::processBatch(const std::vector<LogMessage> &batch) {
  std::lock_guard<std::mutex> fileLock(fileMutex_);

  // Lazy file opening
  if (!logFile_) {
    logFile_ = std::make_unique<std::ofstream>(fileName_, std::ios::app);
    if (!logFile_->is_open()) {
      throw std::runtime_error("Failed to open log file: " + fileName_);
    }
  }

  // Write the batch
  for (const auto &msg : batch) {
    *logFile_ << "[" << msg.timestamp << "] [" << logLevelToString(msg.level)
              << "]: " << msg.message << "\n";
  }

  // Flush if there are any error messages
  if (std::any_of(batch.begin(), batch.end(), [](const LogMessage &msg) {
        return msg.level >= LogLevel::ERROR;
      })) {
    logFile_->flush();
  }
}

void Logger::registerCrashHandler() {
  std::signal(SIGSEGV, crashHandler);        // Segmentation fault
  std::signal(SIGABRT, crashHandler);        // Abort signal
  std::signal(SIGTERM, crashHandler);        // Termination request
  std::signal(SIGINT, crashHandler);         // Program interruption
  std::atexit([]() { Logger::shutdown(); }); // Normal exit
}

namespace {
void signalSafeWrite(const char *msg) noexcept {
  while (true) {
    ssize_t ret = write(STDERR_FILENO, msg, strlen(msg));
    if (ret == -1 && errno == EINTR)
      continue; // Retry if interrupted
    break;
  }
}

void signalSafeWrite(int num) noexcept {
  char buffer[16];
  char *ptr = buffer + sizeof(buffer) - 1;
  *ptr = '\0';

  do {
    *--ptr = '0' + (num % 10);
    num /= 10;
  } while (num > 0);

  signalSafeWrite(ptr);
}
} // namespace

void Logger::crashHandler(int signal) {
  const char *name = "";
  switch (signal) {
  case SIGSEGV:
    name = "SIGSEGV";
    break;
  case SIGABRT:
    name = "SIGABRT";
    break;
  case SIGTERM:
    name = "SIGTERM";
    break;
  case SIGINT:
    name = "SIGINT"; // This isn't really a crash but oh well
    break;
  }

  signalSafeWrite("\nCRASH: Signal ");
  signalSafeWrite(signal);
  signalSafeWrite(" (");
  signalSafeWrite(name);
  signalSafeWrite(")\n");

  shutdown();
  _Exit(signal == SIGINT ? EXIT_SUCCESS : EXIT_FAILURE);
}
