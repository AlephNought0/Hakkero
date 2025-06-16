#pragma once

#include <initializer_list>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

void createVkInstance(VkInstanceCreateInfo &createInfo);

std::vector<const char *>
getInstanceExtensions(std::initializer_list<std::string_view> optional = {});
