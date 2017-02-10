#pragma once

#include <vulkan\vulkan.h>

#include "StagingBuffer.h"

class VulkanMemAllocator
{
public:
	VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device, VkMemoryRequirements req, VkMemoryPropertyFlags memoryFlags);
	~VulkanMemAllocator();

	const void AllocateBufferMemory(VkDeviceSize size, VkBuffer& buffer);
	const void AllocateBufferMemory(VkDeviceSize size, StagingBuffer& buffer);
private:
	VkPhysicalDevice _physicalDevice;
	VkDevice _device;
	VkDeviceMemory _memory;

	VkMemoryRequirements _memoryReq;
	VkDeviceSize _freeSize;
	VkDeviceSize _firstFree;
};

