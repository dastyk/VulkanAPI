#include "VulkanMesh.h"
#include "VulkanHelpers.h"
#include "VulkanVertexBuffer.h"
#include "VulkanConstantBuffer.h"
#include "VulkanMaterial.h"
#include "VulkanTexture2D.h"
#include "VulkanSampler2D.h"

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


		if (textures.size() > 0)
		{
			for (auto& t : textures)
			{
				static_cast<VulkanSampler2D*>(t.second->sampler)->Make();
			}
			_sets.resize(2);
		}
		else
			_sets.resize(1);
		/*Allocate a descritpor set*/
		VulkanHelpers::AllocateDescriptorSets(device, poolBuff, 1, &layoutBuff, &_sets[0]);

		/*Create a write descriptor set for each vertex buffer*/
		std::vector<VkDescriptorBufferInfo> dView;
		dView.reserve(100000);
		std::vector<VkWriteDescriptorSet> WriteDS;
		for (auto& buffer : geometryBuffers)
		{
			dView.push_back(
			{
				static_cast<VulkanVertexBuffer*>(buffer.second.buffer)->GetBuffer(),
				0,
				static_cast<VulkanVertexBuffer*>(buffer.second.buffer)->getSize()
			});
			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[0], buffer.first, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &dView[dView.size() - 1], nullptr));

		}

		/*Create a write descriptor set for the constant buffer*/
		VkDescriptorBufferInfo dbInfo;
		dbInfo.buffer = static_cast<VulkanConstantBuffer*>(txBuffer)->GetBuffer();
		dbInfo.offset = 0;
		dbInfo.range = static_cast<VulkanConstantBuffer*>(txBuffer)->GetSize();

		WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[0], static_cast<VulkanConstantBuffer*>(txBuffer)->GetLocation(), 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &dbInfo, nullptr));
		
		auto cBuffers = static_cast<VulkanMaterial*>(this->technique->getMaterial())->getConstantBuffers();
		std::vector<VkDescriptorBufferInfo> dbIV;

		dbIV.reserve(cBuffers.size());
		for (auto b : cBuffers)
		{
			dbIV.push_back({
				b.second->GetBuffer(),
				0,
				b.second->GetSize()
			});

			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[0], b.first, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &dbIV[dbIV.size()-1], nullptr));
		}
		
		/* Create a descritpor set for the texture/sampler*/
		if(textures.size() > 0)
			VulkanHelpers::AllocateDescriptorSets(device, poolTex, 1, &layoutTex, &_sets[1]);

		
		std::vector<VkDescriptorImageInfo> diI;
		diI.reserve(10);
		for (auto t : textures)
		{
			diI.push_back({
				VK_NULL_HANDLE,
				((VulkanTexture2D*)t.second)->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			});
			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[1], DIFFUSE_SLOT, 0, 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, &diI[diI.size()-1], nullptr, nullptr));
			
			diI.push_back({
				((VulkanTexture2D*)t.second)->GetSampler(),
				VK_NULL_HANDLE,
				VK_IMAGE_LAYOUT_UNDEFINED
			});

			WriteDS.push_back(VulkanHelpers::MakeWriteDescriptorSet(_sets[1], DIFFUSE_SLOT + 1, 0, 1, VK_DESCRIPTOR_TYPE_SAMPLER, &diI[diI.size()-1], nullptr, nullptr));
		}





		/*Update the descriptor set with the binding data*/
		vkUpdateDescriptorSets(device, WriteDS.size(), WriteDS.data(), 0, nullptr);



	}

}
