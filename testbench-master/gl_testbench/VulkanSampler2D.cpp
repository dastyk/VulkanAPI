#include "VulkanSampler2D.h"
#include "VulkanHelpers.h"

VulkanSampler2D::VulkanSampler2D(VkDevice device) : _device(device)
{
	_createInfo = VulkanHelpers::MakeSamplerCreateInfo(
		VK_FILTER_LINEAR, VK_FILTER_LINEAR,
		VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VK_FALSE,
		0.0f,
		VK_FALSE,
		VK_COMPARE_OP_NEVER,
		0.0f, 0.0f,
		VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
		VK_FALSE);
}

VulkanSampler2D::~VulkanSampler2D()
{
}

void VulkanSampler2D::setMagFilter(FILTER filter)
{
	//switch (filter)
	//{
	//case POINT_SAMPLER:
	//	break;
	//case LINEAR:
	//	_createInfo.magFilter = VK_FILTER_LINEAR;
	//	break;
	//default:
	//	break;
	//}
}

void VulkanSampler2D::setMinFilter(FILTER filter)
{
}

void VulkanSampler2D::setWrap(WRAPPING s, WRAPPING t)
{
	switch (s)
	{
	case REPEAT:
		_createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case CLAMP:
		_createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	default:
		break;
	}

	switch (t)
	{
	case REPEAT:
		_createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case CLAMP:
		_createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	default:
		break;
	}
}
