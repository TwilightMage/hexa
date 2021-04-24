#include "VulkanDevice.h"

#include "Game.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"

/**
* Default constructor
*
* @param physicalDevice Physical device that is to be used
*/
VulkanDevice::VulkanDevice(vk::PhysicalDevice physicalDevice)
{
	assert(physicalDevice);
	this->physicalDevice = physicalDevice;

	// Store Properties features, limits and properties of the physical device for later use
	// Device properties also contain limits and sparse properties
	properties = physicalDevice.getProperties();
	// Features should be checked by the examples before using them
	features = physicalDevice.getFeatures();
	// Memory properties are used regularly for creating all kinds of buffers
	memoryProperties = physicalDevice.getMemoryProperties();
	// Queue family properties, used for setting up requested queues upon device creation
	queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	assert(queueFamilyProperties.size() > 0);

	// Get list of supported extensions
	for (auto ext : physicalDevice.enumerateDeviceExtensionProperties())
	{
		supportedExtensions.push_back(ext.extensionName);
	}
}

/**
* Default destructor
*
* @note Frees the logical device
*/
VulkanDevice::~VulkanDevice()
{
	if (commandPool)
	{
		logicalDevice.destroyCommandPool(commandPool);
	}
	if (logicalDevice)
	{
		logicalDevice.destroy();
	}
}

/**
* Get the index of a memory type that has all the requested property bits set
*
* @param typeBits Bit mask with bits set for each memory type supported by the resource to request for (from vk::MemoryRequirements)
* @param properties Bit mask of properties for the memory type to request
* @param (Optional) memTypeFound Pointer to a bool that is set to true if a matching memory type has been found
*
* @return Index of the requested memory type
*
* @throw Throws an exception if memTypeFound is null and no memory type could be found that supports the requested properties
*/
uint32_t VulkanDevice::GetMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties, vk::Bool32* memTypeFound) const
{
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				if (memTypeFound)
				{
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Could not find a matching memory type");
	}
}

/**
* Get the index of a queue family that supports the requested queue flags
*
* @param queueFlags Queue flags to find a queue family index for
*
* @return Index of the queue family index that matches the flags
*
* @throw Throws an exception if no queue family index could be found that supports the requested flags
*/
uint32_t VulkanDevice::GetQueueFamilyIndex(vk::QueueFlags queueFlags) const
{
	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queueFlags & vk::QueueFlagBits::eCompute)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits()))
			{
				return i;
			}
		}
	}

	// Dedicated queue for transfer
	// Try to find a queue family index that supports transfer but not graphics and compute
	if (queueFlags & vk::QueueFlagBits::eTransfer)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits()) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits()))
			{
				return i;
			}
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
	{
		if (queueFamilyProperties[i].queueFlags & queueFlags)
		{
			return i;
		}
	}

	throw std::runtime_error("Could not find a matching queue family index");
}

