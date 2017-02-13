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
	VkDescriptorSet getDescriptorSet(void) const { return _set; }

private:
	VkDescriptorSet _set;
};

