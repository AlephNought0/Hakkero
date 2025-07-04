#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct window_backend {
  /// @brief A vulkan surface for the window that we can draw into.
  VkSurfaceKHR surface = VK_NULL_HANDLE;

  /// @brief The semaphore to indicate image availability.
  VkSemaphore imageAvailableSemaphore;

  /// @brief The semaphore to indicate finished image rendering.
  VkSemaphore renderFinishedSemaphore;

  /// @brief The in-flight fence is used to indicate whether or not a frame is
  /// busy/ready.
  VkFence inFlightFence;
};

struct vulkan_command_buffer {
  /// @brief Opaque handle to a command buffer object.
  VkCommandBuffer buffer;
};

struct vulkan_shader {
  /// @brief Opaque handle to a shader module object.
  VkShaderModule handle = VK_NULL_HANDLE;
};

struct vulkan_pipeline {
  /// @brief Opaque handle to a render pass object.
  VkRenderPass renderPass = VK_NULL_HANDLE;

  /// @brief The pipeline layout.
  VkPipelineLayout layout = VK_NULL_HANDLE;

  /// @brief Opaque handle to a pipeline object.
  VkPipeline graphicsPipeline = VK_NULL_HANDLE;

  /// @brief Opaque handle to a framebuffer object
  std::vector<VkFramebuffer> swapChainFramebuffers;
};

struct vulkan_image {
  /// @brief Handles of the internal image objects.
  std::vector<VkImage> swapChainImages;

  /// @brief Handles of the internal image view objects.
  std::vector<VkImageView> swapChainImageViews;
};

struct vulkan_swapchain_support_info {
  /// @brief Structure describing capabilities of a surface.
  VkSurfaceCapabilitiesKHR capabilities;

  /// @brief Supported surface formats for the surface.
  std::vector<VkSurfaceFormatKHR> formats;

  /// @brief Supported presentation modes for the surface.
  std::vector<VkPresentModeKHR> presentModes;
};

struct vulkan_swapchain {
  /// @brief The swapchain handle.
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;

  /// @brief The swapchain image format.
  VkSurfaceFormatKHR format;

  /// @brief The swapchain presentation mode.
  VkPresentModeKHR presentMode;

  /// @brief The resolution of swap images.
  VkExtent2D extent;
};

struct vulkan_context {
  /// @brief The handle to the vulkan instance.
  VkInstance instance = VK_NULL_HANDLE;

  /// @brief Required instance extensions.
  std::vector<const char *> instanceExtensions;
};

struct vulkan_device {
  /// @brief This is the physical graphics card device.
  VkPhysicalDevice vkPhysDevice = VK_NULL_HANDLE;

  /// @brief This is the logical device. It is used for most of the vulkan
  /// operations.
  VkDevice logicalDevice = VK_NULL_HANDLE;

  /// @brief Features of the physical device.
  VkPhysicalDeviceFeatures deviceFeatures;

  /// @brief Required device extensions.
  std::vector<const char *> deviceExtensions;

  /// @brief A graphics queue index.
  std::optional<uint32_t> graphics_queue_index;

  /// @brief Index of the present queue.
  std::optional<uint32_t> present_queue_index;

  /// @brief A handle to the graphics queue.
  VkQueue graphicsQueue = VK_NULL_HANDLE;

  /// @brief A handle to the present queue.
  VkQueue presentQueue = VK_NULL_HANDLE;

  /// @brief Opaque handle to a command pool object
  VkCommandPool commandPool = VK_NULL_HANDLE;
};

void initializeVkStructs();
vulkan_context &getVulkanContextStruct();
vulkan_device &getVulkanDeviceStruct();
vulkan_swapchain &getVulkanSwapchainStruct();
vulkan_swapchain_support_info &getVulkanSwapchainSupportStruct();
vulkan_image &getVulkanImageStruct();
vulkan_shader &getVulkanShaderStruct();
vulkan_pipeline &getVulkanPipelineStruct();
vulkan_command_buffer &getVulkanCommandBufferStruct();
window_backend &getWindowBackendStruct();

#endif
