#include "VulkanVertexBuffer.h"
#include <glm\glm.hpp>
#include "VulkanHelpers.h"

VulkanVertexBuffer::VulkanVertexBuffer(VkPhysicalDevice phyDevice, VkDevice device, VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer cmdBuffer) :
	_physicalDevice(phyDevice), _device(device), _queue(queue), _cmdPool(cmdPool), _cmdBuffer(cmdBuffer)
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



	/*Create the staging buffer*/
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	_CreateBuffer(
		size, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		&stagingBuffer, &stagingBufferMemory);

	/*Copy the data to device*/
	void* pData;
	VulkanHelpers::MapMemory(_device, stagingBufferMemory, &pData);
	memcpy(pData, data, size);
	vkUnmapMemory(_device, stagingBufferMemory);

	/*Create the buffer*/
	_CreateBuffer(
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&_buffer, &_memory);



	/*Create a new temporary cmdbuffer, */
	const auto allocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(_cmdPool);
	VkCommandBuffer commandBuffer;
	VulkanHelpers::AllocateCommandBuffers(_device, allocInfo, &commandBuffer);

	/*Copy the data between buffers*/
	VulkanHelpers::BeginCommandBuffer(commandBuffer);
	VulkanHelpers::CopyDataBetweenBuffers(commandBuffer, stagingBuffer, 0, _buffer, 0, size);
	VulkanHelpers::EndCommandBuffer(commandBuffer);
	const auto submitInfo = &VulkanHelpers::MakeSubmitInfo(1, &commandBuffer);
	VulkanHelpers::QueueSubmit(_queue, 1, submitInfo);
	vkQueueWaitIdle(_queue);
	vkFreeCommandBuffers(_device, _cmdPool, 1, &commandBuffer);
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

const void VulkanVertexBuffer::_CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer * buffer, VkDeviceMemory * bufferMemory) const
{
	/*Create the buffer*/
	const auto bufferInfo = &VulkanHelpers::MakeBufferCreateInfo(
		size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	VulkanHelpers::CreateBuffer(_device, bufferInfo, buffer);


	/*Get memory requirments*/
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(_device, *buffer, &memReq);
	auto memTypeIndex = VulkanHelpers::ChooseHeapFromFlags(_physicalDevice, &memReq, properties, properties);


	/*Allocate the memory*/
	VulkanHelpers::AllocateMemory(
		_device,
		memReq.size,
		memTypeIndex,
		bufferMemory
	);

	/*Bind the memory to the buffer*/
	vkBindBufferMemory(_device, *buffer, *bufferMemory, 0);
}
