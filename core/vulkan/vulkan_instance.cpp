#include "vulkan_instance.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>

void createVkInstance(vulkan_context *context,
                      VkInstanceCreateInfo &createInfo) {
  if (context->instance) {
    LOG_ERROR("The vulkan instance already exists.");
    throw std::runtime_error("The vulkan instance already exists.");
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &context->instance);
  std::string message = vkResultToString(result);
  if (!checkVkResult(result)) {
    LOG_ERROR(message);
    throw std::runtime_error(message);
  }

  else {
    LOG_INFO("Successfully created the vulkan instance.");
  }
}

std::vector<const char *>
getInstanceExtensions(std::initializer_list<std::string_view> optional) {

  uint32_t glfwCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwCount);

  static thread_local std::vector<std::string> stringStorage;
  stringStorage.clear();
  stringStorage.reserve(glfwCount + optional.size());

  std::unordered_set<std::string_view> uniqueExtensions;
  uniqueExtensions.reserve(glfwCount + optional.size());

  for (uint32_t i = 0; i < glfwCount; ++i) {
    uniqueExtensions.insert(glfwExtensions[i]);
  }

  for (auto &&ext : optional) {
    if (uniqueExtensions.insert(ext).second) {
      stringStorage.emplace_back(ext); // Take ownership
    }
  }

  std::vector<const char *> extensions;
  extensions.reserve(uniqueExtensions.size());

  for (uint32_t i = 0; i < glfwCount; ++i) {
    if (uniqueExtensions.count(glfwExtensions[i])) {
      extensions.push_back(glfwExtensions[i]);
      uniqueExtensions.erase(glfwExtensions[i]);
    }
  }

  for (const auto &ext : stringStorage) {
    extensions.push_back(ext.c_str());
  }

  return extensions;
}
