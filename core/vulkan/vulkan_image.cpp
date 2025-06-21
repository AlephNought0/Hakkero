#include "vulkan_image.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

void createImageViews() {
  vulkan_image &vkImage = getVulkanImageStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_device &vkDevice = getVulkanDeviceStruct();

  vkImage.swapChainImageViews.resize(vkImage.swapChainImages.size());

  for (size_t i = 0; i < vkImage.swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = vkImage.swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = vkSwapchain.format.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult result =
        vkCreateImageView(vkDevice.logicalDevice, &createInfo, nullptr,
                          &vkImage.swapChainImageViews[i]);

    if (!checkVkResult(result)) {
      LOG_ERROR(vkResultToString(result));
      throw std::runtime_error(vkResultToString(result));
    }
  }

  LOG_INFO("Successfully created the image views.");
}
