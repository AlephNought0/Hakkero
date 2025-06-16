#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice {
public:
  /// @brief Gets the available graphics card and stores it into
  /// VkPhysicalDevice hadle
  /// Ideally it will choose dedicated graphics card (if it exists)
  static void getDevice();

private:
  uint32_t deviceCount = 0;

  static bool isSuitable(VkPhysicalDevice vkPhysDevice);
};
