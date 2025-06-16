#include "vulkan_device.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

bool VulkanDevice::isSuitable(VkPhysicalDevice vkPhysDevice) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(vkPhysDevice, &deviceProperties);
  vkGetPhysicalDeviceFeatures(vkPhysDevice, &deviceFeatures);

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader;
}

void VulkanDevice::getDevice() {
  vulkan_context &context = getVulkanContextStruct();
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    LOG_FATAL("Failed to find GPUs with vulkan support.");
    throw std::runtime_error("Failed to find GPUs with vulkan support.");
  }

  else {
    LOG_INFO("Found GPUs with vulkan support.");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(context.instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isSuitable(device)) {
      vkDeviceStruct.vkPhysDevice = device;
      break;
    }
  }

  if (vkDeviceStruct.vkPhysDevice == VK_NULL_HANDLE) {
    LOG_FATAL("Failed to find a suitable GPU.");
    throw std::runtime_error("Failed to find a suitable GPU.");
  }

  else {
    LOG_INFO("Found a suitable GPU.");
  }
}
