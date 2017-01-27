#include "VulkanHelpers.h"



VulkanHelpers::VulkanHelpers()
{
}


VulkanHelpers::~VulkanHelpers()
{
}

VkApplicationInfo VulkanHelpers::MakeApplicationInfo(const char * pApplicationName, uint32_t applicationVersion, const char * pEngineName, uint32_t engineVersion, uint32_t apiVersion, const void * pNext) const
{
	VkApplicationInfo vkAppInfo =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,	// sType;
		pNext,// pNext;
		pApplicationName,// Application name
		applicationVersion,// applicationVersion;
		pEngineName,// pEngineName;
		engineVersion,// engineVersion;
		apiVersion// apiVersion;
	};
	return vkAppInfo;
}

VkInstanceCreateInfo VulkanHelpers::MakeInstanceCreateInfo(VkInstanceCreateFlags flags, const VkApplicationInfo * pApplicationInfo, uint32_t enabledLayerCount, const char * const * ppEnabledLayerNames, const void * pNext, uint32_t enabledExtensionCount, const char * const * ppEnabledExtensionNames) const
{
	VkInstanceCreateInfo vkInstCreateInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,//sType
		pNext,//pNext
		flags,//flags	
		pApplicationInfo,//pApplicationInfo
		enabledLayerCount,//enabledLayerCount
		ppEnabledLayerNames,//ppEnabledLayerNames
		enabledExtensionCount,// enabledExtCount
		ppEnabledExtensionNames// ppEnabledExtNames
	};

	return vkInstCreateInfo;
}

VkDeviceQueueCreateInfo VulkanHelpers::MakeDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const float * pQueuePriorities, VkDeviceQueueCreateFlags flags, const void * pNext) const
{
	const VkDeviceQueueCreateInfo deviceQueueCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,   // sType
		pNext,                                        // pNext
		flags,                                        // flags
		queueFamilyIndex,                             // queueFamilyIndex
		queueCount,                                   // queueCount
		pQueuePriorities                              // pQueuePriorities
	};

	return deviceQueueCreateInfo;
}

VkDeviceCreateInfo VulkanHelpers::MakeDeviceCreateInfo(uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo * pQueueCreateInfos, uint32_t enabledLayerCount, const char * const * ppEnabledLayerNames, const VkPhysicalDeviceFeatures * pEnabledFeatures, const void * pNext, uint32_t enabledExtensionCount, const char * const * ppEnabledExtensionNames, VkDeviceCreateFlags flags) const
{
	const VkDeviceCreateInfo deviceCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,     // sType
		pNext,                                    // pNext
		flags,                                    // flags
		queueCreateInfoCount,                     // queueCreateInfoCount
		pQueueCreateInfos,						  // pQueueCreateInfos
		enabledLayerCount,                        // enabledLayerCount
		ppEnabledLayerNames,                      // ppEnabledLayerNames
		enabledExtensionCount,                    // enabledExtensionCount
		ppEnabledExtensionNames,                  // ppEnabledExtensionNames
		pEnabledFeatures						  // pEnabledFeatures
	};
	return deviceCreateInfo;
}

VkCommandPoolCreateInfo VulkanHelpers::MakeCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, const void * pNext) const
{
	VkCommandPoolCreateInfo cmdPoolInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		pNext,
		flags,
		queueFamilyIndex
	};

	return cmdPoolInfo;
}

VkCommandBufferAllocateInfo VulkanHelpers::MakeCommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t commandBufferCount, const void * pNext) const
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		pNext,
		commandPool,
		level,
		commandBufferCount
	};
	return cmdBufferAllocInfo;
}

VkSubmitInfo VulkanHelpers::MakeSubmitInfo(uint32_t commandBufferCount, const VkCommandBuffer * pCommandBuffers, uint32_t waitSemaphoreCount, const VkSemaphore * pWaitSemaphores, const VkPipelineStageFlags * pWaitDstStageMask, uint32_t signalSemaphoreCount, const VkSemaphore * pSignalSemaphores, const void * pNext) const
{
	VkSubmitInfo submitInfo =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		pNext,
		waitSemaphoreCount,
		pWaitSemaphores,
		pWaitDstStageMask,
		commandBufferCount,
		pCommandBuffers,
		signalSemaphoreCount,
		pSignalSemaphores
	};

	return submitInfo;
}

