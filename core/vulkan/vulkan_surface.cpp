#define GLFW_INCLUDE_VULKAN
#include "vulkan_surface.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

void createVkSurface(GLFWwindow *window) {
  vulkan_context &context = getVulkanContextStruct();
  window_backend &vkWindowBackend = getWindowBackendStruct();
  VkResult result = glfwCreateWindowSurface(context.instance, window, nullptr,
                                            &vkWindowBackend.surface);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Successfully created the vulkan surface.");
  }
}
