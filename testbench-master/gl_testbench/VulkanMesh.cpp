#include "VulkanMesh.h"
#include "VulkanHelpers.h"
#include "VulkanVertexBuffer.h"
#include "VulkanConstantBuffer.h"

VulkanMesh::VulkanMesh() : _set(VK_NULL_HANDLE)
{
}


VulkanMesh::~VulkanMesh()
{
}

const void VulkanMesh::CreateDescriptor(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout)
{
	if (_set == VK_NULL_HANDLE)
	{
		/*Allocate a descritpor set*/
		VulkanHelpers::AllocateDescriptorSets(device, pool, 1, &layout, &_set);

		/*Create a write descriptor set for each vertex buffer*/
		std::vector<VkWriteDescriptorSet> WriteDS;
		for (auto& buffer : geometryBuffers)
		{
			auto view = ((VulkanVertexBuffer*)buffer.second.buffer)->GetBufferView();
			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_set, buffer.first, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, nullptr, nullptr, &view));

		}
		/*Create a write descriptor set for the constant buffer*/
		VkDescriptorBufferInfo cbInfo;
		cbInfo.buffer = static_cast<VulkanConstantBuffer*>(txBuffer)->GetBuffer();
		cbInfo.offset = 0;
		cbInfo.range = VK_WHOLE_SIZE;
		
		WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_set, static_cast<VulkanConstantBuffer*>(txBuffer)->GetLocation(), 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &cbInfo, nullptr));
		
		
		/*Update the descriptor set with the binding data*/
		vkUpdateDescriptorSets(device, WriteDS.size(), WriteDS.data(), 0, nullptr);

		

	}

}
