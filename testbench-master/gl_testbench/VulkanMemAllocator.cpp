#include "VulkanMemAllocator.h"
#include "VulkanHelpers.h"

VulkanMemAllocator::VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device, VkDeviceSize size, VkMemoryRequirements req, VkMemoryPropertyFlags memoryFlags):
_physicalDevice(phyDev), _device(device), _totalSize(size), _freeSize(size), _firstFree(0U)
{

	uint32_t index = VulkanHelpers::ChooseHeapFromFlags(phyDev, &req, memoryFlags, memoryFlags);
	VulkanHelpers::AllocateMemory(_device, _totalSize, index, &_memory);


}

VulkanMemAllocator::~VulkanMemAllocator()
{
	vkFreeMemory(_device, _memory, nullptr);
}

const void VulkanMemAllocator::Allocate(VkDeviceSize size, VkDeviceMemory * memory, VkDeviceSize * offset)
{
	if (_freeSize < size)
		throw std::runtime_error("Out of device memory");

	*memory = _memory;
	*offset = _firstFree;
	_firstFree += size;

	return void();
}

