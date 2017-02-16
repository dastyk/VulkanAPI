#pragma once
#include "Sampler2D.h"
#include <vulkan\vulkan.h>
class VulkanSampler2D : public Sampler2D
{
public:
	VulkanSampler2D(VkDevice device);
	virtual ~VulkanSampler2D();
	virtual void setMagFilter(FILTER filter);
	virtual void setMinFilter(FILTER filter);
	virtual void setWrap(WRAPPING s, WRAPPING t);
private:

	VkSampler _sampler;
	VkDevice _device;

	VkSamplerCreateInfo _createInfo;

};

