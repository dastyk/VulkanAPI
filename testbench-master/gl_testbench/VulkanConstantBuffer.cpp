#include "VulkanConstantBuffer.h"




VulkanConstantBuffer::VulkanConstantBuffer(std::string NAME, unsigned int location) : _name(NAME), _location(location)
{
}

VulkanConstantBuffer::~VulkanConstantBuffer()
{
}

void VulkanConstantBuffer::setData(const void * data, size_t size, Material * m, unsigned int location)
{



}

void VulkanConstantBuffer::bind(Material *)
{
}
