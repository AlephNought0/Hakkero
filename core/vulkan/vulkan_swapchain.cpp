#include "vulkan_swapchain.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"
#include <GLFW/glfw3.h>
#include <limits>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

void querySwapchainSupport() {
  window_backend &vkWindowBackend = getWindowBackendStruct();
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();
  vulkan_swapchain_support_info &details = getVulkanSwapchainSupportStruct();

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkDeviceStruct.vkPhysDevice,
                                            vkWindowBackend.surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vkDeviceStruct.vkPhysDevice,
                                       vkWindowBackend.surface, &formatCount,
                                       nullptr);

  // Get supported surface formats
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkDeviceStruct.vkPhysDevice,
                                         vkWindowBackend.surface, &formatCount,
                                         details.formats.data());

    LOG_INFO("Got the supported surface formats for the surface.");
  }

  else {
    LOG_ERROR("Could not get supported surface formats.");
    throw std::runtime_error("Could not get supported surface formats.");
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vkDeviceStruct.vkPhysDevice,
                                            vkWindowBackend.surface,
                                            &presentModeCount, nullptr);
  // Get supported present modes
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        vkDeviceStruct.vkPhysDevice, vkWindowBackend.surface, &presentModeCount,
        details.presentModes.data());
    LOG_INFO("Got the supported presentation modes for the surface.");
  }

  else {
    LOG_ERROR("Could not get supported presentation modes for the surface.");
    throw std::runtime_error(
        "Could not get supported presentation modes for the surface.");
  }
}

void chooseSwapSurfaceFormat() {
  vulkan_swapchain_support_info &swapSupport =
      getVulkanSwapchainSupportStruct();
  vulkan_swapchain &swapchainStruct = getVulkanSwapchainStruct();

  // Will later make code to choose from a different format if the "ideal" one
  // isn't supported. We don't need to throw std::runtime_error here
  // because this function will never be called if supported surface formats
  // could not be found.
  for (const auto &availableFormat : swapSupport.formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      swapchainStruct.format = availableFormat;
      LOG_INFO("Swap surface format chosen.");
    }
  }
}

void chooseSwapPresentMode() {
  vulkan_swapchain_support_info &swapSupport =
      getVulkanSwapchainSupportStruct();
  vulkan_swapchain &swapchainStruct = getVulkanSwapchainStruct();

  for (const auto &availablePresentMode : swapSupport.presentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      swapchainStruct.presentMode = availablePresentMode;
      LOG_INFO("Using mailbox presentation mode for the swapchain.");
    }
  }

  if (swapchainStruct.presentMode != VK_PRESENT_MODE_MAILBOX_KHR) {
    swapchainStruct.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    LOG_INFO("Using FIFO presentation mode for the swapchain.");
  }
}

void chooseSwapExtent(GLFWwindow *window) {
  vulkan_swapchain_support_info &swapSupport =
      getVulkanSwapchainSupportStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();

  if (swapSupport.capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    vkSwapchain.extent = swapSupport.capabilities.currentExtent;
    LOG_INFO("Chosen the swapchain 2D image extent.");
  }

  else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(
        actualExtent.width, swapSupport.capabilities.minImageExtent.width,
        swapSupport.capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, swapSupport.capabilities.minImageExtent.height,
        swapSupport.capabilities.maxImageExtent.height);

    vkSwapchain.extent = actualExtent;

    LOG_INFO("Chosen the swapchain 2D image extent");
  }
}

void createSwapchain() {
  vulkan_swapchain_support_info &swapSupport =
      getVulkanSwapchainSupportStruct();
  window_backend &vkWindowBackend = getWindowBackendStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  vulkan_image &vkImage = getVulkanImageStruct();

  /// It's better to ask for at least 1 more image so we do not have to wait for
  /// the driver to complete the internal operations
  uint32_t imageCount = swapSupport.capabilities.minImageCount + 1;

  if (swapSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapSupport.capabilities.maxImageCount) {
    imageCount = swapSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = vkWindowBackend.surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = vkSwapchain.format.format;
  createInfo.imageColorSpace = vkSwapchain.format.colorSpace;
  createInfo.imageExtent = vkSwapchain.extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {vkDevice.graphics_queue_index.value(),
                                   vkDevice.present_queue_index.value()};

  if (vkDevice.graphics_queue_index != vkDevice.present_queue_index) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }

  else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = vkSwapchain.presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(vkDevice.logicalDevice, &createInfo,
                                         nullptr, &vkSwapchain.swapchain);

  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Created the vulkan swapchain.");
  }

  vkGetSwapchainImagesKHR(vkDevice.logicalDevice, vkSwapchain.swapchain,
                          &imageCount, nullptr);
  vkImage.swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(vkDevice.logicalDevice, vkSwapchain.swapchain,
                          &imageCount, vkImage.swapChainImages.data());
}
