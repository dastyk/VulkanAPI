#include "VertexBuffer.h"
#include <vulkan\vulkan.h>


#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(VkDevice device);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();


private:
	VkDevice _device;
	VkBuffer _buffer;
};

