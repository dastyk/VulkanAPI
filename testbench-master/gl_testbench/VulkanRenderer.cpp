#include "VulkanRenderer.h"
#include <ConsoleThread.h>
#include <SDL_syswm.h>

#include "MaterialVk.h"

VulkanRenderer::VulkanRenderer()
{
}


VulkanRenderer::~VulkanRenderer()
{
}

Material * VulkanRenderer::makeMaterial()
{
	return new MaterialVk(_vkDevice);
}

Mesh * VulkanRenderer::makeMesh()
{
	return nullptr;
}

VertexBuffer * VulkanRenderer::makeVertexBuffer()
{
	return new VulkanVertexBuffer(_vkPhysicalDevices[0], _vkDevice, _vkMainQueue, _vkCmdPool, _vkCmdBuffer);
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
	// Create command for enumerating stuff
	DebugUtils::DebugConsole::Command_Structure enumerateCommand =
	{
		this,
		&Enumerate,
		&EnumerateHelp,
		"enum",
		"Prints various info about the hardware and vulkan."
	};

	DebugUtils::ConsoleThread::AddCommand(&enumerateCommand);


	// Create the window
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);





	/******** Create the instance***********/
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_core_validation"
	};

	const auto vkAppInfo = &VulkanHelpers::MakeApplicationInfo(
		"Vulkan Renderer",
		VK_MAKE_VERSION(1, 0, 0),
		"Frengine",
		VK_MAKE_VERSION(1, 0, 0)
	);
	const char* extensions[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
	const auto vkInstCreateInfo = VulkanHelpers::MakeInstanceCreateInfo(
		0,
		vkAppInfo,
		validationLayers.size(),
		validationLayers.data(),
		nullptr,
		2,
		extensions
	);


	VulkanHelpers::CreateInstance(&vkInstCreateInfo, &_vkInstance);





	/******** Create the device***********/
	_vkPhysicalDevices = VulkanHelpers::EnumeratePhysicalDevices(_vkInstance);

	if (_vkPhysicalDevices.size() <= 0) {
		throw std::runtime_error("No physical devices found.");
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	VkPhysicalDeviceFeatures requiredFeatures = {};

	vkGetPhysicalDeviceFeatures(_vkPhysicalDevices[0],
		&supportedFeatures);

	requiredFeatures.multiDrawIndirect = supportedFeatures.multiDrawIndirect;

	const auto deviceQueueCreateInfo = &VulkanHelpers::MakeDeviceQueueCreateInfo(0, 1);

	const auto deviceCreateInfo = &VulkanHelpers::MakeDeviceCreateInfo(
		1,
		deviceQueueCreateInfo
	);


	VulkanHelpers::CreateLogicDevice(_vkPhysicalDevices[0], deviceCreateInfo, &_vkDevice);





	/*******Fetch the main queue**********/
	vkGetDeviceQueue(_vkDevice, 0, 0, &_vkMainQueue);





	/***********Create command pool******************/
	const auto cmdPoolInfo = &VulkanHelpers::MakeCommandPoolCreateInfo(0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VulkanHelpers::CreateCommandPool(_vkDevice, cmdPoolInfo, &_vkCmdPool);





	/*********Allocate main command buffer************/
	const auto cmdBufferAllocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(
		_vkCmdPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	);
	VulkanHelpers::AllocateCommandBuffers(_vkDevice, cmdBufferAllocInfo, &_vkCmdBuffer);

	/**************** Set up window surface *******************/
	SDL_SysWMinfo wndInfo;
	SDL_VERSION(&wndInfo.version);
	SDL_GetWindowWMInfo(window, &wndInfo);
	HWND hwnd = wndInfo.info.win.window;

	TCHAR cname[256];
	GetClassName(hwnd, cname, 256);
	WNDCLASS wc;
	GetClassInfo(GetModuleHandle(NULL), cname, &wc);

	VkWin32SurfaceCreateInfoKHR wndCreateInfo;
	wndCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	wndCreateInfo.hwnd = hwnd;
	wndCreateInfo.hinstance = wc.hInstance;
	wndCreateInfo.flags = 0;
	wndCreateInfo.pNext = nullptr;
	auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(_vkInstance, "vkCreateWin32SurfaceKHR");

	if (!CreateWin32SurfaceKHR || vkCreateWin32SurfaceKHR(_vkInstance, &wndCreateInfo, nullptr, &_vkSurface) != VK_SUCCESS)
		throw std::runtime_error("Window surface creation failed.");
	

	return 0;
}

int VulkanRenderer::shutdown()
{
	//vkFreeCommandBuffers(_vkDevice, _vkCmdPool, 1, &_vkCmdBuffer); is freed when pool is destroyed
	vkDestroyCommandPool(_vkDevice, _vkCmdPool, nullptr);
	vkDestroyDevice(_vkDevice, nullptr);
	vkDestroyInstance(_vkInstance, nullptr);
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
		auto& pd = VulkanHelpers::EnumeratePhysicalDevices(me->_vkInstance);
		printf("%zd Physical Devices found\n", pd.size());

		for (auto d : pd)
		{
			auto& prop = VulkanHelpers::GetPhysicalDeviceProperties(d);

			VulkanHelpers::PrintPhysicalDeviceProperties(prop);

		}
	}
	else if (arg == "queueFam")
	{
		auto& pd = VulkanHelpers::EnumeratePhysicalDevices(me->_vkInstance);
		char* arg;
		if (DebugUtils::GetArg("-d", &arg, argc, argv))
		{
			
			int d = std::stoi(arg);
			if (pd.size() <= d)
			{
				printf("Invalid index\n");
				return;
			}
			
			
			
			auto& families = VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(pd[d]);
			if (DebugUtils::GetArg("-i", &arg, argc, argv))
			{
				int i = std::stoi(arg);
				if (families.size() <= i)
				{
					printf("Invalid index\n");
					return;
				}
				VulkanHelpers::PrintQueueFamilyProperties(families[i]);

			}
			else
			{
				
				printf("%zd families found\n", families.size());
				for (auto& fam : families)
				{
					VulkanHelpers::PrintQueueFamilyProperties(fam);
				}

			}


			
		}
		else
		{
			auto& pdfamprop = VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(me->_vkInstance);
			for (int d = 0; d < pdfamprop.size(); d++)
			{
				auto& prop = VulkanHelpers::GetPhysicalDeviceProperties(pd[d]);

				printf("%s: \n", prop.deviceName);
				if (DebugUtils::GetArg("-i", &arg, argc, argv))
				{
					int i = std::stoi(arg);
					if (pdfamprop[d].size() <= i)
					{
						printf("Invalid index\n");
						return;
					}
					VulkanHelpers::PrintQueueFamilyProperties(pdfamprop[d][i]);

				}
				else
				{

					printf("%zd families found\n", pdfamprop[d].size());
					for (auto& fam : pdfamprop[d])
					{
						VulkanHelpers::PrintQueueFamilyProperties(fam);

					}

				}
			}
		}
		
	}
	else if (arg == "memProp")
	{
		auto& pd = VulkanHelpers::EnumeratePhysicalDevices(me->_vkInstance);
		char* arg;
		if (DebugUtils::GetArg("-d", &arg, argc, argv))
		{

			int d = std::stoi(arg);
			if (pd.size() <= d)
			{
				printf("Invalid index\n");
				return;
			}

			auto& prop = VulkanHelpers::GetPhysicalDeviceMemoryProperties(pd[d]);

			VulkanHelpers::PrintPhysicalDeviceMemoryProperties(prop);

		}
		else
		{
			for (auto& d : pd)
			{
				auto& devInfo = VulkanHelpers::GetPhysicalDeviceProperties(d);
				printf("%s\n", devInfo.deviceName);
				
				auto& prop = VulkanHelpers::GetPhysicalDeviceMemoryProperties(d);

				VulkanHelpers::PrintPhysicalDeviceMemoryProperties(prop);
			}
		}
	}
	else if (arg == "devFeat")
	{
		/*char* arg;
		auto& pd = me->EnumeratePhysicalDevices();
		if (DebugUtils::GetArg("-d", &arg, argc, argv))
		{
			
			int d = std::stoi(arg);
			if (pd.size() <= d) {
				printf("Invalid index.");
				return;
			}

			auto& feat = me->GetPhysicalDeviceFeatures(pd[d]);

		}*/
	}

}

void VulkanRenderer::EnumerateHelp(void * userData, int argc, char ** argv)
{
	printf("Enumerates vulkan info, first arg is what to enumerate.\n");
	printf("Thing to enumerate: \n");
	printf("\t phyDev\n");
	printf("\t queueFam (-d physicalDeviceIndex, -i familyIndex)\n");
	printf("\t memProp (-d physicalDeviceIndex)\n ");
}
