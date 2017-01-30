#pragma once
#include "ConstantBuffer.h"
#include <vulkan\vulkan.h>

class VulkanConstantBuffer :
	public ConstantBuffer
{
public:
	VulkanConstantBuffer(std::string NAME, unsigned int location);
	~VulkanConstantBuffer();


	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);

private:
	std::string _name;
	uint32_t _location;


	VkDevice _device;
	VkPhysicalDevice _physicalDevice;
	VkBuffer _buffer;
	VkCommandBuffer _cmdBuffer;
	VkDeviceMemory _memory;
};

