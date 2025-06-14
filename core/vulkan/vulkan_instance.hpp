#pragma once

#include "vulkan_types.hpp"

#include <initializer_list>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

void createVkInstance(vulkan_context *context,
                      VkInstanceCreateInfo &createInfo);

std::vector<const char *>
getInstanceExtensions(std::initializer_list<std::string_view> optional = {});
