#pragma once

#include <initializer_list>
#include <string>
#include <vulkan/vulkan_core.h>

void createVkInstance(VkInstanceCreateInfo &createInfo);

void getInstanceExtensions(
    std::initializer_list<std::string_view> optional = {});
