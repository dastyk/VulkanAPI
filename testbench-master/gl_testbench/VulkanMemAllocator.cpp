#include "VulkanMemAllocator.h"


VulkanMemAllocator::VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device) :
	_physicalDevice(phyDev), _device(device)
{
}

VulkanMemAllocator::~VulkanMemAllocator()
{
}
