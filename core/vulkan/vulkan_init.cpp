#include "vulkan_init.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_render.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"

void vkInitialize(GLFWwindow *window, VkInstanceCreateInfo createInfo) {
  getInstanceExtensions();
  createVkInstance(createInfo);
  createVkSurface(window); // This must be called BEFORE getting devices!!!
  getDevice();
  findQueueFamilies();
  createLogicalDevice();
  querySwapchainSupport();
  chooseSwapSurfaceFormat();
  chooseSwapPresentMode();
  chooseSwapExtent(window);
  createSwapchain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createFrameBuffers();
  createCommandPool();
  createCommandBuffer();
  createSyncObjects();
}
