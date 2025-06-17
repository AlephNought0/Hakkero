#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/vulkan/vulkan_device.hpp>
#include <core/vulkan/vulkan_instance.hpp>
#include <core/vulkan/vulkan_surface.hpp>
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
  vulkan_device &vkDeviceStruct = getVulkanDeviceStruct();

  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
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
  createInfo.ppEnabledExtensionNames = vkDeviceStruct.deviceExtensions.data();
  createInfo.enabledExtensionCount = vkDeviceStruct.deviceExtensions.size();

  GLFWwindow *window =
      glfwCreateWindow(800, 800, appInfo.pApplicationName, nullptr, nullptr);

  createVkInstance(createInfo);
  createVkSurface(window);
  VulkanDevice::getDevice();
  VulkanDevice::findQueueFamilies();
  VulkanDevice::createLogicalDevice();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}
