#include "VulkanVertexBuffer.h"
#include <glm\glm.hpp>
#include "VulkanHelpers.h"

VulkanVertexBuffer::VulkanVertexBuffer(VkPhysicalDevice phyDevice, VkDevice device, VkCommandBuffer cmdBuffer) : _physicalDevice(phyDevice), _device(device), _cmdBuffer(cmdBuffer)
{

	
}


VulkanVertexBuffer::~VulkanVertexBuffer()
{
	if (_memory != VK_NULL_HANDLE)
		vkFreeMemory(_device, _memory, nullptr);
	if (_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(_device, _buffer, nullptr);

}

void VulkanVertexBuffer::setData(const void * data, size_t size, DATA_USAGE usage)
{
	_totalSize = size;

	if (_memory != VK_NULL_HANDLE)
		vkFreeMemory(_device, _memory, nullptr);
	if (_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(_device, _buffer, nullptr);



	/*Create the buffer*/
	const auto bufferInfo = &VulkanHelpers::MakeBufferCreateInfo(
		size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	VulkanHelpers::CreateBuffer(_device, bufferInfo, &_buffer);


	/*Get memory requirments*/
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(_device, _buffer, &memReq);
	VkMemoryPropertyFlags reqFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	auto memTypeIndex = VulkanHelpers::ChooseHeapFromFlags(_physicalDevice, &memReq, reqFlags, reqFlags);


	/*Allocate the memory*/
	VulkanHelpers::AllocateMemory(
		_device,
		memReq.size,
		memTypeIndex,
		&_memory
	);

	/*Bind the memory to the buffer*/
	vkBindBufferMemory(_device, _buffer, _memory, 0);


	/*Copy the data to device*/
	void* pData;
	VulkanHelpers::MapMemory(_device, _memory, &pData);
	memcpy(pData, data, size);
	vkUnmapMemory(_device, _memory);





}

void VulkanVertexBuffer::bind(size_t offset, size_t size, unsigned int location)
{
	assert(offset + size <= _totalSize);
	/*Bind the buffer*/
	VkDeviceSize offsetArr[] = { offset };

	vkCmdBindVertexBuffers(_cmdBuffer, location, 1, &_buffer, offsetArr);
}

void VulkanVertexBuffer::unbind()
{

}

size_t VulkanVertexBuffer::getSize()
{
	return _totalSize;
}
