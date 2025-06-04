#include <logger.hpp>

int main(void) {
  for (int i = 0; i < 10000; ++i) {
    LOG_INFO("Massive meow test");
    LOG_ERROR("What");
  }
  Logger::shutdown();
  return 0;
}
