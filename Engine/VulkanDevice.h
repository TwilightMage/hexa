#pragma once

#include <vulkan/vulkan.hpp>

struct VulkanDevice
{
	/** @brief Physical device representation */
	vk::PhysicalDevice physicalDevice;
	/** @brief Logical device representation (application's view of the device) */
	vk::Device logicalDevice;
	/** @brief Properties of the physical device including limits that the application can check against */
	vk::PhysicalDeviceProperties properties;
	/** @brief Features of the physical device that an application can use to check if a feature is supported */
	vk::PhysicalDeviceFeatures features;
	/** @brief Features that have been enabled for use on the physical device */
	vk::PhysicalDeviceFeatures enabledFeatures;
	/** @brief Memory types and heaps of the physical device */
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	/** @brief Queue family properties of the physical device */
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
	/** @brief List of extensions supported by the device */
	std::vector<std::string> supportedExtensions;
	/** @brief Default command pool for the graphics queue family index */
	vk::CommandPool commandPool = nullptr;
	/** @brief Set to true when the debug marker extension is detected */
	bool enableDebugMarkers = false;
	/** @brief Contains queue family indices */
	struct
	{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	} queueFamilyIndices;

	explicit VulkanDevice(vk::PhysicalDevice physicalDevice);
	~VulkanDevice();

	uint32_t          GetMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties, vk::Bool32* memTypeFound = nullptr) const;
	uint32_t          GetQueueFamilyIndex(vk::QueueFlags queueFlags) const;
	vk::Result        CreateLogicalDevice(vk::PhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain, bool useSwapChain = true, vk::QueueFlags requestedQueueTypes = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);
	void              CreateBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize size, vk::Buffer* buffer, vk::DeviceMemory* memory, void* data = nullptr);
	void              CreateBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, struct VulkanBuffer* buffer, vk::DeviceSize size, void* data = nullptr);
	void              CopyBuffer(struct VulkanBuffer* src, struct VulkanBuffer* dst, vk::Queue queue, vk::BufferCopy* copyRegion = nullptr);
	vk::CommandPool   CreateCommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags createFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	vk::CommandBuffer CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool pool, bool begin = false);
	vk::CommandBuffer CreateCommandBuffer(vk::CommandBufferLevel level, bool begin = false);
	void              FlushCommandBuffer(vk::CommandBuffer commandBuffer, vk::Queue queue, vk::CommandPool pool, bool free = true);
	void              FlushCommandBuffer(vk::CommandBuffer commandBuffer, vk::Queue queue, bool free = true);
	bool              ExtensionSupported(std::string extension);
	vk::Format        GetSupportedDepthFormat(bool checkSamplingSupport);

	operator VkDevice() const { return logicalDevice; }
};
