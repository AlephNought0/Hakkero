#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/vulkan/vulkan_device.hpp>
#include <core/vulkan/vulkan_image.hpp>
#include <core/vulkan/vulkan_instance.hpp>
#include <core/vulkan/vulkan_surface.hpp>
#include <core/vulkan/vulkan_swapchain.hpp>
#include <core/vulkan/vulkan_types.hpp>
#include <core/vulkan/vulkan_utils.hpp>
#include <cstring>
#include <stdexcept>

int main() {
  // TODO: Learn how to commit a buffer to the window so wayland can show it.
  // Learn how to create a swapchain
  // Ideally also learn how to present frames (images) into the surface
  // Yeah, also learn how to create a vulkan surface
  // Also create a function for validation layers
  // Later on create some global function to clear all vulkan objects. Although
  // I do plan to call the said function in the future crash handler, the user
  // should still call it manually just in case because there's no guarantee the
  // crash handler can call all the functions in time (unless my crash handler
  // turns out to be amazing I guess lol).

  initializeVkStructs();
  vulkan_context &context = getVulkanContextStruct();
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();

  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hakkero Engine";

  getInstanceExtensions();

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.ppEnabledExtensionNames = context.instanceExtensions.data();
  createInfo.enabledExtensionCount = context.instanceExtensions.size();

  GLFWwindow *window =
      glfwCreateWindow(800, 800, appInfo.pApplicationName, nullptr, nullptr);

  /* I'd recommend setting some deviceExtensions like this from my current
   * knowledge. Of course some would be better of hardcoded like the swapchain
   * extension but I'll figure that one later.
   */
  vkDeviceStruct.deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}
