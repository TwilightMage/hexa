#pragma once

#include <vulkan/vulkan.hpp>

/**
* @brief Encapsulates access to a Vulkan buffer backed up by device memory
* @note To be filled by an external source like the VulkanDevice
*/
struct VulkanBuffer
{
	vk::Device device;
	vk::Buffer buffer = nullptr;
	vk::DeviceMemory memory = nullptr;
	vk::DescriptorBufferInfo descriptor;
	vk::DeviceSize size = 0;
	vk::DeviceSize alignment = 0;
	void* mapped = nullptr;
	/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
	vk::BufferUsageFlags usageFlags;
	/** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
	vk::MemoryPropertyFlags memoryPropertyFlags;

	vk::Result Map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	void Unmap();
	void Bind(vk::DeviceSize offset = 0);
	void SetupDescriptor(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	void CopyTo(void* data, vk::DeviceSize size);
	vk::Result Flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	vk::Result Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	void Destroy();
};
