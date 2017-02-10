#pragma once
#include "Material.h"
#include "ConstantBufferVk.h"
#include <vulkan\vulkan.h>

class VulkanMaterial : public Material
{
public:
	VulkanMaterial(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
	~VulkanMaterial();

	// Inherited
	void setShader(const std::string& shaderFileName, ShaderType type);
	void removeShader(ShaderType type);
	void setDiffuse(Color c);
	int compileMaterial(std::string& errString);
	void addConstantBuffer(std::string name, unsigned int location);
	void updateConstantBuffer(const void* data, size_t size, unsigned int location);
	int enable();
	void disable();

private:
	bool _compileShader(ShaderType type);

private:
	// Device that created the shaders
	VkDevice _device = VK_NULL_HANDLE;

	// Shader modules corresponding to various shader stages
	VkShaderModule _shaderObjects[4] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };

	std::map<unsigned int, ConstantBufferVk*> constantBuffers;

	VkPipeline _pipeline = VK_NULL_HANDLE;

	// Provided by renderer, same for all materials
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
};