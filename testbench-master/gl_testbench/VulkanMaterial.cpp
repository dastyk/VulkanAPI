#include "VulkanMaterial.h"

#include <assert.h>
#include <fstream>

using namespace std;


VulkanMaterial::VulkanMaterial(VkDevice device, const std::function<void(const void*data, size_t size, VkBuffer&buffer, StagingBuffer&stagingBuffer)>& createBufferCallback, std::function<void(const void*data, size_t size, VkBuffer&buffer, StagingBuffer&stagingBuffer)> updateBufferCallback)
	: _device(device),
	_createBufferCallback(createBufferCallback), _updateBufferCallback(updateBufferCallback) 
{
}

VulkanMaterial::~VulkanMaterial()
{
	for (auto& buffer : constantBuffers)
	{
		delete buffer.second;
		buffer.second = nullptr;
	}
}

void VulkanMaterial::setShader(const std::string& shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}

	shaderFileNames[type] = shaderFileName;
}

void VulkanMaterial::removeShader(ShaderType type)
{
	VkShaderModule shader = shaderObjects[static_cast<uint32_t>(type)];

	// check if name exists (if it doesn't there should not be a shader here.
	if (shaderFileNames.find(type) == shaderFileNames.end())
	{
		assert(shader == 0);
		return;
	}
	else if (shader != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(_device, shader, nullptr);
		shaderObjects[static_cast<uint32_t>(type)] = VK_NULL_HANDLE;
	}
}

void VulkanMaterial::setDiffuse(Color c)
{

}

int VulkanMaterial::compileMaterial(std::string& errString)
{
	// Remove pre-existing shaders
	removeShader(ShaderType::VS);
	removeShader(ShaderType::PS);

	// Create shader modules
	if (!_createShader(ShaderType::VS))
	{
		exit(-1);
		//return -1;
	}
	if (!_createShader(ShaderType::PS))
	{
		exit(-1);
		//return -1;
	}

	// In the provided GL implementation the next step would be to assemble
	// into a program, but since we will need to use the shaders to create a
	// pipeline we leave the modules as they are to use them later.

	return 0;
}

// TODO: not really sure how this should map to Vulkan
void VulkanMaterial::addConstantBuffer(std::string name, unsigned int location)
{
	constantBuffers[location] = new VulkanConstantBuffer(name, location, _createBufferCallback, _updateBufferCallback);
}

void VulkanMaterial::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
	//constantBuffers[location]->setData(data, size, this, location);
}

int VulkanMaterial::enable()
{
	// What!?
	// Vulkan is stateless, it's not like we enable a material by itself

	return 0;
}

void VulkanMaterial::disable()
{
	// Nothing to disable...
}

bool VulkanMaterial::_createShader(ShaderType type)
{
	ifstream file(shaderFileNames[type], ios::binary | ios::ate);
	if (!file)
	{
		return false;
	}

	streampos size = file.tellg();
	file.seekg(0);
	char* spirv = new char[size];

	file.read(spirv, size);
	file.close();

	VkShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pNext = nullptr;
	shaderInfo.flags = 0;
	shaderInfo.codeSize = size;
	shaderInfo.pCode = (uint32_t*)spirv;

	VkResult result = vkCreateShaderModule(_device, &shaderInfo, nullptr, &shaderObjects[static_cast<uint32_t>(type)]);
	if (result != VK_SUCCESS)
	{
		return false;
	}

	return true;
}
