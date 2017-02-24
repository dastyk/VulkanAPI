#include "VertexBuffer.h"
#include <vulkan\vulkan.h>
#include <functional>
#include "StagingBuffer.h"
#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(const std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)>& createBufferCallback);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	VkBuffer GetBuffer();
	size_t getSize();

private:
	VkDeviceSize _totalSize;
	VkBuffer _buffer;
	std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> _createBufferCallback;

};

