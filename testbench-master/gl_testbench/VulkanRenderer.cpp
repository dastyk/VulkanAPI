#include "VulkanRenderer.h"
#include <ConsoleThread.h>
#include <array>
#include <SDL_syswm.h>
#include <algorithm>
#undef max
#undef min

#include "VulkanMaterial.h"

#define MB *1024*1024

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	printf("validation layer: %s\n", msg);
	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

using namespace std;

VulkanRenderer::VulkanRenderer() : _first(true)
{
}


VulkanRenderer::~VulkanRenderer()
{
}

Material * VulkanRenderer::makeMaterial()
{
	return new VulkanMaterial(_vkDevice, _pipelineLayout, _renderPass);
}

Mesh * VulkanRenderer::makeMesh()
{
	return nullptr;
}

VertexBuffer * VulkanRenderer::makeVertexBuffer()
{
	VulkanVertexBuffer* vBuffer = new VulkanVertexBuffer([this](const void* data, size_t size) -> VkDeviceSize {
	
		VkBuffer buffer;
		VkDeviceSize offset;
		_vertexBufferAllocator->Allocate(size, &buffer, &offset);
		printf("Offset: %d\n", offset);

		/*Create the staging buffer*/
		VulkanRenderer::StagingBuffer stagingBuffer;
		VulkanHelpers::CreateBuffer(
			_vkPhysicalDevices[0],
			_vkDevice,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer.buffer, &stagingBuffer.memory);

		/*Copy the data to staging*/
		void* pData;
		VulkanHelpers::MapMemory(_vkDevice, stagingBuffer.memory, &pData);
		memcpy(pData, data, size);
		vkUnmapMemory(_vkDevice, stagingBuffer.memory);
		_vertexStagingBuffers.push_back(stagingBuffer);
		if (!_first)
		{			
			VulkanHelpers::BeginCommandBuffer(_vkInitTransferCmdBuffer);
		}

		VulkanHelpers::CopyDataBetweenBuffers(_vkInitTransferCmdBuffer, stagingBuffer.buffer, 0, buffer, offset, size);

		_first = true;
	
		return offset;
	});

	return vBuffer;
}

