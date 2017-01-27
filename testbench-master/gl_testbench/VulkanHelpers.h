#include <vulkan\vulkan.h>
#include <vector>

#pragma comment(lib,"vulkan-1.lib")

#pragma once
class VulkanHelpers
{
public:
	VulkanHelpers();
	~VulkanHelpers();




	/*Struct creation*/
	VkApplicationInfo MakeApplicationInfo(
		const char*									pApplicationName			= nullptr,
		uint32_t           							applicationVersion			= VK_MAKE_VERSION(0,0,0),
		const char*        							pEngineName					= nullptr,
		uint32_t           							engineVersion				= VK_MAKE_VERSION(0,0,0),
		uint32_t           							apiVersion					= VK_API_VERSION_1_0,
		const void*        							pNext						= nullptr)const;

	VkInstanceCreateInfo MakeInstanceCreateInfo(
		VkInstanceCreateFlags						flags,
		const VkApplicationInfo*					pApplicationInfo			= nullptr,
		uint32_t									enabledLayerCount			= 0,
		const char* const*							ppEnabledLayerNames			= nullptr,
		const void*									pNext						= nullptr,
		uint32_t									enabledExtensionCount		= 0,
		const char* const*							ppEnabledExtensionNames		= nullptr)const;

	VkDeviceQueueCreateInfo MakeDeviceQueueCreateInfo(		
		uint32_t									queueFamilyIndex,
		uint32_t									queueCount,
		const float*								pQueuePriorities			= nullptr,
		VkDeviceQueueCreateFlags					flags						= 0,
		const void*									pNext						= nullptr)const;

	VkDeviceCreateInfo MakeDeviceCreateInfo(	
		uint32_t									queueCreateInfoCount		= 0,
		const VkDeviceQueueCreateInfo*				pQueueCreateInfos			= nullptr,
		uint32_t									enabledLayerCount			= 0,
		const char* const*							ppEnabledLayerNames			= nullptr,
		const VkPhysicalDeviceFeatures*				pEnabledFeatures			= nullptr,
		const void*									pNext						= nullptr,
		uint32_t									enabledExtensionCount		= 0,
		const char* const*							ppEnabledExtensionNames		= nullptr,
		VkDeviceCreateFlags							flags						= 0)const;

	VkCommandPoolCreateInfo MakeCommandPoolCreateInfo(
		uint32_t									queueFamilyIndex,
		VkCommandPoolCreateFlags					flags						= 0,
		const void*									pNext						= nullptr)const;

	VkCommandBufferAllocateInfo MakeCommandBufferAllocateInfo(	
		VkCommandPool								commandPool,
		VkCommandBufferLevel						level,
		uint32_t									commandBufferCount,
		const void*									pNext						=	nullptr)const;

	VkSubmitInfo MakeSubmitInfo(
		uint32_t									commandBufferCount,
		const VkCommandBuffer*						pCommandBuffers,
		uint32_t									waitSemaphoreCount			= 0,
		const VkSemaphore*							pWaitSemaphores				= nullptr,
		const VkPipelineStageFlags*					pWaitDstStageMask			= nullptr,
		uint32_t									signalSemaphoreCount		= 0,
		const VkSemaphore*							pSignalSemaphores			= nullptr,
		const void*									pNext						= nullptr)const;

	VkMappedMemoryRange MakeMappedMemoryRange(
		VkDeviceMemory								memory,
		VkDeviceSize								size						= 0,
		VkDeviceSize      							offset						= VK_WHOLE_SIZE,
		const void*        							pNext						= nullptr)const;
	
	VkBufferCreateInfo MakeBufferCreateInfo(
		VkDeviceSize								size,
		VkBufferUsageFlags     						usage,
		VkBufferCreateFlags    						flags						= 0,
		VkSharingMode          						sharingMode					= VK_SHARING_MODE_EXCLUSIVE,
		uint32_t               						queueFamilyIndexCount		= 0,
		const uint32_t*        						pQueueFamilyIndices			= nullptr,
		const void*            						pNext						= nullptr)const;

	VkImageCreateInfo MakeImageCreateInfo(
		VkFormat									format,
		VkExtent3D               					extent,
		VkImageUsageFlags        					usage,
		VkImageLayout            					initialLayout				 = VK_IMAGE_LAYOUT_UNDEFINED,
		VkSampleCountFlagBits    					samples						 = VK_SAMPLE_COUNT_1_BIT,
		uint32_t                 					arrayLayers					 = 1,
		VkImageType              					imageType					 = VK_IMAGE_TYPE_2D,
		uint32_t                 					mipLevels					 = 10,
		VkImageCreateFlags       					flags						 = 0,
		VkImageTiling            					tiling						 = VK_IMAGE_TILING_OPTIMAL,
		VkSharingMode            					sharingMode					 = VK_SHARING_MODE_EXCLUSIVE,
		uint32_t                 					queueFamilyIndexCount		 = 0,
		const uint32_t*          					pQueueFamilyIndices			 = nullptr,
		const void*              					pNext						 = nullptr)const;


