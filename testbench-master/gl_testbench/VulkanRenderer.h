#pragma once

#include "Renderer.h"
#include <SDL.h>
#include "VulkanHelpers.h"
#include "VulkanVertexBuffer.h"
#include "VulkanMemAllocator.h"
#include "VulkanConstantBuffer.h"
#include "VulkanMesh.h"
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

class VulkanRenderer :
	public Renderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	Material* makeMaterial();
	Mesh* makeMesh();
	VertexBuffer* makeVertexBuffer();
	ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location);
	ResourceBinding* makeResourceBinding();
	RenderState* makeRenderState();
	Technique* makeTechnique(Material* material, RenderState* renderState);
	Texture2D* makeTexture2D();
	Sampler2D* makeSampler2D();
	std::string getShaderPath();
	std::string getShaderExtension();

	int initialize(unsigned int width = 640, unsigned int height = 480);
	int shutdown();

	void setClearColor(float, float, float, float);
	void clearBuffer(unsigned int);
	void setRenderTarget(RenderTarget* rt); // complete parameters
	void setRenderState(RenderState* ps);
	void submit(Mesh* mesh);
	void frame();
	void present();

private:
	static void Enumerate(void* userData, int argc, char** argv);
	static void EnumerateHelp(void* userData, int argc, char** argv);
	void _createSemaphores(void);
	void _createRenderPass(void);
	void _createFramebuffers(void);
	void _createTestPipeline();

private:
	float _clearColor[4];
	SDL_Window* window;

	VkPipelineLayout _testPipelineLayout = VK_NULL_HANDLE;
	

	VkInstance _vkInstance;
	std::vector<VkPhysicalDevice> _vkPhysicalDevices;
	VkDevice _vkDevice;
	VkQueue _vkMainQueue;

	VkCommandPool _vkCmdPool;

	std::vector<VkCommandBuffer> _cmdBuffers;

	VkDescriptorPool _vkBufferDescriptorPool;
	VkDescriptorPool _vkTextureDescriptorPool;

	VkDescriptorSetLayout _bufferSetLayout;
	VkDescriptorSetLayout _textureSetLayout;

	VkSurfaceKHR _vkSurface;
	VkFormat _swapchainFormat;
	VkExtent2D _swapchainExtent;
	VkSwapchainKHR _vkSwapChain;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	uint32_t _swapchainImageIndex = UINT32_MAX;
	VkSemaphore _swapchainImageAvailable = VK_NULL_HANDLE;
	VkSemaphore _renderingComplete = VK_NULL_HANDLE;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> _framebuffers;
#ifdef _DEBUG
	VkDebugReportCallbackEXT _vkDebugCallback;
#endif
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;

	VulkanMemAllocator* _vertexBufferAllocator;
	VulkanMemAllocator* _constantBufferAllocator;
	VulkanMemAllocator* _constantBufferStagingAllocator;
	VulkanMemAllocator* _vertexStagingBufferAllocator;

	std::vector<VulkanMesh*> drawList;

	bool globalWireframeMode = false;

	float clearColor[4] = { 0,0,0,0 };
	//std::unordered_map<int, int> BUFFER_MAP = {
	//	{ 0, 0 },
	//	{ CLEAR_BUFFER_FLAGS::COLOR, GL_COLOR_BUFFER_BIT },
	//	{ CLEAR_BUFFER_FLAGS::DEPTH, GL_DEPTH_BUFFER_BIT },
	//	{ CLEAR_BUFFER_FLAGS::STENCIL, GL_STENCIL_BUFFER_BIT }
	//};


	std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> _createBufferCallback;
	std::function<void(const void* data, size_t size, VkBuffer& buffer, StagingBuffer& stagingBuffer)> _updateBufferCallback;
};

