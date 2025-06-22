#include "vulkan_command_buffer.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

void createCommandPool() {
  vulkan_device &vkDevice = getVulkanDeviceStruct();

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = vkDevice.graphics_queue_index.value();

  VkResult result = vkCreateCommandPool(vkDevice.logicalDevice, &poolInfo,
                                        nullptr, &vkDevice.commandPool);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Created the command pool.");
  }
}

void createCommandBuffer() {
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  vulkan_command_buffer &vkCommandBuffer = getVulkanCommandBufferStruct();

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = vkDevice.commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkResult result = vkAllocateCommandBuffers(vkDevice.logicalDevice, &allocInfo,
                                             &vkCommandBuffer.buffer);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Allocated the command buffers.");
  }
}

void recordCommandBuffer(uint32_t imageIndex) {
  vulkan_command_buffer &vkCommandBuffer = getVulkanCommandBufferStruct();
  vulkan_pipeline &vkPipeline = getVulkanPipelineStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  VkResult result = vkBeginCommandBuffer(vkCommandBuffer.buffer, &beginInfo);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vkPipeline.renderPass;
  renderPassInfo.framebuffer = vkPipeline.swapChainFramebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vkSwapchain.extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(vkCommandBuffer.buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(vkCommandBuffer.buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vkPipeline.graphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(vkSwapchain.extent.width);
  viewport.height = static_cast<float>(vkSwapchain.extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(vkCommandBuffer.buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = vkSwapchain.extent;
  vkCmdSetScissor(vkCommandBuffer.buffer, 0, 1, &scissor);

  vkCmdDraw(vkCommandBuffer.buffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(vkCommandBuffer.buffer);

  VkResult endResult = vkEndCommandBuffer(vkCommandBuffer.buffer);
  if (!checkVkResult(endResult)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }
}
