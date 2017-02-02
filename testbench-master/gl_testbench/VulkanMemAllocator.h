#pragma once

#include <vulkan\vulkan.h>

class VulkanMemAllocator
{
public:
	VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device, VkDeviceSize size, VkMemoryRequirements req, VkMemoryPropertyFlags memoryFlags);
	~VulkanMemAllocator();

	const void Allocate(VkDeviceSize size, VkDeviceMemory* memory, VkDeviceSize* offset);
private:
	VkPhysicalDevice _physicalDevice;
	VkDevice _device;
	VkDeviceMemory _memory;

	VkDeviceSize _totalSize;
	VkDeviceSize _freeSize;
	VkDeviceSize _firstFree;

};

