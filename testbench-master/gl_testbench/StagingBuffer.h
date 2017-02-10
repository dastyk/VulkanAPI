#pragma once
#include <vulkan\vulkan.h>

#ifndef StagingBuffer
struct StagingBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize offset;
};
#endif // !StagingBuffer