ConstantBuffer * VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
	//return new VulkanConstantBuffer(name, location;
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
	std::array<const char*, 3> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
	const auto vkInstCreateInfo = VulkanHelpers::MakeInstanceCreateInfo(
		0,
		vkAppInfo,
		validationLayers.size(),
		validationLayers.data(),
		nullptr,
		extensions.size(),
		extensions.data()
	);


	VulkanHelpers::CreateInstance(&vkInstCreateInfo, &_vkInstance);


	/*Create debug callback*/
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	
	if (CreateDebugReportCallbackEXT(_vkInstance, &createInfo, nullptr, &_vkDebugCallback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}




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
	std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const auto deviceCreateInfo = &VulkanHelpers::MakeDeviceCreateInfo(
		1,
		deviceQueueCreateInfo,
		0,
		nullptr,
		nullptr,
		nullptr,
		deviceExtensions.size(),
		deviceExtensions.data(),
		0
	);

	// For validation purposes
	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_vkPhysicalDevices[0], &familyCount, nullptr);
	VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[familyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(_vkPhysicalDevices[0], &familyCount, familyProperties);
	delete[] familyProperties;

	VulkanHelpers::CreateLogicDevice(_vkPhysicalDevices[0], deviceCreateInfo, &_vkDevice);








	/*******Fetch the main queue**********/
	vkGetDeviceQueue(_vkDevice, 0, 0, &_vkMainQueue);





	/***********Create command pool******************/
	const auto cmdPoolInfo = &VulkanHelpers::MakeCommandPoolCreateInfo(0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VulkanHelpers::CreateCommandPool(_vkDevice, cmdPoolInfo, &_vkCmdPool);





	/*********Allocate main command buffer************/
	auto cmdBufferAllocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(
		_vkCmdPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	);
	VulkanHelpers::AllocateCommandBuffers(_vkDevice, cmdBufferAllocInfo, &_vkCmdBuffer);

	/*********Allocate Init transfer buffer***********/
	cmdBufferAllocInfo = &VulkanHelpers::MakeCommandBufferAllocateInfo(
		_vkCmdPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	);
	VulkanHelpers::AllocateCommandBuffers(_vkDevice, cmdBufferAllocInfo, &_vkInitTransferCmdBuffer);
	



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
	
	if (vkCreateWin32SurfaceKHR(_vkInstance, &wndCreateInfo, nullptr, &_vkSurface) != VK_SUCCESS)
		throw std::runtime_error("Window surface creation failed.");

	// For validation purposes
	VkBool32 surfaceSupported;
	vkGetPhysicalDeviceSurfaceSupportKHR(_vkPhysicalDevices[0], 0, _vkSurface, &surfaceSupported);
	if (surfaceSupported == VK_FALSE)
	{
		throw runtime_error("Surface is not supported for the physical device!");
	}

	
	/************** Set up swap chain ****************/
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> supportedFormats;
	std::vector<VkPresentModeKHR> supportedPresentModes;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_vkPhysicalDevices[0], _vkSurface, &capabilities);


	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_vkPhysicalDevices[0], _vkSurface, &formatCount, nullptr);
	supportedFormats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(_vkPhysicalDevices[0], _vkSurface, &formatCount, supportedFormats.data());

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_vkPhysicalDevices[0], _vkSurface, &presentModeCount, nullptr);
	supportedPresentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(_vkPhysicalDevices[0], _vkSurface, &presentModeCount, supportedPresentModes.data());

	VkSurfaceFormatKHR bestFormat;
	if (supportedFormats.size() == 1 && supportedFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		bestFormat = supportedFormats[0];
	}
	else
	{
		//Settle for first format unless something better comes along
		bestFormat = supportedFormats[0];
		for (const auto& i : supportedFormats)
		{
			if (i.format == VK_FORMAT_B8G8R8A8_UNORM && i.format == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				bestFormat = i;
				break;
			}
		}
	}
	_swapchainFormat = bestFormat.format;
	//Unless something better comes along, use the standard mode
	VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& i : supportedPresentModes)
	{
		if (i == VK_PRESENT_MODE_MAILBOX_KHR)
			bestPresentMode = i;
	}

	VkExtent2D bestExtent = { width, height };
	bestExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, width));
	bestExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, height));
	_swapchainExtent = bestExtent;

	uint32_t imageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapCreateInfo;
	ZeroMemory(&swapCreateInfo, sizeof(swapCreateInfo));
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = _vkSurface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = bestFormat.format;
	swapCreateInfo.imageColorSpace = bestFormat.colorSpace;
	swapCreateInfo.imageExtent = bestExtent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapCreateInfo.preTransform = capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = bestPresentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	//Assume graphics family is the same as present family
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapCreateInfo.queueFamilyIndexCount = 0;
	swapCreateInfo.pQueueFamilyIndices = nullptr;
	
	if (vkCreateSwapchainKHR(_vkDevice, &swapCreateInfo, nullptr, &_vkSwapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swapchain");

	uint32_t swapchainImageCount = 0;
	if (vkGetSwapchainImagesKHR(_vkDevice, _vkSwapChain, &swapchainImageCount, nullptr) != VK_SUCCESS)
	{
		throw runtime_error("Failed to get swapchain image count!");
	}
	_swapchainImages.resize(swapchainImageCount);
	if (vkGetSwapchainImagesKHR(_vkDevice, _vkSwapChain, &swapchainImageCount, _swapchainImages.data()) != VK_SUCCESS)
	{
		throw runtime_error("Failed to get swapchain images!");
	}

	_swapchainImageViews.resize(_swapchainImages.size());
	for (uint32_t i = 0; i < _swapchainImages.size(); ++i)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = nullptr;
		viewInfo.flags = 0;
		viewInfo.image = _swapchainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = _swapchainFormat;
		viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_vkDevice, &viewInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS)
			throw runtime_error("Failed to create swapchain image view!");
	}

	_createSemaphores();
	_createRenderPass();
	_createFramebuffers();

	DebugUtils::DebugConsole::Command_Structure bufferReqCmd =
	{
		this,
		[](void * userData, int argc, char ** argv) {
		VulkanRenderer* me = (VulkanRenderer*)userData;
		
		if (DebugUtils::GetArg("-b", nullptr, argc, argv))
		{
			if (DebugUtils::GetArg("-v", nullptr, argc, argv))
			{
				auto buff = me->makeVertexBuffer();
				char test[21233];
				buff->setData(test, 21233, VertexBuffer::DATA_USAGE::DONTCARE);
			}
		}
		

	},
		[](void * userData, int argc, char ** argv) {printf("Creates stuff (-b to create buffer, -v specifies vertex buffer, -c for constant buffer)\n");},
		"Create",
		"Creates stuff"

	};

	DebugUtils::ConsoleThread::AddCommand(&bufferReqCmd);


	/********************Allocate device memory************************/
	VkBuffer buff;
	const auto bInfo = &VulkanHelpers::MakeBufferCreateInfo(
		256 MB,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);


	VulkanHelpers::CreateBuffer(_vkDevice, bInfo, &buff);


	// Get memory requirments
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(_vkDevice, buff, &memReq);
	vkDestroyBuffer(_vkDevice, buff, nullptr);
	// Create the allocator
	_vertexBufferAllocator = new VulkanMemAllocator(_vkPhysicalDevices[0], _vkDevice, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	//_constantBufferAllocator = new VulkanMemAllocator(_vkPhysicalDevices[0], _vkDevice, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	/**********Start recording init buffers****************/
	VulkanHelpers::BeginCommandBuffer(_vkInitTransferCmdBuffer);

	return 0;
}

