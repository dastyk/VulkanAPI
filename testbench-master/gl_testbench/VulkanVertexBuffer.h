#include "VertexBuffer.h"
#include <vulkan\vulkan.h>
#include <functional>

#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(const std::function<void(const void* data, size_t size, VkBuffer& buffer, VkBufferView& view)>& createBufferCallback);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	VkBufferView GetBufferView();
	size_t getSize();

private:
	VkDeviceSize _totalSize;
	VkBuffer _buffer;
	VkBufferView _view;
	std::function<void(const void* data, size_t size, VkBuffer& buffer, VkBufferView& view)> _createBufferCallback;

};

