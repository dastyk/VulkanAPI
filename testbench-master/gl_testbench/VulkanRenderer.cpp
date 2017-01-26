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

	// Create the window
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

	// Create the instance
	VkResult result = vkCreateInstance(&vkInstCreateInfo, nullptr, &_vkInstance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance.");
	}

	// Create command for enumerating stuff
	DebugUtils::DebugConsole::Command_Structure enumerateCommand =
	{
		this,
		&Enumerate,
		&EnumerateHelp
	};

	DebugUtils::ConsoleThread::AddCommand("enum", &enumerateCommand);





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


	if (argc <= 1) {
		printf("enum requires an argument\n");

		return;
	}


	std::string arg = argv[1];

	if (arg == "phyDev")
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
	else if (arg == "queueFam")
	{
		auto& pd = me->EnumeratePhysicalDevices();
		if (argc > 2)
		{
			
			int i = std::stoi(argv[2]);
			if (pd.size() <= i)
			{
				printf("Invalid index\n");
				return;
			}
			
			auto& families = me->EnumeratePhysicalDeviceQueueFamilyProperties(pd[i]);
			printf("%zd families found\n", families.size());
			for (auto& fam : families)
			{
				printf("\tQueue Flags: \n");
				if (fam.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					printf("\t\t VK_QUEUE_GRAPHICS_BIT\n");
				if (fam.queueFlags & VK_QUEUE_COMPUTE_BIT)
					printf("\t\t VK_QUEUE_COMPUTE_BIT\n");
				if (fam.queueFlags & VK_QUEUE_TRANSFER_BIT)
					printf("\t\t VK_QUEUE_TRANSFER_BIT\n");
				if (fam.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
					printf("\t\t VK_QUEUE_SPARSE_BINDING_BIT\n");

				printf("\tQueue Count: %d\n", fam.queueCount);

				printf("***********************************\n\n");
			}

			
		}
		else
		{
			auto& pdfamprop = me->EnumeratePhysicalDeviceQueueFamilyProperties();
			for (int i = 0; i < pdfamprop.size(); i++)
			{
				auto& prop = me->GetPhysicalDeviceProperties(pd[i]);

				printf("%s: \n", prop.deviceName);
				printf("%zd families found\n", pdfamprop[i].size());
				for (auto& fam : pdfamprop[i])
				{
					printf("\t\tQueue Flags: \n");
					if (fam.queueFlags & VK_QUEUE_GRAPHICS_BIT)
						printf("\t\t\t VK_QUEUE_GRAPHICS_BIT\n");
					if (fam.queueFlags & VK_QUEUE_COMPUTE_BIT)
						printf("\t\t\t VK_QUEUE_COMPUTE_BIT\n");
					if (fam.queueFlags & VK_QUEUE_TRANSFER_BIT)
						printf("\t\t\t VK_QUEUE_TRANSFER_BIT\n");
					if (fam.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
						printf("\t\t\t VK_QUEUE_SPARSE_BINDING_BIT\n");

					printf("\t\tQueue Count: %d\n", fam.queueCount);

					printf("***********************************\n\n");
				}
			}
		}
		
	}
	else if (arg == "memProp")
	{
		auto& pd = me->EnumeratePhysicalDevices();
		if (argc > 2)
		{

			int i = std::stoi(argv[2]);
			if (pd.size() <= i)
			{
				printf("Invalid index\n");
				return;
			}

			auto& prop = me->GetPhysicalDeviceMemoryProperties(pd[i]);
			auto& devprop = me->GetPhysicalDeviceProperties(pd[i]);

			printf("%s: \n", devprop.deviceName);

			printf("\tMemory Type Count: %zd\n", prop.memoryTypeCount);
			for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			{
				auto& memType = prop.memoryTypes[i];
				printf("\t\tHeap Index: %zd\n", memType.heapIndex);
				printf("\t\tPropertyFlags: \n");
				if (memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
					printf("\t\t\tVK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");

				if (memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
					printf("\t\t\tVK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");

				if (memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
					printf("\t\t\tVK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");

				if (memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
					printf("\t\t\tVK_MEMORY_PROPERTY_HOST_CACHED_BIT\n");

				if (memType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
					printf("\t\t\tVK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n");
			}

			printf("\tMemory Heap Count: %zd\n", prop.memoryHeapCount);

			
		}
		else
		{

		}
	}
	

}

void VulkanRenderer::EnumerateHelp(void * userData, int argc, char ** argv)
{
	printf("Enumerates vulkan info, first arg is what to enumerate.\n");
	printf("Thing to enumerate: \n");
	printf("\t phyDev\n");
	printf("\t queueFam\n");
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

VkPhysicalDeviceMemoryProperties VulkanRenderer::GetPhysicalDeviceMemoryProperties(VkPhysicalDevice phydev)
{
	VkPhysicalDeviceMemoryProperties prop;

	vkGetPhysicalDeviceMemoryProperties(phydev, &prop);

	return prop;
}

std::vector<VkQueueFamilyProperties> VulkanRenderer::EnumeratePhysicalDeviceQueueFamilyProperties(VkPhysicalDevice phydev)
{
	uint32_t queueFamilyPropertyCount;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	// First determine the number of queue families supported by the physical
	// device.
	vkGetPhysicalDeviceQueueFamilyProperties(phydev,
		&queueFamilyPropertyCount,
		nullptr);

	// Allocate enough space for the queue property structures.
	queueFamilyProperties.resize(queueFamilyPropertyCount);

	// Now query the actual properties of the queue families.
	vkGetPhysicalDeviceQueueFamilyProperties(phydev,
		&queueFamilyPropertyCount,
		queueFamilyProperties.data());

	return queueFamilyProperties;
}

std::vector<std::vector<VkQueueFamilyProperties>> VulkanRenderer::EnumeratePhysicalDeviceQueueFamilyProperties()
{
	std::vector<std::vector<VkQueueFamilyProperties>> propDev;

	auto& devices = EnumeratePhysicalDevices();

	for (auto& d : devices)
	{
		propDev.push_back(EnumeratePhysicalDeviceQueueFamilyProperties(d));
	}


	return propDev;
}

