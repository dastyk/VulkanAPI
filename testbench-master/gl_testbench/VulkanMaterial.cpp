#include "VulkanMaterial.h"

#include "VulkanHelpers.h"
#include <array>
#include <assert.h>
#include <fstream>
#include <Windows.h>

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

VkShaderModule VulkanMaterial::getShaderModule(ShaderType type)
{
	return _shaderObjects[static_cast<uint32_t>(type)];
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
	VkShaderModule shader = _shaderObjects[static_cast<uint32_t>(type)];

	// check if name exists (if it doesn't there should not be a shader here.
	if (shaderFileNames.find(type) == shaderFileNames.end())
	{
		assert(shader == 0);
		return;
	}
	else if (shader != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(_device, shader, nullptr);
		_shaderObjects[static_cast<uint32_t>(type)] = VK_NULL_HANDLE;
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
	if (!_compileShader(ShaderType::VS))
	{
		exit(-1);
		//return -1;
	}
	if (!_compileShader(ShaderType::PS))
	{
		exit(-1);
		//return -1;
	}

	return 0;
}

// TODO: not really sure how this should map to Vulkan
void VulkanMaterial::addConstantBuffer(std::string name, unsigned int location)
{
	constantBuffers[location] = new VulkanConstantBuffer(name, location, _createBufferCallback, _updateBufferCallback);
}

void VulkanMaterial::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
	constantBuffers[location]->setData(data, size, this, location);
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

bool VulkanMaterial::_compileShader(ShaderType type)
{
	// Open the file and read to string
	ifstream file(shaderFileNames[type]);
	string shaderText;
	if (file)
	{
		shaderText = string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
		file.close();
	}
	else
	{
		throw runtime_error("Can't find file: " + shaderFileNames[type]);
	}

	// Final vector of string
	string finalShader = _expandShaderText(shaderText, type);

	// Call glslangvalidator to compile the shader code

	// Extension of shader file for glslang to deduce shader type
	wstring glslangStage = L"";
	if (type == ShaderType::VS)
		glslangStage = L"vert";
	else if (type == ShaderType::PS)
		glslangStage = L"frag";

	// Dump shader code in a temporary file
	ofstream tempstorage(L"temp.glsl");
	if (!tempstorage)
	{
		throw runtime_error("Failed to create temporary shader file!");
	}
	tempstorage << finalShader << endl;
	tempstorage.close();
	wstring glslangcommand = L"..\\assets\\Vulkan\\glslangvalidator -V -S " + glslangStage + L" temp.glsl -o temp.spv";

	// Create a process that runs glslang
	// https://msdn.microsoft.com/en-us/library/ms682512(v=vs.85).aspx
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	memset(&processInfo, 0, sizeof(processInfo));
	CreateProcess(NULL, const_cast<wchar_t*>(glslangcommand.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);

	// Wait for glslangvalidator to finish
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	// Close process and thread handles
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	if (exitCode != 0)
	{
		throw runtime_error("Failed to compile GLSL code (I think)");
	}

	// With shader compiled it's time to create a shader module from SPIR-V

	file.open("temp.spv", ios::binary | ios::ate);
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

	VkResult result = vkCreateShaderModule(_device, &shaderInfo, nullptr, &_shaderObjects[static_cast<uint32_t>(type)]);
	if (result != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

string VulkanMaterial::_expandShaderText(const string& shaderSource, ShaderType type)
{
	string result("#version 450\n");
	for (auto& define : shaderDefines[type])
	{
		result += define;
		printf("%s\n", define.c_str());
	}
	result += shaderSource;

	return result;
}
