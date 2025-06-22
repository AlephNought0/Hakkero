#pragma once

#include <stdint.h>

void createCommandPool();
void createCommandBuffer();
void recordCommandBuffer(uint32_t imageIndex);
