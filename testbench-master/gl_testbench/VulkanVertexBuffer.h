#include "VertexBuffer.h"
#include <vulkan\vulkan.h>


#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(VkPhysicalDevice phyDevice, VkDevice device, VkCommandBuffer cmdBuffer);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();


private:
	VkDevice _device;
	VkPhysicalDevice _physicalDevice;
	VkBuffer _buffer;
	VkCommandBuffer _cmdBuffer;
	VkDeviceMemory _memory;

	size_t _totalSize;
};

