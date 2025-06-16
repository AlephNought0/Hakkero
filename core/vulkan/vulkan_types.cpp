#include "vulkan_types.hpp"

#include <cassert>
#include <stdexcept>
#include <vulkan/vulkan.h>

static vulkan_context s_context;
static vulkan_device s_device;
static bool initialized = false;

void checkInit() {
  if (!initialized) {
    throw std::runtime_error("Vulkan structures were not initialized. Call "
                             "initializeVkStructs() first.");
  }
}

void initializeVkStructs() {
  if (initialized)
    return;
  initialized = true;
  s_context = vulkan_context{};
  s_device = vulkan_device{};
}

vulkan_context &getVulkanContextStruct() {
  checkInit();

  return s_context;
}

vulkan_device &getVulkanDeviceStruct() {
  checkInit();

  return s_device;
}
