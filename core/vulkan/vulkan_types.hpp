#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct window_backend {
  /// @brief A vulkan surface for the window that we can draw into.
  VkSurfaceKHR surface = VK_NULL_HANDLE;
};

struct vulkan_swapchain {
  /// @brief Structure describing capabilities of a surface.
  VkSurfaceCapabilitiesKHR capabilities;

  /// @brief The swapchain handle.
  VkSwapchainKHR swapchain;

  /// @brief Swapchain image formats.
  std::vector<VkSurfaceFormatKHR> formats;

  /// @brief  Presentation modes supported for a surface.
  std::vector<VkPresentModeKHR> presentModes;
};

struct vulkan_context {
  /// @brief The handle to the vulkan instance.
  VkInstance instance = VK_NULL_HANDLE;
};

struct vulkan_device {
  /// @brief This is the physical graphics card device.
  VkPhysicalDevice vkPhysDevice = VK_NULL_HANDLE;

  /// @brief This is the logical device. It is used for most of the vulkan.
  /// operations.
  VkDevice logical_device = VK_NULL_HANDLE;

  /// @brief Features of the physical device.
  VkPhysicalDeviceFeatures deviceFeatures;

  /// @brief A graphics queue index.
  std::optional<uint32_t> graphics_queue_index;

  /// @brief Index of the present queue.
  std::optional<uint32_t> present_queue_index;

  /// @brief A handle to the graphics queue.
  VkQueue graphicsQueue = VK_NULL_HANDLE;

  /// @brief A handle to the present queue.
  VkQueue presentQueue = VK_NULL_HANDLE;

  /// @bried Required device extensions.
  std::vector<const char *> deviceExtensions;
};

void initializeVkStructs();
vulkan_context &getVulkanContextStruct();
vulkan_device &getVulkanDeviceStruct();
vulkan_swapchain &getVulkanSwapchainStruct();
window_backend &getWindowBackendStruct();

#endif
