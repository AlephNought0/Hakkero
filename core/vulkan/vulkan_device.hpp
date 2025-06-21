#pragma once

#include <vulkan/vulkan.h>

/// @brief Gets the available graphics card and stores it into
/// VkPhysicalDevice handle
/// Ideally it will choose dedicated graphics card (if it exists)
void getDevice();

void findQueueFamilies();
void createLogicalDevice();
