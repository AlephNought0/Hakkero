#include "vulkan/vulkan_types.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/vulkan/vulkan_instance.hpp>
#include <core/vulkan/vulkan_utils.hpp>
#include <stdexcept>

int main() {
  // TODO: Learn how to commit a buffer to the window so wayland can show it.
  // So basically, learn how to select a physical device
  // Create a logical one
  // Get graphics queue
  // Learn how to create a swapchain
  // Ideally also learn how to present frames (images) into the surface
  // Yeah, also learn how to create a vulkan surface

  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hakkero Engine";

  // auto extensionNames = getInstanceExtensions();

  const char *extensionNames = "EAE";

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.ppEnabledExtensionNames = &extensionNames;
  createInfo.enabledExtensionCount = 1;

  GLFWwindow *window =
      glfwCreateWindow(800, 800, appInfo.pApplicationName, nullptr, nullptr);

  vulkan_context context = {};
  createVkInstance(&context, createInfo);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}
