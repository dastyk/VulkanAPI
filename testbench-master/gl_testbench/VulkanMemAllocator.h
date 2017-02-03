#pragma once

#include <vulkan\vulkan.h>

class VulkanMemAllocator
{
public:
	VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device, VkMemoryRequirements req, VkMemoryPropertyFlags memoryFlags);
	~VulkanMemAllocator();

	const void Allocate(VkDeviceSize size, VkBuffer* buffer, VkDeviceSize* offset);
private:
	VkPhysicalDevice _physicalDevice;
	VkDevice _device;
	VkBuffer _buffer;
	VkDeviceMemory _memory;

	VkMemoryRequirements _memoryReq;
	VkDeviceSize _freeSize;
	VkDeviceSize _firstFree;
};

