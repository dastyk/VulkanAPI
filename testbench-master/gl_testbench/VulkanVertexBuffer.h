#include "VertexBuffer.h"
#include <vulkan\vulkan.h>
#include <functional>

#pragma once
class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(const std::function<VkDeviceSize(const void* data, size_t size)>& callback);
	~VulkanVertexBuffer();


	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();

private:
	VkDeviceSize _totalSize;
	VkDeviceSize _offset;
	std::function<VkDeviceSize(const void* data, size_t size)> _callBack;

};

