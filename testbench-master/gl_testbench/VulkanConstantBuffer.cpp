#include "VulkanConstantBuffer.h"
#include "VulkanHelpers.h"



VulkanConstantBuffer::VulkanConstantBuffer(std::string NAME, unsigned int location, const std::function<void(const void*data, size_t size, VkBuffer&buffer, StagingBuffer&stagingBuffer)>& createBufferCallback, std::function<void(const void*data, size_t size, VkBuffer&buffer, StagingBuffer&stagingBuffer)> updateBufferCallback)
: _name(NAME), _location(location), _createBufferCallback(createBufferCallback), _buffer(VK_NULL_HANDLE), _updateBufferCallback(updateBufferCallback){
}

VulkanConstantBuffer::~VulkanConstantBuffer()
{
}

void VulkanConstantBuffer::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	auto f = _createBufferCallback;
	if (_buffer != VK_NULL_HANDLE)
	{
		if (size != _size)
		{
			// Recreate the buffer
			throw std::runtime_error("Not supported");
			return;
		}
		else
		{
			// Update data
			f = _updateBufferCallback;
		}		
	}

	_size = size;
	f(data, size, _buffer, _stagningBuffer);

	//if (_first)
	//{

	//	/*Create the staging buffer*/
	//	VulkanHelpers::CreateBuffer(
	//		_physicalDevice,
	//		_device,
	//		size,
	//		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	//		&_stagingBuffer, &_stagingBufferMemory);

	//	/*Create the buffer*/
	//	VulkanHelpers::CreateBuffer(
	//		_physicalDevice,
	//		_device,
	//		size,
	//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	//		&_buffer, &_memory);
	//}

	///*Copy the data to staging*/
	//void* pData;
	//VulkanHelpers::MapMemory(_device, _stagingBufferMemory, &pData);
	//memcpy(pData, data, size);
	//vkUnmapMemory(_device, _stagingBufferMemory);


	///*Create a new temporary cmdbuffer, */
	//const auto allocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(_cmdPool);
	//VkCommandBuffer commandBuffer;
	//VulkanHelpers::AllocateCommandBuffers(_device, allocInfo, &commandBuffer);

	///*Copy the data between buffers*/
	//VulkanHelpers::BeginCommandBuffer(commandBuffer);
	//VulkanHelpers::CopyDataBetweenBuffers(commandBuffer, _stagingBuffer, 0, _buffer, 0, size);
	//VulkanHelpers::EndCommandBuffer(commandBuffer);
	//const auto submitInfo = &VulkanHelpers::MakeSubmitInfo(1, &commandBuffer);
	//VulkanHelpers::QueueSubmit(_queue, 1, submitInfo);
	//vkQueueWaitIdle(_queue);
	//vkFreeCommandBuffers(_device, _cmdPool, 1, &commandBuffer);


}

void VulkanConstantBuffer::bind(Material *m)
{

}

uint32_t VulkanConstantBuffer::GetLocation() const
{
	return _location;
}

VkBuffer VulkanConstantBuffer::GetBuffer() const
{
	return _buffer;
}
