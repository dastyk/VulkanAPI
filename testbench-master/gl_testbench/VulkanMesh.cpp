#include "VulkanMesh.h"
#include "VulkanHelpers.h"


VulkanMesh::VulkanMesh() : _set(VK_NULL_HANDLE)
{
}


VulkanMesh::~VulkanMesh()
{
}

const void VulkanMesh::CreateDescriptor(VkDevice device, VkDescriptorPool pool)
{
	if (_set == VK_NULL_HANDLE)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings(geometryBuffers.size());
		for (auto& buffer : geometryBuffers)
		{
			VkDescriptorSetLayoutBinding binding =
			{
				buffer.first,
				VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			};

			
		}

		VulkanHelpers::CreateDescriptorSetLayout(device, &_setLayout, bindings.size(), bindings.data());


		VulkanHelpers::CreatePipelineLayout(device, &_pipelineLayout, 1, &_setLayout);


		VulkanHelpers::AllocateDescriptorSets(device, pool, 1, &_setLayout, &_set);


	}

}