int VulkanRenderer::shutdown()
{
	vkDeviceWaitIdle(_vkDevice);

	if (_pipelineLayout)
	{
		vkDestroyPipelineLayout(_vkDevice, _pipelineLayout, nullptr);
		_pipelineLayout = VK_NULL_HANDLE;
	}
	//delete _constantBufferAllocator;
	_constantBufferAllocator = nullptr;
	//delete _vertexBufferAllocator;
	_vertexBufferAllocator = nullptr;
	for (auto& f : _framebuffers)
	{
		vkDestroyFramebuffer(_vkDevice, f, nullptr);
	}
	vkDestroyRenderPass(_vkDevice, _renderPass, nullptr);
	for (auto& view : _swapchainImageViews)
	{
		vkDestroyImageView(_vkDevice, view, nullptr);
	}
	vkDestroySemaphore(_vkDevice, _renderingComplete, nullptr);
	vkDestroySemaphore(_vkDevice, _swapchainImageAvailable, nullptr);
	//vkFreeCommandBuffers(_vkDevice, _vkCmdPool, 1, &_vkCmdBuffer); is freed when pool is destroyed
	vkDestroySwapchainKHR(_vkDevice, _vkSwapChain, nullptr);
	vkDestroyCommandPool(_vkDevice, _vkCmdPool, nullptr);
	vkDestroyDevice(_vkDevice, nullptr);
	vkDestroySurfaceKHR(_vkInstance, _vkSurface, nullptr);
	DestroyDebugReportCallbackEXT(_vkInstance, _vkDebugCallback, nullptr);
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
	drawList.push_back(mesh);
}