/**
* Create the logical device based on the assigned physical device, also gets default queue family indices
*
* @param enabledFeatures Can be used to enable certain features upon device creation
* @param pNextChain Optional chain of pointer to extension structures
* @param useSwapChain Set to false for headless rendering to omit the swapchain device extensions
* @param requestedQueueTypes Bit flags specifying the queue types to be requested from the device
*
* @return vk::Result of the device creation call
*/
vk::Result VulkanDevice::CreateLogicalDevice(vk::PhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain, bool useSwapChain, vk::QueueFlags requestedQueueTypes)
{
	// Desired queues need to be requested upon logical device creation
	// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
	// requests different queue types

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation

	const float defaultQueuePriority(0.0f);

	// Graphics queue
	if (requestedQueueTypes & vk::QueueFlagBits::eGraphics)
	{
		queueFamilyIndices.graphics = GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics);
		vk::DeviceQueueCreateInfo queueInfo;
		queueInfo.queueFamilyIndex = queueFamilyIndices.graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(queueInfo);
	}
	else
	{
		queueFamilyIndices.graphics = VK_NULL_HANDLE;
	}

	// Dedicated compute queue
	if (requestedQueueTypes & vk::QueueFlagBits::eCompute)
	{
		queueFamilyIndices.compute = GetQueueFamilyIndex(vk::QueueFlagBits::eCompute);
		if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			vk::DeviceQueueCreateInfo queueInfo;
			queueInfo.queueFamilyIndex = queueFamilyIndices.compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		queueFamilyIndices.compute = queueFamilyIndices.graphics;
	}

	// Dedicated transfer queue
	if (requestedQueueTypes & vk::QueueFlagBits::eTransfer)
	{
		queueFamilyIndices.transfer = GetQueueFamilyIndex(vk::QueueFlagBits::eTransfer);
		if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute))
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			vk::DeviceQueueCreateInfo queueInfo;
			queueInfo.queueFamilyIndex = queueFamilyIndices.transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		queueFamilyIndices.transfer = queueFamilyIndices.graphics;
	}

	// Create the logical device representation
	std::vector<const char*> deviceExtensions(enabledExtensions);
	if (useSwapChain)
	{
		// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	// If a pNext(Chain) has been passed, we need to add it to the device creation info
	vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2;
	if (pNextChain)
	{
		physicalDeviceFeatures2.features = enabledFeatures;
		physicalDeviceFeatures2.pNext = pNextChain;
		deviceCreateInfo.pEnabledFeatures = nullptr;
		deviceCreateInfo.pNext = &physicalDeviceFeatures2;
	}

	// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
	if (ExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		enableDebugMarkers = true;
	}

	if (deviceExtensions.size() > 0)
	{
		for (const char* enabledExtension : deviceExtensions)
		{
			if (!ExtensionSupported(enabledExtension))
			{
				PrintError("Vulkan", "Enabled device extension %s is not present at device level", enabledExtension);
			}
		}

		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	this->enabledFeatures = enabledFeatures;

	vk::Result result = physicalDevice.createDevice(&deviceCreateInfo, nullptr, &logicalDevice);
	if (result != vk::Result::eSuccess)
	{
		return result;
	}

	// Create a default command pool for graphics command buffers
	commandPool = CreateCommandPool(queueFamilyIndices.graphics);

	return result;
}

/**
* Create a buffer on the device
*
* @param usageFlags Usage flag bit mask for the buffer (i.e. index, vertex, uniform buffer)
* @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
* @param size Size of the buffer in byes
* @param buffer Pointer to the buffer handle acquired by the function
* @param memory Pointer to the memory handle acquired by the function
* @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
*
* @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
*/
void VulkanDevice::CreateBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize size, vk::Buffer* buffer, vk::DeviceMemory* memory, void* data)
{
	// Create the buffer handle
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.size = size;
	bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	CHECK_VULKAN_RESULT(logicalDevice.createBuffer(&bufferCreateInfo, nullptr, buffer));

	// Create the memory backing up the buffer handle
	vk::MemoryRequirements memReqs;
	vk::MemoryAllocateInfo memAlloc;
	logicalDevice.getBufferMemoryRequirements(*buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	// Find a memory type index that fits the properties of the buffer
	memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
	// If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
	vk::MemoryAllocateFlagsInfoKHR allocFlagsInfo;
	if (usageFlags & vk::BufferUsageFlagBits::eShaderDeviceAddress)
	{
		allocFlagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
		memAlloc.pNext = &allocFlagsInfo;
	}
	CHECK_VULKAN_RESULT(logicalDevice.allocateMemory(&memAlloc, nullptr, memory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (data != nullptr)
	{
		void* mapped;
		CHECK_VULKAN_RESULT(logicalDevice.mapMemory(*memory, 0, size, vk::MemoryMapFlagBits(), &mapped));
		memcpy(mapped, data, size);
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		if ((memoryPropertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits())
		{
			vk::MappedMemoryRange mappedRange;
			mappedRange.memory = *memory;
			mappedRange.offset = 0;
			mappedRange.size = size;
			CHECK_VULKAN_RESULT(logicalDevice.flushMappedMemoryRanges(1, &mappedRange));
		}
		vkUnmapMemory(logicalDevice, *memory);
	}

	logicalDevice.bindBufferMemory(*buffer, *memory, 0);
}

/**
* Create a buffer on the device
*
* @param usageFlags Usage flag bit mask for the buffer (i.e. index, vertex, uniform buffer)
* @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
* @param buffer Pointer to a vk::Vulkan buffer object
* @param size Size of the buffer in bytes
* @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
*
* @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
*/
void VulkanDevice::CreateBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, struct VulkanBuffer* buffer, vk::DeviceSize size, void* data)
{
	buffer->device = logicalDevice;

	// Create the buffer handle
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.size = size;
	CHECK_VULKAN_RESULT(logicalDevice.createBuffer(&bufferCreateInfo, nullptr, &buffer->buffer));

	// Create the memory backing up the buffer handle
	vk::MemoryRequirements memReqs;
	vk::MemoryAllocateInfo memAlloc;
	logicalDevice.getBufferMemoryRequirements(buffer->buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	// Find a memory type index that fits the properties of the buffer
	memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
	// If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
	vk::MemoryAllocateFlagsInfoKHR allocFlagsInfo;
	if (usageFlags & vk::BufferUsageFlagBits::eShaderDeviceAddress)
	{
		allocFlagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
		memAlloc.pNext = &allocFlagsInfo;
	}
	CHECK_VULKAN_RESULT(logicalDevice.allocateMemory(&memAlloc, nullptr, &buffer->memory));

	buffer->alignment = memReqs.alignment;
	buffer->size = size;
	buffer->usageFlags = usageFlags;
	buffer->memoryPropertyFlags = memoryPropertyFlags;

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (data != nullptr)
	{
		CHECK_VULKAN_RESULT(buffer->Map());
		memcpy(buffer->mapped, data, size);
		if ((memoryPropertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent) == vk::MemoryPropertyFlagBits())
		{
			buffer->Flush();
		}

		buffer->Unmap();
	}

	// Initialize a default descriptor that covers the whole buffer size
	buffer->SetupDescriptor();

	// Attach the memory to the buffer object
	buffer->Bind();
}

/**
* Copy buffer data from src to dst using vk::CmdCopyBuffer
*
* @param src Pointer to the source buffer to copy from
* @param dst Pointer to the destination buffer to copy to
* @param queue Pointer
* @param copyRegion (Optional) Pointer to a copy region, if NULL, the whole buffer is copied
*
* @note Source and destination pointers must have the appropriate transfer usage flags set (TRANSFER_SRC / TRANSFER_DST)
*/
void VulkanDevice::CopyBuffer(struct VulkanBuffer* src, struct VulkanBuffer* dst, vk::Queue queue, vk::BufferCopy* copyRegion)
{
	assert(dst->size <= src->size);
	assert(src->buffer);
	vk::CommandBuffer copyCmd = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, true);
	vk::BufferCopy bufferCopy;
	if (copyRegion == nullptr)
	{
		bufferCopy.size = src->size;
	}
	else
	{
		bufferCopy = *copyRegion;
	}

	copyCmd.copyBuffer(src->buffer, dst->buffer, 1, &bufferCopy);

	FlushCommandBuffer(copyCmd, queue);
}

/**
* Create a command pool for allocation command buffers from
*
* @param queueFamilyIndex Family index of the queue to create the command pool for
* @param createFlags (Optional) Command pool creation flags (Defaults to VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
*
* @note Command buffers allocated from the created pool can only be submitted to a queue with the same family index
*
* @return A handle to the created command buffer
*/
vk::CommandPool VulkanDevice::CreateCommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags)
{
	vk::CommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
	cmdPoolInfo.flags = createFlags;
	vk::CommandPool cmdPool;
	CHECK_VULKAN_RESULT(logicalDevice.createCommandPool(&cmdPoolInfo, nullptr, &cmdPool));
	return cmdPool;
}

/**
* Allocate a command buffer from the command pool
*
* @param level Level of the new command buffer (primary or secondary)
* @param pool Command pool from which the command buffer will be allocated
* @param (Optional) begin If true, recording on the new command buffer will be started (vkBeginCommandBuffer) (Defaults to false)
*
* @return A handle to the allocated command buffer
*/
vk::CommandBuffer VulkanDevice::CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool pool, bool begin)
{
	vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
	cmdBufAllocateInfo.commandPool = pool;
	cmdBufAllocateInfo.level = level;
	cmdBufAllocateInfo.commandBufferCount = 1;
	vk::CommandBuffer cmdBuffer;
	CHECK_VULKAN_RESULT(logicalDevice.allocateCommandBuffers(&cmdBufAllocateInfo, &cmdBuffer));
	// If requested, also start recording for the new command buffer
	if (begin)
	{
		vk::CommandBufferBeginInfo cmdBufInfo;
		CHECK_VULKAN_RESULT(cmdBuffer.begin(&cmdBufInfo));
	}
	return cmdBuffer;
}

vk::CommandBuffer VulkanDevice::CreateCommandBuffer(vk::CommandBufferLevel level, bool begin)
{
	return CreateCommandBuffer(level, commandPool, begin);
}

/**
* Finish command buffer recording and submit it to a queue
*
* @param commandBuffer Command buffer to flush
* @param queue Queue to submit the command buffer to
* @param pool Command pool on which the command buffer has been created
* @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
*
* @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
* @note Uses a fence to ensure command buffer has finished executing
*/
void VulkanDevice::FlushCommandBuffer(vk::CommandBuffer commandBuffer, vk::Queue queue, vk::CommandPool pool, bool free)
{
	if (commandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	// Create fence to ensure that the command buffer has finished executing
	vk::FenceCreateInfo fenceInfo;
	vk::Fence fence;
	CHECK_VULKAN_RESULT(logicalDevice.createFence(&fenceInfo, nullptr, &fence));
	// Submit to the queue
	CHECK_VULKAN_RESULT(queue.submit(1, &submitInfo, fence));
	// Wait for the fence to signal that command buffer has finished executing
	CHECK_VULKAN_RESULT(logicalDevice.waitForFences(1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
	logicalDevice.destroyFence(fence, nullptr);
	if (free)
	{
		logicalDevice.freeCommandBuffers(pool, 1, &commandBuffer);
	}
}

void VulkanDevice::FlushCommandBuffer(vk::CommandBuffer commandBuffer, vk::Queue queue, bool free)
{
	return FlushCommandBuffer(commandBuffer, queue, commandPool, free);
}

/**
* Check if an extension is supported by the (physical device)
*
* @param extension Name of the extension to check
*
* @return True if the extension is supported (present in the list read at device creation time)
*/
bool VulkanDevice::ExtensionSupported(std::string extension)
{
	return (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end());
}

/**
* Select the best-fit depth format for this device from a list of possible depth (and stencil) formats
*
* @param checkSamplingSupport Check if the format can be sampled from (e.g. for shader reads)
*
* @return The depth format that best fits for the current device
*
* @throw Throws an exception if no depth format fits the requirements
*/
vk::Format VulkanDevice::GetSupportedDepthFormat(bool checkSamplingSupport)
{
	// All depth formats may be optional, so we need to find a suitable depth format to use
	std::vector<vk::Format> depthFormats = { vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };
	for (auto& format : depthFormats)
	{
		vk::FormatProperties formatProperties;
		physicalDevice.getFormatProperties(format, &formatProperties);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			if (checkSamplingSupport)
			{
				if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage))
				{
					continue;
				}
			}
			return format;
		}
	}
	throw std::runtime_error("Could not find a matching depth format");
}