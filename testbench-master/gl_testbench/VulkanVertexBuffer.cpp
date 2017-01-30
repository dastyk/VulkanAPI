#include "VulkanVertexBuffer.h"
#include <glm\glm.hpp>


VulkanVertexBuffer::VulkanVertexBuffer(VkDevice device) : _device(device)
{
}


VulkanVertexBuffer::~VulkanVertexBuffer()
{
}

void VulkanVertexBuffer::setData(const void * data, size_t size, DATA_USAGE usage)
{
}

void VulkanVertexBuffer::bind(size_t offset, size_t size, unsigned int location)
{
}

void VulkanVertexBuffer::unbind()
{
}

size_t VulkanVertexBuffer::getSize()
{
	return size_t();
}