VkMappedMemoryRange VulkanHelpers::MakeMappedMemoryRange(VkDeviceMemory memory, VkDeviceSize size, VkDeviceSize offset, const void * pNext) const
{
	VkMappedMemoryRange range =
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		pNext,
		memory,
		offset,
		size
	};

	return range;
}

const void VulkanHelpers::CreateInstance(const VkInstanceCreateInfo * pCreateInfo, VkInstance * pInstance, const VkAllocationCallbacks * pAllocator) const
{
	/******** Create the instance***********/
	VkResult result = vkCreateInstance(pCreateInfo, pAllocator, pInstance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance.");
	}
}

const void VulkanHelpers::CreateLogicDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo * pCreateInfo, VkDevice * pDevice, const VkAllocationCallbacks * pAllocator) const
{
	VkResult	result = vkCreateDevice(physicalDevice,
		pCreateInfo,
		pAllocator,
		pDevice);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logic device.");
	}
}

const void VulkanHelpers::CreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo * pCreateInfo, VkCommandPool * pCommandPool, const VkAllocationCallbacks * pAllocator) const
{
	VkResult result = vkCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);


	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool.");
	}
}

const void VulkanHelpers::AllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo * pAllocateInfo, VkCommandBuffer * pCommandBuffers) const
{
	VkResult result = vkAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers.");
	}
}

const void VulkanHelpers::BeginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo * pInheritanceInfo, const void* pNext) const
{
	/*typedef struct VkCommandBufferBeginInfo {
    VkStructureType                        sType;
    const void*                            pNext;
    VkCommandBufferUsageFlags              flags;
    const VkCommandBufferInheritanceInfo*  pInheritanceInfo;
} VkCommandBufferBeginInfo;*/

	VkCommandBufferBeginInfo cmdInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		pNext,
		flags,
		pInheritanceInfo
	};

	VkResult result = vkBeginCommandBuffer(commandBuffer, &cmdInfo);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to being command buffer");
	}
}

const void VulkanHelpers::EndCommandBuffer(VkCommandBuffer commandBuffer) const
{
	VkResult result = vkEndCommandBuffer(commandBuffer);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to end command buffer");
	}
}

const void VulkanHelpers::ResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const
{
	VkResult result = vkResetCommandBuffer(commandBuffer, flags);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to reset command buffer");
	}
}

const void VulkanHelpers::ResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) const
{
	VkResult result = vkResetCommandPool(device, commandPool, flags);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to reset command pool");
	}
}

const void VulkanHelpers::QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo * pSubmits, VkFence fence) const
{
	VkResult result = vkQueueSubmit(queue, submitCount, pSubmits, fence);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit queue.");
	}
}

const void VulkanHelpers::AllocateMemory(VkDevice device, VkDeviceSize allocationSize, uint32_t memoryTypeIndex, VkDeviceMemory * pMemory, const void * pNext, const VkAllocationCallbacks * pAllocator) const
{
	VkMemoryAllocateInfo allocInfo =
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		pNext,
		allocationSize,
		memoryTypeIndex
	};


	VkResult result = vkAllocateMemory(device, &allocInfo, pAllocator, pMemory);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate memory.");
	}
}

const void VulkanHelpers::MapMemory(VkDevice device, VkDeviceMemory memory, void ** ppData, VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags) const
{
	VkResult result = vkMapMemory(device, memory, offset, size, flags, ppData);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to map memory.");
	}
}

const void VulkanHelpers::CopyDataBetweenBuffers(VkCommandBuffer cmdBuffer, VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size) const
{
	const VkBufferCopy copyRegion =
	{
		srcOffset, dstOffset, size
	};

	vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}


