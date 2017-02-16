#pragma once
#include "Mesh.h"
#include <vulkan\vulkan.h>
class VulkanMesh :
	public Mesh
{
public:
	VulkanMesh();
	~VulkanMesh();



	const void CreateDescriptor(VkDevice device, VkDescriptorPool poolBuff, VkDescriptorSetLayout layoutBuff, VkDescriptorPool poolTex, VkDescriptorSetLayout layoutTex);
	const VkDescriptorSet* getDescriptorSet(uint32_t& count) const { count = _sets.size(); return _sets.data(); }

private:
	std::vector<VkDescriptorSet> _sets;
};

