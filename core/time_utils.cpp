#include "time_utils.hpp"
#include <iomanip>
#include <sstream>

TimeUtils::TimeData TimeUtils::captureCurrentTime() {
  TimeData td;
  td.timePoint = std::chrono::system_clock::now();
  td.timeT = std::chrono::system_clock::to_time_t(td.timePoint);
  td.localTime = localtimeThreadSafe(td.timeT);
  return td;
}

std::tm TimeUtils::localtimeThreadSafe(std::time_t t) {
  std::tm tm;
  localtime_r(&t, &tm);
  return tm;
}

std::string TimeUtils::formatAsDate(const TimeData &td) {
  std::ostringstream ss;
  ss << std::put_time(&td.localTime, "%Y-%m-%d");
  return ss.str();
}

std::string TimeUtils::formatAsHourMinSec(const TimeData &td) {
  std::ostringstream ss;
  ss << std::put_time(&td.localTime, "%H:%M:%S");
  return ss.str();
}
