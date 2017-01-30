#include "VertexBuffer.h"
#include <vulkan\vulkan.h>


#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(VkPhysicalDevice phyDevice, VkDevice device, VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer cmdBuffer);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();


private:

	const void _CreateBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer* buffer, VkDeviceMemory* bufferMemory)const;

	VkDevice _device;
	VkPhysicalDevice _physicalDevice;
	VkBuffer _buffer;
	VkCommandPool _cmdPool;
	VkQueue _queue;
	VkCommandBuffer _cmdBuffer;
	VkDeviceMemory _memory;


	size_t _totalSize;
};

