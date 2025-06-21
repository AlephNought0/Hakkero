#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

void querySwapchainSupport();
void chooseSwapSurfaceFormat();
void chooseSwapPresentMode();
void chooseSwapExtent(GLFWwindow *window);
void createSwapchain();
