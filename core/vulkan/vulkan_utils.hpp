#pragma once

#include <string>
#include <vulkan/vulkan.h>

std::string vkResultToString(VkResult result);
bool checkVkResult(VkResult result);
