#include "VulkanMemAllocator.h"
#include "VulkanHelpers.h"

VulkanMemAllocator::VulkanMemAllocator(VkPhysicalDevice phyDev, VkDevice device,VkMemoryRequirements req, VkMemoryPropertyFlags memoryFlags):
_physicalDevice(phyDev), _device(device), _memoryReq(req), _freeSize(req.size), _firstFree(0U)
{

	uint32_t index = VulkanHelpers::ChooseHeapFromFlags(phyDev, &req, memoryFlags, memoryFlags);
	VulkanHelpers::AllocateMemory(_device, _memoryReq.size, index, &_memory);
	

}

VulkanMemAllocator::~VulkanMemAllocator()
{
	vkFreeMemory(_device, _memory, nullptr);
}

const void VulkanMemAllocator::Allocate(VkDeviceSize size, VkBuffer * buffer, VkDeviceSize * offset)
{
	uint32_t blocks = (uint32_t)std::ceil((float)size / (float)_memoryReq.alignment);
	VkDeviceSize allocSize = blocks*_memoryReq.alignment;

	if (_freeSize < allocSize)
		throw std::runtime_error("Out of device memory");

	*buffer = _buffer;
	*offset = _firstFree;
	_firstFree += allocSize;

	return void();
}

