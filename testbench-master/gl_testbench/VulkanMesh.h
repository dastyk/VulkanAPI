#pragma once
#include "Mesh.h"
#include <vulkan\vulkan.h>
class VulkanMesh :
	public Mesh
{
public:
	VulkanMesh();
	~VulkanMesh();



	const void CreateDescriptor(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout);

private:
	VkDescriptorSet _set;
	VkPipelineLayout _pipelineLayout;
};

