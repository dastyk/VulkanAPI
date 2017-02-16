#include "VulkanMesh.h"
#include "VulkanHelpers.h"
#include "VulkanVertexBuffer.h"
#include "VulkanConstantBuffer.h"

VulkanMesh::VulkanMesh()
{
}


VulkanMesh::~VulkanMesh()
{
}

const void VulkanMesh::CreateDescriptor(VkDevice device, VkDescriptorPool poolBuff, VkDescriptorSetLayout layoutBuff, VkDescriptorPool poolTex, VkDescriptorSetLayout layoutTex)
{
	if (_sets.size() == 0)
	{
		_sets.resize(2);
		/*Allocate a descritpor set*/
		VulkanHelpers::AllocateDescriptorSets(device, poolBuff, 1, &layoutBuff, &_sets[0]);

		/*Create a write descriptor set for each vertex buffer*/
		std::vector<VkWriteDescriptorSet> WriteDS;
		for (auto& buffer : geometryBuffers)
		{
			auto view = ((VulkanVertexBuffer*)buffer.second.buffer)->GetBufferView();
			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[0], buffer.first, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, nullptr, nullptr, &view));

		}
		/*Create a write descriptor set for the constant buffer*/
		VkDescriptorBufferInfo dbInfo;
		dbInfo.buffer = static_cast<VulkanConstantBuffer*>(txBuffer)->GetBuffer();
		dbInfo.offset = 0;
		dbInfo.range = VK_WHOLE_SIZE;
		
		WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[0], static_cast<VulkanConstantBuffer*>(txBuffer)->GetLocation(), 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &dbInfo, nullptr));

		/* Create a descritpor set for the texture/sampler*/
		VulkanHelpers::AllocateDescriptorSets(device, poolTex, 1, &layoutTex, &_sets[1]);

		VkDescriptorImageInfo imageInfo;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//imageInfo.imageView = textureImageView;
		//imageInfo.sampler = textureSampler;

		//WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[1], DIFFUSE_SLOT, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo, nullptr, nullptr));


		/*Update the descriptor set with the binding data*/
		vkUpdateDescriptorSets(device, WriteDS.size(), WriteDS.data(), 0, nullptr);



	}

}
