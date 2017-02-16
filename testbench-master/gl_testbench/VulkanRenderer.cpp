#include "VulkanRenderer.h"
#include "VulkanTexture2D.h"
#include <ConsoleThread.h>
#include <array>
#include <SDL_syswm.h>
#include <algorithm>
#include <fstream>
#undef max
#undef min

#include "VulkanMaterial.h"
#include "VulkanRenderState.h"
#include "VulkanTechnique.h"

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
	_createBufferCallback = [this](const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer) {

		auto info = &VulkanHelpers::MakeBufferCreateInfo(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		VulkanHelpers::CreateBuffer(_vkDevice, info, &buffer);

		_constantBufferAllocator->AllocateBufferMemory(size, buffer);

		info = &VulkanHelpers::MakeBufferCreateInfo(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		VulkanHelpers::CreateBuffer(_vkDevice, info, &stagingBuffer.buffer);

		_constantBufferStagingAllocator->AllocateBufferMemory(size, stagingBuffer);

		/*Copy the data to staging*/
		void* pData;
		VulkanHelpers::MapMemory(_vkDevice, stagingBuffer.memory, &pData);
		memcpy((char*)pData + stagingBuffer.offset, data, size);
		vkUnmapMemory(_vkDevice, stagingBuffer.memory);



		VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffers[1], stagingBuffer.buffer, 0, buffer, 0, size);



	};
	
	_updateBufferCallback = [this](const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer) {

		/*Copy the data to staging*/
		void* pData;
		VulkanHelpers::MapMemory(_vkDevice, stagingBuffer.memory, &pData);
		memcpy((char*)pData + stagingBuffer.offset, data, size);
		vkUnmapMemory(_vkDevice, stagingBuffer.memory);

		VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffers[1], stagingBuffer.buffer, 0, buffer, 0, size);
	};
}


VulkanRenderer::~VulkanRenderer()
{
}

Material * VulkanRenderer::makeMaterial()
{
	return new VulkanMaterial(_vkDevice, _testPipelineLayout, _renderPass, _createBufferCallback, _updateBufferCallback);
}

Mesh * VulkanRenderer::makeMesh()
{
	return new VulkanMesh();
}

VertexBuffer * VulkanRenderer::makeVertexBuffer()
{
	VulkanVertexBuffer* vBuffer = new VulkanVertexBuffer([this](const void* data, size_t size, VkBuffer& buffer, VkBufferView& view) {

		const auto info = &VulkanHelpers::MakeBufferCreateInfo(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);

		VulkanHelpers::CreateBuffer(_vkDevice, info, &buffer);

		_vertexBufferAllocator->AllocateBufferMemory(size, buffer);


		/*Create a view for the buffer*/
		VulkanHelpers::CreateBufferView(_vkDevice, buffer, &view, VK_FORMAT_R32G32B32A32_SFLOAT);

		/*Create the staging buffer*/
		StagingBuffer stagingBuffer;
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
	

		VulkanHelpers::CopyDataBetweenBuffers(_cmdBuffers[1], stagingBuffer.buffer, 0, buffer, 0, size);



	});

	return vBuffer;
}

ConstantBuffer * VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
	return new VulkanConstantBuffer(NAME, location,_createBufferCallback, _updateBufferCallback);
}

ResourceBinding * VulkanRenderer::makeResourceBinding()
{
	return nullptr;
}

RenderState * VulkanRenderer::makeRenderState()
{
	return new VulkanRenderState();
}

Technique* VulkanRenderer::makeTechnique(Material* material, RenderState* renderState)
{
	return new VulkanTechnique(material, renderState);
}

Texture2D * VulkanRenderer::makeTexture2D()
{
	return (Texture2D*) new VulkanTexture2D(_vkDevice, _vkPhysicalDevices[0]);
}

Sampler2D * VulkanRenderer::makeSampler2D()
{
	return nullptr;
}

std::string VulkanRenderer::getShaderPath()
{
	return std::string("..\\assets\\Vulkan\\");
}