uint32_t VulkanHelpers::ChooseHeapFromFlags(VkPhysicalDevice physicalDevice, const VkMemoryRequirements * memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags) const
{
	const auto& deviceMemoryProperties = GetPhysicalDeviceMemoryProperties(physicalDevice);

	uint32_t selectedType = ~0u;
	uint32_t memoryType;

	for (memoryType = 0; memoryType < 32; ++memoryType)
	{
		if (memoryRequirements->memoryTypeBits & (1 << memoryType))
		{
			const VkMemoryType& type =
				deviceMemoryProperties.memoryTypes[memoryType];

			// If it exactly matches my preferred properties, grab it.
			if ((type.propertyFlags & preferredFlags) == preferredFlags)
			{
				selectedType = memoryType;
				break;
			}
		}
	}
	if (selectedType != ~0u)
	{
		for (memoryType = 0; memoryType < 32; ++memoryType)
		{
			if (memoryRequirements->memoryTypeBits & (1 << memoryType))
			{
				const VkMemoryType& type =
					deviceMemoryProperties.memoryTypes[memoryType];

				// If it has all my required properties, it'll do.
				if ((type.propertyFlags & requiredFlags) == requiredFlags)
				{
					selectedType = memoryType;
					break;
				}
			}
		}
	}

	return selectedType;
}

std::vector<VkPhysicalDevice> VulkanHelpers::EnumeratePhysicalDevices(VkInstance instance)const
{
	std::vector<VkPhysicalDevice> physicalDevices;
	// First figure out how many devices are in the system.
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);


	// Size the device array appropriately and get the physical
	// device handles.
	physicalDevices.resize(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance,
		&physicalDeviceCount,
		&physicalDevices[0]);

	return physicalDevices;
}

VkPhysicalDeviceProperties VulkanHelpers::GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)const
{
	VkPhysicalDeviceProperties properties;

	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	return properties;
}

VkPhysicalDeviceMemoryProperties VulkanHelpers::GetPhysicalDeviceMemoryProperties(VkPhysicalDevice phydev)const
{
	VkPhysicalDeviceMemoryProperties prop;

	vkGetPhysicalDeviceMemoryProperties(phydev, &prop);

	return prop;
}

VkPhysicalDeviceFeatures VulkanHelpers::GetPhysicalDeviceFeatures(VkPhysicalDevice phydev)const
{
	VkPhysicalDeviceFeatures feat;

	vkGetPhysicalDeviceFeatures(phydev, &feat);

	return feat;
}

std::vector<VkPhysicalDeviceFeatures> VulkanHelpers::EnumeratePhysicalDeviceFeatures(VkInstance instance)const
{
	std::vector<VkPhysicalDeviceFeatures> features;

	auto& phydevs = EnumeratePhysicalDevices(instance);

	for (auto& pd : phydevs)
	{
		features.push_back(GetPhysicalDeviceFeatures(pd));
	}

	return features;
}

std::vector<VkQueueFamilyProperties> VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(VkPhysicalDevice phydev)const
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

std::vector<std::vector<VkQueueFamilyProperties>> VulkanHelpers::EnumeratePhysicalDeviceQueueFamilyProperties(VkInstance instance)const
{
	std::vector<std::vector<VkQueueFamilyProperties>> propDev;

	auto& devices = EnumeratePhysicalDevices(instance);

	for (auto& d : devices)
	{
		propDev.push_back(EnumeratePhysicalDeviceQueueFamilyProperties(d));
	}


	return propDev;
}

const void VulkanHelpers::PrintPhysicalDeviceProperties(VkPhysicalDeviceProperties prop) const
{
	printf("\tDevice Name: %s\n", prop.deviceName);
	printf("\tDevice Driver Version: %d\n", prop.driverVersion);
	printf("***********************************\n\n");
}

const void VulkanHelpers::PrintQueueFamilyProperties(VkQueueFamilyProperties fam) const
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

	printf("\tQueue Count: %I32d\n", fam.queueCount);
	printf("***********************************\n\n");
}

const void VulkanHelpers::PrintPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties prop) const
{
	printf("\tMemory Type Count: %I32d\n", prop.memoryTypeCount);
	for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
	{
		auto& memType = prop.memoryTypes[i];
		printf("\t\tHeap Index: %I32d\n", memType.heapIndex);
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
		printf("------------------------------------\n\n");
	}

	printf("\tMemory Heap Count: %I32d\n", prop.memoryHeapCount);
	for (uint32_t i = 0; i < prop.memoryHeapCount; i++)
	{
		auto& memHeap = prop.memoryHeaps[i];
		printf("\t\tHeap Size: %I64d\n", memHeap.size);
		printf("\t\tPropertyFlags: \n");
		if (memHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			printf("\t\t\tVK_MEMORY_HEAP_DEVICE_LOCAL_BIT\n");
		printf("------------------------------------\n\n");
	}

	printf("***********************************\n\n");
}