	/*Creation*/
	const void CreateInstance(
		const VkInstanceCreateInfo*                 pCreateInfo,
		VkInstance*                                 pInstance,
		const VkAllocationCallbacks*                pAllocator					= nullptr)const;


	const void CreateLogicDevice(
		VkPhysicalDevice                            physicalDevice,
		const VkDeviceCreateInfo*                   pCreateInfo,
		VkDevice*                                   pDevice,
		const VkAllocationCallbacks*                pAllocator					= nullptr)const;

	const void CreateCommandPool(
		VkDevice									device,
		const VkCommandPoolCreateInfo*				pCreateInfo,
		VkCommandPool*								pCommandPool,
		const VkAllocationCallbacks*				pAllocator					= nullptr)const;

	const void AllocateCommandBuffers(
		VkDevice									device,
		const VkCommandBufferAllocateInfo*			pAllocateInfo,
		VkCommandBuffer*							pCommandBuffers)const;

	const void CreateBuffer(
		VkDevice									device,
		const VkBufferCreateInfo*					pCreateInfo,
		VkBuffer*									pBuffer,
		const VkAllocationCallbacks*				pAllocator					= nullptr)const;

	const void CreateImage(
		VkDevice									device,
		const VkImageCreateInfo*					pCreateInfo,
		VkImage*									pImage,
		const VkAllocationCallbacks*				pAllocator					= nullptr)const;


	/*Command recording*/
	const void BeginCommandBuffer(
		VkCommandBuffer								commandBuffer,
		VkCommandBufferUsageFlags					flags						= 0,
		const VkCommandBufferInheritanceInfo*		pInheritanceInfo			= nullptr,
		const void*									pNext						= nullptr)const;

	const void EndCommandBuffer(
		VkCommandBuffer								commandBuffer)const;

	const void ResetCommandBuffer(
		VkCommandBuffer								commandBuffer,
		VkCommandBufferResetFlags					flags)const;

	const void ResetCommandPool(
		VkDevice									device,
		VkCommandPool								commandPool,
		VkCommandPoolResetFlags						flags)const;

	const void QueueSubmit(
		VkQueue										queue,
		uint32_t									submitCount,
		const VkSubmitInfo*							pSubmits,
		VkFence										fence)const;




	/*Memory management*/
	const void AllocateMemory(
		VkDevice                                    device,
		VkDeviceSize								allocationSize,
		uint32_t									memoryTypeIndex,
		VkDeviceMemory*                             pMemory,
		const void*									pNext						= nullptr,
		const VkAllocationCallbacks*                pAllocator					= nullptr)const;

	const void MapMemory(
		VkDevice                                    device,
		VkDeviceMemory                              memory,
		void**                                      ppData,
		VkDeviceSize                                size						= VK_WHOLE_SIZE,
		VkDeviceSize                                offset						= 0,
		VkMemoryMapFlags                            flags						= 0)const;




	/*Various operations*/
	const void CopyDataBetweenBuffers(
		VkCommandBuffer								cmdBuffer,
		VkBuffer srcBuffer, VkDeviceSize srcOffset,
		VkBuffer dstBuffer, VkDeviceSize dstOffset,
		VkDeviceSize								size)const;




	/*Utilities*/
	uint32_t ChooseHeapFromFlags(
		VkPhysicalDevice							physicalDevice,
		const VkMemoryRequirements*					memoryRequirements,
		VkMemoryPropertyFlags						requiredFlags,
		VkMemoryPropertyFlags						preferredFlags)const;




	/*Enumerations and get functions*/
	VkSubresourceLayout GetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource)const;
	std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance)const;
	VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)const;
	VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice phydev)const;
	VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice phydev)const;
	std::vector<VkPhysicalDeviceFeatures> EnumeratePhysicalDeviceFeatures(VkInstance instance)const;
	std::vector<VkQueueFamilyProperties> EnumeratePhysicalDeviceQueueFamilyProperties(VkPhysicalDevice phydev)const;
	std::vector<std::vector<VkQueueFamilyProperties>> EnumeratePhysicalDeviceQueueFamilyProperties(VkInstance instance)const;
	VkFormatProperties GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format)const;
	VkImageFormatProperties GetPhysicalDeviceImageFormatProperties(
		VkPhysicalDevice							physicalDevice,
		VkFormat									format,
		VkImageType									type,
		VkImageTiling								tiling,
		VkImageUsageFlags							usage,
		VkImageCreateFlags							flags)const;


	/*Property printing*/
	const void PrintPhysicalDeviceProperties( VkPhysicalDeviceProperties prop)const;
	const void PrintQueueFamilyProperties(VkQueueFamilyProperties fam)const;
	const void PrintPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties prop)const;


};

