#pragma once

#include <vulkan\vulkan.h>

class VulkanMemAllocator
{
public:
	VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device);
	~VulkanMemAllocator();

	const void Allocate(VkDeviceSize size, VkDeviceMemory* memory);
private:
	VkPhysicalDevice _physicalDevice;
	VkDevice _device;


};

