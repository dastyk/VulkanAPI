#pragma once

#include <vulkan\vulkan.h>
#include "Technique.h"

class VulkanTechnique : public Technique
{
public:
	VulkanTechnique(VkDevice device, Material* material, RenderState* renderState);
	~VulkanTechnique();

	void make(VkPipelineLayout pipelineLayout, VkRenderPass renderPass);

private:
	VkDevice _device = VK_NULL_HANDLE;
	VkPipeline _pipeline = VK_NULL_HANDLE;
};
