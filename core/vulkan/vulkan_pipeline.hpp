#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

void createGraphicsPipeline();
void createRenderPass();
void createFrameBuffers();
VkShaderModule createShaderModule(const std::vector<char> &code);

std::vector<char> readFile(const std::string &filename);