void VulkanRenderer::frame()
{
	if (_first)
	{
		_first = false;
		VulkanHelpers::EndCommandBuffer(_vkInitTransferCmdBuffer);


		const auto submitInfo = &VulkanHelpers::MakeSubmitInfo(1, &_vkInitTransferCmdBuffer);
		VulkanHelpers::QueueSubmit(_vkMainQueue, 1, submitInfo);
		for (auto& buffer : _vertexStagingBuffers)
		{
			vkFreeMemory(_vkDevice, buffer.memory, nullptr);
			vkDestroyBuffer(_vkDevice, buffer.buffer, nullptr);
		}
		_vertexStagingBuffers.clear();
	}

	// Note: this is a really bad way of synchronizing frames, but for the sake
	// of getting things running it'll suffice.
	vkQueueWaitIdle(_vkMainQueue);

	if (vkAcquireNextImageKHR(_vkDevice, _vkSwapChain, UINT64_MAX, _swapchainImageAvailable, VK_NULL_HANDLE, &_swapchainImageIndex) != VK_SUCCESS)
		throw runtime_error("Failed to acquire swapchain error or suboptimal");

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(_vkCmdBuffer, &beginInfo);

	array<VkClearValue, 1> clearValues = { { 0.7f, 0.2f, 0.3f} };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.renderPass = _renderPass;
	renderPassInfo.framebuffer = _framebuffers[_swapchainImageIndex];
	renderPassInfo.renderArea = { {0, 0}, _swapchainExtent };
	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(_vkCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	// Render here

	vkCmdEndRenderPass(_vkCmdBuffer);

	vkEndCommandBuffer(_vkCmdBuffer);

	const uint32_t waitSemaphoreCount = 1;
	array<VkSemaphore, waitSemaphoreCount> waitSemaphores = { _swapchainImageAvailable };
	array<VkPipelineStageFlags, waitSemaphoreCount> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = waitSemaphores.size();
	submit_info.pWaitSemaphores = waitSemaphores.data();
	submit_info.pWaitDstStageMask = waitStages.data();
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_vkCmdBuffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &_renderingComplete;

	vkQueueSubmit(_vkMainQueue, 1, &submit_info, VK_NULL_HANDLE);





	// Förslag:
	// Till en början kan vi dra igång en render pass och skita i descriptors och sånt
	// och bara rendera med en hårdkodad pipeline för att se om vi får ut saker på skärmen.
	// Efter det kan vi kanske successivt lägga till constant buffers och material?
}

void VulkanRenderer::present()
{
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = nullptr;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &_renderingComplete;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &_vkSwapChain;
	present_info.pImageIndices = &_swapchainImageIndex;
	present_info.pResults = nullptr;

	if (vkQueuePresentKHR(_vkMainQueue, &present_info) != VK_SUCCESS)
		throw runtime_error("Failed to present!");
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

void VulkanRenderer::_createSemaphores(void)
{
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	if (vkCreateSemaphore(_vkDevice, &semaphore_info, nullptr, &_swapchainImageAvailable) != VK_SUCCESS)
		throw runtime_error("Failed to create swapchain image available semaphore!");

	if (vkCreateSemaphore(_vkDevice, &semaphore_info, nullptr, &_renderingComplete) != VK_SUCCESS)
		throw runtime_error("Failed to create rendering complete semaphore!");
}

void VulkanRenderer::_createRenderPass(void)
{
	array<VkAttachmentDescription, 1> attachments = {};
	attachments[0].flags = 0;
	attachments[0].format = _swapchainFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachment = {};
	colorAttachment.attachment = 0;
	colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachment;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = nullptr;

	VkResult result = vkCreateRenderPass(_vkDevice, &renderPassInfo, nullptr, &_renderPass);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");
	}
}

void VulkanRenderer::_createFramebuffers(void)
{
	_framebuffers.resize(_swapchainImages.size());

	for (uint32_t i = 0; i < _swapchainImages.size(); ++i)
	{
		array<VkImageView, 1> attachments = { _swapchainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.flags = 0;
		framebufferInfo.renderPass = _renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = _swapchainExtent.width;
		framebufferInfo.height = _swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_vkDevice, &framebufferInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS)
			throw runtime_error("Failed to create framebuffer!");
	}
}
