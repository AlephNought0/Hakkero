#include "vulkan_pipeline.hpp"
#include "logger.hpp"
#include "vulkan_types.hpp"
#include "vulkan_utils.hpp"

#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

void createFrameBuffers() {
  vulkan_image &vkImage = getVulkanImageStruct();
  vulkan_pipeline &vkPipeline = getVulkanPipelineStruct();
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_device &vkDevice = getVulkanDeviceStruct();

  vkPipeline.swapChainFramebuffers.resize(vkImage.swapChainImageViews.size());

  for (size_t i = 0; i < vkImage.swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {vkImage.swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vkPipeline.renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = vkSwapchain.extent.width;
    framebufferInfo.height = vkSwapchain.extent.height;
    framebufferInfo.layers = 1;

    VkResult result =
        vkCreateFramebuffer(vkDevice.logicalDevice, &framebufferInfo, nullptr,
                            &vkPipeline.swapChainFramebuffers[i]);
    if (!checkVkResult(result)) {
      LOG_ERROR(vkResultToString(result));
      throw std::runtime_error(vkResultToString(result));
    }
  }

  LOG_INFO("Created the framebuffer.");
}

std::vector<char> readFile(const std::string &filename) {
  // Opening a file at the end makes us able to determine the size of the file.
  // That's actually cool
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    LOG_ERROR(std::format("Failed to open the file: {}", filename));
    throw std::runtime_error(
        std::format("Failed to open the file: {}", filename));
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

void createRenderPass() {
  vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  vulkan_pipeline &vkPipeline = getVulkanPipelineStruct();

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = vkSwapchain.format.format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass(vkDevice.logicalDevice, &renderPassInfo,
                                       nullptr, &vkPipeline.renderPass);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Created the vulkan render pass.");
  }
}

void createGraphicsPipeline() {
  vulkan_device &vkDevice = getVulkanDeviceStruct();
  // vulkan_swapchain &vkSwapchain = getVulkanSwapchainStruct();
  vulkan_pipeline &vkPipeline = getVulkanPipelineStruct();

  auto vertShaderCode = readFile("../../samples/shaders/vert.spv");
  auto fragShaderCode = readFile("../../samples/shaders/frag.spv");

  VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  VkResult result = vkCreatePipelineLayout(
      vkDevice.logicalDevice, &pipelineLayoutInfo, nullptr, &vkPipeline.layout);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  else {
    LOG_INFO("Created the pipeline layout.");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = vkPipeline.layout;
  pipelineInfo.renderPass = vkPipeline.renderPass;
  pipelineInfo.subpass = 0;

  VkResult pipelineResult = vkCreateGraphicsPipelines(
      vkDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
      &vkPipeline.graphicsPipeline);
  if (!checkVkResult(pipelineResult)) {
    LOG_ERROR(vkResultToString(pipelineResult));
    throw std::runtime_error(vkResultToString(pipelineResult));
  }

  else {
    LOG_INFO("Created the graphics pipeline.");
  }

  vkDestroyShaderModule(vkDevice.logicalDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(vkDevice.logicalDevice, vertShaderModule, nullptr);
}

VkShaderModule createShaderModule(const std::vector<char> &code) {
  vulkan_device &vkDevice = getVulkanDeviceStruct();

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  VkResult result = vkCreateShaderModule(vkDevice.logicalDevice, &createInfo,
                                         nullptr, &shaderModule);
  if (!checkVkResult(result)) {
    LOG_ERROR(vkResultToString(result));
    throw std::runtime_error(vkResultToString(result));
  }

  return shaderModule;
}
