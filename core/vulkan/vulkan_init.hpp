#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

void vkInitialize(GLFWwindow *window, VkInstanceCreateInfo createInfo);
