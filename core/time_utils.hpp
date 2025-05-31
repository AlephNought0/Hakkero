#pragma once
#include <chrono>
#include <ctime>
#include <string>

class TimeUtils {
public:
  struct TimeData {
    std::chrono::system_clock::time_point timePoint;
    std::time_t timeT;
    std::tm localTime;
  };

  static TimeData captureCurrentTime();

  static std::string formatAsDate(const TimeData &td);
  static std::string formatAsHourMinSec(const TimeData &td);

private:
  static std::tm localtimeThreadSafe(std::time_t t);
};
