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

const void VulkanMemAllocator::AllocateBufferMemory(VkDeviceSize size, VkBuffer& buffer)
{
	uint32_t blocks = (uint32_t)std::ceil((float)size / (float)_memoryReq.alignment);
	VkDeviceSize allocSize = blocks*_memoryReq.alignment;

	if (_freeSize < allocSize)
		throw std::runtime_error("Out of device memory");

	vkBindBufferMemory(_device, buffer, _memory, _firstFree);
	_firstFree += allocSize;

	return void();
}

const void VulkanMemAllocator::AllocateBufferMemory(VkDeviceSize size, StagingBuffer & buffer)
{
	uint32_t blocks = (uint32_t)std::ceil((float)size / (float)_memoryReq.alignment);
	VkDeviceSize allocSize = blocks*_memoryReq.alignment;

	if (_freeSize < allocSize)
		throw std::runtime_error("Out of device memory");
	buffer.memory = _memory;
	buffer.offset = _firstFree;
	vkBindBufferMemory(_device, buffer.buffer, buffer.memory, buffer.offset);
	_firstFree += allocSize;

	return void();
}

