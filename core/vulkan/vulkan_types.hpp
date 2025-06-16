#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct vulkan_context {
  /// @brief The handle to the vulkan instance.
  VkInstance instance = VK_NULL_HANDLE;
};

struct vulkan_device {
  /// @brief This is the physical graphics card device.
  VkPhysicalDevice vkPhysDevice = VK_NULL_HANDLE;
};

void initializeVkStructs();
vulkan_context &getVulkanContextStruct();
vulkan_device &getVulkanDeviceStruct();

#endif
