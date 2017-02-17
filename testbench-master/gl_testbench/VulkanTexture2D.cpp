#include "VulkanTexture2D.h"
#include <vulkan\vulkan.h>
#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb_image.h>





VulkanTexture2D::VulkanTexture2D(VkDevice & device, VkPhysicalDevice & physDevice, VkCommandBuffer & transferCommandBuffer) : _device(&device), _physDevice(&physDevice), _cmdBuffer(&transferCommandBuffer)
{
}

VulkanTexture2D::~VulkanTexture2D()
{
}

int VulkanTexture2D::loadFromFile(std::string filename)
{
	//Load pixel data from image
	int tWidth, tHeight, tChannels;
	stbi_uc* imageData = stbi_load(filename.c_str(), &tWidth, &tHeight, &tChannels, STBI_rgb_alpha);
	
	if (!imageData)
		throw std::runtime_error(std::string("Failed to load texture: ").append(filename).c_str());
	
	VkDeviceSize tSize = tWidth * tHeight * 4;

	VkImage staging;
	VkDeviceMemory stagingMem;

	_createImage(tWidth, tHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging, stagingMem);

	void* data;
	vkMapMemory(*_device, stagingMem, 0, tSize, 0, &data);

	VkImageSubresource subRes = {};
	subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRes.mipLevel = 0;
	subRes.arrayLayer = 0;

	VkSubresourceLayout stagingLayout;
	vkGetImageSubresourceLayout(*_device, staging, &subRes, &stagingLayout);

	if (stagingLayout.rowPitch == tWidth * 4)
	{
		memcpy(data, imageData, tSize);
	}
	else
	{
		//Deal with padding issues, copy row by row
		uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
		for (int row = 0; row < tHeight; row++)
		{
			memcpy(&bytes[row * stagingLayout.rowPitch], &imageData[row * tWidth * 4], tWidth * 4);
		}
	}

	vkUnmapMemory(*_device, stagingMem);

	stbi_image_free(imageData);

	_createImage(tWidth, tHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _texture, _textureMemory);

	_transitionLayout(staging, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	_transitionLayout(_texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	_copyImage(staging, _texture, tWidth, tHeight);

	_transitionLayout(_texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	


	return 0;
}

void VulkanTexture2D::bind(unsigned int slot)
{
}
#include "VulkanSampler2D.h"
VkSampler VulkanTexture2D::GetSampler() const
{
	return ((VulkanSampler2D*)sampler)->GetSampler();
}

void VulkanTexture2D::_createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory)
{

	VkImageCreateInfo imgInfo = {};
	imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgInfo.imageType = VK_IMAGE_TYPE_2D;
	imgInfo.extent.width = width;
	imgInfo.extent.height = height;
	imgInfo.extent.depth = 1;
	imgInfo.mipLevels = 1;
	imgInfo.arrayLayers = 1;
	imgInfo.format = format;
	imgInfo.tiling = tiling;
	imgInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imgInfo.usage = usage;
	imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imgInfo.flags = 0;

	if (vkCreateImage(*_device, &imgInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image");

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(*_device, image, &memReq);

	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(*_physDevice, &memProp);
	uint32_t memTypeIndex = 0;
	for (uint32_t i = 0; i < memProp.memoryTypeCount; i++)
	{
		if ((memReq.memoryTypeBits & (1 << i)) && (memProp.memoryTypes[i].propertyFlags & (properties)))
		{
			memTypeIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo memInfo = {};
	memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memInfo.allocationSize = memReq.size;
	memInfo.memoryTypeIndex = memTypeIndex;

	if (vkAllocateMemory(*_device, &memInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocated memory for image");

	vkBindImageMemory(*_device, image, imageMemory, 0);

}

void VulkanTexture2D::_transitionLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(*_cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void VulkanTexture2D::_copyImage(VkImage src, VkImage dst, uint32_t width, uint32_t height)
{
	VkImageSubresourceLayers subRes = {};
	subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRes.baseArrayLayer = 0;
	subRes.mipLevel = 0;
	subRes.layerCount = 1;

	VkImageCopy region = {};
	region.srcSubresource = subRes;
	region.dstSubresource = subRes;
	region.srcOffset = { 0,0,0 };
	region.dstOffset = { 0,0,0 };
	region.extent.width = width;
	region.extent.height = height;
	region.extent.depth = 1;
	
	vkCmdCopyImage(*_cmdBuffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
