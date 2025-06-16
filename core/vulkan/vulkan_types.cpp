#include "vulkan_types.hpp"

#include <cassert>
#include <vulkan/vulkan.h>

static vulkan_context s_context;
static vulkan_device s_device;
static bool initialized = false;

void initializeVkStructs() {
  if (initialized)
    return;
  initialized = true;
  s_context = vulkan_context{};
  s_device = vulkan_device{};
}

vulkan_context &getVulkanContextStruct() {
  assert(initialized && "Vulkan structures were not initialized. Call "
                        "initializeVkStructs() first.");
  return s_context;
}

vulkan_device &getVulkanDeviceStruct() {
  assert(initialized && "Vulkan structures were not initialized. Call "
                        "initializeVkStructs() first.");
  return s_device;
}
