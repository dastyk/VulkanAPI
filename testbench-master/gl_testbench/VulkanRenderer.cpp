#include "VulkanRenderer.h"



VulkanRenderer::VulkanRenderer()
{
}


VulkanRenderer::~VulkanRenderer()
{
}

Material * VulkanRenderer::makeMaterial()
{
	return nullptr;
}

Mesh * VulkanRenderer::makeMesh()
{
	return nullptr;
}

VertexBuffer * VulkanRenderer::makeVertexBuffer()
{
	return nullptr;
}

ConstantBuffer * VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
	return nullptr;
}

ResourceBinding * VulkanRenderer::makeResourceBinding()
{
	return nullptr;
}

RenderState * VulkanRenderer::makeRenderState()
{
	return nullptr;
}

Texture2D * VulkanRenderer::makeTexture2D()
{
	return nullptr;
}

Sampler2D * VulkanRenderer::makeSampler2D()
{
	return nullptr;
}

std::string VulkanRenderer::getShaderPath()
{
	return std::string();
}

std::string VulkanRenderer::getShaderExtension()
{
	return std::string();
}

int VulkanRenderer::initialize(unsigned int width, unsigned int height)
{
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_core_validation"
	};

	VkApplicationInfo vkAppInfo = 
	{
	VK_STRUCTURE_TYPE_APPLICATION_INFO,	// sType;
	nullptr,// pNext;
	"Vulkan Renderer",// Application name
	VK_MAKE_VERSION(1,0,0),// applicationVersion;
	"Frengine",// pEngineName;
	VK_MAKE_VERSION(1,0,0),// engineVersion;
	VK_API_VERSION_1_0// apiVersion;
	};
	VkInstanceCreateInfo vkInstCreateInfo = 
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,//sType
		nullptr,//pNext
		0,//flags	
		&vkAppInfo,//pApplicationInfo
		validationLayers.size(),//enabledLayerCount
		validationLayers.data(),//ppEnabledLayerNames
		0,// enabledExtCount
		nullptr// ppEnabledExtNames
	};

	VkResult result = vkCreateInstance(&vkInstCreateInfo, nullptr, _vkInstance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance.");
	}


	DebugUtils::DebugConsole::Command_Structure =
	{
		this,
		nullptr,
		nullptr
	};

	DebugUtils::ConsoleThread::AddCommand("Enumerate", )


	return 0;
}

int VulkanRenderer::shutdown()
{
	return 0;
}

void VulkanRenderer::setClearColor(float, float, float, float)
{
}

void VulkanRenderer::clearBuffer(unsigned int)
{
}

void VulkanRenderer::setRenderTarget(RenderTarget * rt)
{
}

void VulkanRenderer::setRenderState(RenderState * ps)
{
}

void VulkanRenderer::submit(Mesh * mesh)
{
}

void VulkanRenderer::frame()
{
}

void VulkanRenderer::present()
{
}
