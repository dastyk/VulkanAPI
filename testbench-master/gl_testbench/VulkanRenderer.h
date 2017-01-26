#pragma once
#include "Renderer.h"
#include <SDL.h>
#include <vulkan\vulkan.h>
#include <ConsoleThread.h>

#pragma comment(lib,"vulkan-1.lib")
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
	Technique* makeTechnique() {};
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
	std::vector<VkPhysicalDevice> EnumeratePhysicalDevices();
	VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
	VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice phydev);

	std::vector<VkQueueFamilyProperties> EnumeratePhysicalDeviceQueueFamilyProperties(VkPhysicalDevice phydev);
	std::vector<std::vector<VkQueueFamilyProperties>> EnumeratePhysicalDeviceQueueFamilyProperties();

private:
	SDL_Window* window;
	

	VkInstance _vkInstance;



	std::vector<Mesh*> drawList;

	bool globalWireframeMode = false;

	float clearColor[4] = { 0,0,0,0 };
	//std::unordered_map<int, int> BUFFER_MAP = {
	//	{ 0, 0 },
	//	{ CLEAR_BUFFER_FLAGS::COLOR, GL_COLOR_BUFFER_BIT },
	//	{ CLEAR_BUFFER_FLAGS::DEPTH, GL_DEPTH_BUFFER_BIT },
	//	{ CLEAR_BUFFER_FLAGS::STENCIL, GL_STENCIL_BUFFER_BIT }
	//};

};

