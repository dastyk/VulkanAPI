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
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);



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

	VkResult result = vkCreateInstance(&vkInstCreateInfo, nullptr, &_vkInstance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance.");
	}


	DebugUtils::DebugConsole::Command_Structure enumerateCommand =
	{
		this,
		&Enumerate,
		[](void* userData, int argc, char** argv) { printf("Enumerates vulkan info, first arg is what to enumerate.");}
	};

	DebugUtils::ConsoleThread::AddCommand("Enumerate", &enumerateCommand);


	return 0;
}

int VulkanRenderer::shutdown()
{
	SDL_Quit();
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

void VulkanRenderer::Enumerate(void * userData, int argc, char ** argv)
{

	VkResult result = VK_SUCCESS;
	VulkanRenderer* me = static_cast<VulkanRenderer*>(userData);


	if (argc <= 1)
		return;


	std::string arg = argv[1];

	if (arg == "PhysicalDevice")
	{
		auto& pd = me->EnumeratePhysicalDevices();
		printf("%zd Physical Devices found\n", pd.size());

		for (auto d : pd)
		{
			auto& prop = me->GetPhysicalDeviceProperties(d);

			printf("\tDevice Name: %s\n", prop.deviceName);
			printf("\tDevice Driver Version: %d\n", prop.driverVersion);
		}
	}


	

}

std::vector<VkPhysicalDevice> VulkanRenderer::EnumeratePhysicalDevices()
{
	std::vector<VkPhysicalDevice> physicalDevices;
	// First figure out how many devices are in the system.
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, nullptr);


	// Size the device array appropriately and get the physical
	// device handles.
	physicalDevices.resize(physicalDeviceCount);
	vkEnumeratePhysicalDevices(_vkInstance,
		&physicalDeviceCount,
		&physicalDevices[0]);

	return physicalDevices;
}

VkPhysicalDeviceProperties VulkanRenderer::GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceProperties properties;

	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	return properties;
}