std::string VulkanRenderer::getShaderExtension()
{
	return std::string(".glsl");
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

	/*Enumerate the queue family properties*/
	auto famProps = VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(_vkPhysicalDevices[0]);



	VulkanHelpers::CreateLogicDevice(_vkPhysicalDevices[0], deviceCreateInfo, &_vkDevice);








	/*******Fetch the main queue**********/
	vkGetDeviceQueue(_vkDevice, 0, 0, &_vkMainQueue);





	/***********Create command pool******************/
	const auto cmdPoolInfo = &VulkanHelpers::MakeCommandPoolCreateInfo(0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VulkanHelpers::CreateCommandPool(_vkDevice, cmdPoolInfo, &_vkCmdPool);





	/*********Allocate two command buffers************/
	_cmdBuffers.resize(2);
	VulkanHelpers::AllocateCommandBuffers(_vkDevice, _cmdBuffers.data(), _vkCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, _cmdBuffers.size());



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
		if (!me->_first)
		{
			VulkanHelpers::BeginCommandBuffer(me->_cmdBuffers[1]);
		}
		if (DebugUtils::GetArg("-b", nullptr, argc, argv))
		{
			
			if (DebugUtils::GetArg("-v", nullptr, argc, argv))
			{
				auto buff = me->makeVertexBuffer();
				char test[21233];
				buff->setData(test, 21233, VertexBuffer::DATA_USAGE::DONTCARE);
			}
			if (DebugUtils::GetArg("-c", nullptr, argc, argv))
			{
				auto buffer = me->makeConstantBuffer("Get", 0);
				char test[12312];
				buffer->setData(test, 12312, nullptr, 0);
			}
			
		}
		if (DebugUtils::GetArg("-d", nullptr, argc, argv))
		{
			
			VulkanMesh mesh;
			auto buff = me->makeVertexBuffer();
			char test[21233];
			buff->setData(test, 21233, VertexBuffer::DATA_USAGE::DONTCARE);
			mesh.addIAVertexBufferBinding(buff, 0, 21233/(sizeof(float)*3), POSITION);
			mesh.txBuffer = me->makeConstantBuffer("Translation", 4);
			char test2[sizeof(float) * 4];
			mesh.txBuffer->setData(test2, sizeof(float)*4, nullptr, 0);

			//mesh.CreateDescriptor(me->_vkDevice, me->_vkDescriptorPool, );



		}
		me->_first = true;

	},
		[](void * userData, int argc, char ** argv) {
		printf("Creates stuff:\n \t-b to create buffer, -v specifies vertex buffer, -c for constant buffer\n");
		printf("\t-d to create descriptor set\n");
	},
		"Create",
		"Creates stuff"

	};

	DebugUtils::ConsoleThread::AddCommand(&bufferReqCmd);


	/********************Allocate device memory************************/
	VkBuffer buff;
	auto bInfo = &VulkanHelpers::MakeBufferCreateInfo(
		256 MB,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);


	VulkanHelpers::CreateBuffer(_vkDevice, bInfo, &buff);


	// Get memory requirments
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(_vkDevice, buff, &memReq);
	vkDestroyBuffer(_vkDevice, buff, nullptr);
	// Create the allocator
	_vertexBufferAllocator = new VulkanMemAllocator(_vkPhysicalDevices[0], _vkDevice, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	bInfo = &VulkanHelpers::MakeBufferCreateInfo(
		256 MB,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);


	VulkanHelpers::CreateBuffer(_vkDevice, bInfo, &buff);


	// Get memory requirments
	vkGetBufferMemoryRequirements(_vkDevice, buff, &memReq);
	vkDestroyBuffer(_vkDevice, buff, nullptr);


	_constantBufferAllocator = new VulkanMemAllocator(_vkPhysicalDevices[0], _vkDevice, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	bInfo = &VulkanHelpers::MakeBufferCreateInfo(
		256 MB,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT);


	VulkanHelpers::CreateBuffer(_vkDevice, bInfo, &buff);


	// Get memory requirments
	vkGetBufferMemoryRequirements(_vkDevice, buff, &memReq);
	vkDestroyBuffer(_vkDevice, buff, nullptr);
	_constantBufferStagingAllocator = new VulkanMemAllocator(_vkPhysicalDevices[0], _vkDevice, memReq, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	/**********Start recording init buffers****************/
	VulkanHelpers::BeginCommandBuffer(_cmdBuffers[1]);





	/*************Create descriptor pool**************/
	VkDescriptorPoolSize pSize[] = {
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 3 * 10000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 * 10000}
	};


	//{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 * 10000}
	VulkanHelpers::CreateDescriptorPool(_vkDevice, &_vkBufferDescriptorPool, 0, 10000, 2, pSize);

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	/*Create descriptor layoutbinding for each vertex buffer*/
	bindings.push_back({
		POSITION,
		VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});
	bindings.push_back({
		NORMAL,
		VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});
	bindings.push_back({
		TEXTCOORD,
		VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});
	/*Create layoutbinding for the constantbuffer*/
	bindings.push_back({
		TRANSLATION,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});

	

	/* Create the descriptor set layout*/
	VulkanHelpers::CreateDescriptorSetLayout(_vkDevice, &_bufferSetLayout, bindings.size(), bindings.data());

	/* Create a new descriptor pool/set layout for the texture/sampler*/
	VkDescriptorPoolSize pSize2[] = {
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 * 10000}
	};



	VulkanHelpers::CreateDescriptorPool(_vkDevice, &_vkTextureDescriptorPool, 0, 10000, 1, pSize2);

	bindings.clear();

	bindings.push_back({
		DIFFUSE_SLOT,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		1,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	});

	/* Create the descriptor set layout*/
	VulkanHelpers::CreateDescriptorSetLayout(_vkDevice, &_textureSetLayout, bindings.size(), bindings.data()); 

	_createTestPipeline();
	
	return 0;
}

