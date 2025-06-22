#include "vulkan_types.hpp"

#include <cassert>
#include <stdexcept>
#include <vulkan/vulkan.h>

static vulkan_context s_context;
static vulkan_device s_device;
static vulkan_swapchain s_swapchain;
static vulkan_swapchain_support_info s_swapchainSupport;
static vulkan_image s_image;
static vulkan_shader s_shader;
static vulkan_pipeline s_pipeline;
static vulkan_command_buffer s_command_buffer;
static window_backend s_window;

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
  s_swapchain = vulkan_swapchain{};
  s_window = window_backend{};
}

vulkan_context &getVulkanContextStruct() {
  checkInit();

  return s_context;
}

vulkan_device &getVulkanDeviceStruct() {
  checkInit();

  return s_device;
}

vulkan_swapchain &getVulkanSwapchainStruct() {
  checkInit();

  return s_swapchain;
}

vulkan_swapchain_support_info &getVulkanSwapchainSupportStruct() {
  checkInit();

  return s_swapchainSupport;
}

vulkan_image &getVulkanImageStruct() {
  checkInit();

  return s_image;
}

vulkan_shader &getVulkanShaderStruct() {
  checkInit();

  return s_shader;
}

vulkan_pipeline &getVulkanPipelineStruct() {
  checkInit();

  return s_pipeline;
}

vulkan_command_buffer &getVulkanCommandBufferStruct() {
  checkInit();

  return s_command_buffer;
}

window_backend &getWindowBackendStruct() {
  checkInit();

  return s_window;
}
