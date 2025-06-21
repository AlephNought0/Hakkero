#include "vulkan_device.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"

#include <set>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysDevice) {
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(vkPhysDevice, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(vkPhysDevice, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(
      vkDeviceStruct.deviceExtensions.begin(),
      vkDeviceStruct.deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

bool isSuitable(VkPhysicalDevice vkPhysDevice) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(vkPhysDevice, &deviceProperties);
  vkGetPhysicalDeviceFeatures(vkPhysDevice, &deviceFeatures);

  // I wonder if copying the VkPhysicalDevice twice is a smart choice
  bool extensionsSupported = checkDeviceExtensionSupport(vkPhysDevice);

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader && extensionsSupported;
}

void getDevice() {
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

void createLogicalDevice() {
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      vkDeviceStruct.graphics_queue_index.value(),
      vkDeviceStruct.present_queue_index.value()};

  float queuePrio = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePrio;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(vkDeviceStruct.deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = vkDeviceStruct.deviceExtensions.data();

  VkResult result = vkCreateDevice(vkDeviceStruct.vkPhysDevice, &createInfo,
                                   nullptr, &vkDeviceStruct.logicalDevice);

  vkGetDeviceQueue(vkDeviceStruct.logicalDevice,
                   vkDeviceStruct.graphics_queue_index.value(), 0,
                   &vkDeviceStruct.graphicsQueue);
  vkGetDeviceQueue(vkDeviceStruct.logicalDevice,
                   vkDeviceStruct.present_queue_index.value(), 0,
                   &vkDeviceStruct.presentQueue);

  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Successfully created the logical vulkan device.");
  }
}

void findQueueFamilies() {
  uint32_t queueFamilyCount = 0;
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();
  window_backend &vkWindowBackend = getWindowBackendStruct();

  vkGetPhysicalDeviceQueueFamilyProperties(vkDeviceStruct.vkPhysDevice,
                                           &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      vkDeviceStruct.vkPhysDevice, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      vkDeviceStruct.graphics_queue_index = i;
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(vkDeviceStruct.vkPhysDevice, i,
                                           vkWindowBackend.surface,
                                           &presentSupport);

      if (presentSupport) {
        vkDeviceStruct.present_queue_index = i;
      }

      if (vkDeviceStruct.graphics_queue_index.has_value() &&
          vkDeviceStruct.present_queue_index.has_value()) {
        LOG_INFO("Found a graphics queue family for the physical device.");
        LOG_INFO("Found a present queue family for the physical device.");
        break;
      }
    }
    i++;
  }
  if (!vkDeviceStruct.graphics_queue_index.has_value()) {
    LOG_ERROR("Could not find a graphics queue family.");
    throw std::runtime_error("Could not find a graphics queue family.");
  }

  if (!vkDeviceStruct.graphics_queue_index.has_value()) {
    LOG_ERROR("Could not find a present queue family.");
    throw std::runtime_error("Could not find a present queue family.");
  }
}