int VulkanRenderer::shutdown()
{
	vkDeviceWaitIdle(_vkDevice);

	vkDestroyDescriptorPool(_vkDevice, _vkTextureDescriptorPool, nullptr);
	vkDestroyDescriptorPool(_vkDevice, _vkBufferDescriptorPool, nullptr);
	delete _constantBufferStagingAllocator;
	_constantBufferStagingAllocator = nullptr;
	delete _constantBufferAllocator;
	_constantBufferAllocator = nullptr;

	delete _vertexBufferAllocator;
	_vertexBufferAllocator = nullptr;

	if (_testPipeline)
	{
		vkDestroyPipeline(_vkDevice, _testPipeline, nullptr);
		_testPipeline = VK_NULL_HANDLE;
	}
	if (_testPipelineLayout)
	{
		vkDestroyPipelineLayout(_vkDevice, _testPipelineLayout, nullptr);
		_pipelineLayout = VK_NULL_HANDLE;
	}

	if (_pipelineLayout)
	{
		vkDestroyPipelineLayout(_vkDevice, _pipelineLayout, nullptr);
		_pipelineLayout = VK_NULL_HANDLE;
	}

	for (auto& f : _framebuffers)
	{
		vkDestroyFramebuffer(_vkDevice, f, nullptr);
	}
	vkDestroyRenderPass(_vkDevice, _renderPass, nullptr);
	for (auto& view : _swapchainImageViews)
	{
		vkDestroyImageView(_vkDevice, view, nullptr);
	}
	for (auto& i : _shaderModules)
	{
		vkDestroyShaderModule(_vkDevice, i.second, nullptr);
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
	auto vMesh = (VulkanMesh*)mesh;
	vMesh->CreateDescriptor(_vkDevice, _vkBufferDescriptorPool, _bufferSetLayout, _vkTextureDescriptorPool, _textureSetLayout);
	drawList.push_back(vMesh);
}

void VulkanRenderer::frame()
{
	_first = false;
	VulkanHelpers::EndCommandBuffer(_cmdBuffers[1]);


	const auto submitInfo = &VulkanHelpers::MakeSubmitInfo(1, &_cmdBuffers[1]);
	VulkanHelpers::QueueSubmit(_vkMainQueue, 1, submitInfo);
	vkQueueWaitIdle(_vkMainQueue);
	for (auto& buffer : _vertexStagingBuffers)
	{
		vkFreeMemory(_vkDevice, buffer.memory, nullptr);
		vkDestroyBuffer(_vkDevice, buffer.buffer, nullptr);
	}
	_vertexStagingBuffers.clear();

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

	vkBeginCommandBuffer(_cmdBuffers[0], &beginInfo);

	array<VkClearValue, 1> clearValues = { { 0.7f, 0.2f, 0.3f} };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.renderPass = _renderPass;
	renderPassInfo.framebuffer = _framebuffers[_swapchainImageIndex];
	renderPassInfo.renderArea = { {0, 0}, _swapchainExtent };
	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(_cmdBuffers[0], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(_cmdBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, _testPipeline);

	for (auto m : drawList)
	{
		uint32_t descriptorSetCount = 0;
		auto sets = m->getDescriptorSet(descriptorSetCount);
		vkCmdBindDescriptorSets(_cmdBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, _testPipelineLayout, 0, descriptorSetCount, sets, 0, nullptr);
		vkCmdDraw(_cmdBuffers[0], 3, 1, 0, 0);
	}

	vkCmdEndRenderPass(_cmdBuffers[0]);

	vkEndCommandBuffer(_cmdBuffers[0]);

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
	submit_info.pCommandBuffers = &_cmdBuffers[0];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &_renderingComplete;

	vkQueueSubmit(_vkMainQueue, 1, &submit_info, VK_NULL_HANDLE);

	drawList.clear();

	VulkanHelpers::BeginCommandBuffer(_cmdBuffers[1]);

	// F�rslag:
	// Till en b�rjan kan vi dra ig�ng en render pass och skita i descriptors och s�nt
	// och bara rendera med en h�rdkodad pipeline f�r att se om vi f�r ut saker p� sk�rmen.
	// Efter det kan vi kanske successivt l�gga till constant buffers och material?
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

void VulkanRenderer::_createTestPipeline()
{
	const std::string vertexFileName = "../assets/Vulkan/VertexShader.spv";
	const std::string fragmentFileName = "../assets/Vulkan/FragmentShader.spv";
	_createShaderModule(vertexFileName);
	_createShaderModule(fragmentFileName);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = _shaderModules[vertexFileName];
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = _shaderModules[fragmentFileName];
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Skipping inputAssemblyStateCreateInfo, attributeDescriptions and bindingDescriptions since this is not needed when we use vertex pulling (?)
	//Edit: oops we do need them
	VkPipelineVertexInputStateCreateInfo vertexInput = {};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.pNext = nullptr;
	vertexInput.flags = 0;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.pVertexBindingDescriptions = nullptr;
	vertexInput.vertexAttributeDescriptionCount = 0;
	vertexInput.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr;
	inputAssembly.flags = 0;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport vp = {};
	vp.x = 0.0f;
	vp.y = 0.0f;
	vp.width = _swapchainExtent.width;
	vp.height = _swapchainExtent.height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = _swapchainExtent;

	VkPipelineViewportStateCreateInfo vpCreateInfo = {};
	vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vpCreateInfo.pScissors = &scissor;
	vpCreateInfo.pViewports = &vp;
	vpCreateInfo.viewportCount = 1;
	vpCreateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
	rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rastCreateInfo.cullMode = VK_CULL_MODE_NONE;
	rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rastCreateInfo.depthClampEnable = VK_FALSE;
	rastCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rastCreateInfo.lineWidth = 1.0f;
	rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rastCreateInfo.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo msCreateInfo = {};
	msCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	msCreateInfo.sampleShadingEnable = VK_FALSE;
	msCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendAttachment;
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;


	VkDescriptorSetLayout layouts[] = { _bufferSetLayout , _textureSetLayout };

	/*Create the pipelinelayout*/
	VulkanHelpers::CreatePipelineLayout(_vkDevice, &_testPipelineLayout, 2, layouts);

	/* Create the graphics pipeline*/
	auto pipelineInfo = VulkanHelpers::MakePipelineCreateInfo(
		2,
		shaderStages,
		&vertexInput,
		&inputAssembly,
		nullptr,
		&vpCreateInfo,
		&rastCreateInfo,
		&msCreateInfo,
		nullptr,
		&colorBlendInfo,
		nullptr,
		_testPipelineLayout,
		_renderPass
	);
	VulkanHelpers::CreateGraphicsPipelines(_vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, &_testPipeline);

}


void VulkanRenderer::_createShaderModule(const std::string & filename)
{
	if (_shaderModules.find(filename) != _shaderModules.end())
		return;
	
	std::vector<char> shaderCode;
	std::ifstream fin(filename, std::ios_base::in | std::ios_base::binary);
	if (!fin.is_open())
		throw std::runtime_error("Could not find shader: " + filename);
	fin.seekg(0,std::ios_base::end);
	size_t size = fin.tellg();
	fin.seekg(0,std::ios_base::beg);
	shaderCode.resize(size);
	fin.read(shaderCode.data(), shaderCode.size());

	VkShaderModuleCreateInfo cinfo = {};
	cinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	cinfo.codeSize = shaderCode.size();
	cinfo.pCode = (uint32_t*)shaderCode.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_vkDevice, &cinfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module.");

	_shaderModules[filename] = shaderModule;
}


