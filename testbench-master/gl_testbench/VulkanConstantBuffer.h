#pragma once
#include "ConstantBuffer.h"
#include <vulkan\vulkan.h>
#include <functional>
#include "StagingBuffer.h"

class VulkanConstantBuffer :
	public ConstantBuffer
{
public:
	VulkanConstantBuffer(std::string NAME, unsigned int location, 
		const std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)>& createBufferCallback,
		std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> updateBufferCallback);
	~VulkanConstantBuffer();


	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);
	uint32_t GetLocation()const;
	VkBuffer GetBuffer()const;
	VkDeviceSize GetSize()const {
		return _size;
	}
private:
	std::string _name;
	uint32_t _location;
	VkDeviceSize _size;
	VkBuffer _buffer;
	StagingBuffer _stagningBuffer;

	std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> _createBufferCallback;
	std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> _updateBufferCallback;
};

