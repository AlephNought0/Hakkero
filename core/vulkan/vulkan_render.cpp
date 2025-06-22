#include "vulkan_render.hpp"
#include "logger.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

void drawFrame() {
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_command_buffer &vkCommandBuffer = getVulkanCommandBufferStruct();
  window_backend &vkWindow = getWindowBackendStruct();

  vkWaitForFences(vkDevice.logicalDevice, 1, &vkWindow.inFlightFence, VK_TRUE,
                  UINT64_MAX);
  vkResetFences(vkDevice.logicalDevice, 1, &vkWindow.inFlightFence);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(vkDevice.logicalDevice, vkSwapchain.swapchain,
                        UINT64_MAX, vkWindow.imageAvailableSemaphore,
                        VK_NULL_HANDLE, &imageIndex);

  vkResetCommandBuffer(vkCommandBuffer.buffer, 0);

  // TODO: There will be multiple command buffers later most likely. So we'll
  // want to pass the one we want to record.
  recordCommandBuffer(imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {vkWindow.imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &vkCommandBuffer.buffer;

  VkSemaphore signalSemaphores[] = {vkWindow.renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkResult result = vkQueueSubmit(vkDevice.graphicsQueue, 1, &submitInfo,
                                  vkWindow.inFlightFence);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vkSwapchain.swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(vkDevice.presentQueue, &presentInfo);
}

void createSyncObjects() {
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  window_backend &vkWindow = getWindowBackendStruct();

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  /// Creating a signaled fence will make the first wait call to return
  /// immediately.
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkResult imageResult =
      vkCreateSemaphore(vkDevice.logicalDevice, &semaphoreInfo, nullptr,
                        &vkWindow.imageAvailableSemaphore);

  VkResult renderFinishResult =
      vkCreateSemaphore(vkDevice.logicalDevice, &semaphoreInfo, nullptr,
                        &vkWindow.renderFinishedSemaphore);

  VkResult fenceResult = vkCreateFence(vkDevice.logicalDevice, &fenceInfo,
                                       nullptr, &vkWindow.inFlightFence);

  if (!checkVkResult(imageResult) || !checkVkResult(renderFinishResult)) {
    LOG_ERROR(vkResultToString(imageResult));
    LOG_ERROR(vkResultToString(renderFinishResult));
    throw std::runtime_error("Failed to create the semaphores.");
  }

  else {
    LOG_INFO("Successfully created the semaphores.");
  }

  if (!checkVkResult(fenceResult)) {
    LOG_ERROR(vkResultToString(fenceResult));
    throw std::runtime_error(vkResultToString(fenceResult));
  }
}
