#include "VulkanConstantBuffer.h"
#include "VulkanHelpers.h"



VulkanConstantBuffer::VulkanConstantBuffer(std::string NAME, unsigned int location, VkPhysicalDevice phyDevice, VkDevice device, VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer cmdBuffer) : 
	_name(NAME), _location(location),
	_physicalDevice(phyDevice), _device(device),
	_queue(queue),
	_cmdPool(cmdPool), _cmdBuffer(cmdBuffer),
	_first(true)
{
}

VulkanConstantBuffer::~VulkanConstantBuffer()
{
}

void VulkanConstantBuffer::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	if (_first)
	{

		/*Create the staging buffer*/
		VulkanHelpers::CreateBuffer(
			_physicalDevice,
			_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&_stagingBuffer, &_stagingBufferMemory);

		/*Create the buffer*/
		VulkanHelpers::CreateBuffer(
			_physicalDevice,
			_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&_buffer, &_memory);
	}

	/*Copy the data to staging*/
	void* pData;
	VulkanHelpers::MapMemory(_device, _stagingBufferMemory, &pData);
	memcpy(pData, data, size);
	vkUnmapMemory(_device, _stagingBufferMemory);


	/*Create a new temporary cmdbuffer, */
	const auto allocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(_cmdPool);
	VkCommandBuffer commandBuffer;
	VulkanHelpers::AllocateCommandBuffers(_device, allocInfo, &commandBuffer);

	/*Copy the data between buffers*/
	VulkanHelpers::BeginCommandBuffer(commandBuffer);
	VulkanHelpers::CopyDataBetweenBuffers(commandBuffer, _stagingBuffer, 0, _buffer, 0, size);
	VulkanHelpers::EndCommandBuffer(commandBuffer);
	const auto submitInfo = &VulkanHelpers::MakeSubmitInfo(1, &commandBuffer);
	VulkanHelpers::QueueSubmit(_queue, 1, submitInfo);
	vkQueueWaitIdle(_queue);
	vkFreeCommandBuffers(_device, _cmdPool, 1, &commandBuffer);


}

void VulkanConstantBuffer::bind(Material *m)
{

}
