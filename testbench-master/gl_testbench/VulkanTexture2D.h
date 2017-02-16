#pragma once
#include "Texture2D.h"
#include "Sampler2D.h"
#include <vulkan\vulkan.h>

class VulkanTexture2D : public Texture2D
{
public:
	VulkanTexture2D(VkDevice & device, VkPhysicalDevice& physDevice);
	~VulkanTexture2D();

	// returns 0 if texture was loaded.
	virtual int loadFromFile(std::string filename);

	// bind texture to be used in the pipeline, binding to
	// slot "slot" in the active fragment shader.
	// slot can have different interpretation depending on the API.
	virtual void bind(unsigned int slot);

	// if no sampler is set here, a default sampler should be used.
	Sampler2D* sampler = nullptr;
private:
	VkDevice* _device;
	VkPhysicalDevice* _physDevice;

	VkImage _texture;
	VkDeviceMemory _textureMemory;

	void _createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
};

